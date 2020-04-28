/* SPDX-License-Identifier: GPL-2.0-or-later */

#if CONFIG(NVIDIA_OPTIMUS)
#include <soc/intel/skylake/acpi/peg.asl>

#define DGPU_PRESENT	GPP_A20	/* Active low */
#define DGPU_HOLD_RST	GPP_B4	/* Active low */
#define DGPU_PWR_EN	GPP_B21	/* Active low */
#define DGPU_PWROK	GPP_B2	/* Active high */

Scope (\_SB.PCI0.PEGP.DEV0)
{
	Method (_OFF, 0, Serialized)
	{
		If (GTXS(DGPU_PWR_EN) == 0)
		{
			CTXS(DGPU_HOLD_RST)	// Assert dGPU_HOLD_RST#
			STXS(DGPU_PWR_EN)	// Deassert dGPU_PWR_EN#
		}
	}

	Method (_ON, 0, Serialized)
	{
		If (GTXS(DGPU_PWR_EN) == 1)
		{
			CTXS(DGPU_HOLD_RST)	// Assert dGPU_HOLD_RST#
			CTXS(DGPU_PWR_EN)	// Assert dGPU_PWR_EN#
			Sleep(7)
			STXS(DGPU_HOLD_RST)	// Deassert dGPU_HOLD_RST#
			Sleep(30)
		}
	}

	Method (_STA, 0, Serialized)
	{
		If (GRXS(DGPU_PRESENT) != 0)
		{
			Return (0)
		}
		If (GRXS(DGPU_PWROK) == 1)
		{
			Return (0xf)
		}

		Return (0x5)
	}
}
#endif
