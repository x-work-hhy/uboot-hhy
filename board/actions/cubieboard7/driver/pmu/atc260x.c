#include <common.h>
#include <errno.h>
#include <asm/io.h>

#include "../../include/owl_common.h"
#include "../../include/owl_io.h"
#include "atc260x_core.h"

#define ATC260X_ICTYPE_2603A 0
#define ATC260X_ICTYPE_2603C 1
#define ATC260X_ICTYPE_2609A 2

#define GPIO_MFP_PWM_BASE		0xE01B0000
#define PWM_CTL1			(GPIO_MFP_PWM_BASE + 0X54)  
#define	PAD_DRV1 			(GPIO_MFP_PWM_BASE + 0x0084)
#define	MFP_CTL3			(GPIO_MFP_PWM_BASE + 0x004C)
#define DEBUG_SEL			(GPIO_MFP_PWM_BASE + 0x0090)
#define DEBUG_OEN0			(GPIO_MFP_PWM_BASE + 0x0094) 

#define CMU_BASE			0xE0168000
#define	CMU_ANALOGDEBUG			(CMU_BASE+0x00D4)
#define CMU_DIGITALDEBUG            	(CMU_BASE+0x00D0)

#define ATC2603C_PMU_BASE		(0X00)
#define ATC2603C_PMU_DC1_CTL0		(ATC2603C_PMU_BASE + 0x11)
#define ATC2603C_PMU_DC1_CTL1		(ATC2603C_PMU_BASE + 0x12)
#define ATC2603C_PMU_DC1_CTL2		(ATC2603C_PMU_BASE + 0x13)
#define ATC2603C_PMU_DC2_CTL1		(ATC2603C_PMU_BASE + 0x15)
#define ATC2603C_PMU_DC2_CTL2           (ATC2603C_PMU_BASE + 0x16)
#define ATC2603C_PMU_SWITCH_CTL		(ATC2603C_PMU_BASE + 0x29)
#define ATC2603C_PMU_OC_EN		(ATC2603C_PMU_BASE + 0x31)
#define ATC2603C_PMU_OC_INT_EN		(ATC2603C_PMU_BASE + 0x32)
#define ATC2603C_PMU_CHARGER_CTL0	(ATC2603C_PMU_BASE + 0x39)
#define ATC2603C_PMU_BDG_CTL            (ATC2603C_PMU_BASE + 0x51)
#define ATC2603C_PMU_ABNORMAL_STATUS	(ATC2603C_PMU_BASE + 0x62)
#define ATC2603C_PMU_MUX_CTL0		(ATC2603C_PMU_BASE + 0x66)
#define ATC2603C_IRC_CTL		(ATC2603C_PMU_BASE + 0x80)

/* MFP Register Address */
#define ATC2603C_MFP_BASE		(0xD0)
#define ATC2603C_CHIP_VER		(ATC2603C_MFP_BASE + 0x0C)

/* TWSI Register Address */
#define ATC2603C_TWI_BASE                       (0xF8)
#define ATC2603C_SADDR                          (ATC2603C_TWI_BASE + 0x7)

#define	WAKEUP_SRC_RESET			(1 << 1)
#define	WAKEUP_SRC_HDSW				(1 << 2)
#define	WAKEUP_SRC_ALARM			(1 << 3)
#define	WAKEUP_SRC_ONOFF_SHORT		(1 << 7)
#define	WAKEUP_SRC_ONOFF_LONG		(1 << 8)
#define	WAKEUP_SRC_WALL_IN			(1 << 9)
#define	WAKEUP_SRC_VBUS_IN			(1 << 10)
extern int atc260x_time_to_rtc_tm(unsigned long time);

struct atc260x_dev g_atc260x_master_dev = {
	.dev_id		= 0,
	.ic_type	= 0x1,			/* TESTCODE! */
	.bus_num	= 0x0,			/* I2C-3     */
	.bus_mfp	= 0,
};

/* 
 * register
 * read/write
 */
int atc260x_m_reg_read(struct atc260x_dev *atc260x, unsigned int reg)
{
	int ret;
	
	ATC260X_ASSERT_VALID_DEV(atc260x);
	
	ret = (atc260x->reg_read)(atc260x, reg);
	
	if (ret < 0) {
		dev_err(atc260x, "read reg 0x%x	err, ret=%d\n", reg, ret);
	}
	return ret;
}

int atc260x_m_reg_write(struct atc260x_dev *atc260x, unsigned int reg, u16 val)
{
	int ret;
	ATC260X_ASSERT_VALID_DEV(atc260x);

	#if 0
	/* TESTCODE! */
	if (atc260x->ic_type == ATC260X_ICTYPE_2609A &&
	    reg == ATC2609A_PMU_SYS_CTL0 && (val & 0x1) == 0) {
		dev_err(atc260x, "GOT YOU, PMU_SYS_CTL0[0] set to 0!\n"
			"  builtin_return_address 0x%lx\n",
			__builtin_return_address(1));

		while (1) ;
	}
	#endif

	ret = (atc260x->reg_write)(atc260x, reg, val);
	if (ret < 0) {
		dev_err(atc260x, "write	reg 0x%x err, ret=%d\n", reg, ret);
	}
	return ret;
}

