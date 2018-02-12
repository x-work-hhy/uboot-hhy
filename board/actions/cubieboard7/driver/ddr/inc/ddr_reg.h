/*
 * (C) Copyright 2012
 * Actions Semi .Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_DDR_REG_H
#define __ASM_DDR_REG_H

//#define CONFIG_GL5206
#define CONFIG_GL5211
//...
/****************** Macro  project *********************/
#if defined (CONFIG_GL5206)
	#define	PCTL0_BASE		0xb01C0000
	#define	PCTL1_BASE		0xb01D0000
	#define	PUBL0_BASE		0xb01c8000
	#define	PUBL1_BASE		0xb01d8000
	#define DMM_BASE      0xb0290000
#endif
#if defined (CONFIG_GL5211)
	#define	PCTL0_BASE		0xe01C0000
	#define	PCTL1_BASE		0xe01D0000
	#define	PUBL0_BASE		0xe01c8000
	#define	PUBL1_BASE		0xe01d8000
	#define DMM_BASE      0xe0290000	
#endif
/****************** PCTL  register *********************/

#define	SCFG		(0x00)
#define	SCTL		(0x04)
#define	STAT		(0x08)
#define	INTRSTAT		(0x0c)

#define	MCMD		    (0X40)
#define	POWCTL		  (0X44)
#define	POWSTAT		  (0X48)
#define	CMDTSTAT		(0X4C)
#define	CMDTSTATEN	(0X50)

#define	MRRCFG0		  (0X60)
#define	MRRSTAT0		(0X64)
#define	MRRSTAT1		(0X68)

#define	MCFG1		(0X7C)
#define	MCFG		(0X80)
#define	PPCFG		(0X84)
#define	MSTAT		(0X88)
#define	LPDDR2ZQCFG	(0X8C)

#define	DTUPDES		(0X94)
#define	DTUNA		  (0X98)
#define	DTUNE		  (0X9C)
#define	DTUPRD0		(0XA0)
#define	DTUPRD1		(0XA4)
#define	DTUPRD2		(0XA8)
#define	DTUPRD3		(0XAC)
#define	DTUAWDT		(0XB0)

#define	TOGCNT1U		(0XC0)
#define	TINIT		    (0XC4)
#define	TRSTH		    (0XC8)
#define	TOGCNT100N	(0XCC)
#define	TREFI		    (0XD0)
#define	TMRD		    (0XD4)
#define	TRFC		    (0XD8)
#define	TRP		      (0XDC)
#define	TRTW		    (0XE0)
#define	TAL		      (0XE4)
#define	TCL		      (0XE8)
#define	TCWL		    (0XEC)
#define	TRAS		    (0xF0)
#define	TRC		      (0xF4)
#define	TRCD		    (0xF8)
#define	TRRD		    (0xFC)
#define	TRTP		    (0x100)
#define	TWR		      (0x104)
#define	TWTR		    (0x108)
#define	TEXSR		    (0x10C)
#define	TXP		      (0x110)
#define	TXPDLL		  (0x114)
#define	TZQCS		    (0x118)
#define	TZQCSI		  (0x11C)
#define	TDQS		    (0X120)
#define	TCKSRE		  (0X124)
#define	TCKSRX		  (0X128)
#define	TCKE		    (0X12C)
#define	TMOD		    (0X130)
#define	TRSTL		    (0X134)
#define	TZQCL		    (0X138)
#define	TMRR		    (0X13C)
#define	TCKESR		  (0X140)
#define	TDPD		    (0X144)
#define TREFI_MEM   (0X148)

#define	ECCCFG		(0X180)
#define	ECCTST		(0X184)
#define	ECCCLR		(0X188)
#define	ECCLOG		(0X18C)


#define	DTUWACTL		(0x200)
#define	DTURACTL		(0x204)
#define	DTUCFG		(0x208)
#define	DTUECTL		(0x20C)
#define	DTUWD0		(0x210)
#define	DTUWD1		(0x214)
#define	DTUWD2		(0x218)
#define	DTUWD3		(0x21C)
#define	DTUWDM		(0X220)
#define	DTURD0		(0X224)
#define	DTURD1		(0X228)
#define	DTURD2		(0X22C)
#define	DTURD3		(0X230)
#define	DTULFSRWD	(0X234)
#define	DTULFSRRD	(0X238)
#define	DTUEAF		(0X23C)
#define	DFITCTRLDELAY	(0X240)
#define	DFIODTCFG	    (0X244)
#define	DFIODTCFG1	  (0X248)
#define	DFIODTRANKMAP	(0X24C)
#define	DFITPHYWRDATA	(0X250)
#define	DFITPHYWRLAT	(0X254)

#define	DFITPRDDATAEN	(0X260)
#define	DFITPHYRDLAT	(0X264)

#define	DFITPHYUPDTYPE0	(0X270)
#define	DFITPHYUPDTYPE1	(0X274)
#define	DFITPHYUPDTYPE2	(0X278)
#define	DFITPHYUPDTYPE3	(0X27C)
#define	DFITCTRLUPDMIN	(0X280)
#define	DFITCTRLUPDMAX	(0X284)
#define	DFITCTRLUPDDLY	(0X288)

