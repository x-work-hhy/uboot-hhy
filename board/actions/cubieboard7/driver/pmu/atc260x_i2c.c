#include "atc260x_core.h"

enum owl_atc260x_i2c_cmd_type {
 	OWL_I2C_WRITE,
 	OWL_I2C_READ,
};


static int _atc260x_i2c_reg_read(struct atc260x_dev *dev, unsigned int reg)
{
	int ret;
	unsigned char  dat[2];

	ret = owl_i2c_transfer(OWL_I2C_READ,
				dev->bus_addr,
				(unsigned char *)&reg,
				1,
				(unsigned char *)dat,
				2);
	if (ret) {
		dev_err(dev, "i2c read err, reg=0x%x ret=%d\n", reg, ret);
		return ret;
	}
	ret = (dat[0] << 8) | dat[1];
	dev_dbg(dev, "read reg 0x%x value 0x%x\n", reg, ret);
	return ret;
}

static int _atc260x_i2c_reg_write(struct atc260x_dev *dev, unsigned int reg, unsigned short val)
{
	int ret;
	unsigned char  dat[2];

	dev_dbg(dev, "write reg 0x%x value 0x%x\n", reg, val);
	dat[0] = (val >> 8) & 0xff;
	dat[1] = val & 0xff;

	ret = owl_i2c_transfer(OWL_I2C_WRITE,
				dev->bus_addr,
				(unsigned char *)&reg,
				1,
				(unsigned char *)dat,
				2);
	if (ret) {
		dev_err(dev, "i2c write failed, reg=0x%x ret=%d\n", reg, ret);
		return ret;
	}

	return 0;
}

int atc260x_i2c_if_init(struct atc260x_dev *dev)
{
	dev_dbg(dev, "i2c_if_init enter\n");

	#if 0
	dev->i2c_client = owl_i2c_init(dev->bus_num, dev->bus_mfp);
	if (dev->i2c_client == NULL) {
		dev_err(dev, "failed to get I2C client, bus=%u\n", dev->bus_num);
		return -1;
	}
	#endif

	dev->reg_read = _atc260x_i2c_reg_read;
	dev->reg_write = _atc260x_i2c_reg_write;

	return 0;
}
