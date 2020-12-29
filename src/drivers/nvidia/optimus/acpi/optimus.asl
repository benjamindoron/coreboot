/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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
 *
 */

Scope (\_SB.PCI0.PEGP.DEV0)
{
	Name (DGOS, 0x00)
	Name (OMPR, 0x02)
	Name (HDAS, 0x00)

	Method (NVOP, 4, Serialized)
	{
		If (Arg1 != 0x0100)
		{
			Return (0x80000001)
		}
		If (\_SB.PCI0.PEGP.DEV0._STA () == 0)
		{
			Return (0x80000002)
		}

		Local0 = ToInteger (Arg2)

		/* Supported Optimus functions advertisement */
		If (Local0 == 0)
		{
			Return (Buffer (0x04)
			{
				0x09, 0x00, 0x00, 0x06
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
			CreateField (Arg3, 0x18, 0x02, OPCE)
			CreateField (Arg3, 0x00, 0x01, FLCH)
			// Did flags change?
			If (ToInteger (FLCH) > 0)
			{
				// NOUVEAU_DSM_OPTIMUS_SET_POWERDOWN
				// only called if no _PR3 in parent
				OMPR = OPCE
			}

			Store (Buffer (0x04)
			{
				 0x00, 0x00, 0x00, 0x00
			}, Local0)
			CreateField (Local0, 0x00, 0x01, OPEN)
			CreateField (Local0, 0x03, 0x02, CGCS)
			CreateField (Local0, 0x06, 0x01, SHPC)
			CreateField (Local0, 0x18, 0x03, DGPC)
			CreateField (Local0, 0x1B, 0x02, HDAC)
			OPEN = One // OPTIMUS_ENABLED
			SHPC = One // OPTIMUS_DISPLAY_HOTPLUG
			DGPC = One // OPTIMUS_DYNAMIC_PWR_CAP
			If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
			{
				CGCS = 0x03 // OPTIMUS_STATUS_PWR_STABLE
			}

			HDAC = 0x02 // OPTIMUS_HDA_CODEC_MASK
			Return (Local0)
		}
		/* NOUVEAU_DSM_OPTIMUS_FLAGS.
		 * Called prior to disabling the dGPU if not _PR3 present to
		 * update BIT0 and BIT1.
		 *
		 * Returns the current Optimus state.
		 */
		ElseIf (Local0 == 0x1B)
		{
			CreateField (Arg3, 0x00, 0x01, HDAU)
			CreateField (Arg3, 0x01, 0x01, HDAR)
			Store (Buffer (0x04)
			{
				 0x00, 0x00, 0x00, 0x00
			}, Local0)
			CreateField (Local0, 0x02, 0x02, RQGS)
			CreateField (Local0, 0x04, 0x01, PWST)
			PWST = One
			/* TODO: set RQGS to one if external dock is connected
			 * and powered.
			 */
			RQGS = Zero
			If (ToInteger (HDAR))
			{
				//VGA switcheroo: Called on card power off
				HDAS = ToInteger (HDAU)
			}

			Return (Local0)
		}
		/* NOUVEAU_DSM_POWER
		 * Only used if Optimus is disabled?
		 * Only used if _PR3 on root isn't supported?
		 */
		ElseIf (Local0 == 3)
		{
			If (ToInteger(Arg3) > 0)
			{
				\_SB.PCI0.PEGP._ON()
				\_SB.PCI0.PEGP.DEV0._ON()
			}
			Else
			{
				\_SB.PCI0.PEGP.DEV0._OFF()
				\_SB.PCI0.PEGP._OFF()
			}
		}

		Return (0x80000002)
	}

	// helper function to compare two buffers
	Method (CMP, 2, NotSerialized)
	{
		If (SizeOf (Arg0) != 0x10)
		{
			Return (0x00)
		}

		If (SizeOf (Arg1) != 0x10)
		{
			Return (0x00)
		}

		Local0 = 0
		While (Local0 < 0x10)
		{
			If (DerefOf (Index (Arg0, Local0)) != DerefOf (Index (Arg1, Local0)))
			{
				Return (0x00)
			}

			Local0++
		}

		Return (0x01)
	}

	Method (_DSM, 4, NotSerialized)
	{
		/* NV DSM OPTIMUS */
		If (CMP (Arg0, Buffer (0x10) {
			0xF8, 0xD8, 0x86, 0xA4, 0xDA, 0x0B, 0x1B, 0x47,
			0xA7, 0x2B, 0x60, 0x42, 0xA6, 0xB5, 0xBE, 0xE0}))
		{
			Return (NVOP (Arg0, Arg1, Arg2, Arg3))
		}

		Return (Buffer (0x04)
		{
			0x01, 0x00, 0x00, 0x80
		})
	}

	PowerResource (PWRR, 0, 0)
	{
		Method (_STA)
		{
			If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
			{
				Return (1)
			}
			Else
			{
				Return (0)
			}
		}

		Method (_ON)
		{
			\_SB.PCI0.PEGP.DEV0._ON ()
			Notify (\_SB.PCI0.PEGP, Zero) // Bus Check
		}

		Method (_OFF)
		{
			\_SB.PCI0.PEGP.DEV0._OFF ()
			Notify (\_SB.PCI0.PEGP, Zero) // Bus Check
		}
	}

	/* For resource enumeration, turn on the power and PCIe root port */
	Name (_PRE, Package () { \_SB.PCI0.PEGP.DEV0.PWRR,
				 \_SB.PCI0.PEGP.PWRR })
	Name (_PR0, Package () { \_SB.PCI0.PEGP.DEV0.PWRR,
				 \_SB.PCI0.PEGP.PWRR })
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
		/* Only power up PEG if enabled by _DSM */
		If (\_SB.PCI0.PEGP.DEV0.DGOS)
		{
			\_SB.PCI0.PEGP._ON ()
			DGOS = Zero
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

		HDAS = Zero
	}

	/* GPU power is turned off by PWRR after executing _PS3 */
	Method (_PS3, 0, NotSerialized)
	{
		/* Set link down */
		\_SB.PCI0.PEGP._PS3 ()

		/* Only use _PS3 to power down PEG if enabled by _DSM */
		If (\_SB.PCI0.PEGP.DEV0.OMPR == 0x03)
		{
			\_SB.PCI0.PEGP._OFF ()

			\_SB.PCI0.PEGP.DEV0.OMPR = 0x02
			\_SB.PCI0.PEGP.DEV0.DGOS = 0x01
		}
	}

	Method (_PSC, 0, Serialized)
	{
		If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
		{
			Return (0)
		}
		Else
		{
			Return (3)
		}
	}

}
