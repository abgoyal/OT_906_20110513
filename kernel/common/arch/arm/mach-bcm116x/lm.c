/*
 *  Copyright (C) 2003 Deep Blue Solutions Ltd, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

/*******************************************************************************
* Copyright 2010 Broadcom Corporation.  All rights reserved.
*
* 	@file	arch/arm/mach-bcm116x/lm.c
*
* Unless you and Broadcom execute a separate written software license agreement
* governing use of this software, this software is licensed to you under the
* terms of the GNU General Public License version 2, available at
* http://www.gnu.org/copyleft/gpl.html (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*******************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <mach/lm.h>

#define to_lm_device(d) container_of(d, struct lm_device, dev)
#define to_lm_driver(d) container_of(d, struct lm_driver, drv)

static int
lm_match(struct device *dev, struct device_driver *drv)
{
	pr_info("lm_match()\n");
	return 1;
}

static int
lm_bus_probe(struct device *dev)
{
	struct lm_device *lmdev = to_lm_device(dev);
	struct lm_driver *lmdrv = to_lm_driver(dev->driver);

	pr_info("lm_bus_probe()\n");
	return lmdrv->probe(lmdev);
}

static int
lm_bus_remove(struct device *dev)
{
	struct lm_device *lmdev = to_lm_device(dev);
	struct lm_driver *lmdrv = to_lm_driver(dev->driver);

	pr_info("lm_bus_remove()\n");
	if (lmdrv->remove)
		lmdrv->remove(lmdev);
	return 0;
}

static struct bus_type lm_bustype = {
	.name = "logicmodule",
	.match = lm_match,
	.probe = lm_bus_probe,
	.remove = lm_bus_remove,
	/*      .suspend        = lm_bus_suspend, */
	/*      .resume         = lm_bus_resume, */
};

static int __init
lm_init(void)
{
	pr_info("lm_init()\n");
	return bus_register(&lm_bustype);
}

postcore_initcall(lm_init);

int
lm_driver_register(struct lm_driver *drv)
{
	pr_info("lm_driver_register()\n");
	drv->drv.bus = &lm_bustype;
	return driver_register(&drv->drv);
}

void
lm_driver_unregister(struct lm_driver *drv)
{
	pr_info("lm_driver_unregister()\n");
	driver_unregister(&drv->drv);
}

static void
lm_device_release(struct device *dev)
{
	struct lm_device *d = to_lm_device(dev);
	pr_info("lm_device_release()\n");
	kfree(d);
}

int
lm_device_register(struct lm_device *dev)
{
	int ret;

	pr_info("lm_device_register()\n");
	dev->dev.release = lm_device_release;
	dev->dev.bus = &lm_bustype;

        ret = dev_set_name(&dev->dev, "lm%d", dev->id);
	if (ret)
		return ret;
	ret = request_resource(&iomem_resource, &dev->resource);

	if (ret == 0) {
		ret = device_register(&dev->dev);
		if (ret)
			release_resource(&dev->resource);
	}
	return ret;
}

EXPORT_SYMBOL(lm_driver_register);
EXPORT_SYMBOL(lm_driver_unregister);
