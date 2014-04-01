#ifndef __ASM_SH7377_H__
#define __ASM_SH7377_H__

enum {
	
	GPIO_PORT0, GPIO_PORT1, GPIO_PORT2, GPIO_PORT3, GPIO_PORT4,
	GPIO_PORT5, GPIO_PORT6, GPIO_PORT7, GPIO_PORT8, GPIO_PORT9,

	GPIO_PORT10, GPIO_PORT11, GPIO_PORT12, GPIO_PORT13, GPIO_PORT14,
	GPIO_PORT15, GPIO_PORT16, GPIO_PORT17, GPIO_PORT18, GPIO_PORT19,

	GPIO_PORT20, GPIO_PORT21, GPIO_PORT22, GPIO_PORT23, GPIO_PORT24,
	GPIO_PORT25, GPIO_PORT26, GPIO_PORT27, GPIO_PORT28, GPIO_PORT29,

	GPIO_PORT30, GPIO_PORT31, GPIO_PORT32, GPIO_PORT33, GPIO_PORT34,
	GPIO_PORT35, GPIO_PORT36, GPIO_PORT37, GPIO_PORT38, GPIO_PORT39,

	GPIO_PORT40, GPIO_PORT41, GPIO_PORT42, GPIO_PORT43, GPIO_PORT44,
	GPIO_PORT45, GPIO_PORT46, GPIO_PORT47, GPIO_PORT48, GPIO_PORT49,

	GPIO_PORT50, GPIO_PORT51, GPIO_PORT52, GPIO_PORT53, GPIO_PORT54,
	GPIO_PORT55, GPIO_PORT56, GPIO_PORT57, GPIO_PORT58, GPIO_PORT59,

	GPIO_PORT60, GPIO_PORT61, GPIO_PORT62, GPIO_PORT63, GPIO_PORT64,
	GPIO_PORT65, GPIO_PORT66, GPIO_PORT67, GPIO_PORT68, GPIO_PORT69,

	GPIO_PORT70, GPIO_PORT71, GPIO_PORT72, GPIO_PORT73, GPIO_PORT74,
	GPIO_PORT75, GPIO_PORT76, GPIO_PORT77, GPIO_PORT78, GPIO_PORT79,

	GPIO_PORT80, GPIO_PORT81, GPIO_PORT82, GPIO_PORT83, GPIO_PORT84,
	GPIO_PORT85, GPIO_PORT86, GPIO_PORT87, GPIO_PORT88, GPIO_PORT89,

	GPIO_PORT90, GPIO_PORT91, GPIO_PORT92, GPIO_PORT93, GPIO_PORT94,
	GPIO_PORT95, GPIO_PORT96, GPIO_PORT97, GPIO_PORT98, GPIO_PORT99,

	GPIO_PORT100, GPIO_PORT101, GPIO_PORT102, GPIO_PORT103, GPIO_PORT104,
	GPIO_PORT105, GPIO_PORT106, GPIO_PORT107, GPIO_PORT108, GPIO_PORT109,

	GPIO_PORT110, GPIO_PORT111, GPIO_PORT112, GPIO_PORT113, GPIO_PORT114,
	GPIO_PORT115, GPIO_PORT116, GPIO_PORT117, GPIO_PORT118,

	GPIO_PORT128, GPIO_PORT129,

	GPIO_PORT130, GPIO_PORT131, GPIO_PORT132, GPIO_PORT133, GPIO_PORT134,
	GPIO_PORT135, GPIO_PORT136, GPIO_PORT137, GPIO_PORT138, GPIO_PORT139,

	GPIO_PORT140, GPIO_PORT141, GPIO_PORT142, GPIO_PORT143, GPIO_PORT144,
	GPIO_PORT145, GPIO_PORT146, GPIO_PORT147, GPIO_PORT148, GPIO_PORT149,

