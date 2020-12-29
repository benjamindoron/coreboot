/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <bootblock_common.h>
#include <delay.h>
#include <gpio.h>
#include "gpio.h"

#define DGPU_PRESENT	GPP_A20	/* Active low */
#define DGPU_HOLD_RST	GPP_B4	/* Active low */
#define DGPU_PWR_EN	GPP_B21	/* Active low */

static void dgpu_power_on(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
	if (!gpio_get(DGPU_PRESENT)) {
		gpio_set(DGPU_HOLD_RST, 0);	// Assert dGPU_HOLD_RST#
		mdelay(2);
		gpio_set(DGPU_PWR_EN, 0);	// Assert dGPU_PWR_EN#
		mdelay(7);
		gpio_set(DGPU_HOLD_RST, 1);	// Deassert dGPU_HOLD_RST#
		mdelay(30);
	} else {
		gpio_set(DGPU_HOLD_RST, 0);	// Assert dGPU_HOLD_RST#
		gpio_set(DGPU_PWR_EN, 1);	// Deassert dGPU_PWR_EN#
	}
}

/* TODO: Continue EC init from PeiOemModule (not KbcPeim?) and SIO from PeiOemSioInit RE
 * (SIO init not necessary - only conditionally executed; not executed on system */
static void ec_init(void)
{
	outb(0x5A, 0x6C);	// 6Ch is the EC legacy IO port
	/* FIXME: Functions in PeiOemModule continue from here only on S3 resume, checking
	 * the battery page and executing for pages 0x2 and 0x3 - manufacturer and model.
	 * We do paging differently, so this may be unnecessary */
}

void bootblock_mainboard_init(void)
{
	ec_init();
	dgpu_power_on();
}
