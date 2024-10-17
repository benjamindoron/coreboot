#include <commonlib/device_tree.h>
#include <commonlib/list.h>
#include <coreboot_tables.h>
#include <endian.h>
#include <libpayload.h>
#include <stdint.h>
#include <string.h>

#define FDT_IORESOURCE_FLAG 0x100000000

enum bus {
	BUS_ISA,
	BUS_DEFAULT,
};

static void upl_fdt_parse_memory_node(struct device_tree_node *memory_node, u32 addr_cells,
				      u32 size_cells, struct sysinfo_t *info)
{
	struct device_tree_property *prop;
	list_for_each(prop, memory_node->properties, list_node) {
		if (!strcmp("device_type", prop->prop.name) &&
		    !strcmp("memory", prop->prop.data)) {

			struct device_tree_region regions[SYSINFO_MAX_MEM_RANGES];
			size_t count = dt_read_reg_prop(memory_node, addr_cells, size_cells,
							regions, SYSINFO_MAX_MEM_RANGES);
			if ((count + info->n_memranges) > SYSINFO_MAX_MEM_RANGES) {
				printf("WARN: more memory ranges found then supported\n");
				continue;
			}
			for (int i = 0; i < count; i++) {
				info->memrange[info->n_memranges].base = regions[i].addr;
				info->memrange[info->n_memranges].size = regions[i].size;
				info->memrange[info->n_memranges].type = CB_MEM_RAM;
				info->n_memranges++;
			}
		}
	}
}

static int upl_fdt_parse_memory(struct device_tree *dt, struct sysinfo_t *info)
{
	struct device_tree_node *node = dt->root;
	info->n_memranges = 0;

	u32 addr_cells, size_cells;
	dt_read_cell_props(node, &addr_cells, &size_cells);

	// find memory nodes and add the regions to the sysinfo struct
	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		if (!strncmp("memory@", child->name, 7) || !strcmp("memory", child->name))
			upl_fdt_parse_memory_node(child, addr_cells, size_cells, info);

	// find reserved-memory nodes
	struct device_tree_node *rsv_node =
		dt_find_node_by_path(dt, "/reserved-memory", &addr_cells, &size_cells, 0);
	if (!rsv_node) {
		printf("WARN: reserved-memory node not found\n");
		return -1;
	}
	struct device_tree_node *rsv_child;
	list_for_each(rsv_child, rsv_node->children, list_node) {
		struct device_tree_region regions[SYSINFO_MAX_MEM_RANGES];

		size_t count = dt_read_reg_prop(rsv_child, addr_cells, size_cells, regions,
						SYSINFO_MAX_MEM_RANGES);
		if ((count + info->n_memranges) > SYSINFO_MAX_MEM_RANGES) {
			printf("WARN: more memory ranges found then supported\n");
			continue;
		}

		struct device_tree_property *compatible;
		bool found = false;
		list_for_each(compatible, rsv_node->properties, list_node)
			if (strcmp(compatible->prop.name, "compatible"))
				found = true;

		// add reserved memory ranges to sysinfo struct
		for (int i = 0; i < count; i++) {
			info->memrange[info->n_memranges].base = regions[i].addr;
			info->memrange[info->n_memranges].size = regions[i].size;

			if (!found || !compatible->prop.data) {
				info->memrange[info->n_memranges].type = CB_MEM_RESERVED;
			} else if (!strcmp(compatible->prop.data, "acpi")) {
				info->memrange[info->n_memranges].type = CB_MEM_ACPI;
				info->acpi_rsdp = regions[i].addr;
			} else if (!strcmp(compatible->prop.data, "acpi-nvs")) {
				info->memrange[info->n_memranges].type = CB_MEM_NVS;
			} else if (!strcmp(compatible->prop.data, "smbios")) {
				info->memrange[info->n_memranges].type = CB_MEM_RESERVED; //TODO
				info->smbios = regions[i].addr;
			} else {
				info->memrange[info->n_memranges].type = CB_MEM_RESERVED;
			}
			info->n_memranges++;
		}
	}

	for (int i = 0; i < info->n_memranges; i++) {
		printf("addr: 0x%016llx, size: 0x%016llx, type: %d\n",
		       info->memrange[i].base, info->memrange[i].size, info->memrange[i].type);
	}

	return 0;
}

