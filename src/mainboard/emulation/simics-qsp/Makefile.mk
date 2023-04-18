## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += memmap.c
bootblock-y += ../../../northbridge/intel/ironlake/bootblock.c

romstage-y += memmap.c
romstage-y += gpio.c
romstage-y += romstage.c
romstage-y += ../../../northbridge/intel/ironlake/early_init.c

postcar-y += memmap.c
postcar-y += exit_car.S

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += cpu.c
ramstage-y += ../../../cpu/intel/smm/gen1/smmrelocate.c

smm-y += memmap.c