int atc260x_m_reg_setbits(struct atc260x_dev *atc260x, unsigned int reg, u16 mask,
			  u16 val)
{
	int ret;
	u16 new_val, old_val;
	ret = atc260x_m_reg_read(atc260x, reg);
	if (ret < 0)
		return ret;
	new_val = old_val = ret;
	new_val &= ~mask;
	new_val |= (val & mask);
	ret = 0;
	if (new_val != old_val) {
		ret = atc260x_m_reg_write(atc260x, reg, new_val);
	}
	return ret;
}

int atc260x_reg_read(unsigned int reg)
{
	return atc260x_m_reg_read(&g_atc260x_master_dev, reg);
}

int atc260x_reg_write(unsigned int reg, u16 val)
{
	return atc260x_m_reg_write(&g_atc260x_master_dev, reg, val);
}

int atc260x_reg_setbits(unsigned int reg, u16 mask, u16 val)
{
	return atc260x_m_reg_setbits(&g_atc260x_master_dev, reg, mask, val);
}

static int _atc260x_get_version(struct atc260x_dev *atc260x)
{
	int ret;

	ret = atc260x_m_reg_read(atc260x, ATC2603C_CHIP_VER);
	if (ret < 0) {
		dev_err(atc260x, "failed to get IC ver\n");
		return ret;
	}
	switch (ret & 0xf) {
	case 0:
		owl_printf("ATC260X_ICVER_A\n");
		return 0;
	case 1:
		owl_printf("ATC260X_ICVER_B\n");
		return 1;
	case 3:
		owl_printf("ATC260X_ICVER_C\n");
		return 2;
	case 7:
		owl_printf("ATC260X_ICVER_D\n");
		return 3;
	case 15:
		owl_printf("ATC260X_ICVER_E\n");
		return 4;
	}

	return -1;
}

/*
    config 24M output to pmu
    debug_port:
        0---clko;
        1---P_KS_OUT2
*/
static void switch_24m_output(int debug_port)
{
	unsigned int reg_val;
	int temp;

	owl_printf("%s, debug_port %d\n", __func__, debug_port);		
	
	if (debug_port == 2) {
		owl_printf("KS_OUT2_25M\n");
		/* disable CLKO_24M output */
		temp = owl_readl(MFP_CTL3);	//dis select h25mhz clk ouput clock
		temp &= ~(1 << 30);
		owl_writel(temp, MFP_CTL3);
		temp = owl_readl(PAD_DRV1) & (~(0x3 << 30));
		owl_writel(temp | (0x3 << 30), PAD_DRV1);	/*      set ksout2 drv level to highest */

		owl_writel(0x80000038, CMU_DIGITALDEBUG);	/*      Select timer_clk        */
		owl_writel(0x10, DEBUG_SEL);	/*      CMU debug select        */
		owl_writel(0x20000000, DEBUG_OEN0);	/*  P_KS_OUT2: Debug29      */
		owl_readl(DEBUG_OEN0);
	} else if  (debug_port == 1) {
		owl_printf("PAD_CLKO_25M\n");		
		/* enable CLKO_24M output */
		temp = owl_readl(MFP_CTL3);	//select h25mhz clk ouput     clock
		temp |= (1 << 30);
		owl_writel(temp, MFP_CTL3);
		//owl_writel(owl_readl(CMU_ANALOGDEBUG) | (0x1f), CMU_ANALOGDEBUG);
	} else {
		owl_printf("CLKO_24M\n");  // v700
		temp = owl_readl(CMU_ANALOGDEBUG);
		temp = temp&(~0xf);         
		owl_writel(temp|(0x80000110+1), CMU_ANALOGDEBUG);    
		//owl_udelay(10);
	}
}
static void _pmu_init_hosc_clk(void)
{
	int dat, i;	
	for (i = 0; i < 3; i++ ) {
		switch_24m_output(i);
		dat = atc260x_reg_read(ATC2603C_SADDR);
		if (dat == 0xca) {
			owl_printf("find HOSC\n");
			return;
		}
	}
	owl_printf("not find HOSC\n");
}

static int _atc2603c_probe(struct atc260x_dev *atc260x)
{
	int ret;

	owl_printf("%s\n", __func__);
	
	_pmu_init_hosc_clk();

	owl_printf("%s here\n", __func__);
	
	ret = atc260x_m_reg_read(atc260x, ATC2603C_SADDR);
	owl_printf("%s 0xff: %x\n", __func__, ret);

	if (ret != 0xca) {
		return -1;
	}

	ret = atc260x_m_reg_read(atc260x, ATC2603C_PMU_OC_INT_EN);
	if (ret < 0) {
		return -1;
	}

	if (ret != 0x1bc0) {
		return -1;
	}

	return 0;
}

