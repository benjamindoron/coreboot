/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SIMICS_MEMORY_H_
#define __SIMICS_MEMORY_H_

uint64_t simics_get_high_memory_size(void);
uintptr_t simics_get_memory_size(void);

uintptr_t below_4g(void);
uintptr_t above_4g(void);

void dump_cmos(void);

// CMOS 0x34/0x35 specifies the system memory above 16 MB.
// * CMOS(0x35) is the high byte
// * CMOS(0x34) is the low byte
// * The size is specified in 64kb chunks
// * Since this is memory above 16MB, the 16MB must be added
//   into the calculation to get the total memory size.
#define CMOS_SYSTEM_MEM_ABOVE_16MB_LOW_BYTE         0x34
#define CMOS_SYSTEM_MEM_ABOVE_16MB_HIGH_BYTE        0x35

// CMOS 0x5b-0x5d specifies the system memory above 4GB MB.
// * CMOS(0x5d) is the most significant size byte
// * CMOS(0x5c) is the middle size byte
// * CMOS(0x5b) is the least significant size byte
// * The size is specified in 64kb chunks
#define CMOS_SYSTEM_MEM_ABOVE_4GB_LOW_BYTE          0x5b
#define CMOS_SYSTEM_MEM_ABOVE_4GB_MIDDLE_BYTE       0x5c
#define CMOS_SYSTEM_MEM_ABOVE_4GB_HIGH_BYTE         0x5d

// from Platform/Intel/SimicsOpenBoardPkg/SimicsPei/Platform.c
#define SIMICS_TOLM_START               0x00000000
#define SIMICS_TOLM_END                 0xdeffffff

#define SIMICS_TOHM                     // TODO: read from PCI?
#define SIMICS_4GB                      0x1 << 0x9 // 0x100000000

/* DRAM Memory Regions */
// Compatibility Area
#define SIMICS_COMP_START               0x00000
#define SIMICS_COMP_DOS_RANGE_END       0x9ffff
#define SIMICS_COMP_VGA_SMM_START       0xa0000
#define SIMICS_COMP_VGA_SMM_END         0xbffff

// Software must
// program one of the system DRAM memory decode ranges that the IOH uses for
// inbound system memory decoding to include these ranges.
#define SIMICS_COMP_C_AND_D_START       0xc0000
#define SIMICS_COMP_C_AND_D_END         0xdffff

//  IOH does not explicitly
// decode the E/F region in the outbound direction and relies on subtractive decoding to
// forward accesses to this region to the legacy ICH10
#define SIMICS_COMP_E_AND_E_START       0xe0000
#define SIMICS_COMP_E_AND_E_END         0xfffff

// Low Memory
#define SIMICS_1M_TO_TOLM_START         0x00100000
#define SIMICS_1M_TO_TOLM_END           SIMICS_TOLM_END

#define SIMICS_LM_PCIE_MMCFG_START      SIMICS_TOLM_END + 1
#define SIMICS_LM_PCIE_MMCFG_END        0xfbffffff
//  TODO: this size is variable!
#define SIMICS_LM_CPUCSR_START          0xfc000000
#define SIMICS_LM_CPUCSR_END            0xfcffffff
#define SIMICS_MMIOL_START              // TODO
#define SIMICS_MMIOL_END                0xfe7fffff
#define SIMICS_LM_MISC_START            0xfe800000
#define SIMICS_LM_MISC_END              0xfe9fffff
#define SIMICS_LM_LOCAL_CSR_START       0xfeb00000
#define SIMICS_LM_LOCAL_CSR_END         0xfebfffff
#define SIMICS_LM_IOAPIC_START          0xfec00000
#define SIMICS_LM_IOAPIC_END            0xfecfffff
#define SIMICS_LM_LEGACY_TMP_START      0xfed00000
#define SIMICS_LM_LEGACY_TMP_END        0xfedfffff
#define SIMICS_LM_LOCAL_APIC_START      0xfee00000
#define SIMICS_LM_LOCAL_APIC_END        0xfeefffff
#define SIMICS_LM_INTA_RSVD_START       0xfef00000
#define SIMICS_LM_INTA_RSVD_END         0xfeffffff
#define SIMICS_LM_FWH_START             0xff000000
#define SIMICS_LM_FWH_END               0xffffffff

// High Memory
#define SIMICS_HM_BOHM_TO_TOHM_START    SIMICS_4GB
#define SIMICS_HM_BOHM_TO_TOHM_END      SIMICS_TOHM

// After TOHM, there comes MMIOH, Reserved and Privileged CSR
// These values come from BARs (?)
#endif
