/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/legacy_save_state.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/smm.h>
#include <stddef.h>
#include <stdint.h>

static void post_mp_init(void)
{
	/* Now that all APs have been relocated as well as the BSP let SMIs start flowing. */
	global_smi_enable();

	/* Lock down the SMRAM space. */
	smm_lock();
}

static int fw_cfg_max_cpus(void)
{
	return 1;
}

/* Parallel MP initialization support. */
const struct mp_ops mp_ops_no_smm = {
	.get_cpu_count = fw_cfg_max_cpus,
};

const struct mp_ops mp_ops_with_smm = {
	.get_cpu_count       = fw_cfg_max_cpus,
	.get_smm_info        = smm_info,
	.pre_mp_smm_init     = smm_southbridge_clear_state,
	.relocation_handler  = smm_relocation_handler,
	.post_mp_init        = post_mp_init,
};
