/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2011 Sven Schnelle <svens@stackframe.org> */

/*
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
#ifndef DGPU_BUS_NUMBER
/* Mainboard to override in DSDT */
#define DGPU_BUS_NUMBER 1
#endif
	// Dev.Fn: 0.0
	OperationRegion (PCNV, SystemMemory, (CONFIG_MMCONF_BASE_ADDRESS + (DGPU_BUS_NUMBER << 20)), 0x1000)
	Field (PCNV, AnyAcc, NoLock, Preserve)
	{
		Offset (0x488),
		    , 25,
		MLTF, 1,	// HDA devfn enable
	}

	Name (OMPR, 2)	// Optimus DGPU Power-Control Ready
	Name (DGOS, 0)	// DGPU Power Status
#if 0
	Name (GPRF, 0)
#endif
	Name (HDAS, 0)	// HDA state; TODO: What is this for?

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
			// functions 0, 16, 26, 27 supported
			Return (Buffer (0x04) { 0x01, 0x00, 0x01, 0x0c })
		}
		/* GETOBJ - Get Data Object */
		ElseIf (Local0 == 16)
		{
			Debug = "RP01.DEV0 - NVOP: func 16"
			Return (GOBT (Arg3))
		}
		/* NOUVEAU_DSM_OPTIMUS_CAPS
		 * Called prior to disabling the dGPU if not _PR3 present to
		 * update BIT0, BIT18 and BIT19.
		 *
		 * Returns Optimus capabilties.
		 */
		ElseIf (Local0 == 26)
		{
			Debug = "RP01.DEV0 - NVOP: func 1a"
			CreateField (Arg3, 24, 2, OPCE)
			CreateField (Arg3, 0, 1, FLCH)
#if 0
			CreateField (Arg3, 1, 1, DVSR)
			CreateField (Arg3, 2, 1, DVSC)
#endif
			// Did flags change?
			If (ToInteger (FLCH))
			{
				// NOUVEAU_DSM_OPTIMUS_SET_POWERDOWN
				// only called if no _PR3 in parent
				OMPR = ToInteger (OPCE)	// Optimus Power Control enable
			}

			Name (RBUF, Buffer () { 0x00, 0x00, 0x00, 0x00 })
			CreateField (RBUF, 0, 1, OPEN)
			CreateField (RBUF, 3, 2, CGCS)
			CreateField (RBUF, 6, 1, SHPC)
#if 0
			CreateField (RBUF, 8, 1, SNSR)
#endif
			CreateField (RBUF, 24, 3, DGPC)
			CreateField (RBUF, 27, 2, HDAC)
			OPEN = 1	// OPTIMUS_ENABLED

			SHPC = 1	// OPTIMUS_DISPLAY_HOTPLUG
			HDAC = 2	// OPTIMUS_HDA_CODEC_MASK

			DGPC = 1	// OPTIMUS_DYNAMIC_PWR_CAP
#if 0
			If (ToInteger (DVSC))
			{
				If (ToInteger (DVSR))
				{
					GPRF = 1
				}
			}
			SNSR = GPRF
#endif
			If (\_SB.PCI0.PEGP.DEV0._STA () == 0xf)
			{
				CGCS = 3	// OPTIMUS_STATUS_PWR_STABLE
			}

			Return (RBUF)
		}
		// NOTE(benjamindoron): Only used on muxed devices/devices with HDA?
		/* NOUVEAU_DSM_OPTIMUS_FLAGS.
		 * Called prior to disabling the dGPU if not _PR3 present to
		 * update BIT0 and BIT1.
		 *
		 * Returns the current Optimus state.
		 */
		ElseIf (Local0 == 27)
		{
			CreateField (Arg3, 0x00, 0x01, HDAU)
			CreateField (Arg3, 0x01, 0x01, HDAR)
			Local0 = Buffer () { 0x00, 0x00, 0x00, 0x00 }
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

	Method (GOBT, 1, NotSerialized)
	{
		Name (OPVK, Buffer (0xE2)
		{
			/* 0000 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0008 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0010 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0018 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0020 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0028 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0030 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0038 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0040 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0048 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0050 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0058 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0060 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0068 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0070 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0078 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0080 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0088 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0090 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0098 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00A0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00A8 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00B0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00B8 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00C0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00C8 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00D0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00D8 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 00E0 */  0x00, 0x00
		})
		CreateWordField (Arg0, 2, USRG)	// Object type signature passed in by driver
		If ((USRG == 0x564B))	// 'VK' for Optimus Validation Key Object
		{
			Return (OPVK)
		}

		Return (Zero)
	}

	Method (SGON, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in SGON"
		/* Power on dGPU */
		\_SB.PCI0.PEGP.DEV0._ON ()

		/* Retrain link */
		\_SB.PCI0.PEGP.PS0_ ()	// BUGBUG(benjamindoron): Let the kernel do it?

		/* Enable device */
		\_SB.PCI0.PEGP._ON ()

		/* FIXME: Actually wait for device? */

		// FIXME(benjamindoron): Don't notify on new chipsets?
//		Notify (\_SB.PCI0.PEGP, 0) // Bus Check
	}

	Method (_PS0, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in PS0"
		/* Only power up PEG if enabled by _DSM */
		If (DGOS)
		{
			Debug = "power up PEG"
			SGON ()
			DGOS = 0
			MLTF = 0
		}
	}

	Method (SGOF, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in SGOF"
		/* Disable device */
		\_SB.PCI0.PEGP._OFF ()

		/* Set link down */
		\_SB.PCI0.PEGP.PS3_ ()	// BUGBUG(benjamindoron): Let the kernel do it?

		/* Power off dGPU */
		\_SB.PCI0.PEGP.DEV0._OFF ()

		// FIXME(benjamindoron): Don't notify on new chipsets?
//		Notify (\_SB.PCI0.PEGP, 0) // Bus Check
	}

	Method (_PS3, 0, NotSerialized)
	{
		Debug = "RP01.DEV0 in PS3"
		/* Only use _PS3 to power down PEG if enabled by _DSM */
		If (OMPR == 0x03)
		{
			Debug = "using _PS3 to power down PEG"
			SGOF ()

			OMPR = 2
			DGOS = 1
		}
	}

#if CONFIG(NVIDIA_OPTIMUS_GC6)
	#include "optimus_gc6.asl"
#endif

	Method (_DSM, 4, Serialized)
	{
		Debug = "RP01.DEV0 in _DSM"
		/* NV_OPTIMUS DSM */
		If (Arg0 == ToUUID ("a486d8f8-0bda-471b-a72b-6042a6b5bee0"))
		{
			Debug = "RP01.DEV0 - calling NVOP"
			Return (NVOP (Arg0, Arg1, Arg2, Arg3))
		}

		/* OPTIMUS_GC6 DSM */
		If (Arg0 == ToUUID ("cbeca351-067b-4924-9cbd-b46b00b86f34"))
		{
#if CONFIG(NVIDIA_OPTIMUS_GC6)
			Debug = "RP01.DEV0 - calling OGC6; possibly incomplete"
			Return (OGC6 (Arg0, Arg1, Arg2, Arg3))
#endif
		}

		Debug = "RP01.DEV0 - NOT calling OP"
		Return (0x80000001)
	}
}
