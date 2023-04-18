/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <bootblock_common.h>
#include <southbridge/intel/i82801jx/i82801jx.h>

void bootblock_soc_init(void)
{
	bootblock_early_northbridge_init();
	bootblock_early_southbridge_init();
}
