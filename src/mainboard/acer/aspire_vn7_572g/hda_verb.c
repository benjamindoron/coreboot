/* SPDX-License-Identifier: GPL-2.0-or-later */

/* TODO: Convert to macros */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* --- Codec #0 --- */
	/* coreboot specific header */
	0x10ec0255,	/* Codec Vendor / Device ID: Realtek ALC255 */
	0x10251036,	/* Subsystem ID */
	20,		/* Number of jacks (NID entries) */

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x10251036),
	/* Reset Codec */
	AZALIA_RESET(0x1),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x12, 0x411111c0),
	AZALIA_PIN_CFG(0, 0x14, 0x90172120),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40700001),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x02211030),

	/* Undocumented settings in vendor firmware */
	0x02050038,
	0x02048981,
	0x02050045,
	0x0204c489,

	0x02050037,
	0x02044a05,
	0x05750003,
	0x057486a6,

	0x02050046,
	0x02040004,
	0x0205001b,
	0x02040a0b,

	0x02050008,
	0x02046a0c,
	0x02050009,
	0x0204e003,

	0x0205000a,
	0x02047770,
	0x02050040,
	0x02049800,

	0x02050010,
	0x02040e20,
	0x0205000d,
	0x02042801,

	0x0143b000,
	0x0143b000,
	0x01470740,
	0x01470740,

	0x01470740,
	0x01470740,
	0x02050010,
	0x02040f20,

	/* --- Codec #2 --- */
	/* coreboot specific header */
	0x80862809,	/* Codec Vendor / Device ID: Intel Skylake HDMI */
	0x80860101,	/* Subsystem ID */
	5,		/* Number of jacks (NID entries) */

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* Enable the third converter and pin first (NID 08h) */
	0x00878101,
	0x00878101,
	0x00878101,
	0x00878101,

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),

	/* Disable the third converter and third pin (NID 08h) */
	0x00878100,
	0x00878100,
	0x00878100,
	0x00878100,
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
