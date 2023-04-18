/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/mp.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <identity.h>
#include <stdlib.h>
#include <smbios.h>
#include <types.h>

#include "memory.h"
//#include "acpi.h"

static void cpu_pci_domain_read_resources(struct device *dev)
{
	struct resource *res;
	uintptr_t smm_start;
	size_t smm_size;
	int idx = 10;

	pci_domain_read_resources(dev);

	const uint64_t tomk = simics_get_memory_size() >> 20;
	const uint64_t high = simics_get_high_memory_size() >> 20;

	printk(BIOS_DEBUG, "SIMICS: cmos: %llu MiB RAM below 4G.\n", tomk);
	printk(BIOS_DEBUG, "SIMICS: cmos: %llu MiB RAM above 4G.\n", high);

	// intel x58, p. 279
	// FIXME: simics doesn't implement TOLM and TOHM???

	/* Report the memory regions. */
	ram_from_to(dev, idx++, 0, 0xa0000);

	smm_region(&smm_start, &smm_size);
	ram_from_to(dev, idx++, MiB, smm_start - MiB);

	reserved_ram_from_to(dev, idx++, smm_start, smm_size);

	if (high) {
		upper_ram_end(dev, idx++, (4ull * GiB) + (high * MiB));
	}

	/* A segment is legacy VGA region */
	mmio_from_to(dev, idx++, 0xa0000, 0xc0000);

	/* C segment to 1MB is reserved RAM (low tables) */
	reserved_ram_from_to(dev, idx++, 0xc0000, 1 * MiB);

	/* Reserve space for the LAPIC.  There's one in every processor, but
	 * the space only needs to be reserved once, so we do it here. */
	res = new_resource(dev, 3);
	res->base = cpu_get_lapic_addr();
	res->size = 0x10000UL;
	res->limit = 0xffffffffUL;
	res->flags =
		IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int qemu_get_smbios_data16(int handle, unsigned long *current)
{
	struct smbios_type16 *t =
		smbios_carve_table(*current, SMBIOS_PHYS_MEMORY_ARRAY, sizeof(*t), handle);

	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->memory_error_correction = MEMORY_ARRAY_ECC_NONE;
	t->maximum_capacity = simics_get_memory_size();

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int qemu_get_smbios_data17(int handle, int parent_handle, unsigned long *current)
{
	struct smbios_type17 *t =
		smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE, sizeof(*t), handle);

	t->phys_memory_array_handle = parent_handle;
	t->size = simics_get_memory_size() / 1024;
	t->data_width = 64;
	t->total_width = 64;
	t->form_factor = MEMORY_FORMFACTOR_DIMM;
	t->device_locator = smbios_add_string(t->eos, "Virtual");
	t->memory_type = MEMORY_TYPE_DDR;
	t->type_detail = MEMORY_TYPE_DETAIL_SYNCHRONOUS;
	t->speed = 200;
	t->clock_speed = 200;
	t->manufacturer = smbios_add_string(t->eos, mainboard_vendor);

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int simics_get_smbios_data19(int handle, int parent_handle, unsigned long *current)
{
	struct smbios_type19 *t =
		smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE, sizeof(*t), handle);

	// Platform/Intel/SimicsOpenBoardPkg/SmbiosPlatformDxe/SmbiosPlatformDxe.c
	const uintptr_t ending_address_below_4g = (simics_get_memory_size() >> 10) - 1;
	const uint64_t ending_address_above_4g = simics_get_high_memory_size() + 0xFFFFFFFF - 1;
	uint32_t len_total = 0;

	// below 4g
	t->starting_address = 0;
	t->ending_address = ending_address_below_4g;
	t->partition_width = 2;

	len_total = smbios_full_table_len(&t->header, t->eos);
	*current += len_total;

	// above 4g
	t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE, sizeof(*t), handle);

	t->starting_address = 0xFFFFFFFF;
	t->extended_starting_address = 0xFFFFFFFF;
	t->extended_ending_address = ending_address_above_4g;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	len_total += len;

	return len_total;
}

static int qemu_get_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len = qemu_get_smbios_data16(*handle, current);
	len += qemu_get_smbios_data17(*handle + 1, *handle, current);
	len += simics_get_smbios_data19(*handle + 2, *handle, current);

	*handle += 2;
	return len;
}
#endif

#if CONFIG(HAVE_ACPI_TABLES)
static const char *qemu_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (!is_pci_dev_on_bus(dev, 0))
		return NULL;

	return NULL;
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources = cpu_pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data = qemu_get_smbios_data,
#endif
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = qemu_acpi_name,
#endif
};

extern const struct mp_ops mp_ops_no_smm, mp_ops_with_smm;

void mp_init_cpus(struct bus *cpu_bus)
{
	const struct mp_ops *ops = CONFIG(NO_SMM) ? &mp_ops_no_smm : &mp_ops_with_smm;

	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, ops);
}

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
};

static void northbridge_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations mainboard_emulation_simics_qsp_ops = {
	CHIP_NAME("Simics Northbridge").enable_dev = northbridge_enable,
};