	GPIO_PORT150, GPIO_PORT151, GPIO_PORT152, GPIO_PORT153, GPIO_PORT154,
	GPIO_PORT155, GPIO_PORT156, GPIO_PORT157, GPIO_PORT158, GPIO_PORT159,

	GPIO_PORT160, GPIO_PORT161, GPIO_PORT162, GPIO_PORT163, GPIO_PORT164,

	GPIO_PORT192, GPIO_PORT193, GPIO_PORT194,
	GPIO_PORT195, GPIO_PORT196, GPIO_PORT197, GPIO_PORT198, GPIO_PORT199,

	GPIO_PORT200, GPIO_PORT201, GPIO_PORT202, GPIO_PORT203, GPIO_PORT204,
	GPIO_PORT205, GPIO_PORT206, GPIO_PORT207, GPIO_PORT208, GPIO_PORT209,

	GPIO_PORT210, GPIO_PORT211, GPIO_PORT212, GPIO_PORT213, GPIO_PORT214,
	GPIO_PORT215, GPIO_PORT216, GPIO_PORT217, GPIO_PORT218, GPIO_PORT219,

	GPIO_PORT220, GPIO_PORT221, GPIO_PORT222, GPIO_PORT223, GPIO_PORT224,
	GPIO_PORT225, GPIO_PORT226, GPIO_PORT227, GPIO_PORT228, GPIO_PORT229,

	GPIO_PORT230, GPIO_PORT231, GPIO_PORT232, GPIO_PORT233, GPIO_PORT234,
	GPIO_PORT235, GPIO_PORT236, GPIO_PORT237, GPIO_PORT238, GPIO_PORT239,

	GPIO_PORT240, GPIO_PORT241, GPIO_PORT242, GPIO_PORT243, GPIO_PORT244,
	GPIO_PORT245, GPIO_PORT246, GPIO_PORT247, GPIO_PORT248, GPIO_PORT249,

	GPIO_PORT250, GPIO_PORT251, GPIO_PORT252, GPIO_PORT253, GPIO_PORT254,
	GPIO_PORT255, GPIO_PORT256, GPIO_PORT257, GPIO_PORT258, GPIO_PORT259,