#define	DFIUPDCFG		  (0X290)
#define	DFITREFMSKI	  (0X294)
#define	DFITCTRLUPDI	(0X298)

#define	DFITRCFG0		(0X2AC)
#define	DFITRSTAT0	(0X2B0)
#define	DFITRWRLVLEN	(0X2B4)
#define	DFITRRDLVLEN	(0X2B8)
#define	DFITRRDLVLGATEEN	(0X2BC)
#define	DFISTSTAT0	(0X2C0)
#define	DFISTCFG0		(0X2C4)
#define	DFISTCFG1		(0X2C8)

#define	DFITDRAMCLKEN	(0X2D0)
#define	DFITDRAMCLKDIS	(0X2D4)
#define	DFISTCFG2		(0X2D8)
#define	DFISTPARCLR	(0X2DC)
#define	DFISTPARLOG	(0X2E0)

#define	DFILPCFG0		(0X2F0)

#define	DFITRWRLVLRESP0	(0x300)
#define	DFITRWRLVLRESP1	(0x304)
#define	DFITRWRLVLRESP2	(0x308)
#define	DFITRRDLVLRESP0	(0x30C)
#define	DFITRRDLVLRESP1	(0x310)
#define	DFITRRDLVLRESP2	(0x314)
#define	DFITRWRLVLDELAY0	(0x318)
#define	DFITRWRLVLDELAY1	(0x31C)
#define	DFITRWRLVLDELAY2	(0X320)
#define	DFITRRDLVLDELAY0	(0X324)
#define	DFITRRDLVLDELAY1	(0X328)
#define	DFITRRDLVLDELAY2	(0X32C)
#define	DFITRRDLVLGATEDELAY0		(0X330)
#define	DFITRRDLVLGATEDELAY1		(0X334)
#define	DFITRRDLVLGATEDELAY2		(0X338)
#define	DFITRCMD				(0X33C)

#define	IPVR		(0X3F8)
#define	IPTR		(0X3FC)

/******************* PUBL  register *******************/
          
#define	    RIDR			              (0x00)
#define	    PIR			                (0x04)
#define	    PGCR			              (0x08)
#define	    PGSR			              (0x0c)
#define	    DLLGCR		              (0x10)
#define	    ACDLLCR		              (0x14)
#define	    PTR0			              (0x18)
#define	    PTR1			              (0x1c)
#define	    PTR2			              (0x20)
#define	    ACIOCR		              (0x24)
#define	    DXCCR			              (0x28)
#define	    DSGCR			              (0x2c)
#define	    DCR			                (0x30)
#define	    DTPR0			              (0x34)
#define	    DTPR1			              (0x38)
#define	    DTPR2			              (0x3c)
#define	    MR0			                (0x40)
#define	    MR1			                (0x44)
#define	    MR2			                (0x48)
#define	    MR3			                (0x4c)
#define	    ODTCR			              (0x50)
#define	    DTAR			              (0x54)
#define	    DTDR0			              (0x58)
#define	    DTDR1			              (0x5c)
                                    
#define	    DCUAR			              (0xc0)
#define	    DCUDR			              (0xc4)
#define	    DCURR			              (0xc8)
#define	    DCULR			              (0xcc)
#define	    DCUGCR		              (0xd0)
#define	    DCUTPR		              (0xd4)
#define	    DCUSR0		              (0xd8)
#define	    DCUSR1	                (0xdc)
                                    
#define	    BISTRR		              (0x100)
#define	    BISTMSKR0	              (0x104)
#define	    BISTMSKR1	              (0x108)
#define	    BISTWCR		              (0x10c)
#define	    BISTLSR		              (0x110)
#define	    BISTAR0		              (0x114)
#define	    BISTAR1		              (0x118)
#define	    BISTAR2		              (0x11c)
#define	    BISTUDPR	              (0x120)
#define	    BISTGSR		              (0x124)
#define	    BISTWER		              (0x128)
#define	    BISTBER0	              (0x12c)
#define	    BISTBER1	              (0x130)
#define	    BISTBER2	              (0x134)
#define	    BISTWCSR	              (0x138)
#define	    BISTFWR0	              (0x13c)
#define	    BISTFWR1	              (0x140)
            
#define	    ZQ0CR0			            (0x180)
#define	    ZQ0CR1			            (0x184)
#define	    ZQ0SR0			            (0x188)
#define	    ZQ0SR1			            (0x18c)
#define	    ZQ1CR0			            (0x190)
#define	    ZQ1CR1			            (0x194)
#define	    ZQ1SR0			            (0x198)
#define	    ZQ1SR1			            (0x19c)
#define	    ZQ2CR0			            (0x1a0)
#define	    ZQ2CR1			            (0x1a4)
#define	    ZQ2SR0			            (0x1a8)
#define	    ZQ2SR1			            (0x1ac)
#define	    ZQ3CR0			            (0x1b0)
#define	    ZQ3CR1			            (0x1b4)
#define	    ZQ3SR0			            (0x1b8)
#define	    ZQ3SR1			            (0x1bc)
                                    
