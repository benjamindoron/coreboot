## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CRASHLOG) += early_crashlog.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CRASHLOG) += crashlog.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CRASHLOG) += crashlog.c
