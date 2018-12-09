/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <bootstate.h>
#include <commonlib/region.h>
#include <cpu/cpu.h>
#include <device/resource.h>
#include <stdint.h>

static void mmu_init_ranges(void *gp, struct device *dev, struct resource *res)
{
	if (!res->size || !IS_ALIGNED(res->size, GRANULE_SIZE))
		return;

	const int addr_bits = MIN(cpu_phys_address_size(), BITS_PER_VA);
	const uint64_t limit = 1ULL << addr_bits;

	if (res->size >= limit || res->base >= limit || (res->size + res->size) >= limit)
		return;

	if (res->flags & IORESOURCE_CACHEABLE)
		mmu_config_range(res->base, res->size, PAT_WB);
	else if (res->flags & IORESOURCE_PREFETCH)
		mmu_config_range(res->base, res->size, PAT_WT);
	else if (res->flags & IORESOURCE_READONLY)
		mmu_config_range(res->base, res->size, PAT_WP);

	/* Default mapping is UC, skip it */
}

static void mmu_setup_all_resources(void *unused)
{
	mmu_init();

	/* 4 level paging only supports 48 bit */
	const int addr_bits = MIN(cpu_phys_address_size(), BITS_PER_VA);

	/* Mark the default range as UC. */
	mmu_config_range(0, (1ULL * GiB) << MAX(addr_bits - 30, 0), PAT_UC);

	search_global_resources(IORESOURCE_MEM | IORESOURCE_ASSIGNED,
				IORESOURCE_MEM | IORESOURCE_ASSIGNED, mmu_init_ranges, 0);

	mmu_enable();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_EXIT, mmu_setup_all_resources, NULL);
