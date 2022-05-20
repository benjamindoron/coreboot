/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <intelblocks/lpc_lib.h>
#include <soc/ramstage.h>
#include <variant/ramstage.h>

/* If EC requests, clear the RTC.
   TODO: Is this implemented to invoke UEFI variable clear, or other? */
static void ec_should_clear_rtc(void)
{
#if 0
	// "NMSG" field; unknown field between "GFOF" and "KPCR"
	if (read8p(0xFE0B0100) != 0xFF && (read8p(0xFE0B01E8) & 0x20)) {
		for (int i = 0x40; i < 0x80; i++) {
			// Recall, arguments: Data, Port
			outb(i, 0x70);
			outb(0, 0x71);
		}
		for (int i = 0x0; i < 0x80; i++) {
			// Recall, arguments: Data, Port
			outb(i, 0x72);
			outb(0, 0x73);
		}

		/* Inform EC that clear is complete */
		write8p(0xFE0B01E8, (read8p(0xFE0B01E8) & ~0x20));
		/* TODO: Now, reset the system
		   - Implement as UEFI would: Cold, or Warm? */
	}
#endif
}

void variant_silicon_init_params(FSP_S_CONFIG *params)
{
	// Disable AER to fix suspend failing with some SSDs.
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
}

void variant_ec_init(void)
{
	/* Program the same 64K range of EC memory as vendor FW */
	lpc_open_mmio_window(0xFE0B0000, 0x10000);
	/* TODO: Also initialise touchpad better? See I2cToPs2Pei notify. */
	ec_should_clear_rtc();
}