static int _atc2603c_init(struct atc260x_dev *atc260x,
			  struct atc260x_pmu_initdata *initdata)
{
	int dat, val;
	int ic_ver;

	ic_ver = atc260x_reg_read(ATC2603C_CHIP_VER);

#ifdef CONFIG_GT7	
	/* init ir key */
	val = atc260x_reg_read(ATC2603C_IRC_CTL);
	val |= 0x0f;		/*enable ir,RC6 default */
	atc260x_reg_write(ATC2603C_IRC_CTL, val);
	
	val = atc260x_reg_read(ATC2603C_PMU_MUX_CTL0);
	val &= ~(3 << 12);
	val |= (1 << 12);	/*SGPIO5-IR default */
	atc260x_reg_write(ATC2603C_PMU_MUX_CTL0, val);
#endif
	/* update bandgap from BDG_VOL */
	val = atc260x_reg_read(ATC2603C_PMU_BDG_CTL);
	val |= 1 << 11;
	atc260x_reg_write(ATC2603C_PMU_BDG_CTL, val);

	//why close the charger here?
	/*close charger */
	val = atc260x_reg_read(ATC2603C_PMU_CHARGER_CTL0);
	val &= ~(1 << 15);
	atc260x_reg_write(ATC2603C_PMU_CHARGER_CTL0, val);

	/*close the OC EN of LDO3/6/7*/
	val = atc260x_reg_read(ATC2603C_PMU_OC_EN);
	val &= ~((1 << 9) | (1 << 10) | (1 << 13));
	atc260x_reg_write(ATC2603C_PMU_OC_EN, val);

#ifdef CONFIG_700
       /* dcdc1 VDD_CORE=1.15V */
       initdata->dcdc_cfg[1] = 0x12;
#endif
	if (ic_ver == 0) {
		val = 0xe04f;
		owl_printf("dcdc_cfg[1] in bootloader = %d\n",
			   initdata->dcdc_cfg[1]);
		val |= initdata->dcdc_cfg[1] << 7;
		atc260x_reg_write(ATC2603C_PMU_DC1_CTL0, val);
		val = 0x2cae;
		atc260x_reg_write(ATC2603C_PMU_DC1_CTL1, val);

	} else if (ic_ver > 0) {
		/*dcdc 1 ic     version B configutation */
		val = 0xe04f;
		val |= initdata->dcdc_cfg[1] << 7;
		atc260x_reg_write(ATC2603C_PMU_DC1_CTL0, val);
		/*val= 0xe8cc; */
		val = 0x88cc;
		atc260x_reg_write(ATC2603C_PMU_DC1_CTL1, val);
	} else {
		owl_printf("ERROR: ic_ver = %d\n", ic_ver);
		return ic_ver;
	}
	val = 0x334b;
	atc260x_reg_write(ATC2603C_PMU_DC1_CTL2, val);

	/*only check reg */
	val = atc260x_reg_read(ATC2603C_PMU_ABNORMAL_STATUS);
	owl_printf("0x%x:0x%x\n", ATC2603C_PMU_ABNORMAL_STATUS, val);
	if (ic_ver == 0) {
		val = 0x6cae;
		atc260x_reg_write(ATC2603C_PMU_DC2_CTL1, val);
	} else  {  /*if (ic_ver > 0)*/
		val = 0xe8ce;
		atc260x_reg_write(ATC2603C_PMU_DC2_CTL1, val);
	} 
	
	val = 0x334b;
	atc260x_reg_write(ATC2603C_PMU_DC2_CTL2, val);

#ifdef CONFIG_S700
	/*for gt7 pwm1-dcdc-cpu and s700 pwm1-dcdc-gpu init */
	owl_writel(0x10103f, PWM_CTL1);
	val = owl_readl(PWM_CTL1);
	owl_printf("PWM_CTL1 = 0x%x", val);
	dat = atc260x_reg_read(ATC2603C_PMU_SWITCH_CTL);
	dat &= ~0x803a;		/*LDO mode */
	dat |= (3 << 3) | (0 << 15);
	atc260x_reg_write(ATC2603C_PMU_SWITCH_CTL, dat);
#endif
	
	return 0;
}

/* common ------------------------------------------------------------------- */
#if	ATC260X_SUPPORT_MULTI_DEVS
struct atc260x_dev g_atc260x_aux_dev;
#endif

