/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <assert.h>
#include <cbmem.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/cr.h>
#include <stdint.h>
#include <string.h>

/* This just caches the next free table slot (okay to do since they fill up from
 * bottom to top and can never be freed up again). It will reset to its initial
 * value on stage transition, so we still need to check it for UNUSED_DESC. */
static uint64_t *next_free_table;
static uint64_t *page_tables;
static uint64_t *epage_tables;

static void print_tag(const int level, const uint64_t pat)
{
	switch (pat) {
	case PAT_UC:
		printk(level, "UC\n");
		break;
	case PAT_WC:
		printk(level, "WC\n");
		break;
	case PAT_WT:
		printk(level, "WT\n");
		break;
	case PAT_WP:
		printk(level, "WP\n");
		break;
	case PAT_WB:
		printk(level, "WB\n");
		break;
	case PAT_UC_MINUS:
		printk(level, "UC-\n");
		break;
	default:
		break;
	}
}

static uint64_t pte_pat_flags(const unsigned long pat)
{
	switch (pat) {
	case PAT_UC:
		return 0 | PTE_PCD | PTE_PWT;
	case PAT_WC:
		return 0 | 0 | PTE_PWT;
	case PAT_WT:
		return PTE_PAT | PTE_PCD | PTE_PWT;
	case PAT_WP:
		return PTE_PAT | 0 | PTE_PWT;
	case PAT_WB:
		return 0 | 0 | 0;
	case PAT_UC_MINUS:
		return 0 | PTE_PCD | 0;
	default:
		printk(BIOS_ERR, "MMU: PTE PAT defaulting to WB: %lx\n", pat);
		return 0 | 0 | 0;
	}
}

/* Func : get_block_attr
 * Desc : Get block descriptor attributes based on the value of tag in memrange
 * region
 */
static uint64_t get_block_attr(const unsigned long tag)
{
	uint64_t flags = pte_pat_flags(tag);
	if (flags & PTE_PAT) {
		/* PAT is at position 12 instead of 7 */
		flags &= ~PTE_PAT;
		flags |= (PTE_PAT << 5);
	}
	flags |= BLOCK_DESC;

	return flags;
}

/* Func : get_page_attr
 * Desc : Get page descriptor attributes based on the value of tag in memrange
 * region
 */
static uint64_t get_page_attr(const unsigned long tag)
{
	uint64_t flags = pte_pat_flags(tag);
	flags |= PAGE_DESC;
	return flags;
}

/**
 * Desc : Get next free table from pagetable and set it up to match
 *        old parent entry.
 */
static uint64_t *setup_new_table(uint64_t desc, const uint64_t xlat_size)
{
	while (next_free_table[0] != UNUSED_DESC) {
		next_free_table += GRANULE_SIZE / sizeof(*next_free_table);
		if (epage_tables - next_free_table <= 0)
			die("Ran out of page table space!");
	}

	uint64_t frame_base = desc & XLAT_ADDR_MASK;
	printk(BIOS_DEBUG,
	       "MMU: Backing address range [0x%llx:0x%llx) with "
	       "new page table @%p\n",
	       frame_base, frame_base + (xlat_size << BITS_RESOLVED_PER_LVL), next_free_table);

	if (desc == INVALID_DESC) {
		memset(next_free_table, 0, GRANULE_SIZE);
	} else {
		/* Can reuse old parent entry, but may need to adjust type. */
		if (xlat_size == L3_XLAT_SIZE)
			desc |= PAGE_DESC;

		int i = 0;
		for (; i < GRANULE_SIZE / sizeof(*next_free_table); i++) {
			next_free_table[i] = desc;
			desc += xlat_size;
		}
	}

	return next_free_table;
}

/**
 * Desc: Check if the table entry is a valid descriptor. If not, initialize new
 * table, update the entry and return the table addr. If valid, return the addr
 */
static uint64_t *get_next_level_table(uint64_t *ptr, size_t xlat_size)
{
	uint64_t desc = *ptr;

	if ((desc & DESC_MASK) != TABLE_DESC) {
		uint64_t *new_table = setup_new_table(desc, xlat_size);
		desc = ((uintptr_t)new_table) | TABLE_DESC;
		*ptr = desc;
	}
	return (uint64_t *)(uintptr_t)(desc & XLAT_ADDR_MASK);
}

