/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <boot/coreboot_tables.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <lib.h>
#include <payload_mm_interface.h>
#if CONFIG(SOC_INTEL_COMMON)
#include <soc/pci_devs.h>
#endif
#include <string.h>

void lb_payload_mm(struct lb_header *header)
{
	uintptr_t tseg_base;
	size_t tseg_size;

	/* Enables payload to perform MM initialisation and runtime handling */
	struct lb_pld_mm_interface_info *mm_info = (struct lb_pld_mm_interface_info *)lb_new_record(header);

	mm_info->tag = LB_TAG_PLD_MM_INTERFACE_INFO;
	mm_info->size = sizeof(*mm_info);

	mm_info->revision = 0;
	mm_info->requires_long_mode_call = !ENV_X86_64;
	mm_info->register_mm_entry_swsmi = APM_CNT_PAYLOAD_MM;

	/* SMRAM ranges */
	struct lb_payload_mm_smram_region *smram_desc = (struct lb_payload_mm_smram_region *)lb_new_record(header);

	smram_desc->tag = LB_TAG_PAYLOAD_MM_SMRAM_REGION;
	smram_desc->size = sizeof(*smram_desc) + sizeof(struct lb_pld_smram_descriptor);
	smram_desc->number_of_smm_regions = 2;

	payload_mm_get_reserved_region(&tseg_base, &tseg_size);

	smram_desc->descriptor[0].physical_start = tseg_base;
	smram_desc->descriptor[0].physical_size = PLD_MM_SHARED_STRUCT_MAX_SIZE;
	smram_desc->descriptor[0].region_state = 0x10; // EFI_ALLOCATED

	smram_desc->descriptor[1].physical_start = tseg_base + PLD_MM_SHARED_STRUCT_MAX_SIZE;
	smram_desc->descriptor[1].physical_size = tseg_size - PLD_MM_SHARED_STRUCT_MAX_SIZE;
	smram_desc->descriptor[1].region_state = 0;

	/* Fixed communication range. Payload informs bootloader of its entrypoint */
	struct lb_payload_mm_shared_mem *mm_shared_mem = (struct lb_payload_mm_shared_mem *)lb_new_record(header);

	mm_shared_mem->tag = LB_TAG_PAYLOAD_MM_SHARED_MEM;
	mm_shared_mem->size = sizeof(*mm_shared_mem);

	mm_shared_mem->comm_buffer.physical_start = tseg_base;
	mm_shared_mem->comm_buffer.physical_size = PLD_MM_SHARED_STRUCT_MAX_SIZE;

	if (!acpi_is_wakeup_s3())
		memset((void *)(uintptr_t)mm_shared_mem->comm_buffer.physical_start, 0, PLD_MM_SHARED_STRUCT_MAX_SIZE);

	/*
	 * SPI info tables
	 */
	/* SPI registers */
	struct lb_pld_spi_flash_info *spi_info = (struct lb_pld_spi_flash_info *)lb_new_record(header);

	spi_info->tag = LB_TAG_PLD_SPI_FLASH_INFO;
	spi_info->size = sizeof(*spi_info);
	spi_info->revision = 0;

	spi_info->flags = 0;
	// TODO: Don't set for capsule update? (meta)
	if (CONFIG(BOOTMEDIA_SMM_BWP))
		spi_info->flags |= FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT;

	spi_info->spi_address.address_space_id = PLD_EFI_ACPI_3_0_PCI_CONFIGURATION_SPACE;
	spi_info->spi_address.register_bit_width = 32;
	spi_info->spi_address.register_bit_offset = 0;
	// FIXME: Avoid hard-coding?
#ifdef PCH_DEV_SPI
	spi_info->spi_address.address = CONFIG_ECAM_MMCONF_BASE_ADDRESS + PCI_BDF(PCH_DEV_SPI);
#else
	//#warning "BUGBUG: PCH_DEV_SPI not defined!"
#endif
}
