/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <cbmem.h>
#include <commonlib/region.h>
#include <console/console.h>

/* Need 9 page table entries in the worst case. See mmu_setup_default_map
 * for details. */
static u8 __aligned(4096) page_tables_cbmem_setup[9 * GRANULE_SIZE];

static bool installed_tmp_table;

static void mmu_setup_default_map(void)
{
	const uintptr_t top = (uintptr_t)cbmem_top();
	size_t len;

	/* Need 1 page table entry for PM4LE */

	/* Need (1 PDPE + 4 PDE) entries for 0-4GiB in the worst case */
	mmu_config_range(0, 4ULL * GiB, PAT_WB);

	/* Need (1 PDPE + 2 PDE) entries for 1 GiB range in the worst case */
	len = 1 * GiB + (top & (2 * MiB - 1));

	/* Align to 2MiB page size to reduce page table usage. */
	mmu_config_range(ALIGN_DOWN(top, 2 * MiB) - len, len, PAT_WB);
}

/* Create a temporary page table structure in page_tables_cbmem_setup,
 * to be able to install cbmem root structure. As soon as cbmem is initialized
 * it will switch to page tables in cbmem.
 */
void mmu_init_pre_cbmem(void)
{
	struct region r;
	uintptr_t top = (uintptr_t)cbmem_top();

	if (top < 4ULL * GiB) {
		/* Page tables in ROM identity map everything in 32bit address space.
		   There's no need to install additional page tables to access cbmem. */
		printk(BIOS_DEBUG, "MMU: Skipping pre-cbmem init\n");
		return;
	}

	installed_tmp_table = true;

	printk(BIOS_DEBUG, "MMU: Setup pagetables to map CBMEM at 0x%zx\n", top);

	r.offset = (uintptr_t)page_tables_cbmem_setup;
	r.size = sizeof(page_tables_cbmem_setup);
	mmu_init_region(&r);

	mmu_setup_default_map();

	mmu_enable();

	printk(BIOS_DEBUG, "MMU: Setup done\n");
}

/* Migrate page table into cbmem as the heap allocation is in CAR and will
 * be invalid as soon as postcar stage loads.
 */
static void mmu_hook(int is_recovery)
{
	struct region r;
	void *ptr;

	if (installed_tmp_table) {
		printk(BIOS_INFO, "MMU: Switching to CBMEM page tables\n");
		ptr = (uint64_t *)cbmem_add(CBMEM_ID_PAGE_RAM, 10 * GRANULE_SIZE);
		if (!ptr)
			die("Failed to allocate page tables in cbmem\n");

		/* Page table directories must be aligned to page size */
		r.offset = ALIGN_UP((uintptr_t)ptr, GRANULE_SIZE);
		r.size = 9 * GRANULE_SIZE;
		mmu_init_region(&r);
		mmu_setup_default_map();
		mmu_enable();

		printk(BIOS_INFO, "MMU: Switch done\n");
	}
}

CBMEM_CREATION_HOOK(mmu_hook);
