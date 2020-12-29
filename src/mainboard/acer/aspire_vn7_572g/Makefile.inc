## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
