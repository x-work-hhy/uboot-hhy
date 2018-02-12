/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __COMMON_H_
#define __COMMON_H_	1


/******************************************************************************
 *				owl common api
 *****************************************************************************/
void owl_udelay(unsigned short time);
void owl_mdelay(unsigned short time);


/******************************************************************************
 *				owl serial api
 *****************************************************************************/
int owl_serial_init(void);
int owl_serial_setbrg(void);
int owl_serial_getc(void);
int owl_serial_putc(const char ch);
void owl_serial_puts(const char *s);
/*
 * owl serial console api
 */
int owl_console_init(int id, int baud);
int owl_console_putc(char c);
int owl_console_getc(void);
/*
 * owl serial print api
 */
void owl_printf(const char *fmt, ...);


/******************************************************************************
 *				owl i2c api
 * ***************************************************************************/
int owl_i2c_init(int bus_id);
int owl_i2c_transfer(unsigned char cmd_type,
			unsigned char chip_addr,
			unsigned char *addr,
			unsigned char addr_len,
			unsigned char *data,
			unsigned char data_len);

/******************************************************************************
 *				owl pmu api
 * ***************************************************************************/
int atc260x_early_init(void);

/******************************************************************************
 *				owl ddr api
 * ***************************************************************************/
//int owl_ddr_init(void);
int ddr_init(int mode);


#endif	/* __COMMON_H_ */
