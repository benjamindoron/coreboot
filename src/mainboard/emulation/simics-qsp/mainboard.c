/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>
#include <cpu/x86/smm.h>

#include "simics.h"

static const unsigned char qemu_q35_irqs[] = {
	10, 10, 11, 11,
	10, 10, 11, 11,
};

static void simics_nb_init(struct device *dev)
{
	/* Map memory at 0xc0000 - 0xfffff */
	int i;
	uint8_t v = pci_read_config8(dev, D0F0_PAM(0));
	v |= 0x30;

	// setup PAM
	pci_write_config8(dev, D0F0_PAM(0), v);
	pci_write_config8(dev, D0F0_PAM(1), 0x33);
	pci_write_config8(dev, D0F0_PAM(2), 0x33);
	pci_write_config8(dev, D0F0_PAM(3), 0x33);
	pci_write_config8(dev, D0F0_PAM(4), 0x33);
	pci_write_config8(dev, D0F0_PAM(5), 0x33);
	pci_write_config8(dev, D0F0_PAM(6), 0x33);

	/* This sneaked in here, because Qemu does not emulate a SuperIO chip. */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* setup IRQ routing for pci slots */
	for (i = 0; i < 25; i++) {
		struct device *d = pcidev_on_root(i, 0);
		if (d)
			pci_assign_irqs(d, qemu_q35_irqs + (i % 4));
	}
	/* setup IRQ routing southbridge devices */
	for (i = 25; i < 32; i++) {
		struct device *d = pcidev_on_root(i, 0);
		if (d)
			pci_assign_irqs(d, qemu_q35_irqs);
	}

	// TODO: pick one location for this, this is done in the southbridge!
	#if 0
	#define SIMICS_ICH10_AHCI_ENABLE 2 << 5
	#define SIMICS_ICH10_UP_TO_SIX_SATA 1 << 5
	// pci_or_config16(PCI_DEV(0, 0x1f, 2), 0x90, SIMICS_ICH10_AHCI_ENABLE | SIMICS_ICH10_UP_TO_SIX_SATA); // 0b01100000
	pci_or_config16(pcidev_on_root(0x1f, 2), 0x90, 0x0060); // 0b01100000
  	//
  	//Enabling Ports 0-5
  	//
  	// PciOr16 (PCI_LIB_ADDRESS (0, 31, 2, 0x92), 0x003F);
	// pci_or_config16(pcidev_on_root(0x1f, 2), 0x92, 0x003f);
	pci_write_config16(pcidev_on_root(0x1f, 2), 0x92, 0x003f);
	u16 must_be_3f = pci_read_config16(pcidev_on_root(0x1f, 2), 0x92);
	printk(BIOS_DEBUG, "[simics_nb_init] pci reg = 0x%08x\n", must_be_3f);
	#endif

}

static void simics_nb_read_resources(struct device *dev)
{
	size_t tseg_size;
	uintptr_t tseg_base;

	pci_dev_read_resources(dev);

	mmconf_resource(dev, 2);

	if (CONFIG(ARCH_RAMSTAGE_X86_64)) {
		/* Reserve page tables in DRAM. FIXME: Remove once x86_64 page tables reside in CBMEM */
		reserved_ram_range(dev, 0, CONFIG_ARCH_X86_64_PGTBL_LOC, 6 * 0x1000);
	}

	smm_region(&tseg_base, &tseg_size);
	reserved_ram_range(dev, ESMRAMC, tseg_base, tseg_size);
}


struct device_operations nb_operations = {
	.read_resources   = simics_nb_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = simics_nb_init,
};

// static const struct pci_driver nb_driver __pci_driver = {
// 	.ops = &nb_operations,
// 	.vendor = 0x8086,
// 	.device = 0x29c0,
// };
