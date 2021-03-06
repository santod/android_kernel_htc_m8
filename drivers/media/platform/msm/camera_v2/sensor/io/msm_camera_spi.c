/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <mach/camera2.h>
#include "msm_camera_spi.h"

#undef SPIDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define SPIDBG(fmt, args...) pr_debug(fmt, ##args)
#define S_I2C_DBG(fmt, args...) pr_debug(fmt, ##args)
#else
#define SPIDBG(fmt, args...) do { } while (0)
#define S_I2C_DBG(fmt, args...) do { } while (0)
#endif

static int msm_camera_spi_txfr(struct spi_device *spi, char *txbuf,
			       char *rxbuf, int num_byte)
{
	struct spi_transfer t;
	struct spi_message m;

	memset(&t, 0, sizeof(t));
	t.tx_buf = txbuf;
	t.rx_buf = rxbuf;
	t.len = num_byte;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	return spi_sync(spi, &m);
}

static void msm_camera_set_addr(uint32_t addr, uint8_t addr_len,
				enum msm_camera_i2c_reg_addr_type type,
				char *str)
{
	int i, len;

	if (addr_len < type)
		SPIDBG("%s: omitting higher bits in address\n", __func__);

	
	len = addr_len - type;
	for (i = len; i < addr_len; i++) {
		if (i >= 0)
			str[i] = (addr >> (BITS_PER_BYTE * (addr_len - i - 1)))
				& 0xFF;
	}

}

int32_t msm_camera_spi_tx_helper(struct msm_camera_i2c_client *client,
	struct msm_camera_spi_inst *inst, uint32_t addr, uint8_t *data,
	uint32_t num_byte, char *tx, char *rx)
{
	int32_t rc = -EINVAL;
	struct spi_device *spi = client->spi_client->spi_master;
	char *ctx = NULL, *crx = NULL;
	uint32_t len, hlen;
	uint8_t retries = client->spi_client->retries;

	if ((client->addr_type != MSM_CAMERA_I2C_BYTE_ADDR)
	    && (client->addr_type != MSM_CAMERA_I2C_WORD_ADDR)
	    && (client->addr_type != MSM_CAMERA_I2C_3B_ADDR))
		return rc;

	hlen = msm_camera_spi_get_hlen(inst);
	len = hlen + num_byte;

	if (tx)
		ctx = tx;
	else
		ctx = kzalloc(len, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	if (num_byte) {
		if (rx)
			crx = rx;
		else
			crx = kzalloc(len, GFP_KERNEL);
		if (!crx) {
			if (!tx)
				kfree(ctx);
			return -ENOMEM;
		}
	} else {
		crx = NULL;
	}

	ctx[0] = inst->opcode;
	msm_camera_set_addr(addr, inst->addr_len, client->addr_type, ctx + 1);
	while ((rc = msm_camera_spi_txfr(spi, ctx, crx, len)) && retries) {
		retries--;
		msleep(client->spi_client->retry_delay);
	}
	if (rc < 0) {
		SPIDBG("%s: failed %d\n", __func__, rc);
		goto out;
	}
	if (data && num_byte && !rx)
		memcpy(data, crx + hlen, num_byte);

out:
	if (!tx)
		kfree(ctx);
	if (!rx)
		kfree(crx);
	return rc;
}

int32_t msm_camera_spi_read(struct msm_camera_i2c_client *client,
	uint32_t addr, uint16_t *data,
	enum msm_camera_i2c_data_type data_type)
{
	int32_t rc = -EINVAL;
	uint8_t temp[2];

	if ((data_type != MSM_CAMERA_I2C_BYTE_DATA)
	    && (data_type != MSM_CAMERA_I2C_WORD_DATA))
		return rc;

	rc = msm_camera_spi_tx_helper(client,
			&client->spi_client->cmd_tbl.read, addr, &temp[0],
			data_type, NULL, NULL);
	if (rc < 0)
		return rc;

	if (data_type == MSM_CAMERA_I2C_BYTE_DATA)
		*data = temp[0];
	else
		*data = (temp[0] << BITS_PER_BYTE) | temp[1];

	SPIDBG("%s: addr 0x%x, data %u\n", __func__, addr, *data);
	return rc;
}

int32_t msm_camera_spi_read_seq(struct msm_camera_i2c_client *client,
	uint32_t addr, uint8_t *data, uint32_t num_byte)
{
	return msm_camera_spi_tx_helper(client,
		&client->spi_client->cmd_tbl.read_seq, addr, data, num_byte,
		NULL, NULL);
}

int32_t msm_camera_spi_read_seq_l(struct msm_camera_i2c_client *client,
	uint32_t addr, uint32_t num_byte, char *tx, char *rx)
{
	return msm_camera_spi_tx_helper(client,
		&client->spi_client->cmd_tbl.read_seq, addr, NULL, num_byte,
		tx, rx);
}

int32_t msm_camera_spi_query_id(struct msm_camera_i2c_client *client,
	uint32_t addr, uint8_t *data, uint32_t num_byte)
{
	return msm_camera_spi_tx_helper(client,
		&client->spi_client->cmd_tbl.query_id, addr, data, num_byte,
		NULL, NULL);
}