static int atc260x_early_init_inner(struct atc260x_dev *atc260x,
				    struct atc260x_pmu_initdata *initdata,
				    unsigned int interface_only)
{
	int (*probe_func) (struct atc260x_dev *);
	int (*init_func) (struct atc260x_dev *, struct atc260x_pmu_initdata *);
	int (*if_init_func) (struct atc260x_dev *);
	int ret;

	atc260x->_obj_type_id = ATC260x_PARENT_OBJ_TYPE_ID;

	switch (ATC260X_ICTYPE(atc260x)) {
	case 1:
		atc260x->bus_addr = 0x65;
		if_init_func = atc260x_i2c_if_init;
		probe_func = _atc2603c_probe;
		init_func = _atc2603c_init;
		owl_printf("ATC2603c!\n");
		break;
	default:
		dev_err(atc260x, "unknown PMIC,	ic_type=%u\n",
			ATC260X_ICTYPE(atc260x));
		return -1;
	}

	/* init interface */
	ret = if_init_func(atc260x);
	if (ret) {
		dev_err(atc260x, "dev if_init err, ic_type=%u bus_num=%u ret=%d\n",
				atc260x->ic_type, atc260x->bus_num, ret);
		return ret;
	}
	/* probe */
	ret = probe_func(atc260x);
	if (ret) {
		dev_err(atc260x, "dev probe err, ic_type=%u bus_num=%u\n",
			atc260x->ic_type, atc260x->bus_num);
		return ret;
	}

	/* get chip version */
	ret = _atc260x_get_version(atc260x);
	if (ret < 0) {
		dev_err(atc260x, "unknown dev ver, ic_type=%u\n",
			atc260x->ic_type);
		return ret;
	}
	atc260x->ic_ver = ret;
	dev_info(atc260x, "found PMIC type %u ver %u\n",
		 atc260x->ic_type, atc260x->ic_ver);

	/* base init */
	if (interface_only == 0) {
		ret = init_func(atc260x, initdata);
		if (ret) {
			dev_err(atc260x, "functional init err\n");
			return ret;
		}
	}

	dev_info(atc260x, "atc260x:PMU_SYS_CTL0:0x%x,PMU_SYS_CTL1:0x%x,PMU_SYS_CTL3:0x%x\n",
		 atc260x_m_reg_read(atc260x, 0x0),
		 atc260x_m_reg_read(atc260x, 0x1),
		 atc260x_m_reg_read(atc260x, 0x3));
	
	return 0;
}

int atc260x_early_init(void)
{
	struct atc260x_pmu_initdata 	initdata;
	
	#if 0
	struct atc260x_dev 		*atc260x;
	struct owl_pmu_config 		pmu_config;
	owl_bootparam_get_config(OWL_BOOTPARAM_PMU, &pmu_config);
	#endif
	initdata.dcdc_en_bm = 0x1f;
	initdata.dcdc_cfg[0] = 0x34;		/* DC0=0.925V, VDD_Core */
	//initdata.dcdc_cfg[1] = 0x40;    	/* DC1=1.0V, VDD_CPU */
	initdata.dcdc_cfg[1] = 0x34;		/* DC1=0.925V, VDD_CPU */
	/*initdata.dcdc_cfg[2] = 0x90; */	/* DC2=1.5V, VDDR */
	//initdata.dcdc_cfg[2] = pmu_config.vddr_pmu_dcdc_cfg;
	initdata.dcdc_cfg[2] = 0xa;
	
	initdata.dcdc_cfg[3] = 0xc4;		/* DC3=3.1V, VCC */
	//initdata.dcdc_cfg[4] = 0x40;    	/* DC4=1.0V, VDD_CPUM */
	initdata.dcdc_cfg[4] = 0x34;		/* DC4=0.925V, VDD_CPUM */
	initdata.ldo_en_bm =
	    (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 6);
	initdata.ldo_cfg[0] = 0x8;	/* LDO2=3.1V, VCC */
	initdata.ldo_cfg[2] = 0x8;	/* LDO2=3.1V, VCC */
	initdata.ldo_cfg[3] = 0xb;	/* LDO3=1.8V */
	initdata.ldo_cfg[4] = 0xb;	/* LDO4=1.8V */
	initdata.ldo_cfg[6] = 0xb;	/* LDO6=1.1V, AVDD1V0 */

	#if 0
	atc260x = &g_atc260x_master_dev;
	atc260x->dev_id = 0;
	#if 0
	atc260x->ic_type = pmu_config.pmu_id;	/* TESTCODE! */
	atc260x->bus_num = pmu_config.bus_id;	/* I2C-3     */
	atc260x->bus_mfp = pmu_config.bus_mfp;
	#endif
	atc260x->ic_type = 0x1;			/* TESTCODE! */
	atc260x->bus_num = 0x0;	/* I2C-3     */
	atc260x->bus_mfp = 0;
	#endif
	
	return atc260x_early_init_inner(&g_atc260x_master_dev, &initdata, 0);
}
