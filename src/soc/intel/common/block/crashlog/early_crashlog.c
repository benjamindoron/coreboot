/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <intelblocks/crashlog.h>

void __weak early_cl_enable_bars(void)
{
	printk(BIOS_WARNING, "%s: Operation unsupported!\n", __func__);
}

void __weak early_cl_disable_bars(void)
{
	printk(BIOS_WARNING, "%s: Operation unsupported!\n", __func__);
}

static bool early_cl_dump_data_from_sram(u32 src_bar,
					 u32 offset,
					 u32 size,
					 u32 buffer_index,
					 bool pmc_sram)
{
	if (src_bar == 0) {
		printk(BIOS_ERR, "Invalid bar 0x%x and offset 0x%x for %s\n",
		       src_bar, offset, __func__);
		return false;
	}

	u32 src_addr = src_bar + offset;

	u32 data = read32((u32 *)src_addr);

	/* First 32bits of the record must not be 0xdeadbeef */
	if (data == INVALID_CRASHLOG_RECORD) {
		printk(BIOS_DEBUG, "Invalid data 0x%x at offset 0x%x from addr 0x%x\n",
				data, offset, src_bar);
		return false;
	}

	/* PMC: copy if 1st DWORD in buffer is not zero and its 31st bit is not set */
	if (pmc_sram && !(data && !(data & BIT(31)))) {
		printk(BIOS_DEBUG, "Invalid data 0x%x at offset 0x%x from addr 0x%x"
			" of PMC SRAM.\n", data, offset, src_bar);
		return false;
	}
	/*CPU: don't copy if 1st DWORD in first buffer is zero */
	if (!pmc_sram && !data && (buffer_index == 0)) {
		printk(BIOS_DEBUG, "Invalid data 0x%x at offset 0x%x from addr 0x%x"
			" of telemetry SRAM.\n", data, offset, src_bar);
		return false;
	}

	printk(BIOS_DEBUG, "%s (buffer %d):\n", pmc_sram ? "PMC SRAM" : "CPU SRAM", buffer_index);

	u32 copied = 0;
	while (copied < size) {
		/* DW by DW copy: byte access to PMC SRAM not allowed */
		data = read32((u32 *)src_addr);
		printk(BIOS_DEBUG, "0x%02x 0x%02x 0x%02x 0x%02x ",
			(data & 0x000000ff),
			(data & 0x0000ff00) >> 8,
			(data & 0x00ff0000) >> 16,
			(data & 0xff000000) >> 24);

		src_addr += 4;
		copied++;

		if (copied % 4 == 0)
			printk(BIOS_DEBUG, "\n");
	}

	printk(BIOS_DEBUG, "\n");
	return true;
}

static void early_cl_dump_pmc_sram_data(void)
{
	u32 tmp_bar_addr = cl_get_cpu_tmp_bar();
	u32 pmc_crashLog_size = cl_get_pmc_record_size();

	if (!cl_pmc_sram_has_mmio_access() || !tmp_bar_addr) {
		printk(BIOS_DEBUG, "%s: unable to collect data.\n", __func__);
		return;
	}

	pmc_ipc_discovery_buf_t discovery_buf = cl_get_pmc_discovery_buf();

	if (discovery_buf.bits.supported != 1) {
		printk(BIOS_DEBUG, "PCH crashlog feature not supported.\n");
		return;
	}

	/* Get the size of data to copy */
	if (discovery_buf.bits.discov_mechanism == 1) {
		if (discovery_buf.bits.base_offset & BIT(31)) {
			printk(BIOS_DEBUG, "PCH discovery to be used is disabled.\n");
			return;
		}
		printk(BIOS_DEBUG, "PMC crashLog size in discovery mode : 0x%X\n",
				pmc_crashLog_size);
	} else {
		if (discovery_buf.bits.dis) {
			printk(BIOS_DEBUG, "PCH crashlog is disabled in legacy mode.\n");
			return;
		}
		pmc_crashLog_size = (discovery_buf.bits.size != 0) ?
					discovery_buf.bits.size : 0xC00;
		printk(BIOS_DEBUG, "PMC crashLog size in legacy mode : 0x%X\n",
				pmc_crashLog_size);
	}

	bool pmc_sram = true;
	pmc_crashlog_desc_table_t descriptor_table = cl_get_pmc_descriptor_table();

	if (discovery_buf.bits.discov_mechanism == 1) {
		for (int i = 0; i < descriptor_table.numb_regions; i++) {
			early_cl_dump_data_from_sram(tmp_bar_addr,
						descriptor_table.regions[i].bits.offset,
						descriptor_table.regions[i].bits.size,
						i,
						pmc_sram);
		}
	} else {
		early_cl_dump_data_from_sram(tmp_bar_addr,
					discovery_buf.bits.base_offset,
					discovery_buf.bits.size,
					0,
					pmc_sram);
	}

#if 0 // Don't clear, but maybe this
	/* when bit 7 of discov cmd resp is set -> bit 2 of size field */
	if (discovery_buf.bits.size & BIT(2))
		cl_pmc_re_arm_after_reset();
#endif
}

static void early_cl_dump_cpu_sram_data(void)
{
	u32 cpu_bar_addr = cl_get_cpu_bar_addr();
	u32 cpu_crashLog_size = cl_get_cpu_record_size();

	if (cpu_crashLog_size < 1) {
		printk(BIOS_DEBUG, "%s: no data to collect.\n", __func__);
		return;
	}

	cpu_crashlog_discovery_table_t cpu_cl_disc_tab = cl_get_cpu_discovery_table();

	printk(BIOS_DEBUG, "CPU crash data size: 0x%X bytes in 0x%X region(s).\n",
			cpu_crashLog_size, cpu_cl_disc_tab.header.fields.count);

	bool pmc_sram = false;
	for (int i = 0; i < cpu_cl_disc_tab.header.fields.count; i++) {
		if (!cpu_cl_disc_tab.buffers[i].fields.size) {
			continue;
		}

		if (!early_cl_dump_data_from_sram(cpu_bar_addr,
					cpu_cl_disc_tab.buffers[i].fields.offset,
					cpu_cl_disc_tab.buffers[i].fields.size,
					i,
					pmc_sram)) {
			/* for CPU skip all buffers if the 1st one is not valid */
			if (i == 0)
				break;
		}
	}

#if 0 // Don't clear, but maybe this
	/* perform any SOC specific cleanup */
	cpu_cl_cleanup();

	/* rearm crashlog */
	cpu_cl_rearm();
#endif
}

void early_dump_pmc_and_cpu_crashlog_from_srams(void)
{
	early_cl_enable_bars();

	if (!discover_crashlog()) {
		printk(BIOS_DEBUG, "Early CrashLog: not found\n");
		goto end;
	}

	if (pmc_crashlog_support() && cl_pmc_data_present()
	    && (cl_get_pmc_record_size() > 0)) {
		early_cl_dump_pmc_sram_data();
	} else {
		printk(BIOS_DEBUG, "Skipping PMC crashLog collection. Data not present.\n");
	}

	if (cpu_crashlog_support() && cl_cpu_data_present()
	    && (cl_get_cpu_record_size() > 0)) {
		early_cl_dump_cpu_sram_data();
	} else {
		printk(BIOS_DEBUG, "Skipping CPU crashLog collection. Data not present.\n");
	}

end:
	early_cl_disable_bars();
}
