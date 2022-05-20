/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>
#include <variant/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_silicon_init_params(params);
}

static void mainboard_init(void *chip_info)
{
	variant_config_stage_gpios();
	variant_ec_init();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
