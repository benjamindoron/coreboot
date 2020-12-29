/* SPDX-License-Identifier: GPL-2.0-only */

// Global FIXME: Are the steps in PSx and _ON/_OFF correct?
// FIXME: Migrate PSx into _ON/_OFF?
// TODO: Is setting PCMR, PMST, LREN and CEDR necessary?

#if CONFIG(SKYLAKE_SOC_PCH_H)
Device (PEGP)
{
	Name (_ADR, 0x00010000)
#else
/*
 * Mobile Skylake platforms do not have a PEG port.
 * A dGPU would instead be connected to a PCH PCIe Root Port.
 */
#ifndef PEGP
/* Mainboard to override in DSDT */
#define PEGP RP01
#endif
Scope (PEGP)
{
#endif
	Name (_PRW, Package (0x02)  // _PRW: Power Resources for Wake
	{
		0x69,
		0x04
	})
	Name (_S0W, 4)	/* D3cold can wake device in S0 */

	OperationRegion (PEGC, PCI_Config, 0x00, 0x1000)
	Field (PEGC, AnyAcc, NoLock, Preserve)
	{
	Offset (0x04),	// Device Command; Primary Status
	/* TODO: Zero the next four bits too? */
		PCMR, 4,	// I/O, Mem, BusMaster and SpecCycle bitmask
	Offset (0x48),	// Device Control; Device Status
		    , 16,
		CEDR, 1,	// Correctable Error Detected
	Offset (0x50),	// Link Control; Link Status
		ASPM, 2,	// ASPM Control
		    , 2,
		LNKD, 1,	// Link Disable
	Offset (0x68),	// Device Control 2; Device Status 2
		    , 10,
		LREN, 1,	// LTR Enable
	Offset (0xa4),	// Power Management
		PMST, 2,	// Power State
	Offset (0x328), // PCIe Status 1
		    , 19,
		LNKS, 4,	// Link Status
	}

	PowerResource (PWRR, 0, 0)
	{
		Name (_STA, 1)

		Method (_ON, 0, Serialized)
		{
			Debug = "RP01 - in PWRR._ON"
			If (_STA == 0)
			{
				Debug = "RP01 - in PWRR._ON executing"
				^^_ON()
				PCMR = 0x7	// BusMaster, Memory and I/O space enabled
				PMST = 0	// Power state: D0
				_STA = 1
			}
		}

		Method (_OFF, 0, Serialized)
		{
			Debug = "RP01 - in PWRR._OFF"
			If (_STA == 1)
			{
				Debug = "RP01 - in PWRR._OFF executing"
				^^_OFF()
				_STA = 0
			}
		}
	}

	/* Depend on the CLK to be active. _PR3 is also searched by nouveau to
	 * detect "Windows 8 compatible Optimus _DSM handler".
	 */
#if !CONFIG(NVIDIA_OPTIMUS)
	Name (_PR0, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR2, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR3, Package () { \_SB.PCI0.PEGP.PWRR })
#endif

	Method (_PS0, 0, NotSerialized)
	{
		Debug = "RP01 - in _PS0"
		If (LNKD == 1)
		{
			Debug = "RP01 - in _PS0 executing"
			LNKD = 0

			Local1 = 50
			While (Local1)
			{
				Sleep (2)
				If (LNKS >= 0x7)	// Link Width >= x4
				{
					Debug = "RP01 - link up"
					Break
				}
				Local1--
			}
		}
	}

	Method (_PS3, 0, NotSerialized)
	{
		Debug = "RP01 - in _PS3"
		If (LNKD == 0)
		{
			Debug = "RP01 - in _PS3 executing"
			LNKD = 1

			Local1 = 50
			While (Local1)
			{
				Sleep (2)
				If (LNKS = 0x0)
				{
					Debug = "RP01 - link down"
					Break
				}
				Local1--
			}
		}
	}

	Method (_PSC, 0, Serialized)
	{
		Debug = "RP01 - in _PSC"
		If (LNKD == 1)
		{
			Debug = "RP01 - _PSC d3"
			Return (3)
		}
		Else
		{
			Debug = "RP01 - _PSC d0"
			Return (0)
		}
	}

	Method (_ON, 0, Serialized)
	{
		Debug = "RP01 - in _ON"
		If (^DEV0.ONOF == 0)
		{
			Debug = "RP01 - in _ON executing"
			^DEV0.ONOF = 1
			LREN = ^DEV0.LTRE	// Restore LTR enable bit
			CEDR = 1
			// Restore the Link Control register
			^DEV0.LCTL = ((^DEV0.ELCT & 0x43) | (^DEV0.LCTL & 0xFFBC))
#if 0
			/* FIXME: Wait for optimus.asl refactor. Don't want to restore Ones */
			If (CondRefOf (^DEV0.GPRF))
			{
				If (^DEV0.GPRF != 1)
				{
					// Restore device registers
					^DEV0.VREG = ^DEV0.VGAB
				}
			}
#endif
		}
	}

	Method (_OFF, 0, Serialized)
	{
		Debug = "RP01 - in _OFF"
		If (^DEV0.ONOF == 1)
		{
			Debug = "RP01 - in _OFF executing"
#if 0
			/* FIXME: Wait for optimus.asl refactor. Don't want to save Ones */
			If (CondRefOf (^DEV0.GPRF))
			{
				If (^DEV0.GPRF != 1)
				{
					// Save device registers
					^DEV0.VGAB = ^DEV0.VREG
				}
			}
#endif
			^DEV0.ONOF = 0
			^DEV0.ELCT = ^DEV0.LCTL // Save the Link Control register
			^DEV0.LTRE = LREN	// Save LTR enable bit
		}
	}

	Method (_STA, 0, Serialized)
	{
		Debug = "RP01 - in _STA"
		If (^DEV0.ONOF)
		{
			Debug = "RP01 - in _STA: present"
			Return (0xF)
		}
		Else
		{
			Debug = "RP01 - in _STA: TODO - unconditionally present?"
			Return (0xF)
		}
	}

	Device (DEV0)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package (0x02)  // _PRW: Power Resources for Wake
		{
			0x69,
			0x04
		})

		Name (ONOF, 1)
		Name (ELCT, 0)
		Name (LTRE, 0)
		External (GPRF)
		OperationRegion (VGAR, SystemMemory, (CONFIG_MMCONF_BASE_ADDRESS + (1 << 20)), 0x100)	// FIXME: Relevant PCIe endpoint? (Bus: 1; Device: 0; Function: 0)
		Field (VGAR, DWordAcc, NoLock, Preserve)
		{
			VREG, 2048,
		}
		Name (VGAB, Buffer (0x100) { 0x00 })

		OperationRegion (PCAP, PCI_Config, 0x78, 0x14)	// FIXME: Endpoint capabilities
		Field (PCAP, DWordAcc, NoLock, Preserve)
		{
			Offset (0x10),
			LCTL, 16,	// Link Control
		}
	}

	Device (DEV1)
	{
		Name (_ADR, 0x00000001)
	}

#if CONFIG(NVIDIA_OPTIMUS)
		#include <drivers/nvidia/optimus/acpi/optimus.asl>
#endif
}
