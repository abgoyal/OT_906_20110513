/*******************************************************************************
* Copyright 2010 Broadcom Corporation.  All rights reserved.
*
*       @file   arch/arm/mach-bcm215xx/include/mach/reg_irq.h
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
#ifndef __REG_BCM215XX_IRQ_H__
#define __REG_BCM215XX_IRQ_H__

#include <mach/io.h>
#include <mach/hardware.h>

#define HW_IRQ_BASE                IO_ADDRESS(BCM21553_INTC_BASE)

/* Interrupt controller register bit mapping */
#define ADDR_IRQ_IMR               (HW_IRQ_BASE + 0x0000)
#define ADDR_IRQ_IMR1              (HW_IRQ_BASE + 0x0100)
#define ADDR_IRQ_IMR2              (HW_IRQ_BASE + 0x0180)

#define ADDR_IRQ_IMR_PHYS          (BCM21553_INTC_BASE + 0x0000)

/* Register bit defines */
#define IMR_WFI_EN                 0x00008000

#endif /*__REG_BCM215XX_IRQ_H__*/
