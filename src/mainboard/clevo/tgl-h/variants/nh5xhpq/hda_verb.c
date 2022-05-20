/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Convert to macros */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* --- Card #0, Codec #0 --- */
	/* coreboot specific header */
	0x10ec0256, /* Codec Vendor / Device ID: Realtek ALC256 */
	0x155850e1, /* Subsystem ID */
	18,	    /* Number of jacks (NID entries) */

	/* Codec Address: Bits 31:28 */
	/* Node ID: Bits 27:20 */
	/* Verb ID: Bits 19:8 / Bits 19:16 */
	/* Payload: Bits 7:0  / Bits 15:0 */

	/*
	 * NOTE: See data blob in "PlatformInitAdvancedPostMem" of vendor firmware
	 * - NID 0x20 holds the "Realtek Defined Hidden registers"
	 */
	0x0205001a, /* Set coeff idx: 0x1a */
	0x02048003, /* Set processing coeff: 0x8003 */
	0x0205001a, /* Set coeff idx: 0x1a */
	0x0204c003, /* Set processing coeff: 0xc003 */

	/* NOTE: Do we program subsystem before reset, as vendor does? */
	/* HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x155850e1),
	/* Reset Codec */
	AZALIA_RESET(0x1),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130), /* Internal Microphone */
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), /* Speaker */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, 0x02a11040), /* External Microphone */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x41789c6d),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x02211020), /* Headphone */

	/* "PlatformInitAdvancedPostMem" programs more coefficients */
	0x02050038, /* Set coeff idx: 0x38 */
	0x02047901, /* Set processing coeff: 0x7901 */
	0x02050038, /* Set coeff idx: 0x38 */
	0x02047901, /* Set processing coeff: 0x7901 */

	0x02050008, /* Set coeff idx: 0x8 */
	0x02046a0c, /* Set processing coeff: 0x6a0c */
	0x0205001b, /* Set coeff idx: 0x1b */
	0x02040a4b, /* Set processing coeff: 0xa4b */

	0x0205003c, /* Set coeff idx: 0x3c */
	0x02040354, /* Set processing coeff: 0x354 */
	0x0205003c, /* Set coeff idx: 0x3c */
	0x02040314, /* Set processing coeff: 0x314 */

	0x02050046, /* Set coeff idx: 0x46 */
	0x02040004, /* Set processing coeff: 0x4 */
	0x05750003, /* Set coeff idx on NID 0x57?: 0x3 */
	0x057409a2, /* Set processing coeff on NID 0x57?: 0x9a2 */

	0x02050036, /* Set coeff idx: 0x36 */
	0x02045757, /* Set processing coeff: 0x5757 */
	0x02050036, /* Set coeff idx: 0x36 */
	0x02045757, /* Set processing coeff: 0x5757 */

	/*
	 * TODO: See data blob in "OemHdaPolicy" of vendor firmware
	 * - Approximately offsets 0x20e0-0x212140
	 * - Naturally, this table (DXE's) executes after the first (PEI's)
	 * - Another reset here, so ordering the flow is significant
	 * NOTE: NID 0x20 holds the "Realtek Defined Hidden registers"
	 */

	/* --- Card #0, Codec #2 --- */
	/* coreboot specific header */
	0x80862812, /* Codec Vendor / Device ID: Intel Tigerlake HDMI */
	0x80860101, /* Subsystem ID */
	6,	    /* Number of jacks (NID entries) */

	/* Codec Address: Bits 31:28 */
	/* Node ID: Bits 27:20 */
	/* Verb ID: Bits 19:8 */
	/* Payload: Bits 7:0 */

	/* Enable the third converter and 4 pins (for GEN10+, the Vendor Node ID is 02h) */
	0x00278111,
	/* Repeated to pad NID */
	0x00278111,
	0x00278111,
	0x00278111,

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x58560020),
	AZALIA_PIN_CFG(2, 0x07, 0x58560030),
	AZALIA_PIN_CFG(2, 0x08, 0x58560040),

	/* Disable the third converter and 4 pins (NID 02h) */
	0x00278100,
	/* Repeated to pad NID */
	0x00278100,
	0x00278100,
	0x00278100,

#if 0 // No references found in vendor firmware
	/* --- Card #1, Codec #0 --- */
	/* coreboot specific header */
	0x10de009f,	/* Codec Vendor / Device ID: Nvidia GPU 9f HDMI/DP */
	0x10de0000,	/* Subsystem ID */
	2,		/* Number of jacks (NID entries) */

	/* Codec Address: Bits 31:28 */
	/* Node ID: Bits 27:20 */
	/* Verb ID: Bits 19:8 */
	/* Payload: Bits 7:0 */

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x04, 0x185600f0),
	AZALIA_PIN_CFG(2, 0x05, 0x185600f0),
#endif
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
