/*
 * (C) Copyright 2016 wowotech
 *
 * wowo<wowo@wowotech.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/armv8/mmu.h>

#include "include/owl_common.h"

DECLARE_GLOBAL_DATA_PTR;

#define GPIOB_OUTEN		(0xe01b000c)
#define GPIOB_OUTDAT		(0xe01b0014)

#define SPS_BASE		(0xe01b0100)
#define CPU_VDD_CTL		(SPS_BASE+0x0028)
#define PAD_CTRL		(0xe01b0074)

#define DEBUG_LED1_BLUE		(9)	/* GPIOB9 */
#define DEBUG_LED2_YELLOW	(8)	/* GPIOB8 */
#define PAD_EN			(1)

static struct mm_region cubieboard7_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x80000000UL,
		.phys = 0x80000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/* List terminator */
		0,
	}
};

static void reboot_to_adfu(void)
{
	void (*call_adfu)(void);

	//call_adfu = (void (*)(void))0xffff5a00;
	call_adfu = (void (*)(void))0xffff0400;

	call_adfu();
	while (1);
}


/*
 * A simple debug function for early debug, in which,
 * we use two LEDs to display sixteen debug codes, from 0 to 3.
 * Using it, we can know, at least roughly, at where out code is run.
 */
void cubieboard7_early_debug(int debug_code)
{
	uint8_t val;

	val = debug_code & 0x1;
	setbits_le32(GPIOB_OUTEN, 1 << DEBUG_LED2_YELLOW);
	clrsetbits_le32(GPIOB_OUTDAT, 1 << DEBUG_LED2_YELLOW,
			val << DEBUG_LED2_YELLOW);

	val = (debug_code >> 1) & 0x1;
	setbits_le32(GPIOB_OUTEN, 1 << DEBUG_LED1_BLUE);
	clrsetbits_le32(GPIOB_OUTDAT, 1 << DEBUG_LED1_BLUE,
			val << DEBUG_LED1_BLUE);
}

struct mm_region *mem_map = cubieboard7_mem_map;

#ifdef CONFIG_SPL_BUILD

/*
 * called from C runtime startup code (arch/arm/lib/crt0_64.S:_main)
 * - we have a stack and a place to store GD, both in SRAM.
 * BSS is not available, so you cannot use uninitialized global/static variables.
 */
void board_init_f(ulong bootflag)
{
	/* all of the pad is enabled, or they are all in high-z state */
	setbits_le32(PAD_CTRL, 1 << PAD_EN);
	/* enable pad_en 0~5 (GPIOE2-3/GPIOD28-31) */
	clrsetbits_le32(CPU_VDD_CTL, 0x3f0, 0x3f0);
	
	/* 
	 * init serial console.
	 */
	owl_serial_init();
 	owl_printf("%s, owl_serial_init ok\n", __func__);

 	owl_i2c_init(0);
 	owl_printf("%s, owl i2c0 init ok\n", __func__);
	
 	atc260x_early_init();
 	owl_printf("%s, owl pmu init ok\n", __func__);
	
	cubieboard7_early_debug(3);
 	//reboot_to_adfu();
}

void board_boot_order(u32 *spl_boot_list)
{
#if 0
	spl_boot_list[0] = BOOT_DEVICE_MMC2;
	spl_boot_list[1] = BOOT_DEVICE_MMC1;
#endif
}

void board_init_r(gd_t *new_gd, ulong dest_addr)
{
}
void panic(const char *fmt, ...)
{
}

#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
	cubieboard7_early_debug(2);
	while (1);

	return 0;
}
#endif

int board_init(void)
{
	cubieboard7_early_debug(2);
	return 0;
}

#if 0
struct serial_device *default_serial_console(void)
{
	return NULL;
}
#endif

void reset_cpu(ulong addr)
{
}

extern void s900_ddr_init(void);
int dram_init(void)
{
	printf("dram_init\n");

	//cubieboard7_early_debug(11);

	/* no need do dram init in here, we have done it in SPL */

	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	printf("dram_init OK\n");
	return 0;
}

#if defined(CONFIG_SYS_DRAM_TEST)
int testdram (void)
{
	uint32_t *pstart = (uint32_t *)CONFIG_SYS_MEMTEST_START;
	uint32_t *pend = (uint32_t *)CONFIG_SYS_MEMTEST_END;
	uint32_t *p;

	printf("SDRAM test phase 1:\n");
	printf("write...\n");
	for (p = pstart; p < pend; p++)
		*p = 0xaaaaaaaa;

	printf("read back and check...\n");
	for (p = pstart; p < pend; p++) {
		if (*p != 0xaaaaaaaa) {
			printf ("SDRAM test fails at: %08x=%08x\n", (uint32_t)p, *p);
			while (1);
		}
	}

	printf("SDRAM test phase 2:\n");
	printf("write...\n");
	for (p = pstart; p < pend; p++)
		*p = 0x55555555;

	printf("read back and check...\n");
	for (p = pstart; p < pend; p++) {
		if (*p != 0x55555555) {
			printf ("SDRAM test fails at: %08x=%08x\n", (uint32_t)p, *p);
			while (1);
		}
	}

	printf("SDRAM test passed.\n");
	return 0;
}
#endif

int board_run_command(const char *cmdline)
{
	printf("## Commands are disabled. Please enable CONFIG_CMDLINE.\n");

	return 1;
}

int print_cpuinfo(void)
{
	return 0;
}
