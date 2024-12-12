/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boardid.h>
#include <boot/upl_fdt_table.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/device_tree.h>
#include <console/console.h>
#include <console/uart.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <fit.h>
#include <stdio.h>
#include <version.h>

__weak uint32_t board_id(void) { return UNDEFINED_STRAPPING_ID; }
__weak uint32_t ram_code(void) { return UNDEFINED_STRAPPING_ID; }
__weak uint32_t sku_id(void) { return UNDEFINED_STRAPPING_ID; }

__weak const char *upl_fdt_add_serial(struct device_tree_node *node) { return ""; }

static int write_options_node(struct device_tree *tree)
{
	// #address-cells = 2, #size-cells = 1 is the default according to devicetree spec
	u32 addr_cells = 2, size_cells = 1;	

	struct device_tree_node *options_node;
	options_node = dt_find_node_by_path(tree, "/options", NULL, NULL, 1);
	if (!options_node) {
		printk(BIOS_ERR, "%s: options node is null", __func__);
		return -1;
	}

	// Add container for secondary images nodes
	struct device_tree_node *images_node;
	images_node = dt_find_node_by_path(tree, "/options/upl-image", NULL, NULL, 1);
	if (!images_node) {
		printk(BIOS_ERR, "%s: upl-params node is null", __func__);
		return -1;
	}
	dt_add_u32_prop(images_node, "#address-cells", addr_cells);
	dt_add_u32_prop(images_node, "#size-cells", size_cells);
	/* Need to add 'ranges' to the intermediate node to make 'reg' work. */
	dt_add_bin_prop(images_node, "ranges", NULL, 0);

	// Add some UPL specific parameters
	// - TODO: PCI enum done (after writing working PCI-RB node)
	struct device_tree_node *params_node;
	params_node = dt_find_node_by_path(tree, "/options/upl-params", NULL, NULL, 1);
	if (!params_node) {
		printk(BIOS_ERR, "%s: upl-params node is null", __func__);
		return -1;
	}
	dt_add_string_prop(params_node, "compatible", "upl");
	dt_add_u32_prop(params_node, "addr-width", cpu_phys_address_size());
	//dt_add_bin_prop(params_node, "pci-enum-done", NULL, 0);

	// Add coreboot node, which contains information about coreboot like version and name
	struct device_tree_node *cb_node;
	cb_node = dt_find_node_by_path(tree, "/options/upl-custom/coreboot", NULL, NULL, 1);
	if (!cb_node) {
		printk(BIOS_ERR, "%s: upl-custom/coreboot node is null", __func__);
		return -1;
	}
	dt_add_string_prop(cb_node, "compatible", "upl-coreboot");
	dt_add_string_prop(cb_node, "name", "coreboot");
	dt_add_string_prop(cb_node, "version", coreboot_version);

	return 0;
}

/* Add references to each secondary image. */
void upl_fdt_add_secondary(struct device_tree *tree, const char *name, struct region *secondary)
{
	char node_name[64];
	uint64_t addr, size;
	u32 addr_cells, size_cells;

	addr = secondary->offset;
	size = secondary->size;

	int status = snprintf(node_name, sizeof(node_name), "/options/upl-image/image@%llx", addr);
	assert(status <= sizeof(node_name));

	struct device_tree_node *image_node;
	image_node = dt_find_node_by_path(tree, node_name, &addr_cells, &size_cells, 1);
	if (image_node) {
		// NB: This is not specced!
		dt_add_string_prop(image_node, "name", name);
		dt_add_string_prop(image_node, "description", "TODO");
		dt_add_reg_prop(image_node, &addr, &size, 1, addr_cells, size_cells);
	}
}

#if 0

#define NON_RELOCATABLE	BIT(31)
#define IO_SPACE	BIT(24)
#define MMIO_SPACE	BIT(25)
#define MMIO64_SPACE	(BIT(24) | BIT(25))

static void write_pci_rb_ranges(uint32_t *ranges, uint32_t *index,
				uint32_t memory_space, u64 base, u64 limit)
{
	// type
	ranges[(*index)++] = cpu_to_be32(NON_RELOCATABLE + memory_space);

	// child-addr
	uint32_t child_addr_index = *index;
	ranges[(*index)++] = cpu_to_be32(base >> 32);
	ranges[(*index)++] = cpu_to_be32(base & 0xffffffff);

	// parent-addr
	ranges[(*index)++] = ranges[child_addr_index];
	ranges[(*index)++] = ranges[child_addr_index + 1];

	// size
	u64 size = limit - base + 1;
	ranges[(*index)++] = cpu_to_be32(size >> 32);
	ranges[(*index)++] = cpu_to_be32(size & 0xffffffff);
}

