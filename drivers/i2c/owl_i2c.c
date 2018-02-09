/*
 * Copyright (c) 2015 Actions Semi Co., Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
//#include <fdtdec.h>
#include <i2c.h>
#include <asm/io.h>
#if 0
#include <asm/arch/clk.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/periph.h>
#include <asm/arch/reset.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define OWL_I2C_DBG
#ifdef OWL_I2C_DBG
#define i2c_dbg(fmt, args...)	printf(fmt, ##args)
#define i2c_warn(fmt, args...)	printf(fmt, ##args)
#else
#define i2c_dbg(fmt, args...)
#define i2c_warn(fmt, args...)
#endif

#define i2c_err(fmt, args...)	printf(fmt, ##args)


/* i2c0 register */
#define CMU_DEVCLKEN1		(0xe01680a4)
#define CMU_DEVRST1		(0xe01680ac)
#define MFP_CTL3		(0xe01b004c)

#define I2C0_BASE		(0xe0170000)
#define I2C_CTL			(0x0000)
#define I2C_CLKDIV		(0x0004)
#define I2C_STAT		(0x0008)
#define I2C_ADDR		(0x000c)
#define I2C_TXDAT		(0x0010)
#define I2C_RXDAT		(0x0014)
#define I2C_CMD			(0x0018)
#define I2C_FIFOCTL		(0x001c)
#define I2C_FIFOSTAT		(0x0020)
#define I2C_DATCNT		(0x0024)
#define I2C_RCNT		(0x0028)

enum i2c_cmd_type {
	I2C_WRITE,
	I2C_READ,
};

struct owl_i2c_ctlr {
	u32 ctl;		/* I2C Control Register */
	u32 clkdiv;		/* I2C Clk Divide Register */
	u32 stat;		/* I2C Status Register */
	u32 addr;		/* I2C Address Register */
	u32 txdat;		/* I2C TX Data Register */
	u32 rxdat;		/* I2C RX Data Register */
	u32 cmd;		/* I2C Command Register */
	u32 fifoctl;		/* I2C FIFO control Register */
	u32 fifostat;		/* I2C FIFO status Register */
	u32 datcnt;		/* I2C Data transmit counter */
	u32 rcnt;		/* I2C Data transmit remain counter */

};

struct owl_i2c_bus {
	int			id;
	int			periph_id;
	int			speed;
	int			mfp_config;
	struct owl_i2c_ctlr	*regs;
};

/* i2c0 initialise */
struct owl_i2c_ctlr i2c0_ctrl = {
	.ctl		= I2C0_BASE + I2C_CTL,
	.clkdiv		= I2C0_BASE + I2C_CLKDIV, 
	.stat		= I2C0_BASE + I2C_STAT,
	.addr		= I2C0_BASE + I2C_ADDR,
	.txdat		= I2C0_BASE + I2C_TXDAT,
	.rxdat		= I2C0_BASE + I2C_RXDAT,
	.cmd		= I2C0_BASE + I2C_CMD,
	.fifoctl	= I2C0_BASE + I2C_FIFOCTL,
	.fifostat	= I2C0_BASE + I2C_FIFOSTAT,
	.datcnt		= I2C0_BASE + I2C_DATCNT,
	.rcnt		= I2C0_BASE + I2C_RCNT,
};

struct owl_i2c_bus i2c0_bus = {
	.id		= 0,
	.speed		= 400000,	/* 400kHz */
	.regs		= &i2c0_ctrl,
};

/* ===============I2Cx_CLKDIV =====================*/
#define I2C_CLKDIV_DIV_MASK		(0xff << 0)
#define I2C_CLKDIV_DIV(x)		(((x) & 0xff) << 0)
#define I2C_CLKDIV_HDIV_MASK		(0xff << 8)
#define I2C_CLKDIV_HDIV(x)		(((x) & 0xff) << 8)
#define I2C_CLKDIV_CLKCOM(x)		(((x) & 0x3) << 16)
#define	I2C_CLKDIV_CLKCOM_0NS		I2C_CLKDIV_CLKCOM(0)
#define	I2C_CLKDIV_CLKCOM_10NS		I2C_CLKDIV_CLKCOM(1)
#define	I2C_CLKDIV_CLKCOM_20NS		I2C_CLKDIV_CLKCOM(2)

