## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += cdp.c
bootblock-y += mmu.c
bootblock-y += reset.c

verstage-y += boardid.c
verstage-y += cdp.c
verstage-y += chromeos.c
verstage-y += gsbi.c
verstage-y += reset.c

romstage-y += romstage.c
romstage-y += cdp.c
romstage-y += chromeos.c
romstage-y += mmu.c
romstage-y += reset.c
romstage-y += gsbi.c

ramstage-y += boardid.c
ramstage-y += cdp.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += mmu.c
ramstage-y += reset.c
ramstage-y += gsbi.c
