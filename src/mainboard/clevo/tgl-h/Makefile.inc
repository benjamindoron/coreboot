## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += variants/$(VARIANT_DIR)/mainboard.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

subdirs-y += variants/$(VARIANT_DIR)
