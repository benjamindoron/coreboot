/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include <variant/romstage.h>

void variant_memory_init_params(FSPM_UPD *mupd)
{
	/*
	 * Schematics indicate that this board supports 2666 MHz DIMMs.
	 * While higher clocked DIMMs are supported, this causes major
	 * system instability: CSME/PMC inevitably triggers fatal CrashLogs.
	 * I strongly suspect UMA corruption.
	 * - NOTE: It's unknown whether the DIMMs/board are flawed.
	 * - DIMMs tested: G.Skill Ripjaws Series (2x 16GiB) @3200 MHz
	 */
	mupd->FspmConfig.DdrFreqLimit = 2667;

	/* NOTE - HSIOs: OEM sets configuration, but doesn't set "*Enable" */

	/* When PrimaryDisplay == Display_Switchable, vendor's
	 * NVdGPUPei:OnSiPreMemPolicyInstalled() will remap to iGFX
	 * (given that InternalGraphics == TRUE. Otherwise, map to PEG).
	 * - Ignoring, allow FSP/VBT to determine output?
	 */
//	mupd->FspmConfig.PrimaryDisplay = 0;
}