static int write_pci_rb_node(struct device_tree *tree)
{
	uint64_t addr, size;
	char node_name[64];
	int status;
	struct device_tree_node *pci_rb_node, *rootport_node;
	uint32_t pci_index = 0, reg_data_index = 0;

	// #address-cells = 3, #size-cells = 2 is required per the PCI Bus Binding spec
	u32 pci_addr_cells = 3, pci_size_cells = 2;
	u32 host_addr_cells = 2, host_size_cells = 1;

	addr = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	status = snprintf(node_name, sizeof(node_name), "/pci-rb0@%llx", addr);
	assert(status <= sizeof(node_name));
	pci_rb_node = dt_find_node_by_path(tree, node_name, NULL, NULL, 1);
	if (!pci_rb_node) {
		printk(BIOS_ERR, "%s: %s node is null", __func__, node_name);
		return -1;
	}
	dt_add_u32_prop(pci_rb_node, "#address-cells", pci_addr_cells);
	dt_add_u32_prop(pci_rb_node, "#size-cells", pci_size_cells);
	dt_add_string_prop(pci_rb_node, "compatible", "pci-rb");

	size = CONFIG_ECAM_MMCONF_LENGTH;
	dt_add_reg_prop(pci_rb_node, &addr, &size, 1, host_addr_cells, host_size_cells);

	struct resource *res;
	u64 io_base = -1, io_limit = 0;
	u64 mmio_base = -1, mmio_limit = 0;
	u64 mmio64_base = -1, mmio64_limit = 0;

	uint8_t slot, func;
	struct device *pci_devs = dev_find_next_pci_device(NULL);
	while (pci_devs != NULL) {
		/* Accumulate resources for root bridge */
		for (res = pci_devs->resource_list; res; res = res->next) {
			if (res->flags & IORESOURCE_FIXED)
				continue;

			if (res->flags & IORESOURCE_IO) {
				io_base = MIN(io_base, res->base);
				io_limit = MAX(io_limit, resource_end(res));
			// This means memory strictly above 4 GiB, not 64-bit BARs
			} else if (res->flags & IORESOURCE_MEM && resource_end(res) < 4ULL*GiB) {
				mmio_base = MIN(mmio_base, res->base);
				mmio_limit = MAX(mmio_limit, resource_end(res));
			} else if (res->flags & IORESOURCE_MEM && resource_end(res) >= 4ULL*GiB) {
				mmio64_base = MIN(mmio64_base, res->base);
				mmio64_limit = MAX(mmio64_limit, resource_end(res));
			}
		}

		/* Create root port node (FIXME: This is the wrong way) */
		if (pci_devs->downstream) {
			slot = PCI_SLOT(pci_devs->path.pci.devfn);
			func = PCI_FUNC(pci_devs->path.pci.devfn);

			status = snprintf(node_name, sizeof(node_name), "rootport%d@%d,%d", pci_index++, slot, func);
			assert(status <= sizeof(node_name));
			const char *node_names_root_port[] = { node_name, NULL };
			rootport_node = dt_find_node(pci_rb_node, node_names_root_port, NULL, NULL, 1);
			if (!rootport_node) {
				printk(BIOS_ERR, "%s: %s node is null", __func__, node_name);
				return -1;
			}
			dt_add_u32_prop(rootport_node, "#address-cells", pci_addr_cells);
			dt_add_u32_prop(rootport_node, "#size-cells", pci_size_cells);
			// FIXME: reg. This one is insane (all device addresses)
			dt_add_bin_prop(rootport_node, "ranges", NULL, 0);
		}

		pci_devs = dev_find_next_pci_device(pci_devs);
	}

	status = snprintf(node_name, sizeof(node_name), "/pci-rb0@%llx/ranges", addr);
	assert(status <= sizeof(node_name));
	uint32_t *reg_data = xzalloc(21 * sizeof(uint32_t));
	printk(BIOS_DEBUG, "IO base 0x%llx limit 0x%llx\n", io_base, io_limit);
	write_pci_rb_ranges(reg_data, &reg_data_index, IO_SPACE, io_base, io_limit);
	printk(BIOS_DEBUG, "MMIO base 0x%llx limit 0x%llx\n", mmio_base, mmio_limit);
	write_pci_rb_ranges(reg_data, &reg_data_index, MMIO_SPACE, mmio_base, mmio_limit);
	printk(BIOS_DEBUG, "MMIO64 base 0x%llx limit 0x%llx\n", mmio64_base, mmio64_limit);
	write_pci_rb_ranges(reg_data, &reg_data_index, MMIO64_SPACE, mmio64_base, mmio64_limit);
	dt_set_bin_prop_by_path(tree, node_name, (void *)reg_data, 21 * sizeof(uint32_t), 1);

	// FIXME?
	status = snprintf(node_name, sizeof(node_name), "/pci-rb0@%llx/bus-range", addr);
	assert(status <= sizeof(node_name));
	uint32_t *data = xzalloc(2 * sizeof(uint32_t));
	data[0] = cpu_to_be32(0), data[1] = cpu_to_be32(pci_index - 1);
	dt_set_bin_prop_by_path(tree, node_name, (void *)data, 2 * sizeof(uint32_t), 1);

	return 0;
}

