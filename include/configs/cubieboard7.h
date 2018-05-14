/*
 * (C) Copyright 2016 wowotech 
 *
 * wowowowo@wowotech.net 
 *
 * Configuration for cubieboard7. 
 *
 * SPDX-License-Identifier:    GPL-2.0+ 
 **/ 
#ifndef __CUBIEBOARD7_H 
#define __CUBIEBOARD7_H 
#define DEBUG

/*
 * u-boot SPL definitions, which is resided in SRAM
 */
#define CONFIG_SPL_TEXT_BASE		0xe4063200
#define CONFIG_SPL_MAX_SIZE		(1024 * 30)

#define CONFIG_SPL_BSS_START_ADDR	(CONFIG_SPL_TEXT_BASE + \
					 CONFIG_SPL_MAX_SIZE)
#define CONFIG_SPL_BSS_MAX_SIZE		(1024 * 16)

#define CONFIG_SPL_STACK		0xe4071000


/*
 * SDRAM Definitions
 */

#define CONFIG_SYS_SDRAM_BASE		0x0
#define CONFIG_NR_DRAM_BANKS		1

#define CONFIG_SYS_SDRAM_SIZE		0x80000000	/* 2GB */

/*
 * u-boot definitions, which is resided in SDRAM, TODO
 */
#define CONFIG_SYS_TEXT_BASE		0x11000000	
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE + 0x7ff00

/* Some commands use this as the default load address, TODO */
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x7ffc0)

/* for DRAM test */
/* #define CONFIG_SYS_DRAM_TEST */

#define CONFIG_SYS_MEMTEST_START	0x0	
#define CONFIG_SYS_MEMTEST_END		CONFIG_SYS_MEMTEST_START + 2048

/* Generic Interrupt Controller Definitions */
#define CONFIG_GICV2

#define GICD_BASE			(0xe00f1000)
#define GICC_BASE			(0xe00f2000)

/* Do not preserve environment */
#define CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_ENV_SIZE			0x1000	/* 4K Bytes */

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * 1024 + CONFIG_ENV_SIZE)

#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max command args */

/* UART Definitions */
#define CONFIG_BAUDRATE			115200

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_CMD_BOOTI
/* #define CONFIG_SPL_FRAMEWORK */

#define CONFIG_BOOTARGS                 "earlycon=owl_serial " \
					"console=ttyS5 " \
					"root=/dev/ram0 " \
					"init=/sbin/init " \
					"loglevel=8"

#define CONFIG_BOOTCOMMAND		"bootm 0x6400000"

#define COUNTER_FREQUENCY		(24000000)      /* 24MHz */
#endif
