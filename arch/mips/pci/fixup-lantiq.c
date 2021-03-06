/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/of_irq.h>
#include <linux/of_pci.h>

int (*ltq_pci_plat_arch_init)(struct pci_dev *dev) = NULL;
int (*ltq_pci_plat_dev_init)(struct pci_dev *dev) = NULL;

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	if (ltq_pci_plat_arch_init)
		return ltq_pci_plat_arch_init(dev);

	if (ltq_pci_plat_dev_init)
		return ltq_pci_plat_dev_init(dev);

	return 0;
}

int __init pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
#if defined(CONFIG_SYNO_LSP_ARMADA)
	struct of_phandle_args dev_irq;
#else /* CONFIG_SYNO_LSP_ARMADA */
	struct of_irq dev_irq;
#endif /* CONFIG_SYNO_LSP_ARMADA */
	int irq;

#if defined(CONFIG_SYNO_LSP_ARMADA)
	if (of_irq_parse_pci(dev, &dev_irq)) {
#else /* CONFIG_SYNO_LSP_ARMADA */
	if (of_irq_map_pci(dev, &dev_irq)) {
#endif /* CONFIG_SYNO_LSP_ARMADA */
		dev_err(&dev->dev, "trying to map irq for unknown slot:%d pin:%d\n",
			slot, pin);
		return 0;
	}
#if defined(CONFIG_SYNO_LSP_ARMADA)
	irq = irq_create_of_mapping(&dev_irq);
#else /* CONFIG_SYNO_LSP_ARMADA */
	irq = irq_create_of_mapping(dev_irq.controller, dev_irq.specifier,
					dev_irq.size);
#endif /* CONFIG_SYNO_LSP_ARMADA */
	dev_info(&dev->dev, "SLOT:%d PIN:%d IRQ:%d\n", slot, pin, irq);
	return irq;
}