/* ================I2Cx_STAT======================= */
#define I2C_STAT_RACK			(0x1 << 0)
#define I2C_STAT_BEB			(0x1 << 1)
#define I2C_STAT_IRQP			(0x1 << 2)
#define I2C_STAT_LAB			(0x1 << 3)
#define I2C_STAT_STPD			(0x1 << 4)
#define I2C_STAT_STAD			(0x1 << 5)
#define I2C_STAT_BBB			(0x1 << 6)
#define I2C_STAT_TCB			(0x1 << 7)
#define I2C_STAT_LBST			(0x1 << 8)
#define I2C_STAT_SAMB			(0x1 << 9)
#define I2C_STAT_SRGC			(0x1 << 10)

#define I2C_BUS_ERR_MSK			(I2C_STAT_LAB | I2C_STAT_BEB)

/* ============I2Cx_CMD ===============*/
#define I2C_CMD_SBE			(0x1 << 0)
/* Start bit enable */
#define I2C_CMD_AS_MASK			(0x7 << 1)
/* Address select */
#define I2C_CMD_AS(x)			(((x) & 0x7) << 1)
#define I2C_CMD_RBE			(0x1 << 4)
/* Restart bit enable */
#define I2C_CMD_SAS_MASK		(0x7 << 5)
/* Second Address select */
#define I2C_CMD_SAS(x)			(((x) & 0x7) << 5)
#define I2C_CMD_DE			(0x1 << 8)
/* Data enable */
#define I2C_CMD_NS			(0x1 << 9)
/* NACK select */
#define I2C_CMD_SE			(0x1 << 10)
/* Stop enable */
#define I2C_CMD_MSS			(0x1 << 11)
/* MSS Master or slave mode select */
#define I2C_CMD_WRS			(0x1 << 12)
/* Write or Read select */
#define I2C_CMD_EXEC			(0x1 << 15)
/* Start to execute the command list */
/*FIFO mode write cmd 0x8d01 */
#define I2C_CMD_X	(\
	I2C_CMD_EXEC | I2C_CMD_MSS | \
	I2C_CMD_SE | I2C_CMD_DE | I2C_CMD_SBE)


/* ==========I2Cx_FIFOSTAT============== */
#define I2C_FIFOSTAT_CECB		(0x1 << 0)
/* command Execute Complete bit */
#define I2C_FIFOSTAT_RNB		(0x1 << 1)
/* Receive NACK Error bit */
#define I2C_FIFOSTAT_RFE		(0x1 << 2)
/* RX FIFO empty bit */
#define I2C_FIFOSTAT_RFF		(0x1 << 3)
/* RX FIFO full bit */
#define I2C_FIFOSTAT_TFE		(0x1 << 4)
/* TX FIFO empty bit */
#define I2C_FIFOSTAT_TFF		(0x1 << 5)
/* TX FIFO full bit */
#define I2C_FIFOSTAT_RFD_MASK		(0xff << 8)
/* Rx FIFO level display */
#define I2C_FIFOSTAT_RFD_SHIFT		(8)
#define I2C_FIFOSTAT_TFD_MASK		(0xff << 16)
/* Tx FIFO level display */
#define I2C_FIFOSTAT_TFD_SHIFT		(16)


#define OWL_I2C_TIMEOUT_US		(100 * 1000)	/* 0.1s */


static void owl_i2c_dump_regs(struct owl_i2c_ctlr *i2c)
{
	i2c_dbg("dump phys %x regs:\n"
		"  ctl:      %x  clkdiv: %x  stat:    %x\n"
		"  addr:     %x  cmd:    %x  fifoctl: %x\n"
		"  fifostat: %x  datcnt: %x  rcnt:    %x\n",
		(unsigned int)(&i2c->ctl),
		readl(&i2c->ctl),
		readl(&i2c->clkdiv),
		readl(&i2c->stat),
		readl(&i2c->addr),
		readl(&i2c->cmd),
		readl(&i2c->fifoctl),
		readl(&i2c->fifostat),
		readl(&i2c->datcnt),
		readl(&i2c->rcnt));
}

static int owl_i2c_wait_if_busy(struct owl_i2c_ctlr *i2c)
{
	unsigned long timeout = OWL_I2C_TIMEOUT_US;

	while (readl(&i2c->stat) & I2C_STAT_BBB) {
		if (timeout == 0) {
			i2c_err("Bus busy timeout, stat 0x%x\n",
				readl(&i2c->stat));
			owl_i2c_dump_regs(i2c);
			return -1;
		}
		udelay(1);
		timeout--;
	}

	return 0;
}

