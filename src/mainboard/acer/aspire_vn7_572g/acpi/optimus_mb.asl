/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(NVIDIA_OPTIMUS)
#include <soc/intel/skylake/acpi/peg.asl>

#define DGPU_PRESENT	GPP_A20	/* Active low */
#define DGPU_HOLD_RST	GPP_B4	/* Active low */
#define DGPU_PWR_EN	GPP_B21	/* Active low */
#define DGPU_PWROK	GPP_B2	/* Active high */

Scope (PEGP.DEV0)
{
	Method (_OFF, 0, Serialized)
	{
		If (GTXS (DGPU_PWR_EN) == 0)
		{
			CTXS (DGPU_HOLD_RST)	// Assert dGPU_HOLD_RST#
			STXS (DGPU_PWR_EN)	// Deassert dGPU_PWR_EN#
		}
	}

	Method (_ON, 0, Serialized)
	{
		If (GTXS (DGPU_PWR_EN) == 1)
		{
			CTXS (DGPU_HOLD_RST)	// Assert dGPU_HOLD_RST#
			CTXS (DGPU_PWR_EN)	// Assert dGPU_PWR_EN#
			Sleep (7)
			STXS (DGPU_HOLD_RST)	// Deassert dGPU_HOLD_RST#
			Sleep (30)
		}
	}

	Method (_STA, 0, Serialized)
	{
		If (GRXS (DGPU_PRESENT) != 0)	// dGPU not present
		{
			Return (0)
		}
		If (GRXS (DGPU_PWROK) == 1)	// dGPU power OK
		{
			Return (0x0F)
		}

		Return (0x5)
	}

#if CONFIG(NVIDIA_OPTIMUS_GC6)

#define GC6_FB_EN	GPP_A12	/* Active high */
#define GPU_EVENT	GPP_A22	/* Active low */

	// Mainboard GC6 Enter early hook
	Method (MGIE, 0, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.GCON = 1
		Sleep (10)
	}

	// Mainboard GC6 Exit early hook
	Method (EGOE, 0, Serialized)
	{
		While (GRXS (GC6_FB_EN) != 1)	// Not presently in GC6
		{
			Sleep (1)
		}

		CTXS (GPU_EVENT)	// Assert GPU_EVENT#
		While (GRXS (GC6_FB_EN) != 0)	// Wait while exiting GC6
		{
			Sleep (1)
		}

		STXS (GPU_EVENT)	// Deassert GPU_EVENT#
	}

	// Mainboard GC6 Exit late hook
	Method (EGOL, 0, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.GCON = 0
	}


	Method (MGET, 0, Serialized)
	{
		If (GRXS (GC6_FB_EN) == 0)	// Not presently in GC6
		{
			Return (1)	// Powered on
		}
		Else
		{
			Return (3)	// Powered off
		}
	}

#endif
}

#endif
