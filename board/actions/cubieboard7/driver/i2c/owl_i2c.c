#include <common.h>
#include <errno.h>
#include <asm/io.h>

#include "../../include/owl_common.h"
#include "../../include/owl_io.h"

#define I2C_DEBUG

#define OWL_MAX_I2C			(6)
#define OWL_I2C_TIMEOUT_US		(100 * 1000)	/* 0.1s */

#define	CMU_BASE		(0xE0168000)
#define CMU_DEVCLKEN1		(CMU_BASE+0x00A4)
#define CMU_DEVRST1		(CMU_BASE+0x00AC)
#define	CMU_ETHERNETPLL		(CMU_BASE+0x00B4)

#define GPIO_MFP_PWM_BASE	0xE01B0000
#define PAD_PULLCTL0		(GPIO_MFP_PWM_BASE + 0x0060)
#define	PAD_ST0			(GPIO_MFP_PWM_BASE + 0x006C)
#define	PAD_DRV2		(GPIO_MFP_PWM_BASE + 0x0088) 
#define	MFP_CTL3		(GPIO_MFP_PWM_BASE + 0x004c)

#define I2C0_BASE		(0xe0170000)
#define I2C_BASE		I2C0_BASE

#ifdef I2C_DEBUG
#define i2c_dbg(i2c_dev, format, args...)	\
	owl_printf("[i2c%d] " format,		\
		(i2c_dev)->bus_id, ## args)

#define i2c_vdbg(i2c_dev, format, args...)	\
	owl_printf("[i2c%d] " format,		\
		(i2c_dev)->bus_id, ## args)
#else
#define i2c_dbg(i2c_dev, format, ...)		do {} while (0)
#define i2c_vdbg(i2c_dev, format, ...)		do {} while (0)
#endif

#define i2c_warn(i2c_dev, format, args...)	\
	owl_printf("[i2c%d] " format,		\
		(i2c_dev)->bus_id, ## args)
#define i2c_err(i2c_dev, format, args...)	\
	owl_printf("[i2c%d] " format,		\
		(i2c_dev)->bus_id, ## args)

/* I2C registers */
#define I2C_CTL				0x0000
#define I2C_CLKDIV			0x0004
#define I2C_STAT			0x0008
#define I2C_ADDR			0x000C
#define I2C_TXDAT			0x0010
#define I2C_RXDAT			0x0014
#define I2C_CMD				0x0018
#define I2C_FIFOCTL			0x001C
#define I2C_FIFOSTAT			0x0020
#define I2C_DATCNT			0x0024
#define I2C_RCNT			0x0028

/* I2Cx_CTL */
#define 	I2C_CTL_GRAS			(0x1 << 0)
#define		I2C_CTL_GRAS_ACK		0
#define		I2C_CTL_GRAS_NACK		I2C_CTL_GRAS
#define 	I2C_CTL_RB			(0x1 << 1)
#define 	I2C_CTL_GBCC_MASK		(0x3 << 2)
#define 	I2C_CTL_GBCC(x)			(((x) & 0x3) << 2)
#define		I2C_CTL_GBCC_NONE		I2C_CTL_GBCC(0)
#define		I2C_CTL_GBCC_START		I2C_CTL_GBCC(1)
#define		I2C_CTL_GBCC_STOP		I2C_CTL_GBCC(2)
#define		I2C_CTL_GBCC_RESTART		I2C_CTL_GBCC(3)
#define 	I2C_CTL_IRQE			(0x1 << 5)
#define 	I2C_CTL_EN			(0x1 << 7)
#define 	I2C_CTL_AE			(0x1 << 8)
#define 	I2C_CTL_FHSM			(0x1 << 9)
#define 	I2C_CTL_SHSM			(0x1 << 10)
#define 	I2C_CTL_CSE			(0x1 << 11)

/* I2Cx_CLKDIV */
#define 	I2C_CLKDIV_DIV_MASK		(0xff << 0)
#define 	I2C_CLKDIV_DIV(x)		(((x) & 0xff) << 0)
#define 	I2C_CLKDIV_HDIV_MASK		(0xff << 8)
#define 	I2C_CLKDIV_HDIV(x)		(((x) & 0xff) << 8)
#define		I2C_CLKDIV_CLKCOM(x)		(((x) & 0x3) << 16)
#define		I2C_CLKDIV_CLKCOM_0NS		I2C_CLKDIV_CLKCOM(0)
#define		I2C_CLKDIV_CLKCOM_10NS		I2C_CLKDIV_CLKCOM(1)
#define		I2C_CLKDIV_CLKCOM_20NS		I2C_CLKDIV_CLKCOM(2)
#define		I2C_CLKDIV_CLKCOM_30NS		I2C_CLKDIV_CLKCOM(3)

/* I2Cx_STAT */
#define 	I2C_STAT_RACK			(0x1 << 0)
#define 	I2C_STAT_BEB			(0x1 << 1)
#define 	I2C_STAT_IRQP			(0x1 << 2)
#define 	I2C_STAT_LAB			(0x1 << 3)
#define 	I2C_STAT_STPD			(0x1 << 4)
#define 	I2C_STAT_STAD			(0x1 << 5)
#define 	I2C_STAT_BBB			(0x1 << 6)
#define 	I2C_STAT_TCB			(0x1 << 7)
#define 	I2C_STAT_LBST			(0x1 << 8)
#define 	I2C_STAT_SAMB			(0x1 << 9)
#define 	I2C_STAT_SRGC			(0x1 << 10)

#define 	I2C_BUS_ERR_MSK			(I2C_STAT_LAB | I2C_STAT_BEB)

/* I2Cx_CMD */
#define		I2C_CMD_SBE			(0x1 << 0)
#define		I2C_CMD_AS_MASK			(0x7 << 1)
#define		I2C_CMD_AS(x)			(((x) & 0x7) << 1)
#define		I2C_CMD_RBE			(0x1 << 4)
#define		I2C_CMD_SAS_MASK		(0x7 << 5)
#define 	I2C_CMD_SAS(x)			(((x) & 0x7) << 5)
#define 	I2C_CMD_DE			(0x1 << 8)
#define 	I2C_CMD_NS			(0x1 << 9)
#define	 	I2C_CMD_SE			(0x1 << 10)
#define 	I2C_CMD_MSS			(0x1 << 11)
#define 	I2C_CMD_WRS			(0x1 << 12)
#define 	I2C_CMD_EXEC			(0x1 << 15)
	
/* I2Cx_FIFOCTL */
#define 	I2C_FIFOCTL_NIB			(0x1 << 0)
#define 	I2C_FIFOCTL_RFR			(0x1 << 1)
#define 	I2C_FIFOCTL_TFR			(0x1 << 2)

/* I2Cx_FIFOSTAT */
#define 	I2C_FIFOSTAT_CECB		(0x1 << 0)
#define 	I2C_FIFOSTAT_RNB		(0x1 << 1)
#define 	I2C_FIFOSTAT_RFE		(0x1 << 2)
#define 	I2C_FIFOSTAT_RFF		(0x1 << 3)
#define 	I2C_FIFOSTAT_TFE		(0x1 << 4)
#define 	I2C_FIFOSTAT_TFF		(0x1 << 5)
#define 	I2C_FIFOSTAT_WRS		(0x1 << 6)
#define 	I2C_FIFOSTAT_RFD_MASK		(0xff << 8)
#define 	I2C_FIFOSTAT_RFD_SHIFT		(8)
#define 	I2C_FIFOSTAT_TFD_MASK		(0xff << 16)
#define 	I2C_FIFOSTAT_TFD_SHIFT		(16)

/* extract fifo level from fifostat */
#define 	I2C_RX_FIFO_LEVEL(x)		(((x) >> 8) & 0xff)
#define 	I2C_TX_FIFO_LEVEL(x)		(((x) >> 16) & 0xff)

enum owl_i2c_cmd_type {
 	OWL_I2C_WRITE,
 	OWL_I2C_READ,
};

struct owl_i2c_dev {
	int bus_id;
	int clk_id;
	int rst_id;
	int clk_freq;
	unsigned long base;
};

static struct owl_i2c_dev owl_i2c0_dev = {
	.clk_freq	= 400000,		/* 400kHz */	
	.base 		= I2C0_BASE, 		/* regs */
};

static void owl_i2c_writel(struct owl_i2c_dev *dev,
			   unsigned int val, unsigned int reg)
{
	i2c_vdbg(dev, "-->> write 0x%x to 0x%x\n", val,
		 (unsigned int)(dev->base + reg));
	owl_writel(val, dev->base + reg);
}

static unsigned int owl_i2c_readl(struct owl_i2c_dev *dev, unsigned int reg)
{
	return owl_readl(dev->base + reg);
}

#ifdef I2C_DEBUG 
static void owl_i2c_dump_regs(struct owl_i2c_dev *dev)
{
	i2c_dbg(dev, "dump phys %x regs:\n"
		"  ctl:      %x  clkdiv: %x  stat:    %x\n"
		"  addr:     %x  cmd:    %x  fifoctl: %x\n"
		"  fifostat: %x  datcnt: %x  rcnt:    %x\n",
		(unsigned int)dev->base,
		owl_i2c_readl(dev, I2C_CTL),
		owl_i2c_readl(dev, I2C_CLKDIV),
		owl_i2c_readl(dev, I2C_STAT),
		owl_i2c_readl(dev, I2C_ADDR),
		owl_i2c_readl(dev, I2C_CMD),
		owl_i2c_readl(dev, I2C_FIFOCTL),
		owl_i2c_readl(dev, I2C_FIFOSTAT),
		owl_i2c_readl(dev, I2C_DATCNT),
		owl_i2c_readl(dev, I2C_RCNT));
}

static void owl_i2c_dump_mem(char *label, void *base, int len)
{
	//owl_debug_dump_mem(label, base, len);
}
#else
static void owl_i2c_dump_regs(struct owl_i2c_dev *dev)
{
}

static void owl_i2c_dump_mem(char *label, void *base, int len)
{
}
#endif

static int owl_i2c_wait_if_busy(struct owl_i2c_dev *dev)
{
	unsigned long timeout = OWL_I2C_TIMEOUT_US;

	while (owl_i2c_readl(dev, I2C_STAT) & I2C_STAT_BBB) {

		if (timeout == 0) {
			i2c_err(dev, "Bus busy timeout, stat 0x%x\n",
				owl_i2c_readl(dev, I2C_STAT));
			owl_i2c_dump_regs(dev);
			return -1;
		}
		//owl_udelay(1);
		timeout--;
	}

	return 0;
}

static inline void owl_i2c_clear_status(struct owl_i2c_dev *dev)
{
	unsigned int val;

	val = owl_i2c_readl(dev, I2C_STAT);
	owl_i2c_writel(dev, val, I2C_STAT);

	/* reset FIFO */
	val = owl_i2c_readl(dev, I2C_FIFOCTL);
	val |= I2C_FIFOCTL_RFR | I2C_FIFOCTL_TFR;
	owl_i2c_writel(dev, val, I2C_FIFOCTL);

	val = owl_i2c_readl(dev, I2C_FIFOSTAT);
	owl_i2c_writel(dev, val, I2C_FIFOSTAT);
}

static void owl_i2c_reset(struct owl_i2c_dev *dev)
{
	/* reset i2c controller */
	owl_i2c_writel(dev, 0, I2C_CTL);
	owl_i2c_writel(dev, I2C_CTL_EN, I2C_CTL);
}

static int check_transfer_error(struct owl_i2c_dev *dev)
{
	unsigned int stat, fifostat;
	int ret = 0;

	stat = owl_i2c_readl(dev, I2C_STAT);
	fifostat = owl_i2c_readl(dev, I2C_FIFOSTAT);

	if (fifostat & I2C_FIFOSTAT_RNB) {
		i2c_warn(dev, "no ACK from device");
		ret = -1;
	} else if (stat & I2C_STAT_LAB) {
		i2c_warn(dev, "lose arbitration");
		ret = -2;
	} else if (stat & I2C_STAT_BEB) {
		i2c_warn(dev, "bus error");
		ret = -3;
	}

	if (ret)
		owl_i2c_dump_regs(dev);

	return ret;
}

static void i2c_set_freq(struct owl_i2c_dev *dev)
{
	unsigned int pclk, div_factor;

	if (dev->clk_freq == 0)
		return;

	pclk = 100000000;

	div_factor = (pclk + dev->clk_freq * 16 - 1) / (dev->clk_freq * 16);
	owl_i2c_writel(dev, I2C_CLKDIV_DIV(div_factor), I2C_CLKDIV);

	return;
}

static int owl_i2c_hwinit(struct owl_i2c_dev *dev)
{
	owl_i2c_reset(dev);
	owl_i2c_clear_status(dev);

	i2c_set_freq(dev);

	return 0;
}

/*
 *  write data with internal address
 *  addr: MAX 6 bytes, data MAX 122 bytes
 */
static int owl_i2c_write_data(struct owl_i2c_dev *dev,
			      unsigned char chip,
			      unsigned char *addr,
			      unsigned char addr_len,
				  unsigned char *data,
				  unsigned char data_len)
{
	unsigned int chip_addr = (chip & 0x7f) << 1;
	unsigned int timeout_us = OWL_I2C_TIMEOUT_US;
	unsigned int fifo_cmd, fifostat;
	int i, ret = 0;

	fifo_cmd = I2C_CMD_EXEC | I2C_CMD_MSS | I2C_CMD_SE | I2C_CMD_DE
	    | I2C_CMD_SBE;

	/* set data count for the message */
	owl_i2c_writel(dev, data_len, I2C_DATCNT);

	/* write to device */
	owl_i2c_writel(dev, chip_addr, I2C_TXDAT);

	/* set internal address */
	fifo_cmd |= I2C_CMD_AS(addr_len + 1);

	/* write internal register address */
	for (i = 0; i < addr_len; i++)
		owl_i2c_writel(dev, addr[i], I2C_TXDAT);

	/* Write data to FIFO */
	for (i = 0; i < data_len; i++) {
		if (owl_i2c_readl(dev, I2C_FIFOSTAT) & I2C_FIFOSTAT_TFF)
			break;

		owl_i2c_writel(dev, data[i], I2C_TXDAT);
	}

	owl_i2c_dump_regs(dev);

	/* write fifo command to start transfer */
	owl_i2c_writel(dev, fifo_cmd, I2C_CMD);

	while (timeout_us) {
		ret = check_transfer_error(dev);
		if (ret)
			/* found transfer error */
			break;

		fifostat = owl_i2c_readl(dev, I2C_FIFOSTAT);
		if (fifostat & I2C_FIFOSTAT_CECB)
			/* transfer over */
			break;

		//owl_udelay(1);
		timeout_us--;
	}

	if (timeout_us == 0) {
		i2c_err(dev, "transfer timed out\n");
		ret = -1;
	} else if (ret != 0) {
		i2c_err(dev, "transfer error\n");
		ret = -1;
	} else {
		ret = 0;
	}

	return ret;
}

/*
 *  read data with internal address
 *  addr: MAX 6 bytes, data MAX 122 bytes
 */
static int owl_i2c_read_data(struct owl_i2c_dev *dev,
			     unsigned char chip,
			     unsigned char *addr,
			     unsigned char addr_len,
				 unsigned char *data,
				 unsigned char data_len)
{
	unsigned int chip_addr = (chip & 0x7f) << 1;
	unsigned int timeout_us = OWL_I2C_TIMEOUT_US;
	unsigned int fifo_cmd, fifostat;
	int i, ret = 0;

	fifo_cmd = I2C_CMD_EXEC | I2C_CMD_MSS | I2C_CMD_SE | I2C_CMD_DE |
	    I2C_CMD_NS | I2C_CMD_SBE;

	/* set internal address */
	fifo_cmd |= I2C_CMD_AS(addr_len + 1) | I2C_CMD_SAS(1)
	    | I2C_CMD_RBE;

	/* set data count for the message */
	owl_i2c_writel(dev, data_len, I2C_DATCNT);

	/* write i2c device address */
	owl_i2c_writel(dev, chip_addr, I2C_TXDAT);

	/* write internal register address */
	for (i = 0; i < addr_len; i++)
		owl_i2c_writel(dev, addr[i], I2C_TXDAT);

	/* read from device, with WR bit */
	owl_i2c_writel(dev, chip_addr | 1, I2C_TXDAT);

	/* write fifo command to start transfer */
	owl_i2c_writel(dev, fifo_cmd, I2C_CMD);

	while (timeout_us) {
		ret = check_transfer_error(dev);
		if (ret)
			/* found transfer error */
			break;

		fifostat = owl_i2c_readl(dev, I2C_FIFOSTAT);
		if (fifostat & I2C_FIFOSTAT_CECB)
			/* transfer over */
			break;

		//owl_udelay(1);
		timeout_us--;
	}

	if (timeout_us == 0) {
		i2c_err(dev, "transfer timed out\n");
		ret = -1;
	} else if (ret != 0) {
		i2c_err(dev, "transfer error\n");
		ret = -1;
	} else {
		/* read data */
		for (i = 0; i < data_len; i++)
			data[i] = owl_i2c_readl(dev, I2C_RXDAT);
		ret = 0;
	}

	return ret;
}

int owl_i2c_transfer(unsigned char cmd_type,
			unsigned char chip,
			unsigned char *addr,
			unsigned char addr_len,
			unsigned char *data,
			unsigned char data_len)
{
	int ret;
	struct owl_i2c_dev *dev = &owl_i2c0_dev;
	
	/* dump messages for debug */
	i2c_dbg(dev, "[i2c]: chip addr 0x%x, %s\n",
		chip, cmd_type == OWL_I2C_READ ? "read" : "write");

	if (addr_len)
		owl_i2c_dump_mem("[i2c] internal addr", addr, addr_len);

	owl_i2c_hwinit(dev);

	owl_i2c_writel(dev, 0x06, I2C_FIFOCTL); // reset I2C0 FIFO

	do{
		ret = owl_i2c_readl(dev,  I2C_FIFOCTL) & 0x06;
	} while(ret != 0);

	/* enable I2C controller */
	owl_i2c_writel(dev, I2C_CTL_EN, I2C_CTL);

	/* Make sure the hardware is ready */
	ret = owl_i2c_wait_if_busy(dev);
	if (ret < 0) {
		i2c_warn(dev, "bus busy before transfer\n");
		return ret;
	}

	switch (cmd_type) {
	case OWL_I2C_WRITE:
		if (data_len)
			owl_i2c_dump_mem("[i2c] write buf", data, data_len);

		ret = owl_i2c_write_data(dev, chip, addr, addr_len,
					 data, data_len);
		break;
	case OWL_I2C_READ:
		ret = owl_i2c_read_data(dev, chip, addr, addr_len,
					data, data_len);
		if (data_len)
			owl_i2c_dump_mem("[i2c] read buf", data, data_len);

		break;
	default:
		i2c_warn(dev, "i2c_transfer: bad call\n");
		ret = -1;
		break;
	}

	/* reset the controller if found error */
	if (ret)
		owl_i2c_reset(dev);

	/* disable i2c controller */
	owl_i2c_writel(dev, 0, I2C_CTL);

	return ret;
}

int owl_i2c_init(int bus_id)
{
	int temp;

	i2c_dbg(&owl_i2c0_dev, "bus %d reg_base 0x%x, speed %d:\n",
		bus_id, owl_i2c0_dev.base, owl_i2c0_dev.clk_freq);

	if (bus_id > OWL_MAX_I2C)
		return NULL;

	/* enable assist PLL */
	temp = owl_readl(CMU_ETHERNETPLL);	 //enable Ethernet 500M PLL
	temp |= 0x01;
	owl_writel(temp, CMU_ETHERNETPLL);
	
	if (bus_id == 0) {//S700
		owl_clrsetbits(PAD_PULLCTL0, 0x300, 0x300);
		/* MFP_CTL3 bit 18:16 = 000 config p_i2c0_sclk
		p_i2c0_sdata as i2c0 clk data  */
		owl_clrsetbits(MFP_CTL3, 7<<16, 0);

		temp = owl_readl(CMU_DEVCLKEN1);   //enable	I2C0 clock
		temp |=	0x0000400f;
		owl_writel(temp, CMU_DEVCLKEN1);

		temp = owl_readl(PAD_DRV2);
		temp |=	(0x2<<23);
		owl_writel(temp, PAD_DRV2);

		temp = owl_readl(PAD_PULLCTL0);		//enable I2C0	pull up	resister
		temp |=	0x00000300;
		owl_writel(temp, PAD_PULLCTL0);

		temp = owl_readl(PAD_ST0);		 //enable I2C0 schmitt
		temp |=	(0x1<<30) |	(0x1<<7);
		owl_writel(temp, PAD_ST0);

		temp = owl_readl(MFP_CTL3);	  //select I2C0	pin
		temp &=	0xfff8ffff;
		owl_writel(temp, MFP_CTL3);

		/* reset I2C0 */
		owl_writel(0x00, I2C0_BASE + I2C_CTL);
		
		/* enable I2C0 */
		owl_writel(0x80, I2C0_BASE + I2C_CTL);

		/*  I2C0CLK*16=100M/I2CDIV,set I2C baudrate 400K*/
		owl_writel(0x10, I2C0_BASE + I2C_CLKDIV);
	}
	
#if 0
	/* pinmux */
	clrsetbits_le32(MFP_CTL3, 0x7 << 16, 0);
	
	/* device clock enable */
	setbits_le32(CMU_DEVCLKEN1, 1 << 0);	/* i2c0 */
	
	/* reset de-assert */
	setbits_le32(CMU_DEVRST1, 1 << 0);
#endif
	return 0;
}