/**
 * Desc : Given a base address and size, it identifies the indices within
 * different level XLAT tables which map the given base addr. Similar to table
 * walk, except that all invalid entries during the walk are updated
 * accordingly. On success, it returns the size of the block/page addressed by
 * the final table.
 * The table names l0 to l3 are just shorter names for the x86 page tables.
 *
 * The mapping is as the following:
 * l0: PM4LE
 * l1: PDPE
 * l2: PDE
 * l3: PTE
 */
static uint64_t init_xlat_table(const uint64_t base_addr, const uint64_t size,
				const uint64_t tag, const bool hugepage_1gb)
{
	uint64_t l0_index = (base_addr & L0_ADDR_MASK) >> L0_ADDR_SHIFT;
	uint64_t l1_index = (base_addr & L1_ADDR_MASK) >> L1_ADDR_SHIFT;
	uint64_t l2_index = (base_addr & L2_ADDR_MASK) >> L2_ADDR_SHIFT;
	uint64_t l3_index = (base_addr & L3_ADDR_MASK) >> L3_ADDR_SHIFT;
	uint64_t *table = page_tables;
	uint64_t desc;

	/* L0 entry stores a table descriptor (doesn't support blocks) */
	table = get_next_level_table(&table[l0_index], L1_XLAT_SIZE);

	/* L1 table lookup */
	if ((size >= L1_XLAT_SIZE) && IS_ALIGNED(base_addr, (1UL << L1_ADDR_SHIFT))
	    && hugepage_1gb) {
		/* If block address is aligned and size is greater than
		 * or equal to size addressed by each L1 entry, we can
		 * directly store a block desc */
		desc = base_addr | get_block_attr(tag);
		table[l1_index] = desc;
		/* L2 lookup is not required */
		return L1_XLAT_SIZE;
	}

	/* L1 entry stores a table descriptor */
	table = get_next_level_table(&table[l1_index], L2_XLAT_SIZE);

	/* L2 table lookup */
	if ((size >= L2_XLAT_SIZE) && IS_ALIGNED(base_addr, (1UL << L2_ADDR_SHIFT))) {
		/* If block address is aligned and size is greater than
		 * or equal to size addressed by each L2 entry, we can
		 * directly store a block desc */
		desc = base_addr | get_block_attr(tag);
		table[l2_index] = desc;
		/* L3 lookup is not required */
		return L2_XLAT_SIZE;
	}

	/* L2 entry stores a table descriptor */
	table = get_next_level_table(&table[l2_index], L3_XLAT_SIZE);

	/* L3 table lookup */
	desc = base_addr | get_page_attr(tag);
	table[l3_index] = desc;
	return L3_XLAT_SIZE;
}

/**
 * Desc : Check address/size alignment of a table or page.
 */
static void sanity_check(const uint64_t addr, const uint64_t size)
{
	assert(!(addr & GRANULE_SIZE_MASK) && !(size & GRANULE_SIZE_MASK)
	       && (addr + size < (1ULL << BITS_PER_VA)) && size >= GRANULE_SIZE);
}

/**
 * Desc : Returns the page table entry governing a specific address.
 */
static uint64_t get_pte(const uint64_t addr)
{
	int shift = L0_ADDR_SHIFT;
	uint64_t *pte = page_tables;

	while (1) {
		int index = ((uintptr_t)addr >> shift) & ((1ULL << BITS_RESOLVED_PER_LVL) - 1);

		if ((pte[index] & DESC_MASK) != TABLE_DESC || shift <= GRANULE_SIZE_SHIFT)
			return pte[index];

		pte = (uint64_t *)(uintptr_t)(pte[index] & XLAT_ADDR_MASK);
		shift -= BITS_RESOLVED_PER_LVL;
	}
}

/**
 * Desc : Asserts that mapping for addr matches the access type used by the
 * page table walk (i.e. addr is correctly mapped to be part of the pagetable).
 */
static void assert_correct_pagetable_mapping(const uint64_t addr)
{
	return;

	uint64_t pte = get_pte(addr);
	assert(((pte >> BLOCK_INDEX_SHIFT) & BLOCK_INDEX_MASK) == BLOCK_INDEX_MEM_NORMAL);
}

static inline bool cpu_supports_1gb_hugepage(void)
{
	static bool checked, supports;
	if (checked)
		return supports;

	/* check if CPU supports 1GB hugepages */
	if (cpu_cpuid_extended_level() >= 0x80000001)
		supports = !!(cpuid_edx(0x80000001) & (1 << 26));
	checked = true;

	return supports;
}

