/*
 * wm8958-dsp2.c  --  WM8958 DSP2 support
 *
 * Copyright 2011 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <trace/events/asoc.h>

#include <linux/mfd/wm8994/core.h>
#include <linux/mfd/wm8994/registers.h>
#include <linux/mfd/wm8994/pdata.h>
#include <linux/mfd/wm8994/gpio.h>

#include "wm8994.h"

#define WM_FW_BLOCK_INFO 0xff
#define WM_FW_BLOCK_PM   0x00
#define WM_FW_BLOCK_X    0x01
#define WM_FW_BLOCK_Y    0x02
#define WM_FW_BLOCK_Z    0x03
#define WM_FW_BLOCK_I    0x06
#define WM_FW_BLOCK_A    0x08
#define WM_FW_BLOCK_C    0x0c

static int wm8958_dsp2_fw(struct snd_soc_codec *codec, const char *name,
			  const struct firmware *fw, bool check)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	u64 data64;
	u32 data32;
	const u8 *data;
	char *str;
	size_t block_len, len;
	int ret = 0;

	
	if (wm8994->cur_fw == fw)
		return 0;

	if (fw->size < 32) {
		dev_err(codec->dev, "%s: firmware too short (%zd bytes)\n",
			name, fw->size);
		goto err;
	}

	if (memcmp(fw->data, "WMFW", 4) != 0) {
		memcpy(&data32, fw->data, sizeof(data32));
		data32 = be32_to_cpu(data32);
		dev_err(codec->dev, "%s: firmware has bad file magic %08x\n",
			name, data32);
		goto err;
	}

	memcpy(&data32, fw->data + 4, sizeof(data32));
	len = be32_to_cpu(data32);

	memcpy(&data32, fw->data + 8, sizeof(data32));
	data32 = be32_to_cpu(data32);
	if ((data32 >> 24) & 0xff) {
		dev_err(codec->dev, "%s: unsupported firmware version %d\n",
			name, (data32 >> 24) & 0xff);
		goto err;
	}
	if ((data32 & 0xffff) != 8958) {
		dev_err(codec->dev, "%s: unsupported target device %d\n",
			name, data32 & 0xffff);
		goto err;
	}
	if (((data32 >> 16) & 0xff) != 0xc) {
		dev_err(codec->dev, "%s: unsupported target core %d\n",
			name, (data32 >> 16) & 0xff);
		goto err;
	}

	if (check) {
		memcpy(&data64, fw->data + 24, sizeof(u64));
		dev_info(codec->dev, "%s timestamp %llx\n",
			 name, be64_to_cpu(data64));
	} else {
		snd_soc_write(codec, 0x102, 0x2);
		snd_soc_write(codec, 0x900, 0x2);
	}

	data = fw->data + len;
	len = fw->size - len;
	while (len) {
		if (len < 12) {
			dev_err(codec->dev, "%s short data block of %zd\n",
				name, len);
			goto err;
		}

		memcpy(&data32, data + 4, sizeof(data32));
		block_len = be32_to_cpu(data32);
		if (block_len + 8 > len) {
			dev_err(codec->dev, "%zd byte block longer than file\n",
				block_len);
			goto err;
		}
		if (block_len == 0) {
			dev_err(codec->dev, "Zero length block\n");
			goto err;
		}

		memcpy(&data32, data, sizeof(data32));
		data32 = be32_to_cpu(data32);

		switch ((data32 >> 24) & 0xff) {
		case WM_FW_BLOCK_INFO:
			
			if (!check)
				break;

			str = kzalloc(block_len + 1, GFP_KERNEL);
			if (str) {
				memcpy(str, data + 8, block_len);
				dev_info(codec->dev, "%s: %s\n", name, str);
				kfree(str);
			} else {
				dev_err(codec->dev, "Out of memory\n");
			}
			break;
		case WM_FW_BLOCK_PM:
		case WM_FW_BLOCK_X:
		case WM_FW_BLOCK_Y:
		case WM_FW_BLOCK_Z:
		case WM_FW_BLOCK_I:
		case WM_FW_BLOCK_A:
		case WM_FW_BLOCK_C:
			dev_dbg(codec->dev, "%s: %zd bytes of %x@%x\n", name,
				block_len, (data32 >> 24) & 0xff,
				data32 & 0xffffff);

			if (check)
				break;

			data32 &= 0xffffff;

			wm8994_bulk_write(codec->control_data,
					  data32 & 0xffffff,
					  block_len / 2,
					  (void *)(data + 8));

			break;
		default:
			dev_warn(codec->dev, "%s: unknown block type %d\n",
				 name, (data32 >> 24) & 0xff);
			break;
		}

		
		block_len += block_len % 4;

		data += block_len + 8;
		len -= block_len + 8;
	}

	if (!check) {
		dev_dbg(codec->dev, "%s: download done\n", name);
		wm8994->cur_fw = fw;
	} else {
		dev_info(codec->dev, "%s: got firmware\n", name);
	}

	goto ok;

err:
	ret = -EINVAL;
ok:
	if (!check) {
		snd_soc_write(codec, 0x900, 0x0);
		snd_soc_write(codec, 0x102, 0x0);
	}

	return ret;
}

static void wm8958_dsp_start_mbc(struct snd_soc_codec *codec, int path)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int i;

	
	if (snd_soc_read(codec, WM8958_DSP2_PROGRAM) & WM8958_DSP2_ENA)
		return;

	
	if (wm8994->mbc)
		wm8958_dsp2_fw(codec, "MBC", wm8994->mbc, false);

	snd_soc_update_bits(codec, WM8958_DSP2_PROGRAM,
			    WM8958_DSP2_ENA, WM8958_DSP2_ENA);

	
	if (pdata && pdata->num_mbc_cfgs) {
		struct wm8958_mbc_cfg *cfg
			= &pdata->mbc_cfgs[wm8994->mbc_cfg];

		for (i = 0; i < ARRAY_SIZE(cfg->coeff_regs); i++)
			snd_soc_write(codec, i + WM8958_MBC_BAND_1_K_1,
				      cfg->coeff_regs[i]);

		for (i = 0; i < ARRAY_SIZE(cfg->cutoff_regs); i++)
			snd_soc_write(codec,
				      i + WM8958_MBC_BAND_2_LOWER_CUTOFF_C1_1,
				      cfg->cutoff_regs[i]);
	}

	
	snd_soc_write(codec, WM8958_DSP2_EXECCONTROL,
		      WM8958_DSP2_RUNR);

	
	snd_soc_update_bits(codec, WM8958_DSP2_CONFIG,
			    WM8958_MBC_ENA |
			    WM8958_MBC_SEL_MASK,
			    path << WM8958_MBC_SEL_SHIFT |
			    WM8958_MBC_ENA);
}

static void wm8958_dsp_start_vss(struct snd_soc_codec *codec, int path)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int i, ena;

	if (wm8994->mbc_vss)
		wm8958_dsp2_fw(codec, "MBC+VSS", wm8994->mbc_vss, false);

	snd_soc_update_bits(codec, WM8958_DSP2_PROGRAM,
			    WM8958_DSP2_ENA, WM8958_DSP2_ENA);

	
	if (pdata && pdata->num_mbc_cfgs) {
		struct wm8958_mbc_cfg *cfg
			= &pdata->mbc_cfgs[wm8994->mbc_cfg];

		for (i = 0; i < ARRAY_SIZE(cfg->combined_regs); i++)
			snd_soc_write(codec, i + 0x2800,
				      cfg->combined_regs[i]);
	}

	if (pdata && pdata->num_vss_cfgs) {
		struct wm8958_vss_cfg *cfg
			= &pdata->vss_cfgs[wm8994->vss_cfg];

		for (i = 0; i < ARRAY_SIZE(cfg->regs); i++)
			snd_soc_write(codec, i + 0x2600, cfg->regs[i]);
	}

	if (pdata && pdata->num_vss_hpf_cfgs) {
		struct wm8958_vss_hpf_cfg *cfg
			= &pdata->vss_hpf_cfgs[wm8994->vss_hpf_cfg];

		for (i = 0; i < ARRAY_SIZE(cfg->regs); i++)
			snd_soc_write(codec, i + 0x2400, cfg->regs[i]);
	}

	
	snd_soc_write(codec, WM8958_DSP2_EXECCONTROL,
		      WM8958_DSP2_RUNR);

	
	ena = 0;
	if (wm8994->mbc_ena[path])
		ena |= 0x8;
	if (wm8994->hpf2_ena[path])
		ena |= 0x4;
	if (wm8994->hpf1_ena[path])
		ena |= 0x2;
	if (wm8994->vss_ena[path])
		ena |= 0x1;

	snd_soc_write(codec, 0x2201, ena);

	
	snd_soc_update_bits(codec, WM8958_DSP2_CONFIG,
			    WM8958_MBC_SEL_MASK | WM8958_MBC_ENA,
			    path << WM8958_MBC_SEL_SHIFT | WM8958_MBC_ENA);
}

static void wm8958_dsp_start_enh_eq(struct snd_soc_codec *codec, int path)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int i;

	wm8958_dsp2_fw(codec, "ENH_EQ", wm8994->enh_eq, false);

	snd_soc_update_bits(codec, WM8958_DSP2_PROGRAM,
			    WM8958_DSP2_ENA, WM8958_DSP2_ENA);

	
	if (pdata && pdata->num_enh_eq_cfgs) {
		struct wm8958_enh_eq_cfg *cfg
			= &pdata->enh_eq_cfgs[wm8994->enh_eq_cfg];

		for (i = 0; i < ARRAY_SIZE(cfg->regs); i++)
			snd_soc_write(codec, i + 0x2200,
				      cfg->regs[i]);
	}

	
	snd_soc_write(codec, WM8958_DSP2_EXECCONTROL,
		      WM8958_DSP2_RUNR);

	
	snd_soc_update_bits(codec, WM8958_DSP2_CONFIG,
			    WM8958_MBC_SEL_MASK | WM8958_MBC_ENA,
			    path << WM8958_MBC_SEL_SHIFT | WM8958_MBC_ENA);
}

static void wm8958_dsp_apply(struct snd_soc_codec *codec, int path, int start)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int pwr_reg = snd_soc_read(codec, WM8994_POWER_MANAGEMENT_5);
	int ena, reg, aif;

	switch (path) {
	case 0:
		pwr_reg &= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA);
		aif = 0;
		break;
	case 1:
		pwr_reg &= (WM8994_AIF1DAC2L_ENA | WM8994_AIF1DAC2R_ENA);
		aif = 0;
		break;
	case 2:
		pwr_reg &= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA);
		aif = 1;
		break;
	default:
		BUG();
		return;
	}

	
	ena = wm8994->mbc_ena[path] || wm8994->vss_ena[path] ||
		wm8994->hpf1_ena[path] || wm8994->hpf2_ena[path] ||
		wm8994->enh_eq_ena[path];
	if (!pwr_reg)
		ena = 0;

	reg = snd_soc_read(codec, WM8958_DSP2_PROGRAM);

	dev_dbg(codec->dev, "DSP path %d %d startup: %d, power: %x, DSP: %x\n",
		path, wm8994->dsp_active, start, pwr_reg, reg);

	if (start && ena) {
		
		if (reg & WM8958_DSP2_ENA)
			return;

		
		if (!(snd_soc_read(codec, WM8994_AIF1_CLOCKING_1)
		      & WM8994_AIF1CLK_ENA_MASK) &&
		    !(snd_soc_read(codec, WM8994_AIF2_CLOCKING_1)
		      & WM8994_AIF2CLK_ENA_MASK))
			return;

		
		snd_soc_update_bits(codec, WM8994_CLOCKING_1,
				    WM8958_DSP2CLK_SRC | WM8958_DSP2CLK_ENA,
				    aif << WM8958_DSP2CLK_SRC_SHIFT |
				    WM8958_DSP2CLK_ENA);

		if (wm8994->enh_eq_ena[path])
			wm8958_dsp_start_enh_eq(codec, path);
		else if (wm8994->vss_ena[path] || wm8994->hpf1_ena[path] ||
		    wm8994->hpf2_ena[path])
			wm8958_dsp_start_vss(codec, path);
		else if (wm8994->mbc_ena[path])
			wm8958_dsp_start_mbc(codec, path);

		wm8994->dsp_active = path;

		dev_dbg(codec->dev, "DSP running in path %d\n", path);
	}

	if (!start && wm8994->dsp_active == path) {
		
		if (!(reg & WM8958_DSP2_ENA))
			return;

		snd_soc_update_bits(codec, WM8958_DSP2_CONFIG,
				    WM8958_MBC_ENA, 0);	
		snd_soc_write(codec, WM8958_DSP2_EXECCONTROL,
			      WM8958_DSP2_STOP);
		snd_soc_update_bits(codec, WM8958_DSP2_PROGRAM,
				    WM8958_DSP2_ENA, 0);
		snd_soc_update_bits(codec, WM8994_CLOCKING_1,
				    WM8958_DSP2CLK_ENA, 0);

		wm8994->dsp_active = -1;

		dev_dbg(codec->dev, "DSP stopped\n");
	}
}

int wm8958_aif_ev(struct snd_soc_dapm_widget *w,
		  struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	int i;

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
	case SND_SOC_DAPM_PRE_PMU:
		for (i = 0; i < 3; i++)
			wm8958_dsp_apply(codec, i, 1);
		break;
	case SND_SOC_DAPM_POST_PMD:
	case SND_SOC_DAPM_PRE_PMD:
		for (i = 0; i < 3; i++)
			wm8958_dsp_apply(codec, i, 0);
		break;
	}

	return 0;
}

static int wm8958_dsp2_busy(struct wm8994_priv *wm8994, int aif)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(wm8994->mbc_ena); i++) {
		if (i == aif)
			continue;
		if (wm8994->mbc_ena[i] || wm8994->vss_ena[i] ||
		    wm8994->hpf1_ena[i] || wm8994->hpf2_ena[i])
			return 1;
	}

	return 0;
}

static int wm8958_put_mbc_enum(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int value = ucontrol->value.integer.value[0];
	int reg;

	
	reg = snd_soc_read(codec, WM8994_CLOCKING_1);
	if (reg < 0 || reg & WM8958_DSP2CLK_ENA)
		return -EBUSY;

	if (value >= pdata->num_mbc_cfgs)
		return -EINVAL;

	wm8994->mbc_cfg = value;

	return 0;
}

static int wm8958_get_mbc_enum(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.enumerated.item[0] = wm8994->mbc_cfg;

	return 0;
}

static int wm8958_mbc_info(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int wm8958_mbc_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int mbc = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = wm8994->mbc_ena[mbc];

	return 0;
}

static int wm8958_mbc_put(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int mbc = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (wm8994->mbc_ena[mbc] == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0] > 1)
		return -EINVAL;

	if (wm8958_dsp2_busy(wm8994, mbc)) {
		dev_dbg(codec->dev, "DSP2 active on %d already\n", mbc);
		return -EBUSY;
	}

	if (wm8994->enh_eq_ena[mbc])
		return -EBUSY;

	wm8994->mbc_ena[mbc] = ucontrol->value.integer.value[0];

	wm8958_dsp_apply(codec, mbc, wm8994->mbc_ena[mbc]);

	return 0;
}

#define WM8958_MBC_SWITCH(xname, xval) {\
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.info = wm8958_mbc_info, \
	.get = wm8958_mbc_get, .put = wm8958_mbc_put, \
	.private_value = xval }

static int wm8958_put_vss_enum(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int value = ucontrol->value.integer.value[0];
	int reg;

	
	reg = snd_soc_read(codec, WM8994_CLOCKING_1);
	if (reg < 0 || reg & WM8958_DSP2CLK_ENA)
		return -EBUSY;

	if (value >= pdata->num_vss_cfgs)
		return -EINVAL;

	wm8994->vss_cfg = value;

	return 0;
}

static int wm8958_get_vss_enum(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.enumerated.item[0] = wm8994->vss_cfg;

	return 0;
}

static int wm8958_put_vss_hpf_enum(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int value = ucontrol->value.integer.value[0];
	int reg;

	
	reg = snd_soc_read(codec, WM8994_CLOCKING_1);
	if (reg < 0 || reg & WM8958_DSP2CLK_ENA)
		return -EBUSY;

	if (value >= pdata->num_vss_hpf_cfgs)
		return -EINVAL;

	wm8994->vss_hpf_cfg = value;

	return 0;
}

static int wm8958_get_vss_hpf_enum(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.enumerated.item[0] = wm8994->vss_hpf_cfg;

	return 0;
}

static int wm8958_vss_info(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int wm8958_vss_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int vss = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = wm8994->vss_ena[vss];

	return 0;
}

static int wm8958_vss_put(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int vss = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (wm8994->vss_ena[vss] == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0] > 1)
		return -EINVAL;

	if (!wm8994->mbc_vss)
		return -ENODEV;

	if (wm8958_dsp2_busy(wm8994, vss)) {
		dev_dbg(codec->dev, "DSP2 active on %d already\n", vss);
		return -EBUSY;
	}

	if (wm8994->enh_eq_ena[vss])
		return -EBUSY;

	wm8994->vss_ena[vss] = ucontrol->value.integer.value[0];

	wm8958_dsp_apply(codec, vss, wm8994->vss_ena[vss]);

	return 0;
}


#define WM8958_VSS_SWITCH(xname, xval) {\
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.info = wm8958_vss_info, \
	.get = wm8958_vss_get, .put = wm8958_vss_put, \
	.private_value = xval }

static int wm8958_hpf_info(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int wm8958_hpf_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int hpf = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (hpf < 3)
		ucontrol->value.integer.value[0] = wm8994->hpf1_ena[hpf % 3];
	else
		ucontrol->value.integer.value[0] = wm8994->hpf2_ena[hpf % 3];

	return 0;
}

static int wm8958_hpf_put(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int hpf = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (hpf < 3) {
		if (wm8994->hpf1_ena[hpf % 3] ==
		    ucontrol->value.integer.value[0])
			return 0;
	} else {
		if (wm8994->hpf2_ena[hpf % 3] ==
		    ucontrol->value.integer.value[0])
			return 0;
	}

	if (ucontrol->value.integer.value[0] > 1)
		return -EINVAL;

	if (!wm8994->mbc_vss)
		return -ENODEV;

	if (wm8958_dsp2_busy(wm8994, hpf % 3)) {
		dev_dbg(codec->dev, "DSP2 active on %d already\n", hpf);
		return -EBUSY;
	}

	if (wm8994->enh_eq_ena[hpf % 3])
		return -EBUSY;

	if (hpf < 3)
		wm8994->hpf1_ena[hpf % 3] = ucontrol->value.integer.value[0];
	else
		wm8994->hpf2_ena[hpf % 3] = ucontrol->value.integer.value[0];

	wm8958_dsp_apply(codec, hpf % 3, ucontrol->value.integer.value[0]);

	return 0;
}

#define WM8958_HPF_SWITCH(xname, xval) {\
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.info = wm8958_hpf_info, \
	.get = wm8958_hpf_get, .put = wm8958_hpf_put, \
	.private_value = xval }

static int wm8958_put_enh_eq_enum(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int value = ucontrol->value.integer.value[0];
	int reg;

	
	reg = snd_soc_read(codec, WM8994_CLOCKING_1);
	if (reg < 0 || reg & WM8958_DSP2CLK_ENA)
		return -EBUSY;

	if (value >= pdata->num_enh_eq_cfgs)
		return -EINVAL;

	wm8994->enh_eq_cfg = value;

	return 0;
}

static int wm8958_get_enh_eq_enum(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.enumerated.item[0] = wm8994->enh_eq_cfg;

	return 0;
}

static int wm8958_enh_eq_info(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int wm8958_enh_eq_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int eq = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = wm8994->enh_eq_ena[eq];

	return 0;
}

static int wm8958_enh_eq_put(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	int eq = kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (wm8994->enh_eq_ena[eq] == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0] > 1)
		return -EINVAL;

	if (!wm8994->enh_eq)
		return -ENODEV;

	if (wm8958_dsp2_busy(wm8994, eq)) {
		dev_dbg(codec->dev, "DSP2 active on %d already\n", eq);
		return -EBUSY;
	}

	if (wm8994->mbc_ena[eq] || wm8994->vss_ena[eq] ||
	    wm8994->hpf1_ena[eq] || wm8994->hpf2_ena[eq])
		return -EBUSY;

	wm8994->enh_eq_ena[eq] = ucontrol->value.integer.value[0];

	wm8958_dsp_apply(codec, eq, ucontrol->value.integer.value[0]);

	return 0;
}

#define WM8958_ENH_EQ_SWITCH(xname, xval) {\
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.info = wm8958_enh_eq_info, \
	.get = wm8958_enh_eq_get, .put = wm8958_enh_eq_put, \
	.private_value = xval }

static const struct snd_kcontrol_new wm8958_mbc_snd_controls[] = {
WM8958_MBC_SWITCH("AIF1DAC1 MBC Switch", 0),
WM8958_MBC_SWITCH("AIF1DAC2 MBC Switch", 1),
WM8958_MBC_SWITCH("AIF2DAC MBC Switch", 2),
};

static const struct snd_kcontrol_new wm8958_vss_snd_controls[] = {
WM8958_VSS_SWITCH("AIF1DAC1 VSS Switch", 0),
WM8958_VSS_SWITCH("AIF1DAC2 VSS Switch", 1),
WM8958_VSS_SWITCH("AIF2DAC VSS Switch", 2),
WM8958_HPF_SWITCH("AIF1DAC1 HPF1 Switch", 0),
WM8958_HPF_SWITCH("AIF1DAC2 HPF1 Switch", 1),
WM8958_HPF_SWITCH("AIF2DAC HPF1 Switch", 2),
WM8958_HPF_SWITCH("AIF1DAC1 HPF2 Switch", 3),
WM8958_HPF_SWITCH("AIF1DAC2 HPF2 Switch", 4),
WM8958_HPF_SWITCH("AIF2DAC HPF2 Switch", 5),
};

static const struct snd_kcontrol_new wm8958_enh_eq_snd_controls[] = {
WM8958_ENH_EQ_SWITCH("AIF1DAC1 Enhanced EQ Switch", 0),
WM8958_ENH_EQ_SWITCH("AIF1DAC2 Enhanced EQ Switch", 1),
WM8958_ENH_EQ_SWITCH("AIF2DAC Enhanced EQ Switch", 2),
};

static void wm8958_enh_eq_loaded(const struct firmware *fw, void *context)
{
	struct snd_soc_codec *codec = context;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (fw && (wm8958_dsp2_fw(codec, "ENH_EQ", fw, true) == 0)) {
		mutex_lock(&codec->mutex);
		wm8994->enh_eq = fw;
		mutex_unlock(&codec->mutex);
	}
}

static void wm8958_mbc_vss_loaded(const struct firmware *fw, void *context)
{
	struct snd_soc_codec *codec = context;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (fw && (wm8958_dsp2_fw(codec, "MBC+VSS", fw, true) == 0)) {
		mutex_lock(&codec->mutex);
		wm8994->mbc_vss = fw;
		mutex_unlock(&codec->mutex);
	}

	request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
				"wm8958_enh_eq.wfw", codec->dev, GFP_KERNEL,
				codec, wm8958_enh_eq_loaded);
}

static void wm8958_mbc_loaded(const struct firmware *fw, void *context)
{
	struct snd_soc_codec *codec = context;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	if (wm8958_dsp2_fw(codec, "MBC", fw, true) != 0)
		return;

	mutex_lock(&codec->mutex);
	wm8994->mbc = fw;
	mutex_unlock(&codec->mutex);

	request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
				"wm8958_mbc_vss.wfw", codec->dev, GFP_KERNEL,
				codec, wm8958_mbc_vss_loaded);
}

void wm8958_dsp2_init(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int ret, i;

	wm8994->dsp_active = -1;

	snd_soc_add_codec_controls(codec, wm8958_mbc_snd_controls,
			     ARRAY_SIZE(wm8958_mbc_snd_controls));
	snd_soc_add_codec_controls(codec, wm8958_vss_snd_controls,
			     ARRAY_SIZE(wm8958_vss_snd_controls));
	snd_soc_add_codec_controls(codec, wm8958_enh_eq_snd_controls,
			     ARRAY_SIZE(wm8958_enh_eq_snd_controls));


	
	request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
				"wm8958_mbc.wfw", codec->dev, GFP_KERNEL,
				codec, wm8958_mbc_loaded);

	if (!pdata)
		return;

	if (pdata->num_mbc_cfgs) {
		struct snd_kcontrol_new control[] = {
			SOC_ENUM_EXT("MBC Mode", wm8994->mbc_enum,
				     wm8958_get_mbc_enum, wm8958_put_mbc_enum),
		};

		
		wm8994->mbc_texts = kmalloc(sizeof(char *)
					    * pdata->num_mbc_cfgs, GFP_KERNEL);
		if (!wm8994->mbc_texts) {
			dev_err(wm8994->codec->dev,
				"Failed to allocate %d MBC config texts\n",
				pdata->num_mbc_cfgs);
			return;
		}

		for (i = 0; i < pdata->num_mbc_cfgs; i++)
			wm8994->mbc_texts[i] = pdata->mbc_cfgs[i].name;

		wm8994->mbc_enum.max = pdata->num_mbc_cfgs;
		wm8994->mbc_enum.texts = wm8994->mbc_texts;

		ret = snd_soc_add_codec_controls(wm8994->codec, control, 1);
		if (ret != 0)
			dev_err(wm8994->codec->dev,
				"Failed to add MBC mode controls: %d\n", ret);
	}

	if (pdata->num_vss_cfgs) {
		struct snd_kcontrol_new control[] = {
			SOC_ENUM_EXT("VSS Mode", wm8994->vss_enum,
				     wm8958_get_vss_enum, wm8958_put_vss_enum),
		};

		
		wm8994->vss_texts = kmalloc(sizeof(char *)
					    * pdata->num_vss_cfgs, GFP_KERNEL);
		if (!wm8994->vss_texts) {
			dev_err(wm8994->codec->dev,
				"Failed to allocate %d VSS config texts\n",
				pdata->num_vss_cfgs);
			return;
		}

		for (i = 0; i < pdata->num_vss_cfgs; i++)
			wm8994->vss_texts[i] = pdata->vss_cfgs[i].name;

		wm8994->vss_enum.max = pdata->num_vss_cfgs;
		wm8994->vss_enum.texts = wm8994->vss_texts;

		ret = snd_soc_add_codec_controls(wm8994->codec, control, 1);
		if (ret != 0)
			dev_err(wm8994->codec->dev,
				"Failed to add VSS mode controls: %d\n", ret);
	}

	if (pdata->num_vss_hpf_cfgs) {
		struct snd_kcontrol_new control[] = {
			SOC_ENUM_EXT("VSS HPF Mode", wm8994->vss_hpf_enum,
				     wm8958_get_vss_hpf_enum,
				     wm8958_put_vss_hpf_enum),
		};

		
		wm8994->vss_hpf_texts = kmalloc(sizeof(char *)
						* pdata->num_vss_hpf_cfgs, GFP_KERNEL);
		if (!wm8994->vss_hpf_texts) {
			dev_err(wm8994->codec->dev,
				"Failed to allocate %d VSS HPF config texts\n",
				pdata->num_vss_hpf_cfgs);
			return;
		}

		for (i = 0; i < pdata->num_vss_hpf_cfgs; i++)
			wm8994->vss_hpf_texts[i] = pdata->vss_hpf_cfgs[i].name;

		wm8994->vss_hpf_enum.max = pdata->num_vss_hpf_cfgs;
		wm8994->vss_hpf_enum.texts = wm8994->vss_hpf_texts;

		ret = snd_soc_add_codec_controls(wm8994->codec, control, 1);
		if (ret != 0)
			dev_err(wm8994->codec->dev,
				"Failed to add VSS HPFmode controls: %d\n",
				ret);
	}

	if (pdata->num_enh_eq_cfgs) {
		struct snd_kcontrol_new control[] = {
			SOC_ENUM_EXT("Enhanced EQ Mode", wm8994->enh_eq_enum,
				     wm8958_get_enh_eq_enum,
				     wm8958_put_enh_eq_enum),
		};

		
		wm8994->enh_eq_texts = kmalloc(sizeof(char *)
						* pdata->num_enh_eq_cfgs, GFP_KERNEL);
		if (!wm8994->enh_eq_texts) {
			dev_err(wm8994->codec->dev,
				"Failed to allocate %d enhanced EQ config texts\n",
				pdata->num_enh_eq_cfgs);
			return;
		}

		for (i = 0; i < pdata->num_enh_eq_cfgs; i++)
			wm8994->enh_eq_texts[i] = pdata->enh_eq_cfgs[i].name;

		wm8994->enh_eq_enum.max = pdata->num_enh_eq_cfgs;
		wm8994->enh_eq_enum.texts = wm8994->enh_eq_texts;

		ret = snd_soc_add_codec_controls(wm8994->codec, control, 1);
		if (ret != 0)
			dev_err(wm8994->codec->dev,
				"Failed to add enhanced EQ controls: %d\n",
				ret);
	}
}