	GPIO_PORT260, GPIO_PORT261, GPIO_PORT262, GPIO_PORT263, GPIO_PORT264,

	
	GPIO_FN_PORT66_KEYIN0_PU, GPIO_FN_PORT67_KEYIN1_PU,
	GPIO_FN_PORT68_KEYIN2_PU, GPIO_FN_PORT69_KEYIN3_PU,
	GPIO_FN_PORT70_KEYIN4_PU, GPIO_FN_PORT71_KEYIN5_PU,
	GPIO_FN_PORT72_KEYIN6_PU,

	
	GPIO_FN_VBUS_0,
	GPIO_FN_CPORT0,
	GPIO_FN_CPORT1,
	GPIO_FN_CPORT2,
	GPIO_FN_CPORT3,
	GPIO_FN_CPORT4,
	GPIO_FN_CPORT5,
	GPIO_FN_CPORT6,
	GPIO_FN_CPORT7,
	GPIO_FN_CPORT8,
	GPIO_FN_CPORT9,
	GPIO_FN_CPORT10,
	GPIO_FN_CPORT11, GPIO_FN_SIN2,
	GPIO_FN_CPORT12, GPIO_FN_XCTS2,
	GPIO_FN_CPORT13, GPIO_FN_RFSPO4,
	GPIO_FN_CPORT14, GPIO_FN_RFSPO5,
	GPIO_FN_CPORT15, GPIO_FN_SCIFA0_SCK, GPIO_FN_GPS_AGC2,
	GPIO_FN_CPORT16, GPIO_FN_SCIFA0_TXD, GPIO_FN_GPS_AGC3,
	GPIO_FN_CPORT17_IC_OE, GPIO_FN_SOUT2,
	GPIO_FN_CPORT18, GPIO_FN_XRTS2, GPIO_FN_PORT19_VIO_CKO2,
	GPIO_FN_CPORT19_MPORT1,
	GPIO_FN_CPORT20, GPIO_FN_RFSPO6,
	GPIO_FN_CPORT21, GPIO_FN_STATUS0,
	GPIO_FN_CPORT22, GPIO_FN_STATUS1,
	GPIO_FN_CPORT23, GPIO_FN_STATUS2, GPIO_FN_RFSPO7,
	GPIO_FN_B_SYNLD1,
	GPIO_FN_B_SYNLD2, GPIO_FN_SYSENMSK,
	GPIO_FN_XMAINPS,
	GPIO_FN_XDIVPS,
	GPIO_FN_XIDRST,
	GPIO_FN_IDCLK, GPIO_FN_IC_DP,
	GPIO_FN_IDIO, GPIO_FN_IC_DM,
	GPIO_FN_SOUT1, GPIO_FN_SCIFA4_TXD, GPIO_FN_M02_BERDAT,
	GPIO_FN_SIN1, GPIO_FN_SCIFA4_RXD, GPIO_FN_XWUP,
	GPIO_FN_XRTS1, GPIO_FN_SCIFA4_RTS, GPIO_FN_M03_BERCLK,
	GPIO_FN_XCTS1, GPIO_FN_SCIFA4_CTS,
	GPIO_FN_PCMCLKO,
	GPIO_FN_SYNC8KO,

	
	GPIO_FN_DNPCM_A,
	GPIO_FN_UPPCM_A,
	GPIO_FN_VACK,
	GPIO_FN_XTALB1L,
	GPIO_FN_GPS_AGC1, GPIO_FN_SCIFA0_RTS,
	GPIO_FN_GPS_AGC4, GPIO_FN_SCIFA0_RXD,
	GPIO_FN_GPS_PWRDOWN, GPIO_FN_SCIFA0_CTS,
	GPIO_FN_GPS_IM,
	GPIO_FN_GPS_IS,
	GPIO_FN_GPS_QM,
	GPIO_FN_GPS_QS,
	GPIO_FN_FMSOCK, GPIO_FN_PORT49_IRDA_OUT, GPIO_FN_PORT49_IROUT,
	GPIO_FN_FMSOOLR, GPIO_FN_BBIF2_TSYNC2, GPIO_FN_TPU2TO2, GPIO_FN_IPORT3,
	GPIO_FN_FMSIOLR,
	GPIO_FN_FMSOOBT, GPIO_FN_BBIF2_TSCK2, GPIO_FN_TPU2TO3, GPIO_FN_OPORT1,
	GPIO_FN_FMSIOBT,
	GPIO_FN_FMSOSLD, GPIO_FN_BBIF2_TXD2, GPIO_FN_OPORT2,
	GPIO_FN_FMSOILR, GPIO_FN_PORT53_IRDA_IN, GPIO_FN_TPU3TO3,
	GPIO_FN_OPORT3, GPIO_FN_FMSIILR,
	GPIO_FN_FMSOIBT, GPIO_FN_PORT54_IRDA_FIRSEL, GPIO_FN_TPU3TO2,
	GPIO_FN_FMSIIBT,
	GPIO_FN_FMSISLD, GPIO_FN_MFG0_OUT1, GPIO_FN_TPU0TO0,
	GPIO_FN_A0_EA0, GPIO_FN_BS,
	GPIO_FN_A12_EA12, GPIO_FN_PORT58_VIO_CKOR, GPIO_FN_TPU4TO2,
	GPIO_FN_A13_EA13, GPIO_FN_PORT59_IROUT, GPIO_FN_MFG0_OUT2,
	GPIO_FN_TPU0TO1,
	GPIO_FN_A14_EA14, GPIO_FN_PORT60_KEYOUT5,
	GPIO_FN_A15_EA15, GPIO_FN_PORT61_KEYOUT4,
	GPIO_FN_A16_EA16, GPIO_FN_PORT62_KEYOUT3, GPIO_FN_MSIOF0_SS1,
	GPIO_FN_A17_EA17, GPIO_FN_PORT63_KEYOUT2, GPIO_FN_MSIOF0_TSYNC,
	GPIO_FN_A18_EA18, GPIO_FN_PORT64_KEYOUT1, GPIO_FN_MSIOF0_TSCK,
	GPIO_FN_A19_EA19, GPIO_FN_PORT65_KEYOUT0, GPIO_FN_MSIOF0_TXD,
	GPIO_FN_A20_EA20, GPIO_FN_PORT66_KEYIN0, GPIO_FN_MSIOF0_RSCK,
	GPIO_FN_A21_EA21, GPIO_FN_PORT67_KEYIN1, GPIO_FN_MSIOF0_RSYNC,
	GPIO_FN_A22_EA22, GPIO_FN_PORT68_KEYIN2, GPIO_FN_MSIOF0_MCK0,
	GPIO_FN_A23_EA23, GPIO_FN_PORT69_KEYIN3, GPIO_FN_MSIOF0_MCK1,
	GPIO_FN_A24_EA24, GPIO_FN_PORT70_KEYIN4, GPIO_FN_MSIOF0_RXD,
	GPIO_FN_A25_EA25, GPIO_FN_PORT71_KEYIN5, GPIO_FN_MSIOF0_SS2,
	GPIO_FN_A26, GPIO_FN_PORT72_KEYIN6,
	GPIO_FN_D0_ED0_NAF0,
	GPIO_FN_D1_ED1_NAF1,
	GPIO_FN_D2_ED2_NAF2,
	GPIO_FN_D3_ED3_NAF3,
	GPIO_FN_D4_ED4_NAF4,
	GPIO_FN_D5_ED5_NAF5,
	GPIO_FN_D6_ED6_NAF6,
	GPIO_FN_D7_ED7_NAF7,
	GPIO_FN_D8_ED8_NAF8,
	GPIO_FN_D9_ED9_NAF9,
	GPIO_FN_D10_ED10_NAF10,
	GPIO_FN_D11_ED11_NAF11,
	GPIO_FN_D12_ED12_NAF12,
	GPIO_FN_D13_ED13_NAF13,
	GPIO_FN_D14_ED14_NAF14,
	GPIO_FN_D15_ED15_NAF15,
	GPIO_FN_CS4,
	GPIO_FN_CS5A, GPIO_FN_FMSICK,
	GPIO_FN_CS5B, GPIO_FN_FCE1,

	
	GPIO_FN_CS6B, GPIO_FN_XCS2, GPIO_FN_CS6A, GPIO_FN_DACK0,
	GPIO_FN_FCE0,
	GPIO_FN_WAIT, GPIO_FN_DREQ0,
	GPIO_FN_RD_XRD,
	GPIO_FN_WE0_XWR0_FWE,
	GPIO_FN_WE1_XWR1,
	GPIO_FN_FRB,
	GPIO_FN_CKO,
	GPIO_FN_NBRSTOUT,
	GPIO_FN_NBRST,
	GPIO_FN_GPS_EPPSIN,
	GPIO_FN_LATCHPULSE,
	GPIO_FN_LTESIGNAL,
	GPIO_FN_LEGACYSTATE,
	GPIO_FN_TCKON,
	GPIO_FN_VIO_VD, GPIO_FN_PORT128_KEYOUT0, GPIO_FN_IPORT0,
	GPIO_FN_VIO_HD, GPIO_FN_PORT129_KEYOUT1, GPIO_FN_IPORT1,
	GPIO_FN_VIO_D0, GPIO_FN_PORT130_KEYOUT2, GPIO_FN_PORT130_MSIOF2_RXD,
	GPIO_FN_VIO_D1, GPIO_FN_PORT131_KEYOUT3, GPIO_FN_PORT131_MSIOF2_SS1,
	GPIO_FN_VIO_D2, GPIO_FN_PORT132_KEYOUT4, GPIO_FN_PORT132_MSIOF2_SS2,
	GPIO_FN_VIO_D3, GPIO_FN_PORT133_KEYOUT5, GPIO_FN_PORT133_MSIOF2_TSYNC,
	GPIO_FN_VIO_D4, GPIO_FN_PORT134_KEYIN0, GPIO_FN_PORT134_MSIOF2_TXD,
	GPIO_FN_VIO_D5, GPIO_FN_PORT135_KEYIN1, GPIO_FN_PORT135_MSIOF2_TSCK,
	GPIO_FN_VIO_D6, GPIO_FN_PORT136_KEYIN2,
	GPIO_FN_VIO_D7, GPIO_FN_PORT137_KEYIN3,
	GPIO_FN_VIO_D8, GPIO_FN_M9_SLCD_A01, GPIO_FN_PORT138_FSIAOMC,
	GPIO_FN_VIO_D9, GPIO_FN_M10_SLCD_CK1, GPIO_FN_PORT139_FSIAOLR,
	GPIO_FN_VIO_D10, GPIO_FN_M11_SLCD_SO1, GPIO_FN_TPU0TO2,
	GPIO_FN_PORT140_FSIAOBT,
	GPIO_FN_VIO_D11, GPIO_FN_M12_SLCD_CE1, GPIO_FN_TPU0TO3,
	GPIO_FN_PORT141_FSIAOSLD,
	GPIO_FN_VIO_D12, GPIO_FN_M13_BSW, GPIO_FN_PORT142_FSIACK,
	GPIO_FN_VIO_D13, GPIO_FN_M14_GSW, GPIO_FN_PORT143_FSIAILR,
	GPIO_FN_VIO_D14, GPIO_FN_M15_RSW, GPIO_FN_PORT144_FSIAIBT,
	GPIO_FN_VIO_D15, GPIO_FN_TPU1TO3, GPIO_FN_PORT145_FSIAISLD,
	GPIO_FN_VIO_CLK, GPIO_FN_PORT146_KEYIN4, GPIO_FN_IPORT2,
	GPIO_FN_VIO_FIELD, GPIO_FN_PORT147_KEYIN5,
	GPIO_FN_VIO_CKO, GPIO_FN_PORT148_KEYIN6,
	GPIO_FN_A27, GPIO_FN_RDWR_XWE, GPIO_FN_MFG0_IN1,
	GPIO_FN_MFG0_IN2,
	GPIO_FN_TS_SPSYNC3, GPIO_FN_MSIOF2_RSCK,
	GPIO_FN_TS_SDAT3, GPIO_FN_MSIOF2_RSYNC,
	GPIO_FN_TPU1TO2, GPIO_FN_TS_SDEN3, GPIO_FN_PORT153_MSIOF2_SS1,
	GPIO_FN_SOUT3, GPIO_FN_SCIFA2_TXD1, GPIO_FN_MSIOF2_MCK0,
	GPIO_FN_SIN3, GPIO_FN_SCIFA2_RXD1, GPIO_FN_MSIOF2_MCK1,
	GPIO_FN_XRTS3, GPIO_FN_SCIFA2_RTS1, GPIO_FN_PORT156_MSIOF2_SS2,
	GPIO_FN_XCTS3, GPIO_FN_SCIFA2_CTS1, GPIO_FN_PORT157_MSIOF2_RXD,

	
	GPIO_FN_DINT, GPIO_FN_SCIFA2_SCK1, GPIO_FN_TS_SCK3,
	GPIO_FN_PORT159_SCIFB_SCK, GPIO_FN_PORT159_SCIFA5_SCK, GPIO_FN_NMI,
	GPIO_FN_PORT160_SCIFB_TXD, GPIO_FN_PORT160_SCIFA5_TXD, GPIO_FN_SOUT0,
	GPIO_FN_PORT161_SCIFB_CTS, GPIO_FN_PORT161_SCIFA5_CTS, GPIO_FN_XCTS0,
	GPIO_FN_MFG3_IN2,
	GPIO_FN_PORT162_SCIFB_RXD, GPIO_FN_PORT162_SCIFA5_RXD, GPIO_FN_SIN0,
	GPIO_FN_MFG3_IN1,
	GPIO_FN_PORT163_SCIFB_RTS, GPIO_FN_PORT163_SCIFA5_RTS, GPIO_FN_XRTS0,
	GPIO_FN_MFG3_OUT1,
	GPIO_FN_TPU3TO0,
	GPIO_FN_LCDD0, GPIO_FN_PORT192_KEYOUT0, GPIO_FN_EXT_CKI,
	GPIO_FN_LCDD1, GPIO_FN_PORT193_KEYOUT1, GPIO_FN_PORT193_SCIFA5_CTS,
	GPIO_FN_BBIF2_TSYNC1,
	GPIO_FN_LCDD2, GPIO_FN_PORT194_KEYOUT2, GPIO_FN_PORT194_SCIFA5_RTS,
	GPIO_FN_BBIF2_TSCK1,
	GPIO_FN_LCDD3, GPIO_FN_PORT195_KEYOUT3, GPIO_FN_PORT195_SCIFA5_RXD,
	GPIO_FN_BBIF2_TXD1,
	GPIO_FN_LCDD4, GPIO_FN_PORT196_KEYOUT4, GPIO_FN_PORT196_SCIFA5_TXD,
	GPIO_FN_LCDD5, GPIO_FN_PORT197_KEYOUT5, GPIO_FN_PORT197_SCIFA5_SCK,
	GPIO_FN_MFG2_OUT2, GPIO_FN_TPU2TO1,
	GPIO_FN_LCDD6, GPIO_FN_XWR2,
	GPIO_FN_LCDD7, GPIO_FN_TPU4TO1, GPIO_FN_MFG4_OUT2, GPIO_FN_XWR3,
	GPIO_FN_LCDD8, GPIO_FN_PORT200_KEYIN0, GPIO_FN_VIO_DR0, GPIO_FN_D16,
	GPIO_FN_ED16,
	GPIO_FN_LCDD9, GPIO_FN_PORT201_KEYIN1, GPIO_FN_VIO_DR1, GPIO_FN_D17,
	GPIO_FN_ED17,
	GPIO_FN_LCDD10, GPIO_FN_PORT202_KEYIN2, GPIO_FN_VIO_DR2, GPIO_FN_D18,
	GPIO_FN_ED18,
	GPIO_FN_LCDD11, GPIO_FN_PORT203_KEYIN3, GPIO_FN_VIO_DR3, GPIO_FN_D19,
	GPIO_FN_ED19,
	GPIO_FN_LCDD12, GPIO_FN_PORT204_KEYIN4, GPIO_FN_VIO_DR4, GPIO_FN_D20,
	GPIO_FN_ED20,
	GPIO_FN_LCDD13, GPIO_FN_PORT205_KEYIN5, GPIO_FN_VIO_DR5, GPIO_FN_D21,
	GPIO_FN_ED21,
	GPIO_FN_LCDD14, GPIO_FN_PORT206_KEYIN6, GPIO_FN_VIO_DR6, GPIO_FN_D22,
	GPIO_FN_ED22,
	GPIO_FN_LCDD15, GPIO_FN_PORT207_MSIOF0L_SS1, GPIO_FN_PORT207_KEYOUT0,
	GPIO_FN_VIO_DR7,
	GPIO_FN_D23, GPIO_FN_ED23,
	GPIO_FN_LCDD16, GPIO_FN_PORT208_MSIOF0L_SS2, GPIO_FN_PORT208_KEYOUT1,
	GPIO_FN_VIO_VDR,
	GPIO_FN_D24, GPIO_FN_ED24,
	GPIO_FN_LCDD17, GPIO_FN_PORT209_KEYOUT2, GPIO_FN_VIO_HDR, GPIO_FN_D25,
	GPIO_FN_ED25,
	GPIO_FN_LCDD18, GPIO_FN_DREQ2, GPIO_FN_PORT210_MSIOF0L_SS1, GPIO_FN_D26,
	GPIO_FN_ED26,
	GPIO_FN_LCDD19, GPIO_FN_PORT211_MSIOF0L_SS2, GPIO_FN_D27, GPIO_FN_ED27,
	GPIO_FN_LCDD20, GPIO_FN_TS_SPSYNC1, GPIO_FN_MSIOF0L_MCK0, GPIO_FN_D28,
	GPIO_FN_ED28,
	GPIO_FN_LCDD21, GPIO_FN_TS_SDAT1, GPIO_FN_MSIOF0L_MCK1, GPIO_FN_D29,
	GPIO_FN_ED29,
	GPIO_FN_LCDD22, GPIO_FN_TS_SDEN1, GPIO_FN_MSIOF0L_RSCK, GPIO_FN_D30,
	GPIO_FN_ED30,
	GPIO_FN_LCDD23, GPIO_FN_TS_SCK1, GPIO_FN_MSIOF0L_RSYNC, GPIO_FN_D31,
	GPIO_FN_ED31,
	GPIO_FN_LCDDCK, GPIO_FN_LCDWR, GPIO_FN_PORT216_KEYOUT3,
	GPIO_FN_VIO_CLKR,
	GPIO_FN_LCDRD, GPIO_FN_DACK2, GPIO_FN_MSIOF0L_TSYNC,
	GPIO_FN_LCDHSYN, GPIO_FN_LCDCS, GPIO_FN_LCDCS2, GPIO_FN_DACK3,
	GPIO_FN_PORT218_VIO_CKOR, GPIO_FN_PORT218_KEYOUT4,
	GPIO_FN_LCDDISP, GPIO_FN_LCDRS, GPIO_FN_DREQ3, GPIO_FN_MSIOF0L_TSCK,
	GPIO_FN_LCDVSYN, GPIO_FN_LCDVSYN2, GPIO_FN_PORT220_KEYOUT5,
	GPIO_FN_LCDLCLK, GPIO_FN_DREQ1, GPIO_FN_PWEN, GPIO_FN_MSIOF0L_RXD,
	GPIO_FN_LCDDON, GPIO_FN_LCDDON2, GPIO_FN_DACK1, GPIO_FN_OVCN,
	GPIO_FN_MSIOF0L_TXD,
	GPIO_FN_SCIFA1_TXD, GPIO_FN_OVCN2,
	GPIO_FN_EXTLP, GPIO_FN_SCIFA1_SCK, GPIO_FN_USBTERM,
	GPIO_FN_PORT226_VIO_CKO2,
	GPIO_FN_SCIFA1_RTS, GPIO_FN_IDIN,
	GPIO_FN_SCIFA1_RXD,
	GPIO_FN_SCIFA1_CTS, GPIO_FN_MFG1_IN1,
	GPIO_FN_MSIOF1_TXD, GPIO_FN_SCIFA2_TXD2, GPIO_FN_PORT230_FSIAOMC,
	GPIO_FN_MSIOF1_TSYNC, GPIO_FN_SCIFA2_CTS2, GPIO_FN_PORT231_FSIAOLR,
	GPIO_FN_MSIOF1_TSCK, GPIO_FN_SCIFA2_SCK2, GPIO_FN_PORT232_FSIAOBT,
	GPIO_FN_MSIOF1_RXD, GPIO_FN_SCIFA2_RXD2, GPIO_FN_GPS_VCOTRIG,
	GPIO_FN_PORT233_FSIACK,
	GPIO_FN_MSIOF1_RSCK, GPIO_FN_SCIFA2_RTS2, GPIO_FN_PORT234_FSIAOSLD,
	GPIO_FN_MSIOF1_RSYNC, GPIO_FN_OPORT0, GPIO_FN_MFG1_IN2,
	GPIO_FN_PORT235_FSIAILR,
	GPIO_FN_MSIOF1_MCK0, GPIO_FN_I2C_SDA2, GPIO_FN_PORT236_FSIAIBT,
	GPIO_FN_MSIOF1_MCK1, GPIO_FN_I2C_SCL2, GPIO_FN_PORT237_FSIAISLD,
	GPIO_FN_MSIOF1_SS1, GPIO_FN_EDBGREQ3,

	
	GPIO_FN_MSIOF1_SS2,
	GPIO_FN_SCIFA6_TXD,
	GPIO_FN_PORT241_IRDA_OUT, GPIO_FN_PORT241_IROUT, GPIO_FN_MFG4_OUT1,
	GPIO_FN_TPU4TO0,
	GPIO_FN_PORT242_IRDA_IN, GPIO_FN_MFG4_IN2,
	GPIO_FN_PORT243_IRDA_FIRSEL, GPIO_FN_PORT243_VIO_CKO2,
	GPIO_FN_PORT244_SCIFA5_CTS, GPIO_FN_MFG2_IN1, GPIO_FN_PORT244_SCIFB_CTS,
	GPIO_FN_PORT244_MSIOF2_RXD,
	GPIO_FN_PORT245_SCIFA5_RTS, GPIO_FN_MFG2_IN2, GPIO_FN_PORT245_SCIFB_RTS,
	GPIO_FN_PORT245_MSIOF2_TXD,
	GPIO_FN_PORT246_SCIFA5_RXD, GPIO_FN_MFG1_OUT1,
	GPIO_FN_PORT246_SCIFB_RXD, GPIO_FN_TPU1TO0,
	GPIO_FN_PORT247_SCIFA5_TXD, GPIO_FN_MFG3_OUT2,
	GPIO_FN_PORT247_SCIFB_TXD, GPIO_FN_TPU3TO1,
	GPIO_FN_PORT248_SCIFA5_SCK, GPIO_FN_MFG2_OUT1,
	GPIO_FN_PORT248_SCIFB_SCK, GPIO_FN_TPU2TO0,
	GPIO_FN_PORT248_MSIOF2_TSCK,
	GPIO_FN_PORT249_IROUT, GPIO_FN_MFG4_IN1, GPIO_FN_PORT249_MSIOF2_TSYNC,
	GPIO_FN_SDHICLK0, GPIO_FN_TCK2_SWCLK_MC0,
	GPIO_FN_SDHICD0,
	GPIO_FN_SDHID0_0, GPIO_FN_TMS2_SWDIO_MC0,
	GPIO_FN_SDHID0_1, GPIO_FN_TDO2_SWO0_MC0,
	GPIO_FN_SDHID0_2, GPIO_FN_TDI2,
	GPIO_FN_SDHID0_3, GPIO_FN_RTCK2_SWO1_MC0,
	GPIO_FN_SDHICMD0, GPIO_FN_TRST2,
	GPIO_FN_SDHIWP0, GPIO_FN_EDBGREQ2,
	GPIO_FN_SDHICLK1, GPIO_FN_TCK3_SWCLK_MC1,
	GPIO_FN_SDHID1_0, GPIO_FN_M11_SLCD_SO2, GPIO_FN_TS_SPSYNC2,
	GPIO_FN_TMS3_SWDIO_MC1,
	GPIO_FN_SDHID1_1, GPIO_FN_M9_SLCD_A02, GPIO_FN_TS_SDAT2,
	GPIO_FN_TDO3_SWO0_MC1,
	GPIO_FN_SDHID1_2, GPIO_FN_M10_SLCD_CK2, GPIO_FN_TS_SDEN2, GPIO_FN_TDI3,
	GPIO_FN_SDHID1_3, GPIO_FN_M12_SLCD_CE2, GPIO_FN_TS_SCK2,
	GPIO_FN_RTCK3_SWO1_MC1,
	GPIO_FN_SDHICMD1, GPIO_FN_TRST3,
	GPIO_FN_RESETOUTS,
};

#endif 
