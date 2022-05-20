/*
 * SPDX-License-Identifier: GPL-2.0-only
 * Some bit definitions reference https://github.com/system76/coreboot/blob/gc6-2/src/drivers/system76/dgpu/acpi/dgpu.asl.
 *
 * Required methods to be implemented outside of this scope:
 *
 * TODO
 *
 */

External (\_SB.PCI0.PEGP.DEV0.MGIE, MethodObj)
External (\_SB.PCI0.PEGP.DEV0.MGOE, MethodObj)
External (\_SB.PCI0.PEGP.DEV0.MGOL, MethodObj)

Name (TGPC, Buffer () { 0x00, 0x00, 0x00, 0x00 })
Name (TDGC, 0)
Name (DGCX, 0)
Name (TRPC, 0)

Method (GC6I, 0, Serialized)	// Enter GC6
{
	Debug = "RP01.DEV0 in GC6I"
	// Mainboard GC6 Enter early hook
	If (CondRefOf (\_SB.PCI0.PEGP.DEV0.MGIE))
	{
		\_SB.PCI0.PEGP.DEV0.MGIE ()
	}

	/* TODO: Use PEGP methods? */
	\_SB.PCI0.PEGP.DEV0.LTRE = LREN
	LNKD = 1
}

Method (GC6O, 0, Serialized)	// Exit GC6
{
	Debug = "RP01.DEV0 in GC6O"
	// Mainboard GC6 Exit early hook
	If (CondRefOf (\_SB.PCI0.PEGP.DEV0.MGOE))
	{
		\_SB.PCI0.PEGP.DEV0.MGOE ()
	}

	/* TODO: Use PEGP methods? */
	LNKD = 0
	While (LNKS < 0x7)
	{
		Sleep (1)
	}

	LREN = \_SB.PCI0.PEGP.DEV0.LTRE
	CEDR = 1

	// Mainboard GC6 Exit late hook
	If (CondRefOf (\_SB.PCI0.PEGP.DEV0.MGOL))
	{
		\_SB.PCI0.PEGP.DEV0.MGOL ()
	}
}

Method (GETS, 0, Serialized)	// Return GC6 state value
{
	If (CondRefOf (\_SB.PCI0.PEGP.DEV0.MGET))
	{
		Return (\_SB.PCI0.PEGP.DEV0.MGET ())
	}
	Else
	{
		Return (1)	// Assume dGPU is powered on
	}
}

Method (OGC6, 4, Serialized)
{
	Debug = "RP01.DEV0 - in OGC6"
	If (ToInteger (Arg1) < 0x0100)
	{
		Debug = "RP01.DEV0 - OGC6: incorrect rev"
		Return (0x80000001)
	}

	Local0 = ToInteger (Arg2)

	/* Supported Optimus GC6 functions advertisement */
	If (Local0 == 0)
	{
		Debug = "RP01.DEV0 - OGC6: func 0"
		// functions 0, 1, 3 supported
		Return (Buffer (0x04) { 0x0b, 0x00, 0x00, 0x00 })
	}
	/* OPTIMUS_GC6_CAPS
	 * Called prior to TODO
	 *
	 * Returns Optimus GC6 capabilities
	 */
	 ElseIf (Local0 == 1)
	 {
		Debug = "RP01.DEV0 - OGC6: func 1"
		Name (JTB1, Buffer () { 0x00, 0x00, 0x00, 0x00 })
		CreateField (JTB1, 0x00, 0x01, JTEN)
		CreateField (JTB1, 0x01, 0x02, SREN)
		CreateField (JTB1, 0x03, 0x03, PLPR)
		CreateField (JTB1, 0x06, 0x02, FBPR)
		CreateField (JTB1, 0x08, 0x02, GUPR)
		CreateField (JTB1, 0x0A, 0x01, GC6R)
		CreateField (JTB1, 0x0B, 0x01, PTRH)
		CreateField (JTB1, 0x0D, 0x01, MHYB)
		CreateField (JTB1, 0x0E, 0x01, RPCL)
		CreateField (JTB1, 0x0F, 0x02, GC6V)
		CreateField (JTB1, 0x14, 0x0C, JTRV)
		JTEN = 1	// TODO: JT enabled?
		GC6R = 1	// TODO
		MHYB = 1	// TODO
		RPCL = 1	// TODO
		If (ToInteger (RPCL) == 1)	// FIXME: Opaque predicate
		{
			TRPC = 1	// TODO: What does this do?
		}

		GC6V = 1
		PTRH = 1	// TODO
		SREN = 1	/* "NVSR eDP": G-Sync. TODO: Enabled? Assigned 2 bits.
				 * See https://forums.developer.nvidia.com/t/can-tk1-support-nvsr-edp-lcd-exp-the-11-6-chaimei-innolux-n116hse/48347
				 */
		JTRV = 0x0103	// JT revision
		Return (JTB1)
	}
	/* TODO: OPTIMUS_GC6_PWR
	 *
	 * Controls dGPU power
	 */
	ElseIf (Local0 == 3)
	{
		CreateField (Arg3, 0x00, 0x03, GUPC)
		CreateField (Arg3, 0x04, 0x01, PLPC)
		CreateField (Arg3, 0x07, 0x01, ECOC)
		CreateField (Arg3, 0x0E, 0x02, DFGC)
		CreateField (Arg3, 0x10, 0x03, GPCX)
		TGPC = Arg3
		If ((ToInteger (GUPC) != 0) || (ToInteger (DFGC) != 0))
		{
			TDGC = ToInteger (DFGC)
			DGCX = ToInteger (GPCX)
		}

		Name (JTB3, Buffer () { 0x00, 0x00, 0x00, 0x00 })
		CreateField (JTB3, 0x00, 0x03, GUPS)
		CreateField (JTB3, 0x03, 0x01, GPGS)
		CreateField (JTB3, 0x07, 0x01, PLST)
		If (ToInteger (DFGC) != 0)
		{
			GPGS = 1
			GUPS = 1
			Return (JTB3)
		}

		If (ToInteger (GUPC) == 1)
		{
			GC6I ()
			PLST = 1
			GUPS = 0
		}
		ElseIf (ToInteger (GUPC) == 0x02)
		{
			GC6I ()
			If (ToInteger (PLPC) == 0)
			{
				PLST = 0
			}

			GPGS = 1
			GUPS = 0
		}
		ElseIf (ToInteger (GUPC) == 0x03)
		{
			GC6O ()
			If (ToInteger (PLPC) != 0)
			{
				PLST = 0
			}

			GPGS = 1
			GUPS = 0
		}
		ElseIf (ToInteger (GUPC) == 0x04)
		{
			TGPC = Arg3
			GC6O ()
			If (ToInteger (PLPC) != 0)
			{
				PLST = 0
			}
		}
		ElseIf (ToInteger (GUPC) == 0)
		{
			GUPS = GETS ()
			If (ToInteger (GUPS) == 1)
			{
				GPGS = 1
			}
			Else
			{
				GPGS = 0
			}
		}

		Return (JTB3)
	}

	Return (0x80000002)
}