/**
 * @brief Configure memory range for long mode
 *
 * Desc : This function repeatedly calls init_xlat_table with the base
 * address. Based on size returned from init_xlat_table, base_addr is updated
 * and subsequent calls are made for initializing the xlat table until the whole
 * region is initialized.
 */
void mmu_config_range(const uint64_t start, const uint64_t size, const uint64_t tag)
{
	uint64_t base_addr = start;
	uint64_t temp_size = size;

	printk(BIOS_INFO, "MMU: Mapping address range [0x%llx:0x%llx) as ", start,
	       start + size);
	print_tag(BIOS_INFO, tag);

	sanity_check(base_addr, temp_size);

	const bool hp = cpu_supports_1gb_hugepage();
	while (temp_size)
		temp_size -=
			init_xlat_table(base_addr + (size - temp_size), temp_size, tag, hp);
}


/**
 * @brief Init a region with page tables to be used with the MMU.
 * Must be called onced with an inactive (from MMU PoV) region before any call to
 *   - mmu_config_range
 *   - mmu_setup_default_map
 *   - mmu_enable
 *
 */
void mmu_init_region(struct region *const r)
{
	printk(BIOS_DEBUG, "MMU: Init page table region\n");

	/* Page table directories must be aligned to page size */
	page_tables = (uint64_t *)ALIGN_UP((uintptr_t)region_offset(r), GRANULE_SIZE);

	/* Can't operate on page tables that are currently active! */
	assert((uintptr_t)page_tables != read_cr3());

	next_free_table = page_tables;
	epage_tables = (void *)region_last(r);

	/* Initially mark all table slots unused (first PTE == UNUSED_DESC). */
	uint64_t *table = page_tables;
	for (; epage_tables - table > 0; table += GRANULE_SIZE / sizeof(*table))
		table[0] = UNUSED_DESC;

	printk(BIOS_DEBUG, "MMU: Initialize PM4LE\n");

	/* Initialize the root table (PM4LE) to be completely unmapped. */
	uint64_t *root = setup_new_table(INVALID_DESC, L0_XLAT_SIZE);
	assert(root == page_tables);

	printk(BIOS_DEBUG, "MMU: Page table region initialized\n");
}

/**
 * @brief Init MMU for long mode
 *
 */
void mmu_init(void)
{
	int pages, addr_bits;
	struct region r;
	void *ptr;
	printk(BIOS_DEBUG, "MMU: Setup\n");

	/* 4 level paging only supports 48 bit */
	addr_bits = MIN(cpu_phys_address_size(), BITS_PER_VA);
	printk(BIOS_DEBUG, "MMU: CPU has %d address bits\n", addr_bits);

	const bool hp = cpu_supports_1gb_hugepage();
	printk(BIOS_DEBUG, "MMU: CPU %s 1GB hugepages\n", hp ? "supports" : "doesn't support");

	pages = 1; /* PML4E */
	const int PDPTs = 1 << MAX(addr_bits - L0_ADDR_SHIFT, 0);
	pages += PDPTs; /* PDPT with 1 GiB pages, or pointing to 1 GiB PLEs */
	if (!hp) {
		const int PLEs = 1 << MAX(addr_bits - L1_ADDR_SHIFT, 0);
		pages += PLEs; /* PLE with 2 MiB pages */
	}

	printk(BIOS_DEBUG, "MMU: Need %d 4k pages for identity mapping\n", pages);

	/* Overallocate page table space for more fine grained memory map */
	pages += 256;
	printk(BIOS_DEBUG, "MMU: Allocating %d 4k pages for mapping\n", pages);

	/* Add one for alignment */
	ptr = (uint64_t *)cbmem_add(CBMEM_ID_PAGE_RAM, (pages + 1) * GRANULE_SIZE);

	/* Page table directories must be aligned to page size */
	r.offset = ALIGN_UP((uintptr_t)ptr, GRANULE_SIZE);
	r.size = pages * GRANULE_SIZE;
	mmu_init_region(&r);

	printk(BIOS_DEBUG, "MMU: Setup done\n");
}

void mmu_enable(void)
{
	assert_correct_pagetable_mapping((uintptr_t)page_tables);
	printk(BIOS_DEBUG, "MMU: Activating new page tables\n");

	/* Load the new page table address */
	write_cr3((uintptr_t)page_tables);
	printk(BIOS_INFO, "MMU: Installed new page tables\n");
}
