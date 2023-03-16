/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/smihandler.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

/*
 * Specific SOC SMI handler during ramstage finalize phase
 */
void smihandler_soc_at_finalize(void)
{
	const volatile struct smm_pci_resource_info *res_store;
	size_t res_count, found = 0;
	u32 val;

	/* SMM_FEATURE_CONTROL can only be written within SMM. */
	smm_pci_get_stored_resources(&res_store, &res_count);
	for (size_t i_slot = 0; i_slot < res_count; i_slot++) {
		if (res_store[i_slot].vendor_id != PCI_VID_INTEL ||
		    res_store[i_slot].device_id != UBOX_DFX_DEVID) {
			continue;
		}

		val = pci_s_read_config32(res_store[i_slot].pci_addr, SMM_FEATURE_CONTROL);
		val |= (SMM_CODE_CHK_EN | SMM_FEATURE_CONTROL_LOCK);
		pci_s_write_config32(res_store[i_slot].pci_addr, SMM_FEATURE_CONTROL, val);
		found ++;
	}
	printk(BIOS_DEBUG, "Locked SMM_FEATURE_CONTROL on %zd sockets\n", found);
	if (!found)
		printk(BIOS_ERR, "Failed to lock SMM_FEATURE_CONTROL\n");
}

/*
 * This is the generic entry for SOC SMIs
 */
void cpu_smi_handler(void)
{
}

/* This is needed by common SMM code */
const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
};
