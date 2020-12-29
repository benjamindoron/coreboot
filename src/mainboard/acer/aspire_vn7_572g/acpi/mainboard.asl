/* SPDX-License-Identifier: GPL-2.0-or-later */

Scope (_SB)
{
	Method (MPTS, 1, NotSerialized)  // _PTS: Prepare To Sleep
	{
		\_SB.PCI0.LPCB.EC0.ECPS (Arg0)
	}

	Method (MWAK, 1, Serialized)  // _WAK: Wake
	{
		\_SB.PCI0.LPCB.EC0.ECWK (Arg0)

		If ((Arg0 == 3) || (Arg0 == 4))
		{
			LIDS = \_SB.LID0._LID ()
			Notify (\_SB.LID0, 0x80) // Status Change
			/* TODO: Bus Check? */
		}
	}

	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D") /* Lid Device */)  // _HID: Hardware ID
		Method (_LID, 0, NotSerialized)  // _LID: Lid Status
		{
			If (\_SB.PCI0.LPCB.EC0.ELID)
			{
				Return (1)
			}
			Else
			{
				Return (0)
			}
		}

		Method (_PSW, 1, NotSerialized)  // _PSW: Power State Wake
		{
			\_SB.PCI0.LPCB.EC0.EIDW = Arg0
		}

		Name (_PRW, Package () { 0x0A, 3 })  // _PRW: Power Resources for Wake
	}

	Device (SLPB)
	{
		Name (_HID, EisaId ("PNP0C0E") /* Sleep Button Device */)  // _HID: Hardware ID
		Name (_PRW, Package () { 0x0A, 3 })  // _PRW: Power Resources for Wake
	}
}
