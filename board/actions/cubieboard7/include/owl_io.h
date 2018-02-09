#ifndef __OWL_IO_H__
#define __OWL_IO_H__

/******************************************************************************/
static inline void owl_writeb(unsigned char val, unsigned long reg)
{
    *(volatile unsigned char *)(reg) = val;
}

static inline void owl_writew(unsigned short val, unsigned long reg)
{
    *(volatile unsigned short *)(reg) = val;
}

static inline void owl_writel(unsigned int val, unsigned long reg)
{
    *(volatile unsigned int *)(reg) = val;
}

static inline void owl_writeq(unsigned long long val, unsigned long reg)
{
    *(volatile unsigned long long *)(reg) = val;
}

/******************************************************************************/
static inline unsigned char owl_readb(unsigned long reg)
{
    return (*(volatile unsigned char *)(reg));
}

static inline unsigned short owl_readw(unsigned long reg)
{
    return (*(volatile unsigned short *)(reg));
}

static inline unsigned int owl_readl(unsigned long reg)
{
    return (*(volatile unsigned int *)(reg));
}

static inline unsigned long long owl_readq(unsigned long reg)
{
    return (*(volatile unsigned long long *)(reg));
}

static inline void owl_clrsetbits(unsigned long reg, unsigned int clear, 
		unsigned int set)
{
	unsigned int val;

	val = owl_readl(reg);
	val &= ~clear;
	val |= set;
	owl_writel(val, reg);
}

static inline void owl_clrbits(unsigned long reg, unsigned int clear)
{
	unsigned int val;

	val = owl_readl(reg);
	val &= ~clear;
	owl_writel(val, reg);
}

static inline void owl_setbits(unsigned long reg, unsigned int set)
{
	unsigned int val;

	val = owl_readl(reg);
	val |= set;
	owl_writel(val, reg);
}


#endif /* __OWL_IO_H__ */
