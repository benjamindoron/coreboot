/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <variant/bootblock.h>
#include <variant/gpio.h>

void bootblock_mainboard_init(void)
{
	variant_dgpu_power_on();
}

void bootblock_mainboard_early_init(void)
{
	variant_config_stage_gpios();
	variant_ec_init_script();
}
