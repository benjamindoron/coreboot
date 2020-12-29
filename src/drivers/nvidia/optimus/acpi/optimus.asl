/*
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * Required methods to be implemented outside of this scope:
 *
 * \_SB.PCI0.PEGP.DEV0._STA () that returns:
 *  0x0 if no dGPU present
 *  0x5 if dGPU is powered off
 *  0xf if dGPU is powered on
 *
 * \_SB.PCI0.PEGP.DEV0._ON ()
 *  poweres on the dGPU
 *
 * \_SB.PCI0.PEGP.DEV0._OFF ()
 *  poweres off the dGPU
 *
 * \_SB.PCI0.PEGP._ON ()
 *  poweres on the PEG root port
 *
 * \_SB.PCI0.PEGP._OFF ()
 *  poweres off the PEG root port
 *
 * \_SB.PCI0.PEGP._PS0 ()
 *  enables the PEG link
 *
 * \_SB.PCI0.PEGP._PS3 ()
 *  disabled the PEG link
 *
 * \_SB.PCI0.PEGP.PWRR
 *  the PEG PowerResource handling \_SB.PCI0.PEGP._ON and \_SB.PCI0.PEGP._OFF
 */

Scope (\_SB.PCI0.PEGP.DEV0)
{
	Name (HDAS, 0x00)	// FIXME(benjamindoron): OpRegion ("MLTF") or named?
	Name (OMPR, 0x02)
	Name (DGOS, 0x00)
	Name (GPRF, 0x00)
	
	Method (NVOP, 4, Serialized)
	{
		Debug = "RP01.DEV0 - in NVOP"
		If (Arg1 != 0x0100)
		{
			Debug = "RP01.DEV0 - NVOP: incorrect rev"
			Return (0x80000002)
		}
	
		Local0 = ToInteger (Arg2)
	
		/* Supported Optimus functions advertisement */
		If (Local0 == 0)
		{
			Debug = "RP01.DEV0 - NVOP: func 0"
			Return (Buffer (0x04)
			{
				// functions 0, 26, 27 supported
				// TODO(benjamindoron): Other functions required? (0x10 (16) - "GOBT?")
				0x01, 0x00, 0x00, 0x0c
			})
		}
		/* NOUVEAU_DSM_OPTIMUS_CAPS
		 * Called prior to disabling the dGPU if not _PR3 present to
		 * update BIT0, BIT18 and BIT19.
		 *
		 * Returns Optimus capabilties.
		 */
		ElseIf (Local0 == 0x1A)
		{
			Debug = "RP01.DEV0 - NVOP: func 1a"
			CreateField (Arg3, 0x18, 0x02, OPCE)
			CreateField (Arg3, 0x00, 0x01, FLCH)
			CreateField (Arg3, 0x01, 0x01, DVSR)
			CreateField (Arg3, 0x02, 0x01, DVSC)
			// Did flags change?
			If (ToInteger (FLCH))
			{
				// NOUVEAU_DSM_OPTIMUS_SET_POWERDOWN
				// only called if no _PR3 in parent
				OMPR = ToInteger (OPCE)
			}
	
			Store (Buffer (0x04)
			{
				0x00, 0x00, 0x00, 0x00
			}, Local0)
			CreateField (Local0, 0x00, 0x01, OPEN)
			CreateField (Local0, 0x03, 0x02, CGCS)
			CreateField (Local0, 0x06, 0x01, SHPC)
			CreateField (Local0, 0x08, 0x01, SNSR)
			CreateField (Local0, 0x18, 0x03, DGPC)
			CreateField (Local0, 0x1B, 0x02, HDAC)
			OPEN = 1 // OPTIMUS_ENABLED
	
			SHPC = 1 // OPTIMUS_DISPLAY_HOTPLUG
			HDAC = 0x02 // OPTIMUS_HDA_CODEC_MASK
	
			DGPC = 1 // OPTIMUS_DYNAMIC_PWR_CAP
			If (ToInteger (DVSC))
			{
				If (ToInteger (DVSR))
				{
					GPRF = 1
				}
			}
			SNSR = GPRF
			If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
			{
				CGCS = 0x03 // OPTIMUS_STATUS_PWR_STABLE
			}
	
			Return (Local0)
		}
		// FIXME(benjamindoron): Unrecognised
		/* NOUVEAU_DSM_OPTIMUS_FLAGS.
		 * Called prior to disabling the dGPU if not _PR3 present to
		 * update BIT0 and BIT1.
		 *
		 * Returns the current Optimus state.
		 */
		ElseIf (Local0 == 0x1B)
		{
			Debug = "RP01.DEV0 - NVOP: func 1b"
			CreateField (Arg3, 0x00, 0x01, HDAU)
			CreateField (Arg3, 0x01, 0x01, HDAR)
			Store (Buffer (0x04)
			{
				0x00, 0x00, 0x00, 0x00
			}, Local0)
			CreateField (Local0, 0x02, 0x02, RQGS)
			CreateField (Local0, 0x04, 0x01, PWST)
			PWST = 1
			/* TODO: set RQGS to one if external dock is connected
			 * and powered.
			 */
			RQGS = 0
			If (ToInteger (HDAR))
			{
				//VGA switcheroo: Called on card power off
				HDAS = ToInteger (HDAU)
			}
	
			Return (Local0)
		}
	
		Return (0x80000002)
	}
	
	Method (_DSM, 4, Serialized)
	{
		Debug = "RP01.DEV0 in _DSM"
		/* NV OPTIMUS DSM */
		If (Arg0 == ToUUID ("a486d8f8-0bda-471b-a72b-6042a6b5bee0"))
		{
			Debug = "RP01.DEV0 - calling NVOP"
			Return (NVOP (Arg0, Arg1, Arg2, Arg3))
		}
	
		Debug = "RP01.DEV0 - NOT calling NVOP"
		Return (0x80000001)
	}
	
	PowerResource (PWRR, 0, 0)
	{
		Method (_STA, 0, Serialized)
		{
			Debug = "RP01.DEV0 in PWRR._STA"
			If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
			{
				Debug = "RP01.DEV0 _STA - dev present"
				Return (1)
			}
			Else
			{
				Debug = "RP01.DEV0 _STA - dev NOT present"
				Return (0)
			}
		}
	
		Method (_ON, 0, Serialized)
		{
			Debug = "RP01.DEV0 in PWRR._ON"
			\_SB.PCI0.PEGP.DEV0._ON ()
//			^^MLTF = 0
			// FIXME(benjamindoron): Don't notify on new chipsets?
//			Notify (\_SB.PCI0.PEGP, 0) // Bus Check
		}
	
		Method (_OFF, 0, Serialized)
		{
			Debug = "RP01.DEV0 in PWRR._OFF"
			\_SB.PCI0.PEGP.DEV0._OFF ()
			// FIXME(benjamindoron): Don't notify on new chipsets?
//			Notify (\_SB.PCI0.PEGP, 0) // Bus Check
		}
	}
	
	/* For resource enumeration, turn on the power and PCIe root port */
	Name (_PR0, Package () { \_SB.PCI0.PEGP.DEV0.PWRR,
				 \_SB.PCI0.PEGP.PWRR })
	Name (_PR2, Package () { \_SB.PCI0.PEGP.DEV0.PWRR,
				 \_SB.PCI0.PEGP.PWRR })
			// TODO: PCIe link, then dGPU power?
	Name (_PR3, Package () { \_SB.PCI0.PEGP.DEV0.PWRR,
				 \_SB.PCI0.PEGP.PWRR })
	
	OperationRegion (RPCS, PCI_Config, 0x00, 0x100)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		VID, 16,
	}
	
	/* GPU power is turned on by PWRR before executing _PS0 */
	Method (_PS0, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in PS0"
		/* Only power up PEG if enabled by _DSM */
		If (\_SB.PCI0.PEGP.DEV0.DGOS)
		{
			Debug = "power up PEG"
			\_SB.PCI0.PEGP._ON ()
			DGOS = 0
		}
	
		/* Wait for device init */
		Sleep(50)
	
		/* Retrain link */
		\_SB.PCI0.PEGP._PS0 ()
	
		/* Wait for device to appear */
		Local0 = 0x32
		While (Local0)
		{
			If (VID == 0x10de)
			{
				Break
			}
			Sleep (2)
			Local0--
		}
	
		HDAS = 0
	}
	
	/* GPU power is turned off by PWRR after executing _PS3 */
	Method (_PS3, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in PWRR._OFF"
		/* Set link down */
		\_SB.PCI0.PEGP._PS3 ()
	
		/* Only use _PS3 to power down PEG if enabled by _DSM */
		If (\_SB.PCI0.PEGP.DEV0.OMPR == 0x03)
		{
			Debug = "using _PS3 to power down PEG"
			\_SB.PCI0.PEGP._OFF ()
	
			\_SB.PCI0.PEGP.DEV0.OMPR = 0x02
			\_SB.PCI0.PEGP.DEV0.DGOS = 0x01
		}
	}
	
	Method (_PSC, 0, Serialized)
	{
		Debug = "RP01.DEV0 in _PSC"
		If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
		{
			Debug = "RP01.DEV0 _PSC d0"
			Return (0)
		}
		Else
		{
			Debug = "RP01.DEV0 _PSC d3"
			Return (3)
		}
	}
}
