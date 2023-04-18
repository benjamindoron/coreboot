/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>

#include <arch/romstage.h>
#include <cbmem.h>
#include <southbridge/intel/i82801jx/i82801jx.h>
#include <device/pci_ops.h>
// NB makes assumptions about SB
//#include <northbridge/intel/ironlake/ironlake.h>

void ironlake_early_initialization(int chipset_type);
void ibexpeak_setup_bars(void);

void ibexpeak_setup_bars(void) {}
void mainboard_romstage_entry(void)
{
	i82801jx_early_init();
	ironlake_early_initialization(0);
	cbmem_recovery(0);
}
