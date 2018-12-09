/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_X86_MMU_H__
#define __ARCH_X86_MMU_H__

#include <types.h>

/*
 * Memory attributes for mmap regions.
 * These attributes act as tag values for memrange regions
 */

#define PTE_PRES (1ULL << 0)
#define PTE_RW   (1ULL << 1)
#define PTE_US   (1ULL << 2)
#define PTE_PWT  (1ULL << 3)
#define PTE_PCD  (1ULL << 4)
#define PTE_A    (1ULL << 5)
#define PTE_D    (1ULL << 6)
#define PTE_PAT  (1ULL << 7)
#define PTE_G    (1ULL << 8)
#define PTE_AVAL (7ULL << 9)
#define PTE_XD   (1ULL << 63)
#define PDE_PS   (1ULL << 7)

#define BLOCK_INDEX_MASK (PTE_PWT | PTE_PCD | PTE_PRES | PTE_RW | \
			  PTE_A | PTE_D)
#define BLOCK_INDEX_SHIFT 0
#define BLOCK_INDEX_MEM_NORMAL (PTE_PRES | PTE_RW)

/* Descriptor attributes */

#define AVAIL_SHIFT 9
#define AVAIL_MASK (7 << AVAIL_SHIFT)
#define AVAIL_INVAL 0
#define AVAIL_TABLE 3
#define AVAIL_PAGE 2
#define AVAIL_BLOCK 1

#define DESC_MASK (AVAIL_MASK | PTE_PRES | PTE_RW | PTE_A | PTE_D | \
		   PTE_US | PDE_PS)
#define INVALID_DESC 0
#define TABLE_DESC ((AVAIL_TABLE << AVAIL_SHIFT) | PTE_PRES | PTE_RW | \
		     PTE_US | PTE_A | PTE_D)
#define PAGE_DESC ((AVAIL_PAGE << AVAIL_SHIFT) | PTE_PRES | PTE_RW | \
		    PTE_US | PTE_A | PTE_D)
#define BLOCK_DESC ((AVAIL_BLOCK << AVAIL_SHIFT) | PTE_PRES | PTE_RW | \
		     PTE_US | PTE_A | PTE_D | PDE_PS)

/* Sentinel descriptor to mark first PTE of an unused table. It must be a value
   that cannot occur naturally as part of a page table. (Bits [1:0] = 0b00 makes
   this an unmapped page, but some page attribute bits are still set.) */
#define UNUSED_DESC                0x6EbAAD0BBADbA000ULL

/* XLAT Table Init Attributes */

#define VA_START                   0x0
#define BITS_PER_VA                48
/* Granule size of 4KB is being used */
#define GRANULE_SIZE_SHIFT         12
#define GRANULE_SIZE               (1 << GRANULE_SIZE_SHIFT)
#define XLAT_ADDR_MASK             ((1ULL << BITS_PER_VA) - GRANULE_SIZE)
#define GRANULE_SIZE_MASK          ((1ULL << GRANULE_SIZE_SHIFT) - 1)

#define BITS_RESOLVED_PER_LVL (GRANULE_SIZE_SHIFT - 3)
#define L0_ADDR_SHIFT         (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 3)
#define L1_ADDR_SHIFT         (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 2)
#define L2_ADDR_SHIFT         (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 1)
#define L3_ADDR_SHIFT         (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 0)

#define L0_ADDR_MASK   (((1ULL << BITS_RESOLVED_PER_LVL) - 1) << L0_ADDR_SHIFT)
#define L1_ADDR_MASK   (((1ULL << BITS_RESOLVED_PER_LVL) - 1) << L1_ADDR_SHIFT)
#define L2_ADDR_MASK   (((1ULL << BITS_RESOLVED_PER_LVL) - 1) << L2_ADDR_SHIFT)
#define L3_ADDR_MASK   (((1ULL << BITS_RESOLVED_PER_LVL) - 1) << L3_ADDR_SHIFT)

/* These macros give the size of the region addressed by each entry of a xlat
   table at any given level */
#define L3_XLAT_SIZE               (1ULL << L3_ADDR_SHIFT)
#define L2_XLAT_SIZE               (1ULL << L2_ADDR_SHIFT)
#define L1_XLAT_SIZE               (1ULL << L1_ADDR_SHIFT)
#define L0_XLAT_SIZE               (1ULL << L0_ADDR_SHIFT)

#define PAT_UC		0
#define PAT_WC		1
#define PAT_WT		4
#define PAT_WP		5
#define PAT_WB		6
#define PAT_UC_MINUS	7

void mmu_init_pre_cbmem(void);

struct region;
void mmu_init_region(struct region *r);
/* Initialize MMU registers and page table memory region. */
void mmu_init(void);
/* Install page tables */
void mmu_enable(void);

void mmu_config_range(const uint64_t start, const uint64_t size,
		      const uint64_t tag);
#endif /* __ARCH_X86_MMU_H__ */
