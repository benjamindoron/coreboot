/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__


// instead of separating this into cbmem.c keep this hear
#include <cbmem.h>
#include <pc80/mc146818rtc.h>

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <mainboard/emulation/simics-qsp/memory.h>
#include <cpu/intel/smm_reloc.h>

#include <arch/romstage.h>

#include "simics.h"
#include "memory.h"

void northbridge_write_smram(u8 smram)
{
	const pci_devfn_t qpi_sad = PCI_DEV(255, 0, 1);
	pci_write_config8(qpi_sad, 0x4d, smram);
}

// There's no register on simics???
void smm_region(uintptr_t *start, size_t *size)
{
	*size = CONFIG_SMM_TSEG_SIZE;
	*start = simics_get_memory_size() - *size;

	printk(BIOS_SPEW, "SMM_BASE: 0x%08lx, SMM_SIZE: %zu MiB\n", *start, *size / MiB);
}

/*
 * Get the high memory size below 4G in bytes (addr)
 */
uintptr_t simics_get_memory_size(void)
{
	uint64_t size = cmos_read(CMOS_SYSTEM_MEM_ABOVE_16MB_LOW_BYTE);
	size |= (cmos_read(CMOS_SYSTEM_MEM_ABOVE_16MB_LOW_BYTE + 1) << 8);
	size *= 64 * KiB;
	size += 16 * MiB;

	printk(BIOS_SPEW, "[simics_get_memory_size] size = 0x%llx\n", size);
	return size;
}

/*
 * Get the high memory size above 4G in bytes (addr)
 */
uint64_t simics_get_high_memory_size(void)
{
	uint64_t size = cmos_read(CMOS_SYSTEM_MEM_ABOVE_4GB_LOW_BYTE);
	size |= (cmos_read(CMOS_SYSTEM_MEM_ABOVE_4GB_MIDDLE_BYTE) << 8);
	size |= (cmos_read(CMOS_SYSTEM_MEM_ABOVE_4GB_HIGH_BYTE) << 16);

	size *= 64 * KiB;

	printk(BIOS_SPEW, "[simics_get_high_memory_size] size = 0x%016llx\n", size);
	return size;
}

uintptr_t cbmem_top_chipset(void)
{
	const uintptr_t top = simics_get_memory_size() - CONFIG_SMM_TSEG_SIZE;
	printk(BIOS_DEBUG, "[cbmem_top_chipset] 0x%8lx\n", top);
	return top;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	const uint64_t top_of_ram = cbmem_top_chipset();
	printk(BIOS_DEBUG, "[fill_postcar_frame] writing mttrs MTRR_TYPE_WRBACK (2)\n");
	postcar_frame_add_mtrr(pcf, top_of_ram - 8 * MiB, 8 * MiB, MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, top_of_ram, 8 * MiB, MTRR_TYPE_WRBACK);
}