#define	    DX0GCR			            (0x1c0)
#define	    DX0GSR0			            (0x1c4)
#define	    DX0GSR1			            (0x1c8)
#define	    DX0DLLCR		            (0x1cc)
#define	    DX0DQTR			            (0x1d0)
#define	    DX0DQSTR		            (0x1d4)
            
#define	    DX1GCR		              (0x200)
#define	    DX1GSR0		              (0x204)
#define	    DX1GSR1		              (0x208)
#define	    DX1DLLCR	              (0x20c)
#define	    DX1DQTR		              (0x210)
#define	    DX1DQSTR	              (0x214)
            
#define	    DX2GCR			            (0x240)
#define	    DX2GSR0			            (0x244)
#define	    DX2GSR1			            (0x248)
#define	    DX2DLLCR		            (0x24c)
#define	    DX2DQTR			            (0x250)
#define	    DX2DQSTR		            (0x254)
                        
#define	    DX3GCR			            (0x280)
#define	    DX3GSR0			            (0x284)
#define	    DX3GSR1			            (0x288)
#define	    DX3DLLCR		            (0x28c)
#define	    DX3DQTR			            (0x290)
#define	    DX3DQSTR		            (0x294)
#define	    LPDDR3_FIX              (0x3FC)

/******************* DMM  register *******************/

#define     DMM_SCH_CTRL             (DMM_BASE+0x0000)
#define     DMM_TO_CPU_SI            (DMM_BASE+0x0004)
#define     DMM_TO_VDE               (DMM_BASE+0x0008)
#define     DMM_TO_ETHERNET_DE       (DMM_BASE+0x000C)
#define     DMM_TO_HDE_USB2_0        (DMM_BASE+0x0010)
#define     DMM_TO_USB3_VCE          (DMM_BASE+0x0014)
#define     DMM_TO_GPU_0_DAP         (DMM_BASE+0x0018)
#define     DMM_TO_DMA_GPU_1         (DMM_BASE+0x001C)
#define     DMM_INTERLEAVE_BASEADDR  (DMM_BASE+0x0020)
#define     DMM_INTERLEAVE_FRAGMENT  (DMM_BASE+0x0024)
#define     DMM_INTERLEAVE_CONFIG    (DMM_BASE+0x0028)
#define     DMM_AXI_PRIORITY         (DMM_BASE+0x002C)
#define     DMM_WATCH_ADDR           (DMM_BASE+0x0030)
#define     DMM_WATCH_ADDR_MASK      (DMM_BASE+0x0034)
#define     DMM_WATCH_DATA           (DMM_BASE+0x0038)
#define     DMM_WATCH_DATA_MASK      (DMM_BASE+0x003C)
#define     DMM_WATCH_CTRL           (DMM_BASE+0x0040)
#define     DMM_MONITOR_CTRL         (DMM_BASE+0x0044)
#define     DMM_PM_CTRL0             (DMM_BASE+0x0048)
#define     DMM_PC0                  (DMM_BASE+0x0050)
#define     DMM_PC1                  (DMM_BASE+0x0054)
#define     DMM_DEBUG                (DMM_BASE+0x0060)
#define     PHY_RST                  (DMM_BASE+0x0064)
#define     DMM_TO_ENABLE            (DMM_BASE+0x0068)
#define     DMM_SCH_STATUS           (DMM_BASE+0x006c)
#define     DMM_WRCMD_THRESHOLD      (DMM_BASE+0x0070)
#define     DMM_CMD_TIMEOUT_CNT      (DMM_BASE+0x0074)
#define     DMM_CPU_CMD_CNT          (DMM_BASE+0x0078)
#define     DMM_CPU_TO_PRIORITY      (DMM_BASE+0x007C)
#define     DMM_DDR_TIMING           (DMM_BASE+0x0080)
#define     DMM_HDR                  (DMM_BASE+0x0088)
#define     DMM_CPU_LOWPRIORITY_CNT  (DMM_BASE+0x008c)
#define     DMM_NIC_LOWPRIORITY_CNT  (DMM_BASE+0x0090)
#define     DMM_SECURE_CTRL          (DMM_BASE+0x0100)
#define     DMM_SECURE_BASE          (DMM_BASE+0x0104)
#define     DMM_SECURE_LEN           (DMM_BASE+0x0108)
#define     DMM_SECURE_WR_MODE       (DMM_BASE+0x010c)
#define     DMM_SECURE_RD_MODE       (DMM_BASE+0x0110)
#define     DMM_NON_SECURE_WR_INF    (DMM_BASE+0x0114)
#define     DMM_NON_SECURE_WR_ADDR   (DMM_BASE+0x0118)
#define     DMM_NON_SECURE_RD_INF    (DMM_BASE+0x011c)
#define     DMM_NON_SECURE_RD_ADDR   (DMM_BASE+0x0120)

#endif