// parse /options/upl-params node
static int upl_fdt_parse_params(struct device_tree_node *node, struct sysinfo_t *info)
{
	u32 addr_cells = 2;
	u32 size_cells = 1;
	static const char *params_path[] = { "upl-params", NULL };
	struct device_tree_node *params_node;
	params_node = dt_find_node(node, params_path, &addr_cells, &size_cells, 0);
	if (!params_node)
		return -1;

	//TODO
	return 0;
}

// parse /options node
static int upl_fdt_parse_options(struct device_tree_node *node, struct sysinfo_t *info)
{
	static const char *options_path[] = { "options", NULL };
	struct device_tree_node *options_node = dt_find_node(node, options_path, NULL, NULL, 0);
	if (!options_node)
		return -1;
	return upl_fdt_parse_params(options_node, info);
}

struct cb_serial cb_serial = {
	.tag = CB_TAG_SERIAL,
	.size = sizeof(struct cb_serial),
};

// parse a serial node
static int upl_fdt_parse_serial(struct device_tree_node *node, u32 addr_cells, u32 size_cells, enum bus bus, struct sysinfo_t *info)
{
	struct device_tree_property *prop;
	struct device_tree_region region;
	size_t count = dt_read_reg_prop(node, addr_cells, size_cells, &region, 1);
	if (count == 0) {
		printf("Warning: serial reg property is empty\n");
		return -1;
	}
	cb_serial.type = CB_SERIAL_TYPE_MEMORY_MAPPED;
	if (bus == BUS_ISA)
		if (region.addr & FDT_IORESOURCE_FLAG)
			cb_serial.type = CB_SERIAL_TYPE_IO_MAPPED;

	// libpayload only supports serial base address of 32 bit width
	if (region.addr > UINT32_MAX)
		if (!(region.addr & FDT_IORESOURCE_FLAG))
			return -1;

	cb_serial.baseaddr = (uint32_t)region.addr;

	list_for_each(prop, node->properties, list_node) {
		if (!strcmp("compatible", prop->prop.name)) {
			// compatible doesn't matter for now
		} else if (!strcmp("current-speed", prop->prop.name)) {
			cb_serial.baud = be32toh(*(uint32_t *)prop->prop.data);
		} else if (!strcmp("clock-frequency",  prop->prop.name)) {
			cb_serial.input_hertz = be32toh(*(uint32_t *)prop->prop.data);
		} else if (!strcmp("reg-io-width",  prop->prop.name)) {
			cb_serial.regwidth = be32toh(*(uint32_t *)prop->prop.data);
		} else if (!strcmp("reg",  prop->prop.name)) {
		}
	}

	if (cb_serial.type == 0
	 || cb_serial.baud == 0
	 || cb_serial.input_hertz == 0
	 || cb_serial.baseaddr == 0)
		return -1;

	info->cb_serial = (uintptr_t)&cb_serial;

	return 0;
}

static int upl_fdt_parse_stdout(struct device_tree *dt, struct sysinfo_t *info)
{
	size_t stdout_size;
	char *stdout_str = NULL;
	struct device_tree_node *chosen_node = dt_find_node_by_path(dt, "/chosen", NULL, NULL, 0);
	dt_find_bin_prop(chosen_node, "stdout-path", (const void **)&stdout_str, &stdout_size);
	if (stdout_str) {
		u32 addr_cells = 2;
		u32 size_cells = 1;
		struct device_tree_node *serial;
		serial = dt_find_node_by_path(dt, stdout_str, &addr_cells, &size_cells, 0);
		if (!serial) {
			// serial node not found
			return -1;
		}

		if (!strncmp(stdout_str, "/isa", 4))
			upl_fdt_parse_serial(serial, addr_cells, size_cells, BUS_ISA, info);
		else
			upl_fdt_parse_serial(serial, addr_cells, size_cells, BUS_DEFAULT, info);
	} else {
		// stdout-path property not found
		return -1;
	}
	return 0;
}

int sysinfo_fill_from_upl_fdt(void *addr, struct sysinfo_t *info)
{
	struct device_tree *dt = fdt_unflatten(addr);
	if (!dt) {
		return -1;
	}
	upl_fdt_parse_stdout(dt, info);
	upl_fdt_parse_memory(dt, info);
	upl_fdt_parse_options(dt->root, info);
	return 0;
}
