/*
 * Copyright (c) 2015 Actions Semi Co., Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
//#include <fdtdec.h>
#include <serial.h>

DECLARE_GLOBAL_DATA_PTR;

#define USING_UART5

#define HOSC_FREQ		(24000000)

#define CMU_DEVCLKEN1		(0xe01680a4)
#define CMU_DEVRST1		(0xe01680ac)
#define CMU_UART3CLK		(0xe0168068)
#define CMU_UART5CLK		(0xe0168070)

#define MFP_CTL2		(0xe01b0048)


#define UART3_BASE		(0xe0126000)
#define UART5_BASE		(0xe012a000)

#ifdef USING_UART3
#define UART_BASE		UART3_BASE
#else
#define UART_BASE		UART5_BASE
#endif

#define GPIOD_OUTEN		(0xe01b0024)
#define GPIOD_INEN		(0xe01b0028)

/* UART	Register offset	*/
#define	UART_CTL		(0x0)
#define	UART_RXDAT		(0x4)
#define	UART_TXDAT		(0x8)
#define	UART_STAT		(0xc)

/* UART_CTL */
#define	UART_CTL_TX_IRQ_EN	(0x1 <<	19)	/* UART tx irq enable */
#define	UART_CTL_RX_IRQ_EN	(0x1 <<	18)	/* UART rx irq enable */
#define	UART_CTL_EN		(0x1 <<	15)	/* UART enable */
#define	UART_CTL_AFC		(0x1 <<	12)	/* Auto flow control */
#define	UART_CTL_PARITY		(0x7 <<	4)	/* Parity */
#define	UART_CTL_STOP		(0x1 <<	2)	/* stop bit */
#define	UART_CTL_DATA_WIDTH	(0x3 <<	0)	/* Data width */

#define UART_PARITY_NONE	(0)
#define UART_PARITY_ODD		(4)
#define UART_PARITY_LOGIC1	(5)
#define UART_PARITY_EVEN	(6)
#define UART_PARITY_LOGIC0	(7)

#define UART_DATA_WIDTH_5	(0)
#define UART_DATA_WIDTH_6	(1)
#define UART_DATA_WIDTH_7	(2)
#define UART_DATA_WIDTH_8	(3)


/* UART_STAT */
#define	UART_STAT_TFES		(0x1 <<	10)	/* TX FIFO Empty Status	*/
#define	UART_STAT_RFFS		(0x1 <<	9)	/* RX FIFO full	Status */
#define	UART_STAT_TFFU		(0x1 <<	6)	/* TX FIFO full	Status */
#define	UART_STAT_RFEM		(0x1 <<	5)	/* RX FIFO Empty Status	*/

int owl_dm_serial_setbrg(struct udevice *dev, int baudrate)
{
	int divider;

	divider = (115200 * 8);
	divider = (HOSC_FREQ + divider / 2) / divider;
	if (divider > 0)
		divider--;

#ifdef USING_UART3
	clrsetbits_le32(CMU_UART3CLK, 0x1ff, divider);
	clrsetbits_le32(CMU_UART3CLK, 1 << 16, 0);
#else
	clrsetbits_le32(CMU_UART5CLK, 0x1ff, divider);
	clrsetbits_le32(CMU_UART5CLK, 1 << 16, 0);
#endif

	/*
	 * 8N1
	 */
	clrsetbits_le32(UART_BASE + UART_CTL, UART_CTL_DATA_WIDTH,
			UART_DATA_WIDTH_8);
	clrsetbits_le32(UART_BASE + UART_CTL, UART_CTL_PARITY,
			UART_PARITY_NONE);
	clrbits_le32(UART_BASE + UART_CTL, UART_CTL_STOP);


	return 0;
}

static int owl_dm_serial_getc(struct udevice *dev)
{

	if (readl(UART_BASE + UART_STAT) & UART_STAT_RFEM)
		return -EAGAIN;

	return (int)(readl(UART_BASE + UART_RXDAT));
}

static int owl_dm_serial_putc(struct udevice *dev, const char ch)
{

	/* wait TX FIFO not full */
	while (readl(UART_BASE + UART_STAT) & UART_STAT_TFFU)
		;
	
	writel(ch, UART_BASE + UART_TXDAT);

	/*  drain out of TX FIFO */
	while (!(readl(UART_BASE + UART_STAT) & UART_STAT_TFES))
		;
	
	return 0;
}

static int owl_dm_serial_pending(struct udevice *dev, bool input)
{
	unsigned int stat = readl(UART_BASE + UART_STAT);

	if (input)
		return !(stat &	UART_STAT_RFEM);
	else
		return !(stat &	UART_STAT_TFES);
}

extern void cubieboard7_early_debug(int debug_code);
static int owl_serial_probe(struct udevice *dev)
{
	cubieboard7_early_debug(0);

	/* pinmux */
#ifdef USING_UART3

#else
	/* uart5 */
	clrsetbits_le32(MFP_CTL2, 0x7 << 14, 0x5 << 14);
	clrsetbits_le32(MFP_CTL2, 0x7 << 17, 0x5 << 17);
#endif

	cubieboard7_early_debug(1);

	/* device clock enable */
#ifdef USING_UART3
	setbits_le32(CMU_DEVCLKEN1, 1 << 11);	/* uart3 */
#else
	setbits_le32(CMU_DEVCLKEN1, 1 << 13);	/* uart5 */
#endif

	cubieboard7_early_debug(2);

	/* reset de-assert */
#ifdef USING_UART3
	setbits_le32(CMU_DEVRST1, 1 << 11);
#else
	setbits_le32(CMU_DEVRST1, 1 << 13);
#endif

	cubieboard7_early_debug(3);

	/* set default baudrate and enable UART */
	owl_dm_serial_setbrg(dev, 115200);

	/* enable uart */
	setbits_le32(UART_BASE + UART_CTL, UART_CTL_EN);

	return 0;
}

static int owl_serial_init(void)
{
 	return owl_serial_probe(NULL);
}

static int owl_serial_tstc(void)
{
	return 0;
}

static int owl_serial_setbrg(void)
{
	return owl_dm_serial_setbrg(NULL, 115200);
}

static int owl_serial_putc(const char ch)
{
	return owl_dm_serial_putc(NULL, ch);
}

static int owl_serial_getc(void)
{
 	return owl_dm_serial_getc(NULL);
}

static struct serial_device owl_serial_device = {
	.name	= "owl_serial",
	.start	= owl_serial_init,
	.stop	= NULL,
	.setbrg	= owl_serial_setbrg,
	.putc	= owl_serial_putc,
	.puts	= default_serial_puts,
	.getc	= owl_serial_getc,
	.tstc	= owl_serial_tstc,
};

/* register to U-boot serial core */
void owl_serial_initialize(void)
{
	serial_register(&owl_serial_device);
}

struct serial_device *default_serial_console(void)
{
	return &owl_serial_device;
}

static const struct dm_serial_ops owl_serial_ops = {
	.putc =	owl_dm_serial_putc,
	.pending = owl_dm_serial_pending,
	.getc =	owl_dm_serial_getc,
	.setbrg	= owl_dm_serial_setbrg,
};

U_BOOT_DRIVER(serial_owl) = {
	.name	= "serial_owl",
	.id	= UCLASS_SERIAL,
	.probe  = owl_serial_probe,
	.ops	= &owl_serial_ops,
	.flags  = DM_FLAG_PRE_RELOC,
};

/* TODO */
U_BOOT_DEVICE(serial_owl) = {
	.name = "serial_owl",
};

