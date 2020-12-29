/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Global TODO: TRPS and ?BEC */

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09") /* Embedded Controller Device */)  // _HID: Hardware ID
	Name (_GPE, 0x50)  // _GPE: General Purpose Events
	Name (\ECOK, 0)

	Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
	{
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	#define EC_SC_IO	0x66
	#define EC_DATA_IO	0x62
	#include <ec/acpi/ec.asl>

	OperationRegion (RAM, EmbeddedControl, 0, 0xFF)
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		CMDB, 8,	/* TODO */
		ETID, 8,	/* Thermal page selector. TODO: Do we need this? */
		EBID, 8,	/* Battery page selector */
		Offset (0x06),
		CMD2, 8,
		CMD1, 8,
		CMD0, 8,
		Offset (0x0A),
		    , 1,
		    , 1,
		Offset (0x10),
		EQEN, 1,	/* EQ enable */
		ETEE, 1,	/* TODO */
		Offset (0x4E),
		ISEN, 1,	/* TODO */
		Offset (0x4F),
		ECTP, 8,	/* Touchpad ID */
		Offset (0x51),
		    , 3,
		TPEN, 1,	/* Touchpad enable */
		Offset (0x52),
		WLEX, 1,	/* WLAN present */
		BTEX, 1,	/* Bluetooth present */
		EX3G, 1,	/* 3G */
		    , 3,
		RFEX, 1,	/* RF present */
		Offset (0x57),
		    , 7,
		AHKB, 1,	/* Hotkey triggered */
		AHKE, 8,	/* Hotkey data */
		Offset (0x5C),
		Offset (0x5D),
		Offset (0x6C),
		PWLT, 1,	/* TODO */
		    , 3,
		GCON, 1,	/* Enter Optimus GC6 */
		Offset (0x70),
		    , 1,
		ELID, 1,	/* Lid state */
		    , 3,
		EACS, 1,	/* AC state */
		Offset (0x71),
		WLEN, 1,	/* WLAN enable */
		BTEN, 1,	/* Bluetooth enable */
		    , 3,
		ISS3, 1,
		ISS4, 1,
		ISS5, 1,
		    , 4,
		EIDW, 1,	/* Device wake */
		Offset (0x74),
		    , 2,
		    , 1,
		TPEX, 1,	/* Touchpad present */
		Offset (0x75),
		BLST, 1,	/* Bluetooth state */
		LMIB, 1,	/* TODO */
		Offset (0x76),
		ECSS, 4,	/* EC Notify: "Enter CS" */
		EOSS, 4,	/* EC Notify: "Exit CS" */
		Offset (0x88),	/* TODO: Aliased to "EB0S" */
		EB0A, 1,
		    , 2,
		EB0R, 1,
		EB0L, 1,
		EB0F, 1,
		EB0N, 1,
		Offset (0x90),
		SCPM, 1,	/* Set cooling policy */
		Offset (0x92),	/* TODO: Aliased to "ETAF" */
		ESSF, 1,
		ECTT, 1,
		EDTT, 1,
		EOSD, 1,	/* Trip */
		EVTP, 1,
		ECP1, 1,
		    , 1,
		ECP2, 1,
		Offset (0xA8),
		ES0T, 8,	/* Temperature */
		ES1T, 8,	/* Temperature */
		Offset (0xD0),
		ESP0, 8,	/* Passive temp */
		ESC0, 8,	/* Critical temp */
		ESP1, 8,	/* Passive temp */
		ESC1, 8		/* Critical temp */
	}
	/* Aliases several battery registers */
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x88),
		EB0S, 8		/* Battery 0 state */
	}
	/* Aliases several thermal registers */
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x92),
		ETAF, 8
	}

	Method (_REG, 2, NotSerialized)  // _REG: Region Availability
	{
		If ((Arg0 == 3) && (Arg1 == 1))
		{
			ECOK = 1
			TINI ()
			EOSS = 0x05
//			OSIN ()
		}

		\PWRS = EACS
		\LIDS = ELID
		\TCRT = ESC0
		\TPSV = ESP0
	}

	Name (RFST, 0)	/* RF state */
	Method (ECPS, 1, NotSerialized)  // _PTS: Prepare To Sleep
	{
		ECSS = Arg0
//		COSI = OSYS
//		SPR1 = Arg0
		/* TRPS: Generic SMI trap handler */
//		TRPS (0x82, 0x02)
		If ((Arg0 == 3) || (Arg0 == 4))
		{
			RFST = RFEX
		}
	}

	Method (ECWK, 1, NotSerialized)  // _WAK: Wake
	{
		EQEN = 1
		EOSS = Arg0
		TINI ()
		Notify (BAT0, 0x81) // Information Change
//		COSI = OSYS
//		SPR1 = Arg0
		/* TRPS: Generic SMI trap handler */
//		TRPS (0x82, 0x03)
		If ((Arg0 == 3) || (Arg0 == 4))
		{
			RFEX = RFST
			Notify (SLPB, 0x02) // Device Wake
		}

		\LIDS = ELID
	}

