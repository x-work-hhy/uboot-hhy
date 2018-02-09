#ifndef __ATC260X_CORE_H__
#define __ATC260X_CORE_H__

struct atc260x_pmu_initdata {
	unsigned char dcdc_en_bm;
	unsigned char dcdc_cfg[7];
	unsigned short ldo_en_bm;
	unsigned char ldo_cfg[14];
};

/* ATC260X device */
struct atc260x_dev {
	struct owl_i2c_dev *i2c_client;
	void *spi_client;

	unsigned char dev_id;
	unsigned char ic_type;		/* see ATC260X_ICTYPE_2603A ... */
	unsigned char ic_ver;		/* see ATC260X_ICVER_A ... */
	unsigned char bus_num;		/* SPI / I2C bus number, used by reg direct access. */
	unsigned char bus_mfp;		/* SPI / I2C bus mfp */
	unsigned char bus_addr;		/* device's bus address, only for I2C, 7bit, r/w bit excluded */
	unsigned char s2_resume;

	int (*reg_read) (struct atc260x_dev *atc260x, unsigned int reg);
	int (*reg_write) (struct atc260x_dev *atc260x, unsigned int reg, unsigned short val);

	unsigned long s2_resume_addr;

	unsigned long auxadc_data[16 / sizeof(unsigned long)];

	unsigned int _obj_type_id;
};

#define ATC260x_PARENT_OBJ_TYPE_ID 0x72f80927U

/* for debug */
#if 1

#if ATC260X_SUPPORT_MULTI_DEVS
#define dev_err(DEV, FMT, ARGS...)  owl_printf("PMU#%u:" FMT, (DEV)->dev_id, ## ARGS)
#define dev_warn(DEV, FMT, ARGS...) owl_printf("PMU#%u:" FMT, (DEV)->dev_id, ## ARGS)
#define dev_info(DEV, FMT, ARGS...) owl_printf("PMU#%u:" FMT, (DEV)->dev_id, ## ARGS)
#define dev_dbg(DEV, FMT, ARGS...)  do {} while (0)
#else
#define dev_err(DEV, FMT, ARGS...)  owl_printf("PMU:" FMT, ## ARGS)
#define dev_warn(DEV, FMT, ARGS...) owl_printf("PMU:" FMT, ## ARGS)
#define dev_info(DEV, FMT, ARGS...) owl_printf("PMU:" FMT, ## ARGS)
#define dev_dbg(DEV, FMT, ARGS...)  owl_printf("PMU:" FMT, ## ARGS)
#endif
#define pr_err(FMT, ARGS...)        owl_printf("PMU:" FMT, ## ARGS)
#define pr_warn(FMT, ARGS...)       owl_printf("PMU:" FMT, ## ARGS)
#define pr_info(FMT, ARGS...)       owl_printf("PMU:" FMT, ## ARGS)
#define pr_dbg(FMT, ARGS...)        do {} while (0)

#define ATC260X_ASSERT(CON)  do {if (unlikely(!(CON))) do {owl_printf("PMU ASSERT!\n"); while (1); } while (0); } while (0)	/* only for boot stage! */

#define ATC260X_ASSERT_VALID_DEV(ADEV) \
	ATC260X_ASSERT(!((ADEV) == NULL || (ADEV)->_obj_type_id != ATC260x_PARENT_OBJ_TYPE_ID))
#define ATC260X_CHK_VALID_DEV(ADEV) \
	(!((ADEV) == NULL  || (ADEV)->_obj_type_id != ATC260x_PARENT_OBJ_TYPE_ID))

#else /* no debug */

#define dev_err(DEV, FMT, ARGS...)  owl_printf(FMT, ## ARGS)
#define dev_warn(DEV, FMT, ARGS...) do {} while (0)
#define dev_info(DEV, FMT, ARGS...) do {} while (0)
#define dev_dbg(DEV, FMT, ARGS...)  do {} while (0)
#define pr_err(FMT, ARGS...)        owl_printf(FMT, ## ARGS)
#define pr_warn(FMT, ARGS...)       do {} while (0)
#define pr_info(FMT, ARGS...)       do {} while (0)
#define pr_dbg(FMT, ARGS...)        do {} while (0)
#define ATC260X_ASSERT(CON) do {} while (0)
#define ATC260X_ASSERT_VALID_DEV(ADEV) do {} while (0)
#define ATC260X_CHK_VALID_DEV(ADEV)    (1)

#endif

/* only for core internal. */
#if 0
#if ATC260X_SUPPORT_MULTITYPE_DEVS
#define ATC260X_ICTYPE(ADEV)        ((ADEV)->ic_type)
#define ATC260X_MASTER_DEV_ICTYPE   (g_atc260x_master_dev.ic_type)
#else
#define ATC260X_ICTYPE(ADEV)        (ATC260X_HARD_CODED_IC_TYPE)	/* 促进GCC优化. */
#define ATC260X_MASTER_DEV_ICTYPE   (ATC260X_HARD_CODED_IC_TYPE)
#endif
#endif
#define ATC260X_ICTYPE(ADEV)        ((ADEV)->ic_type)
#define ATC260X_MASTER_DEV_ICTYPE   (g_atc260x_master_dev.ic_type)

extern int atc260x_i2c_if_init(struct atc260x_dev *atc260x);
extern int atc260x_spi_if_init(struct atc260x_dev *atc260x);

extern int atc260x_auxadc_init(struct atc260x_dev *atc260x);
extern int atc260x_onoff_init(struct atc260x_dev *atc260x);

#endif /* __ATC260X_CORE_H__ */
