/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PCI_UTIL_H
#define AMD_BLOCK_PCI_UTIL_H

#include <stdint.h>
#include <soc/amd_pci_int_defs.h>

/* FCH index/data registers */
#define PCI_INTR_INDEX		0xc00
#define PCI_INTR_DATA		0xc01

struct pirq_struct {
	u8 devfn;
	u8 PIN[4];	/* PINA/B/C/D are index 0/1/2/3 */
};

struct irq_idx_name {
	uint8_t index;
	const char *const name;
};

extern const struct pirq_struct *pirq_data_ptr;
extern u32 pirq_data_size;
extern const u8 *intr_data_ptr;
extern const u8 *picr_data_ptr;

u8 read_pci_int_idx(u8 index, int mode);
void write_pci_int_idx(u8 index, int mode, u8 data);
void write_pci_cfg_irqs(void);
void write_pci_int_table(void);
const struct irq_idx_name *sb_get_apic_reg_association(size_t *size);

#endif /* AMD_BLOCK_PCI_UTIL_H */
