/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_EMU_SIMICS_H__
#define __MAINBOARD_EMU_SIMICS_H__

#include <device/pci_type.h>
#include <types.h>

/* Device 0:0.0 PCI configuration space */
#define HOST_BRIDGE PCI_DEV(0, 0, 0)

#define D0F0_PCIEXBAR_LO 0x60
#define D0F0_PCIEXBAR_HI 0x64

#define D0F0_PAM(x) (0x90 + (x)) /* 0-6 */

#define SMRAMC 0x9d
#define G_SMRAME (1 << 3)
#define D_LCK (1 << 4)
#define D_CLS (1 << 5)
#define D_OPEN (1 << 6)

#define ESMRAMC 0x9e
#define H_SMRAME (1 << 7)

#define TSEGMB 0xa8
#define TSEGMB_MB_SHIFT 20

void mainboard_machine_check(void);

#endif