#endif

static int write_isa_node(struct device_tree *tree)
{
	// #address-cells = 2, #size-cells = 1 is the default according to devicetree spec
	u32 addr_cells = 2, size_cells = 1;

	// creates an isa node that holds devices that are accessed via I/O Ports
	// currently that is only the case for x86 devices (e.g. serial, cmos...)
	struct device_tree_node *isa_node = dt_find_node_by_path(tree, "/isa", NULL, NULL, 1);
	if (!isa_node) {
		printk(BIOS_ERR, "%s: isa node is null", __func__);
		return -1;
	}
	dt_add_u32_prop(isa_node, "#address-cells", addr_cells);
	dt_add_u32_prop(isa_node, "#size-cells", size_cells);
	dt_add_string_prop(isa_node, "compatible", "isa");

	// Add stdout serial
	if (CONFIG(CONSOLE_SERIAL)) {
		const char *serial_path = upl_fdt_add_serial(tree->root);
		if (serial_path) {
			struct device_tree_node *chosen_node;
			chosen_node = dt_find_node_by_path(tree, "/chosen", NULL, NULL, 1);
			if (!chosen_node) {
				printk(BIOS_ERR, "%s: chosen node is null", __func__);
				return -1;
			}
			dt_add_string_prop(chosen_node, "stdout-path", serial_path);
		}
	}

	return 0;
}

static int write_framebuffer_node(struct device_tree *tree)
{
	struct lb_framebuffer fb;
	char node_name[32];

	if (fill_lb_framebuffer(&fb))
		return 0;

	int status = snprintf(node_name, sizeof(node_name), "/framebuffer@%lx", (uint32_t)fb.physical_address);
	assert(status <= sizeof(node_name));

	struct device_tree_node *framebuffer_node;
	framebuffer_node = dt_find_node_by_path(tree, node_name, NULL, NULL, 1);
	if (!framebuffer_node) {
		printk(BIOS_ERR, "%s: framebuffer node is null", __func__);
		return -1;
	}

	// "display" property pointing into PCI-RB node is required,
	// but skipping it only means no HOB to describe the graphics device, and that's fine.
	dt_add_string_prop(framebuffer_node, "compatible", "simple-framebuffer");

	u64 addr = fb.physical_address;
	u64 size = fb.x_resolution * fb.y_resolution * (fb.bits_per_pixel / 8);
	// At least EDK2 assumes that framebuffer must be in low memory. But does this violate the spec?
	dt_add_reg_prop(framebuffer_node, &addr, &size, 1, 1, 1);

	dt_add_string_prop(framebuffer_node, "format", "a8r8g8b8");
	dt_add_u32_prop(framebuffer_node, "height", fb.y_resolution);
	dt_add_u32_prop(framebuffer_node, "width", fb.x_resolution);

	return 0;
}