static int check_transfer_error(struct owl_i2c_ctlr *i2c)
{
	unsigned int stat, fifostat;
	int ret = 0;

	stat = readl(&i2c->stat);
	fifostat = readl(&i2c->fifostat);

	if (fifostat & I2C_FIFOSTAT_RNB) {
		i2c_warn("no ACK from device");
		ret = -1;
	} else if (stat & I2C_STAT_LAB) {
		i2c_warn("lose arbitration");
		ret = -2;
	} else if (stat & I2C_STAT_BEB) {
		i2c_warn("bus error");
		ret = -3;
	}

	if (ret)
		owl_i2c_dump_regs(i2c);

	return ret;
}
static int owl_i2c_wait_fininshed(struct owl_i2c_ctlr *i2c)
{
	unsigned long timeout_us = OWL_I2C_TIMEOUT_US / 32;
	unsigned int fifostat;
	int ret = -1;

	while (timeout_us) {
		ret = check_transfer_error(i2c);
		/* found transfer error */
		if (ret)
			break;

		fifostat = readl(&i2c->fifostat);
		if (fifostat & I2C_FIFOSTAT_CECB)
			/* transfer over */
			break;

		udelay(32);
		timeout_us--;
	}

	if (timeout_us == 0) {
		i2c_err("transfer timed out\n");
		ret = -1;
	} else if (ret != 0) {
		i2c_err("transfer error\n");
		ret = -1;
	}

	return ret;
}
/*
 * cmd_type is 0 for write, 1 for read.
 *
 */
static int owl_i2c_transfer(struct owl_i2c_ctlr *i2c,
		 enum i2c_cmd_type  cmd_type,
		 unsigned char chip,
		 unsigned char addr[],
		 unsigned char addr_len,
		 unsigned char data[], unsigned char data_len)
{
	u32 i;
	u32 i2c_cmd;

	/* reset fifo */
	writel(0x06, &i2c->fifoctl);
	while(readl(&i2c->fifoctl) & 0x06);

	/* Make sure the hardware is ready */
	if (owl_i2c_wait_if_busy(i2c) < 0) {
		i2c_warn("bus busy before transfer\n");
		return -1;
	}

	switch (cmd_type) {
	case I2C_WRITE:
		/*1, enable i2c ,not enable interrupt*/
		writel(0x80, &i2c->ctl);
		/*2, write data count*/
		writel(data_len, &i2c->datcnt);
		/*3, write slave addr*/
		writel((chip << 1), &i2c->txdat);
		/*4, write register addr*/
		for (i = 0; i < addr_len; i++)
			writel(addr[i], &i2c->txdat);
		/*5, write data*/
		for (i = 0; i < data_len; i++)
			writel(data[i], &i2c->txdat);
		/*6, write fifo command */
		i2c_cmd = I2C_CMD_X | I2C_CMD_AS((addr_len) + 1);
		writel(i2c_cmd, &i2c->cmd);
		readl(&i2c->cmd);
		/* wait command complete */
		if (owl_i2c_wait_fininshed(i2c))
			goto label_err;
		break;

	case I2C_READ:
		/*1, enable i2c ,not enable interrupt*/
		writel(0x80, &i2c->ctl);
		/*2, write data count*/
		writel(data_len, &i2c->datcnt);
		/*3, write slave addr*/
		writel((chip << 1), &i2c->txdat);
		/*4, write register addr*/
		for (i = 0; i < addr_len; i++)
			writel(addr[i], &i2c->txdat);
		/*5, write slave addr | read_flag*/
		writel((chip << 1) | I2C_READ, &i2c->txdat);
		/*6, write fifo command */
		i2c_cmd = I2C_CMD_X | I2C_CMD_RBE | I2C_CMD_NS | I2C_CMD_SAS(1)
					| I2C_CMD_AS(1 + addr_len);
		writel(i2c_cmd, &i2c->cmd);
				/* wait command complete */
		if (owl_i2c_wait_fininshed(i2c))
			goto label_err;
		/*8, Read data from rxdata*/
		i2c_dbg("i2c_read_data: ");
		for (i = 0; i < data_len; i++) {
			data[i] = readl(&i2c->rxdat);
			i2c_dbg(" 0x%02x", data[i]);
		}
		i2c_dbg("\n");
		break;

	default:
		i2c_dbg("i2c_transfer: bad call\n");
		return -1;
		break;
	}
	/*disable i2c */
	writel(0x0, &i2c->ctl);

	return 0;
label_err:
	/* clear err bit */
	writel(I2C_FIFOSTAT_RNB, &i2c->fifostat);

	/* reset fifo */
	writel(0x06, &i2c->fifoctl);
	while(readl(&i2c->fifoctl) & 0x06);

	/* disable i2c controller */
	writel(0x0, &i2c->ctl);

	return -1;
}

