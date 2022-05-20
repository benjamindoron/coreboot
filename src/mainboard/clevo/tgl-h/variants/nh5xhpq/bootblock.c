/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <variant/bootblock.h>

#define DGPU_RSTN_PCH GPP_F8 /* Active low */
#define DGPU_PWR_EN GPP_F9   /* Active high */

void variant_ec_init_script(void)
{
	/* H2RAMPei:H2RAMPeiEntrypoint(): Play EC init script.
	   - TODO: Determine which registers this writes, precisely,
	     and factor into a PNP wrapper or consume a Clevo EC driver.
	   - Modifications: Stop selecting the same LDN each time. */
	// Recall, arguments: Data, Port
	uint8_t data;

	// `pnp_enter_conf_mode_870155aa()`
	outb(0x87, 0x2E);
	outb(0x01, 0x2E);
	outb(0x55, 0x2E);
	outb(0x55, 0x2E);

	// `pnp_set_logical_device()` to 0x0F
	outb(0x07, 0x2E);
	outb(0x0F, 0x2F);

	// `pnp_write_config(0xF0, 0xFE)`
	outb(0xF0, 0x2E);
	outb(0xFE, 0x2F);

	// `pnp_write_config(0xF1, 0x0B)`
	outb(0xF1, 0x2E);
	outb(0x0B, 0x2F);

	// `pnp_write_config(0xF6, 0x0B)`
	outb(0xF6, 0x2E);
	outb(0x0B, 0x2F);

	// `pnp_write_config(0xF5, 0x00)`
	outb(0xF5, 0x2E);
	outb(0x00, 0x2F);

	// `pnp_write_config(0xFC, 0x00)`
	outb(0xFC, 0x2E);
	outb(0x00, 0x2F);

	// `pnp_exit_conf_mode_0202()`
	outb(0x02, 0x2E);
	outb(0x02, 0x2F);

	// TODO: Init script continues out of conf mode. Some repetition?
	outb(0x2E, 0x2E);
	outb(0x11, 0x2F);
	outb(0x2F, 0x2E);
	outb(0x10, 0x2F);
	outb(0x2E, 0x2E);
	outb(0x10, 0x2F);
	outb(0x2F, 0x2E);
	outb(0x5A, 0x2F);
	outb(0x2E, 0x2E);
	outb(0x12, 0x2F);
	outb(0x2F, 0x2E);
	// TODO: Must this be read here, before being used?
	data = inb(0x2F) & 0xEF;
	outb(0x2E, 0x2E);
	outb(0x11, 0x2F);
	outb(0x2F, 0x2E);
	outb(0x10, 0x2F);
	outb(0x2E, 0x2E);
	outb(0x10, 0x2F);
	outb(0x2F, 0x2E);
	outb(0x5A, 0x2F);
	outb(0x2E, 0x2E);
	outb(0x12, 0x2F);
	outb(0x2F, 0x2E);
	outb(0x2F, data);
}

void variant_dgpu_power_on(void)
{
	/* TODO:
	 * - Confirm that vendor's NVdGPUPei:OnProprietaryPpiInstalled()
	 *   will be notified as callback
	 * - DisplayPort: Investigate writes to pad GPP_I1 and GPP_I2
	 *                MDP_E_HPD_PCH and DP_F_HPD
	 * - Also assert DGPU_RST#_PCH here? GPIO table configures well...
	 * - dGPU disable power code is unreachable?
	 */
	if (!gpio_get(DGPU_RSTN_PCH)) {
		printk(BIOS_DEBUG, "dGPU in reset, enable power...\n");
		gpio_set(DGPU_PWR_EN, 1); // Assert DGPU_PWR_EN
		mdelay(12);
		gpio_set(DGPU_RSTN_PCH, 1); // Deassert DGPU_RST#_PCH
		mdelay(100);
	}
}
