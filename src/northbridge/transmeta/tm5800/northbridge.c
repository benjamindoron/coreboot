#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"

#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

static void pci_domain_read_resources(device_t dev)
{
        struct resource *resource;
        unsigned reg;

        /* Initialize the system wide io space constraints */
        resource = new_resource(dev, 0);
        resource->base  = 0x400;
        resource->limit = 0xffffUL;
        resource->flags = IORESOURCE_IO;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_IO, IORESOURCE_IO);

        /* Initialize the system wide memory resources constraints */
        resource = new_resource(dev, 1);
        resource->limit = 0xffffffffULL;
        resource->flags = IORESOURCE_MEM;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_MEM, IORESOURCE_MEM);
}

static void ram_resource(device_t dev, unsigned long index,
        unsigned long basek, unsigned long sizek)
{
        struct resource *resource;

        if (!sizek) {
                return;
        }
        resource = new_resource(dev, index);
        resource->base  = ((resource_t)basek) << 10;
        resource->size  = ((resource_t)sizek) << 10;
        resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
                IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void pci_domain_set_resources(device_t dev)
{
        struct resource *resource, *last;
	device_t mc_dev;
        uint32_t pci_tolm;

        pci_tolm = 0xffffffffUL;
        last = &dev->resource[dev->resources];
        for(resource = &dev->resource[0]; resource < last; resource++)
        {
                compute_allocate_resource(&dev->link[0], resource,
                        BRIDGE_IO_MASK, resource->flags & BRIDGE_IO_MASK);

                resource->flags |= IORESOURCE_STORED;
                report_resource_stored(dev, resource, "");

                if ((resource->flags & IORESOURCE_MEM) &&
                        (pci_tolm > resource->base))
                {
                        pci_tolm = resource->base;
                }
        }

	mc_dev = dev->link[0].children;
	if (mc_dev) {
		/* Figure out which areas are/should be occupied by RAM.
		 * This is all computed in kilobytes and converted to/from
		 * the memory controller right at the edges.
		 * Having different variables in different units is
		 * too confusing to get right.  Kilobytes are good up to
		 * 4 Terabytes of RAM...
		 */
		unsigned long tomk, tolmk;
		int idx;

#warning "This is hardcoded to 1MiB of RAM for now"
		tomk = 1024;
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory.
			 */
			tolmk = tomk;
		}
		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tolmk - 768);
	}
	assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
        max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
        return max;
}

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
        .enable_resources = enable_childrens_resources,
        .init             = 0,
        .scan_bus         = pci_domain_scan_bus,
};  

static void cpu_bus_init(device_t dev)
{
        initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
        .read_resources   = cpu_bus_noop,
        .set_resources    = cpu_bus_noop,
        .enable_resources = cpu_bus_noop,
        .init             = cpu_bus_init,
        .scan_bus         = 0,
};

static void enable_dev(struct device *dev)
{
        struct device_path path;

        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
}

struct chip_operations northbridge_transmeta_tm5800_control = {
	.name       = "Transmeta tm5800 Northbridge",
	.enable_dev = enable_dev, 
};