static void owl_i2c_set_freq(struct owl_i2c_bus *i2c_bus)
{
	unsigned int pclk, div_factor;

	if (i2c_bus->speed == 0)
		i2c_bus->speed = 400000;
	
	pclk = 24000000;
	
	printf("i2c%d, clk=%d\n", i2c_bus->id, i2c_bus->speed);
	
	div_factor = (pclk + i2c_bus->speed * 16 - 1) / (i2c_bus->speed * 16);
	
	writel(I2C_CLKDIV_DIV(div_factor), &i2c_bus->regs->clkdiv);
}

static void owl_i2c_init(struct owl_i2c_bus *i2c_bus)
{
	owl_clk_enable_by_perip_id(i2c_bus->periph_id);
	owl_reset_by_perip_id(i2c_bus->periph_id);
	owl_i2c_set_freq(i2c_bus);
	pinmux_select(i2c_bus->periph_id, i2c_bus->mfp_config);
}

static int owl_i2c_probe(struct udevice *dev)
{
	/* pinmux */
	clrsetbits_le32(MFP_CTL3, 0x7 << 16, 0);
	
	/* device clock enable */
	setbits_le32(CMU_DEVCLKEN1, 1 << 0);	/* i2c0 */
	
	/* reset de-assert */
	setbits_le32(CMU_DEVRST1, 1 << 0);

	i2c_dbg("i2c: controller bus %d at %p, speed %d:\n",
		i2c0_bus.id, i2c0_bus.regs->ctl, i2c0_bus.speed);

	return 0;
}


static int owl_i2c_set_bus_speed(struct udevice *dev, unsigned int speed)
{
	i2c0_bus.speed = speed;
	owl_i2c_set_freq(&i2c0_bus);

	return 0;
}

static int owl_i2c_xfer(struct udevice *bus, struct i2c_msg *msg,
			  int nmsgs)
{
	int ret;
	u8 *paddr = NULL;
	uint alen = 0;
	enum i2c_cmd_type cmd;

	i2c_dbg("i2c_xfer: %d messages\n", nmsgs);
	
	if (nmsgs >= 2) {
		/* write internal register address */
		paddr = msg->buf;
		alen = msg->len;
		msg++;
		nmsgs--;
	}

	for (; nmsgs > 0; nmsgs--, msg++) {
		i2c_dbg("i2c_xfer: chip=0x%x,len=0x%x\n", msg->addr, msg->len);
		if (msg->flags & I2C_M_RD)
			cmd = I2C_READ;
		else
			cmd = I2C_WRITE;

		ret = owl_i2c_transfer(&i2c0_bus.regs, cmd, msg->addr, paddr,
					alen, msg->buf, msg->len);
		if (ret) {
			i2c_dbg("i2c_write: error sending\n");
			return -EREMOTEIO;
		}
	}

	return 0;
}

/* Probe to see if a chip is present. */
static int owl_i2c_probe_chip(struct udevice *bus, uint chip_addr,
				uint chip_flags)
{
	u8 reg = 0;

	return owl_i2c_transfer(&i2c0_bus.regs, I2C_WRITE, chip_addr,
				NULL, 0, &reg, 0);
}

static const struct dm_i2c_ops owl_i2c_ops = {
	.xfer		= owl_i2c_xfer,
	.probe_chip	= owl_i2c_probe_chip,
	.set_bus_speed	= owl_i2c_set_bus_speed,
};

U_BOOT_DRIVER(i2c_owl) = {
	.name	= "i2c_owl",
	.id	= UCLASS_I2C,
	.probe	= owl_i2c_probe,
	.ops	= &owl_i2c_ops,
};

/* TODO */
U_BOOT_DEVICE(i2c_owl) = {
	.name = "i2c_owl",
};