static int write_reserved_memory_node(struct device_tree *tree)
{
	// #address-cells = 2, #size-cells = 1 is the default according to devicetree spec
	u32 addr_cells = 2, size_cells = 1;

	char node_name[32];
	u64 addr, size;
	int status;
	struct device_tree_node *node;
	struct device_tree_node *rsvd_node = dt_find_node_by_path(tree, "/reserved-memory", NULL, NULL, 0);
	if (!rsvd_node) {
		printk(BIOS_ERR, "%s: reserved-memory node is null", __func__);
		return -1;
	}
	if (CONFIG(HAVE_ACPI_TABLES)) {
		const struct cbmem_entry *acpi_region = cbmem_entry_find(CBMEM_ID_ACPI);
		addr = (unsigned long)cbmem_entry_start(acpi_region);
		size = cbmem_entry_size(acpi_region);

		status = snprintf(node_name, sizeof(node_name), "memory@%llx", addr);
		assert(status <= sizeof(node_name));
		const char *node_names_acpi[] = { node_name, NULL };
		node = dt_find_node(rsvd_node, node_names_acpi, NULL, NULL, 1);
		if (!node) {
			printk(BIOS_ERR, "%s: ACPI node is null", __func__);
			return -1;
		}
		dt_add_string_prop(node, "compatible", "acpi");
		dt_add_reg_prop(node, &addr, &size, 1, addr_cells, size_cells);
	}
	if (CONFIG(GENERATE_SMBIOS_TABLES)) {
		const struct cbmem_entry *smbios_region = cbmem_entry_find(CBMEM_ID_SMBIOS);
		addr = (unsigned long)cbmem_entry_start(smbios_region);
		size = cbmem_entry_size(smbios_region);

		status = snprintf(node_name, sizeof(node_name), "memory@%llx", addr);
		assert(status <= sizeof(node_name));
		const char *node_names_smbios[] = { node_name, NULL };
		node = dt_find_node(rsvd_node, node_names_smbios, NULL, NULL, 1);
		if (!node) {
			printk(BIOS_ERR, "%s: SMBIOS node is null", __func__);
			return -1;
		}
		dt_add_string_prop(node, "compatible", "smbios");
		dt_add_reg_prop(node, &addr, &size, 1, addr_cells, size_cells);
	}

	return 0;
}

/*
 * Writes a FDT (Flattened Device Tree) compliant to the UPL (Universal Payload Specification)
 * at rom_table_end and advances rom_table_end pointer to the size of the FDT.
 * This FDT is used as an alternative to the coreboot tables for the handoff to the payload.
 * Some architectures (like RISC-V) already use an FDT for handoff.
 * In that case the UPL specific FDT structues will simply be extended into the exisitng FDT.
 */
uintptr_t write_upl_fdt_table(uintptr_t rom_table_end)
{
	// #address-cells = 2, #size-cells = 1 is the default according to devicetree spec
	u32 addr_cells = 2, size_cells = 1;

	// these structures are only used if there is no existing devicetree in CBMEM
	struct fdt_header header = {
		.magic = cpu_to_be32(FDT_HEADER_MAGIC),
		.version = cpu_to_be32(FDT_SUPPORTED_VERSION),
		.last_comp_version = cpu_to_be32(FDT_SUPPORTED_VERSION),
	};
	struct device_tree_node root = {
		.name = "/",
	};
	struct device_tree dev_tree = {
		.header = &header,
		.header_size = sizeof(header),
		.root = &root,
	};
	// Required to be present
	struct device_tree_reserve_map_entry *entry = xzalloc(sizeof(*entry));
	entry->start = 0;
	entry->size = 0;
	list_insert_after(&entry->list_node, &dev_tree.reserve_map);
	header.reserve_map_offset = cpu_to_be32(ALIGN_UP(dev_tree.header_size, 8));

	struct device_tree *tree;
	void *dtb = cbmem_find(CBMEM_ID_FDT); // check for existing devicetree
	if (dtb) {
		printk(BIOS_DEBUG, "%s: Using existing devicetree\n", __func__);
		tree = fdt_unflatten(dtb);
		//free(dtb);
		if (!tree) {
			printk(BIOS_ERR, "%s: error unflattening devicetree\n", __func__);
			return rom_table_end;
		}
	} else {
		printk(BIOS_DEBUG, "%s: Creating new UPL devicetree\n", __func__);
		tree = &dev_tree;
		dt_add_u32_prop(tree->root, "#address-cells", addr_cells);
		dt_add_u32_prop(tree->root, "#size-cells", size_cells);
	}

	if (write_options_node(tree))
		goto error;

	//if (write_pci_rb_node(tree))
		//goto error;

	if (write_isa_node(tree))
		goto error;

	if (write_framebuffer_node(tree))
		goto error;

	// add memory and memory-reserved nodes
	upl_fdt_add_memory(tree);

	if (write_reserved_memory_node(tree))
		goto error;

	printk(BIOS_DEBUG, "Writing UPL devicetree at 0x%08lx\n", (long)rom_table_end);

	size_t dt_size = dt_flat_size(tree);
	dt_flatten(tree, (void *)rom_table_end);
	if (dtb)
		free(tree);

	return rom_table_end + dt_size;

error:
	if (dtb)
		free(tree);
	return rom_table_end;
}
