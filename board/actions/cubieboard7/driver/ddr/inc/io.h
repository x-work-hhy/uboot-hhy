/*
 *  linux/include/asm-arm/io.h
 *
 *  Copyright (C) 1996-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Modifications:
 *  16-Sep-1996	RMK	Inlined the inx/outx functions & optimised for both
 *			constant addresses and variable addresses.
 *  04-Dec-1997	RMK	Moved a lot of this stuff to the new architecture
 *			specific IO header files.
 *  27-Mar-1999	PJB	Second parameter of memcpy_toio is const..
 *  04-Apr-1999	PJB	Added check_signature.
 *  12-Dec-1999	RMK	More cleanups
 *  18-Jun-2000 RMK	Removed virt_to_* and friends definitions
 */
#ifndef __IO_H_
#define __IO_H_

#define writel(val,reg)		  *(volatile unsigned int*)(reg)  = (val)
#define readl(reg)			   (*(volatile unsigned int*)(reg)) 
#define writew(val,reg)		  *(volatile unsigned short*)(reg)  = (val)
#define readw(reg)			   (*(volatile unsigned short*)(reg)) 
#define IO_OR_U32(reg,val)  *(volatile unsigned int*)(reg) |= (val)
#define IO_AND_U32(reg,val) *(volatile unsigned int*)(reg) &= (val)
#define clrbits_le32(addr, clear) IO_AND_U32(addr,(~(clear)))
#define setbits_le32(addr, set) IO_OR_U32(addr,set)
#define clrsetbits_le32(addr, clear, set) (IO_AND_U32(addr,(~(clear))) | set)
#define IO_WRITEU32(reg,val) *(volatile unsigned int*)(reg) = (val)
#define IO_READU32(reg) (*(volatile unsigned int*)(reg))

#endif