/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (ADP1)
{
	Name (_HID, "ACPI0003" /* Power Source Device */)  // _HID: Hardware ID
	Name (_PCL, Package () { \_SB })  // _PCL: Power Consumer List

	Method (_PSR, 0, NotSerialized)  // _PSR: Power Source
	{
		Return (EACS)
	}
}