#if 0
	Method (OSIN, 0, NotSerialized)
	{
		COSI = OSYS
		TRPS (0x82, 1)
	}
#endif

	Method (_Q19, 0, NotSerialized)
	{
		Debug = "EC Query: 0x19 - TODO: Graphical hotkey?"
//		^^^GFX0.GHDS (0x03)
	}

	Method (_Q1C, 0, NotSerialized)
	{
		\_SB.PCI0.GFX0.INCB()
	}

	Method (_Q1D, 0, NotSerialized)
	{
		\_SB.PCI0.GFX0.DECB()
	}

	/* Hotkeys */
	Method (_Q2C, 0, NotSerialized)
	{
		If (LMIB)
		{
			If (!AHKB)
			{
				Local1 = AHKE
				If ((Local1 > 0) && (Local1 < 0x80))
				{
					Debug = "Hotkeys - TODO: Airplane mode?"
					/* "GCMS" method */
				}
				ElseIf ((Local1 > 0x80) && (Local1 < 0xA0))
				{
					TPEN ^= 1
				}
			}
		}
	}

	Method (_Q36, 0, NotSerialized)
	{
		If (ECOK)
		{
			EOSD = 1	// Thermal trip
		}
		Else
		{
			Debug = "TODO: Is this DTS?"
			/* MBEC: Calls SMI function 0x12 */
//			MBEC (0x92, 0xF7, 0x08)
		}

		Sleep (500)
		Notify (\_TZ.TZ01, 0x80) // Thermal Status Change
		Notify (\_TZ.TZ00, 0x80) // Thermal Status Change
	}

	Method (_Q3F, 0, NotSerialized)
	{
		Debug = "EC Query: 0x3F - TRPS"
		/* TRPS: Generic SMI trap handler
		 * Maybe 0x80 corresponds to ACPI EC commands? */
//		TRPS (0x80, 0)
	}

	Method (_Q40, 0, NotSerialized)
	{
		Notify (BAT0, 0x81) // Information Change
	}

	Method (_Q41, 0, NotSerialized)
	{
		Notify (BAT0, 0x81) // Information Change
	}

	/* Battery status change */
	Method (_Q48, 0, NotSerialized)
	{
		Notify (BAT0, 0x80)
	}

	/* Battery critical? */
	Method (_Q4C, 0, NotSerialized)
	{
		If (B0ST)
		{
			Notify (BAT0, 0x80) // Status Change
		}
	}

	/* AC status change: present */
	Method (_Q50, 0, NotSerialized)
	{
		Notify (ADP1, 0x80)
	}

	/* AC status change: not present */
	Method (_Q51, 0, NotSerialized)
	{
		Notify (ADP1, 0x80)
	}

	/* Lid status change: open */
	Method (_Q52, 0, NotSerialized)
	{
		Notify (LID0, 0x80)
	}

	/* Lid status change: close */
	Method (_Q53, 0, NotSerialized)
	{
		Notify (LID0, 0x80)
	}

	Method (_Q60, 0, NotSerialized)
	{
		Debug = "EC Query: 0x60 -> WMI"
	}

	Method (_Q61, 0, NotSerialized)
	{
		Debug = "EC Query: 0x61 -> WMI"
	}

	Method (_Q62, 0, NotSerialized)
	{
		Debug = "EC Query: 0x62 -> Optimus GC6"
	}

	Method (_Q63, 0, NotSerialized)
	{
		Debug = "EC Query: 0x63 -> Optimus GC6"
	}

	Method (_Q67, 0, NotSerialized)
	{
		Debug = "EC Query: 0x67 -> Optimus GC6"
	}

	Method (_Q68, 0, NotSerialized)
	{
		Debug = "EC Query: 0x68 -> Optimus GC6"
	}

	Method (_Q6C, 0, NotSerialized)
	{
		Debug = "EC Query: 0x6C - TRPS"
		/* TRPS: Generic SMI trap handler
		 * Maybe 0x81 corresponds to ACPI EC commands? */
//		TRPS (0x81, 0)
	}

	Method (_Q6D, 0, NotSerialized)
	{
		Debug = "EC Query: 0x6D - TRPS"
		/* TRPS: Generic SMI trap handler
		 * Maybe 0x81 corresponds to ACPI EC commands? */
//		TRPS (0x81, 1)
	}

	#include "ac.asl"
	#include "battery.asl"
	#include "thermal.asl"
}

Scope (\_GPE)
{
	/* TODO: Remaining Level-Triggered GPEs? */
	Method (_L0A, 0, NotSerialized)
	{
		Notify (\_SB.SLPB, 0x02) // Device Wake
	}
}
