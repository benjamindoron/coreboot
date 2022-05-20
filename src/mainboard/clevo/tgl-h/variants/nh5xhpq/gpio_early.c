/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <variant/gpio.h>

/* Early pad configuration */
static const struct pad_config early_gpio_table[] = {
	// UART2_RXD == UART2_RXD
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	// UART2_TXD == UART2_TXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	// SATA_DEVSLP6 -> DGPU_RST#_PCH
	PAD_CFG_GPO(GPP_F8, 0, DEEP),
	// SATA_DEVSLP7 -> DGPU_PWR_EN
	PAD_CFG_GPO(GPP_F9, 0, DEEP),
};

void variant_config_stage_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
