/*
 *@Copyright      Copyright (c) Actions（zhuhai）Technology Co., Limited. All Rights Reserved
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>

#include "../../include/owl_common.h"
#include "../../include/owl_io.h"

#include "inc/ddr_reg.h"
#include "inc/ddr_cfg.h"

#define	CMU_BASE	0xE0168000
#define CMU_DDRPLL      (CMU_BASE+0x0008)
#define CMU_DEVCLKEN0	(CMU_BASE+0x00A0)
#define CMU_DEVRST0 	(CMU_BASE+0x00A8)

typedef struct
{
	unsigned char 	init_mode;
	unsigned char 	need_preheat;
	
} ddr_init_param_t;

static ddr_init_param_t ddr_init_param;

static void _set_mode(unsigned char running_state, unsigned char channel) //don't fix
{
	unsigned int pctl_base = (channel == 0) ? PCTL0_BASE : PCTL1_BASE;
	unsigned char temp,current_state,poll_state;
	unsigned int cnt_i;
	current_state = (owl_readl(pctl_base + STAT) & 0x7);            //get state before compare
	current_state = (current_state <= 1) ? current_state : ((current_state == 3) ? ACCESS : LP); //get mode before compare
	if(current_state != running_state)          //if mode isnot change then return
	{
		temp = (current_state > running_state)?(current_state - running_state) : (running_state - current_state);     //judge mode direction
		if(current_state == LP)                   //if current mode is LP, then performance WAKEUP command to ACCESS
		{
			owl_writel(WAKEUP             , pctl_base + SCTL);
			while((owl_readl(pctl_base + STAT) & 0x7) != 3);
			temp -= 1;
		}	
		for(cnt_i=0;cnt_i<temp;cnt_i++)
		{
			running_state = current_state +((current_state > running_state)?(-1):1);      //to next mode
			poll_state    = (running_state <= CONFIG) ? running_state :((running_state == ACCESS) ? 3 : 5);//get next state
			owl_writel(running_state , pctl_base + SCTL);
			while((owl_readl(pctl_base + STAT) & 0x7) != poll_state);
		}
	}
}

static void ddr_clkfreq_init(unsigned int ddr_clk)
{
	int freq_set = (ddr_clk / DDRCLKDIVSTEP) & 0xff;
	owl_clrbits(CMU_DDRPLL     , 0x1 << 8);
	owl_clrbits(CMU_DEVCLKEN0  , DDRCLK_EN);	
	owl_clrbits(CMU_DEVRST0    , DDRRST_EN);//CMU_RST_DDR reset ddr controller
	owl_udelay(10);
	owl_setbits(CMU_DEVRST0    , DDRRST_EN);   //CMU_RST_DDR
	owl_udelay(100);
	owl_writel(0x300 | freq_set     , CMU_DDRPLL);
	owl_udelay(120);
	owl_setbits(CMU_DEVCLKEN0  , DDRCLK_EN);
	owl_udelay(200);
}

#if 0
static void _par_printf(struct dcu_parameter *par) { 
	owl_printf("#tim :tREFI     = 0x%8x ,tMRD  = 0x%8x ,tRFC   = 0x%8x,tRP    = 0x%8x\n" ,
	              par->tREFI        ,par->tMRD     ,par->tRFC     ,par->tRP);
	owl_printf("#tim :tRTW      = 0x%8x ,tAL   = 0x%8x ,tCL    = 0x%8x,tWL    = 0x%8x\n" ,
	              par->tRTW         ,par->tAL      ,par->tCL      ,par->tWL);	
	owl_printf("#tim :tREFI_MEM = 0x%8x ,tRAS  = 0x%8x ,tRC    = 0x%8x,tRCD   = 0x%8x\n" ,
	              par->tREFI_MEM    ,par->tRAS     ,par->tRC      ,par->tRCD);
	owl_printf("#tim :tRRD      = 0x%8x ,tRTP  = 0x%8x ,tWR    = 0x%8x,tWTR   = 0x%8x\n" ,
	              par->tRRD         ,par->tRTP     ,par->tWR      ,par->tWTR);
	owl_printf("#tim :tEXSR     = 0x%8x ,tXP   = 0x%8x ,tXPDLL = 0x%8x,tZQCS  = 0x%8x\n" ,
	              par->tEXSR        ,par->tXP      ,par->tXPDLL   ,par->tZQCS);	
	owl_printf("#tim :tZQCSI    = 0x%8x ,tDQS  = 0x%8x ,tCKSRE = 0x%8x,tCKSRX = 0x%8x\n" ,
	              par->tZQCSI       ,par->tDQS     ,par->tCKSRE   ,par->tCKSRX);
	owl_printf("#tim :tCKE      = 0x%8x ,tMOD  = 0x%8x ,tRSTL  = 0x%8x,tZQCL  = 0x%8x\n" ,
	              par->tCKE         ,par->tMOD     ,par->tRSTL    ,par->tZQCL);
	owl_printf("#tim :tFAW      = 0x%8x ,tMRR  = 0x%8x ,tCKESR = 0x%8x,tDPD   = 0x%8x\n" ,
	              par->tFAW         ,par->tMRR     ,par->tCKESR   ,par->tDPD);

	owl_printf("#pctl :pctl_TINIT    = 0x%8x ,pctl_TRSTH   = 0x%8x ,pctl_MCFG  = 0x%8x\n" ,
	              par->pctl_TINIT        ,par->pctl_TRSTH      ,par->pctl_MCFG);
	owl_printf("#pctl :pctl_dfiwrlat = 0x%8x ,pctl_dfirden = 0x%8x\n",
	              par->pctl_dfiwrlat     ,par->pctl_dfirden);	

	owl_printf("#publ :publ_MR0   = 0x%8x ,publ_MR1   = 0x%8x ,publ_MR2   = 0x%8x,publ_MR3 = 0x%8x\n" ,
	              par->publ_MR0       ,par->publ_MR1      ,par->publ_MR2     ,par->publ_MR3);
	owl_printf("#publ :publ_PTR0  = 0x%8x ,publ_PTR1   = 0x%8x ,publ_PTR2  = 0x%8x\n" ,
	              par->publ_PTR0      ,par->publ_PTR1     ,par->publ_PTR2);
	owl_printf("#publ :publ_DTPR0 = 0x%8x ,publ_DTPR1 = 0x%8x ,publ_DTPR2 = 0x%8x\n" ,
	              par->publ_DTPR0     ,par->publ_DTPR1    ,par->publ_DTPR2);
	owl_printf("#publ :publ_DCR   = 0x%8x\n" ,
	              par->publ_DCR);
}
#endif
static void init_parameter_LP(struct dcu_parameter *parameter, struct owl_ddr_config *ddr_hwinfo)
{
	unsigned int temp_v;
	parameter->tREFI           = 39;
	parameter->tMRD            = 5;
	temp_v = ddr_hwinfo->ddr_cap / ddr_hwinfo->rank;      //default single chip width is 32bit
	temp_v = ((temp_v<128)? 90 : ((temp_v<768)? 130 : 210));
	parameter->tRFC            = ((temp_v * ddr_hwinfo->ddr_clk)/1000 + 1);
	parameter->tAL             = 0;
	if(ddr_hwinfo->ddr_clk <= 400)
	{
		parameter->tCL           = 6;
		parameter->tWL           = 3;
	}
	else if((ddr_hwinfo->ddr_clk <= 467)&&(ddr_hwinfo->ddr_type == tpy_lpddr2))
	{
		parameter->tCL           = 7;
		parameter->tWL           = 4;
	}
	else if(ddr_hwinfo->ddr_clk <= 533)
	{
		parameter->tCL           = 8;
		parameter->tWL           = 4;
	}
	else if(ddr_hwinfo->ddr_clk <= 600)
	{
		parameter->tCL           = 9;
		parameter->tWL           = 5;
	}
	else if(ddr_hwinfo->ddr_clk <= 667)
	{
		parameter->tCL           = 10;
		parameter->tWL           = 6;
	}
	else if(ddr_hwinfo->ddr_clk <= 733)
	{
		parameter->tCL           = 11;
		parameter->tWL           = 6;
	}
	else 
	{
		parameter->tCL           = 12;
		parameter->tWL           = 6;
	}
	parameter->tRRD            = (ddr_hwinfo->ddr_clk < 200) ? (2) : ((10 * ddr_hwinfo->ddr_clk / 1000) + 1);
	parameter->tFAW            = (50 * ddr_hwinfo->ddr_clk / 1000 ) + 1;
	parameter->tRTW            = (55 * ddr_hwinfo->ddr_clk / 10000) + 1;  //See p94~p95 at upctl
	parameter->tRAS            = (42 * ddr_hwinfo->ddr_clk / 1000 ) + 1;	
	parameter->tRP             = (21 * ddr_hwinfo->ddr_clk / 1000 ) + 1;
	if(ddr_hwinfo->ddr_type    == tpy_lpddr2)
	{
		parameter->tRC           = (63 * ddr_hwinfo->ddr_clk / 1000) + 1;
		parameter->tRCD          = (18 * ddr_hwinfo->ddr_clk / 1000) + 1;
		parameter->tRTP          = (ddr_hwinfo->ddr_clk<267) ? (2) : ((75 * ddr_hwinfo->ddr_clk / 10000) + 1);
		parameter->tWR           = (ddr_hwinfo->ddr_clk<200) ? (3) : ((15 * ddr_hwinfo->ddr_clk / 1000 ) + 1);		
		parameter->tXP           = parameter->tRTP;//(ddr_hwinfo->ddr_clk<267) ? (2) : ((75 * ddr_hwinfo->ddr_clk / 10000) + 1); 
		parameter->tCKE          = 3;
		parameter->tEXSR	     = parameter->tRFC + (10 * ddr_hwinfo->ddr_clk / 1000) + 1;	
		parameter->tCKSRE        = 0;
		parameter->tMRR          = 2; 
		parameter->pctl_MCFG       = 0x10E62040;
	}
	else
	{
		parameter->tRC           = (57 * ddr_hwinfo->ddr_clk / 1000) + 1;
		parameter->tRCD          = (18 * ddr_hwinfo->ddr_clk / 1000) + 1;
		parameter->tRTP          = (ddr_hwinfo->ddr_clk<534) ? (4) : ((75 * ddr_hwinfo->ddr_clk / 10000) + 1);
		parameter->tWR           = (ddr_hwinfo->ddr_clk<267) ? (4) : ((15 * ddr_hwinfo->ddr_clk / 1000 ) + 1);		
		parameter->tXP           = (ddr_hwinfo->ddr_clk<400) ? (3) : ((75 * ddr_hwinfo->ddr_clk / 10000) + 1); 
		parameter->tCKE          = (ddr_hwinfo->ddr_clk<400)?(3):((75* ddr_hwinfo->ddr_clk / 10000)+1);
		parameter->tEXSR	     = parameter->tRFC + (10 * ddr_hwinfo->ddr_clk / 1000) + 1;	
		parameter->tCKSRE        = 2;
		parameter->tMRR          = 4; 
		parameter->pctl_MCFG       = 0x10662040;
	}	
	parameter->tWTR            = parameter->tRTP;
	parameter->tZQCS           = (90 * ddr_hwinfo->ddr_clk / 1000) + 1;
	parameter->tXPDLL          = 0;
	
	parameter->tZQCSI          = parameter->tREFI * RFNUM;//0;
	parameter->tDQS            = 2; //See upctl p288 "programming of tDQS"
	
	parameter->tCKSRX          = 2;
	parameter->tCKE            = 3;
	parameter->tMOD            = 0;
	parameter->tRSTL           = 0;
	parameter->tZQCL           = (360 * ddr_hwinfo->ddr_clk / 1000) + 1;
	
	parameter->tCKESR          = (ddr_hwinfo->ddr_clk<200) ? (3) : ((15 * ddr_hwinfo->ddr_clk / 1000) + 1);	
	parameter->tDPD            = 500;
	parameter->tREFI_MEM       = 0;	
//------------------------------INIT_UPCTL_VALUE----------------------
	parameter->pctl_TINIT      = 200;
	parameter->pctl_TRSTH      = 0;
	
	parameter->pctl_dfiwrlat   = (parameter->tWL%2)?((parameter->tWL-1)/2):((parameter->tWL)/2);
	temp_v = parameter->tCL + (40 * ddr_hwinfo->ddr_clk / 10000);
	parameter->pctl_dfirden    = (temp_v%2)?((temp_v-2)/2):((temp_v-3)/2);


//------------------------------init_PUBL_VALUE----------------------
	parameter->publ_MR0        = 0;
	parameter->publ_MR1        = ((((parameter->tWR>9)?(parameter->tWR-10):(parameter->tWR - 2))<<5 ) | ((BSTLen==8)?3:2));
	parameter->publ_DCR        = (0xc |(1<<28));

	parameter->publ_MR2        = (parameter->tCL - 2) | (((parameter->tWR>9)?1:0)<<4);
	parameter->publ_MR3        = DRV_ST;
	temp_v = (10240 * ddr_hwinfo->ddr_clk / 1000) + 1;
	parameter->publ_PTR0       = (100 * ddr_hwinfo->ddr_clk / 1000 + 1) | (((temp_v>0xfff)?0xfff:temp_v) << 6) | 0x280000;
	parameter->publ_PTR1       = (400 * ddr_hwinfo->ddr_clk + 1) | ((ddr_hwinfo->ddr_clk / 5 + 1) <<19 );
	temp_v = (2 * ddr_hwinfo->ddr_clk) + 1;
	parameter->publ_PTR2       = (((temp_v>0x3ff)?0x3ff:temp_v) << 17) | (22 * ddr_hwinfo->ddr_clk + 1);
	parameter->publ_DTPR0      = parameter->tMRD|(parameter->tRTP<<2)|(parameter->tWTR<<5)|(parameter->tRP<<8)|(parameter->tRCD<<12)
	                           | (parameter->tRAS<<16)|(parameter->tRRD<<21)|(parameter->tRC<<25);
	parameter->publ_DTPR1      = (parameter->tFAW << 3)|(parameter->tRFC<<16)|(((55*ddr_hwinfo->ddr_clk/10000)+1)<<27)|((25*ddr_hwinfo->ddr_clk/10000)<<24);
	parameter->publ_DTPR2      = parameter->tEXSR | (parameter->tXP << 10) | (parameter->tCKE << 15) | (512 << 19);
}

static void init_parameter_ddr3(struct dcu_parameter *parameter, struct owl_ddr_config *ddr_hwinfo)
{
	unsigned int temp_v;
	temp_v = ddr_hwinfo->ddr_cap / (((ddr_hwinfo->ddr_bits_width==x32width)?32:16) / 16) / ddr_hwinfo->rank;  //default single chip width is 16bit
	temp_v = ((temp_v<256)? 110 : ((temp_v<512)? 160 : ((temp_v<1024)? 260 : 350)));
	parameter->tRFC            = ((temp_v * ddr_hwinfo->ddr_clk)/1000)+1;
	parameter->tREFI           = 78;
	parameter->tMRD            = 4;
	parameter->tAL             = PCAL;

	if (ddr_hwinfo->ddr_clk     <= 400)
	{
		parameter->tCL           = 6;
		parameter->tWL           = 5;
		parameter->tRP           = (15  * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tRAS          = (375 * ddr_hwinfo->ddr_clk / 10000 ) + 1;
		parameter->tRRD          = 4;
		parameter->tCKE          = 3;
		parameter->tXP           = 3;
		parameter->tFAW          = (50  * ddr_hwinfo->ddr_clk / 1000  ) + 1;
	}
	else if(ddr_hwinfo->ddr_clk <= 533)
	{
		//parameter->tCL           = 7;
		parameter->tCL           = 8;
		parameter->tWL           = 6;
		parameter->tRP           = (15  * ddr_hwinfo->ddr_clk / 1000 ) + 1;
		parameter->tRAS          = (375 * ddr_hwinfo->ddr_clk / 10000) + 1;
		parameter->tCKE          = 3;
		parameter->tXP           = (75  * ddr_hwinfo->ddr_clk / 10000) + 1;
		parameter->tRRD          = (10  * ddr_hwinfo->ddr_clk / 1000 ) + 1;
		parameter->tFAW          = (50  * ddr_hwinfo->ddr_clk / 1000 ) + 1;
	}
	else if(ddr_hwinfo->ddr_clk <= 667)
	{
		//parameter->tCL           = 9;
		parameter->tCL           = 10;
		parameter->tWL           = 7;
		parameter->tRP           = (15   * ddr_hwinfo->ddr_clk / 1000   ) + 1;
		parameter->tRAS          = (36   * ddr_hwinfo->ddr_clk / 1000   ) + 1;
		parameter->tCKE          = (5625 * ddr_hwinfo->ddr_clk / 1000000) + 1;
		parameter->tXP           = (6    * ddr_hwinfo->ddr_clk / 1000   ) + 1;
		parameter->tRRD          = (75   * ddr_hwinfo->ddr_clk / 10000  ) + 1;
		parameter->tFAW          = (45   * ddr_hwinfo->ddr_clk / 1000   ) + 1;
	}
	else if(ddr_hwinfo->ddr_clk <= 800)
	{
		//parameter->tCL           = 9;
		parameter->tCL           = 11;
		parameter->tWL           = 8;
		parameter->tRP           = (1375 * ddr_hwinfo->ddr_clk / 100000) + 1;
		parameter->tRAS          = (35   * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tCKE          = (5    * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tXP           = (6    * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tRRD          = (75   * ddr_hwinfo->ddr_clk / 10000 ) + 1;
		parameter->tFAW          = (40   * ddr_hwinfo->ddr_clk / 1000  ) + 1;
	}
	else   //GL5206 is not supportted
	{
		parameter->tCL           = 13;
		parameter->tWL           = 9;
		parameter->tRP           = (1391 * ddr_hwinfo->ddr_clk / 100000) + 1;
		parameter->tRAS          = (34   * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tCKE          = (5    * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tXP           = (6    * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tRRD          = (6    * ddr_hwinfo->ddr_clk / 1000  ) + 1;
		parameter->tFAW          = (35   * ddr_hwinfo->ddr_clk / 1000  ) + 1;
	}
	parameter->tRC             = parameter->tRP + parameter->tRAS;
	parameter->tRCD            = parameter->tRP;
	parameter->tREFI_MEM       = (78* ddr_hwinfo->ddr_clk/10);//(78 * ddr_hwinfo->ddr_clk / 10 - 32) / 2;
	parameter->tRTW            = parameter->tCL - parameter->tWL + 6;     //zhongxu get through simulation
	parameter->tRTP            = (ddr_hwinfo->ddr_clk<534)?(4):(75 * ddr_hwinfo->ddr_clk / 10000+1);
	parameter->tWR             = (ddr_hwinfo->ddr_clk<400)?(6):(15 * ddr_hwinfo->ddr_clk / 1000+1);
	parameter->tWTR            = parameter->tRTP;			
	parameter->tEXSR	       = 512;
	parameter->tXPDLL          = (ddr_hwinfo->ddr_clk<417)?(10):(24 * ddr_hwinfo->ddr_clk / 1000+1);
	parameter->tZQCS           = 64;
	parameter->tZQCSI          = parameter->tREFI_MEM * RFNUM;//0;
	parameter->tDQS            = 1;
	parameter->tCKSRE          = (ddr_hwinfo->ddr_clk<500)?(5):(10 * ddr_hwinfo->ddr_clk / 1000+1);
	parameter->tCKSRX          = (ddr_hwinfo->ddr_clk<500)?(5):(10 * ddr_hwinfo->ddr_clk / 1000+1);	
	parameter->tMOD            = 12;	
	parameter->tRSTL           = (150 * ddr_hwinfo->ddr_clk / 1000);
	parameter->tZQCL           = 512;
	parameter->tMRR            = 2; 
	parameter->tCKESR          = parameter->tCKE + 1;	
	parameter->tDPD            = 0;
		
//------------------------------INIT_UPCTL_VALUE----------------------
	parameter->pctl_TINIT      = 200;
	parameter->pctl_TRSTH      = 500;
	parameter->pctl_MCFG       = (0x00028029|(((parameter->tFAW)/(parameter->tRRD)+1 -4)<<18));
	if(S_HDRSEL == sdrmode)
	{
		parameter->pctl_dfiwrlat   = (parameter->tWL-1);
		parameter->pctl_dfirden    = (parameter->tCL-2);
	}
	else
	{
		parameter->pctl_dfiwrlat   = (parameter->tWL%2)?((parameter->tWL-1)/2):((parameter->tWL-2)/2);
		parameter->pctl_dfirden    = (parameter->tCL%2)?((parameter->tCL-3)/2):((parameter->tCL-2)/2);
	}
//------------------------------init_PUBL_VALUE----------------------
	parameter->publ_DCR        = (0xb);
	parameter->publ_MR0        = (((BSTLen == 8)?0:2) |((parameter->tCL-4) << 4) | (1 << 8) | (parameter->tWR << 9) | (1 << 12));
	temp_v = (ddr_hwinfo->wrodt == 0)?0:((ddr_hwinfo->wrodt == 1)?4:((ddr_hwinfo->wrodt == 2)?0x40:((ddr_hwinfo->wrodt == 3)?0x44:((ddr_hwinfo->wrodt == 4)?0x200:((ddr_hwinfo->wrodt == 5)?0x204:0)))));
	parameter->publ_MR1        = (DLLEN | ODIC | temp_v | (PCAL<<3) | (WLEVEL << 7));
	parameter->publ_MR2        = ((parameter->tWL - 5) << 3) | (ASREN << 6) | ( RTTWR << 9);
	parameter->publ_MR3        = 0;
	parameter->publ_PTR0       = (((80*ddr_hwinfo->ddr_clk/1000+1)>0x3f)?0x3f:(80*ddr_hwinfo->ddr_clk/1000+1))|((5120*ddr_hwinfo->ddr_clk/1000+1)<<6)|(10 << 18);
	parameter->publ_PTR1       = (500*ddr_hwinfo->ddr_clk)|((parameter->tRFC + (10 * ddr_hwinfo->ddr_clk / 1000))<<19);
	if (ddr_init_param.init_mode == DDR_INIT_FULL)
		parameter->publ_PTR2     = (ddr_hwinfo->ddr_clk << 17) | (200* ddr_hwinfo->ddr_clk);
	else
		parameter->publ_PTR2     = (ddr_hwinfo->ddr_clk/10);
	parameter->publ_DTPR0      = (parameter->tMRD-4)|(parameter->tRTP<<2)|(parameter->tWTR<<5)|(parameter->tRP<<8)|(parameter->tRCD<<12)
	                           | (parameter->tRAS<<16)|(parameter->tRRD<<21)|(parameter->tRC<<25);
	parameter->publ_DTPR1      = (parameter->tFAW << 3)|((parameter->tMOD - 12) << 9)|(parameter->tRFC<<16);
	parameter->publ_DTPR2      = parameter->tEXSR | (parameter->tXPDLL << 10) | (parameter->tCKESR << 15) | (512 << 19);
}

static int _init_dcu_sequence(struct owl_ddr_config *ddr_hwinfo, struct dcu_parameter *parameter, unsigned char channel)
{
	unsigned int pub_base  = (channel == 0) ? PUBL0_BASE : PUBL1_BASE;
	unsigned int pctl_base = (channel == 0) ? PCTL0_BASE : PCTL1_BASE;
	unsigned int temp_v;
	owl_writel(0x0                        , pub_base + ACDLLCR);  //DLL Soft Rest: Soft resets the AC DLL by driving the DLL soft reset pin.
	owl_udelay(10);
	owl_writel(0x40000000                 , pub_base + ACDLLCR);
	owl_udelay(10);
	owl_writel(0x10000000				  , pub_base + PIR);
	while(owl_readl(pub_base + PIR)&0x10000000 == 0);
	_set_mode(INIT                    ,channel);
//step 1:	Assert and release uPCTL reset pins. Program if necessary TOGCNT1U,TOGCNT100N, TINIT, and TRSTH.
//uPCTL loads initial values into registers.
	owl_writel(0x4                        , pctl_base + DFITCTRLUPDMIN);
	owl_writel(0xa                        , pctl_base + DFITCTRLUPDMAX);
	owl_writel(0x8                        , pctl_base + DFITCTRLUPDDLY);
	if(S_HDRSEL == sdrmode)
	{
		owl_writel(ddr_hwinfo->ddr_clk      , pctl_base + TOGCNT1U);
		owl_writel(ddr_hwinfo->ddr_clk /10  , pctl_base + TOGCNT100N);
	}
	else
	{
		owl_writel(ddr_hwinfo->ddr_clk / 2  , pctl_base + TOGCNT1U);
		owl_writel(ddr_hwinfo->ddr_clk / 20 , pctl_base + TOGCNT100N);
	}                                  
	owl_writel(parameter->pctl_TINIT      , pctl_base + TINIT);  
	owl_writel(parameter->pctl_TRSTH      , pctl_base + TRSTH); 
	
	owl_writel(0x1                  , PHY_RST);
	owl_udelay(100);
	owl_writel(0x0                  , PHY_RST);
	owl_udelay(100);
//step 2:	Start power-up sequence by writing POWCTL.power_up_start = 1. 
//Monitor power-up status by polling POWSTAT.power_up_done = 1?
//uPCTL drives CKE and RESETN# signals according to TINIT and TRSTH and sets POWSTAT.power_up_done = 1 when completed.
	owl_writel(0x1                        , pctl_base + POWCTL);
	temp_v = ddr_hwinfo->ddr_clk * 500;
	while (!(owl_readl(pctl_base + POWSTAT) & 0x1));
	{
		while(!(temp_v--))
		{
			return 0x4d00fa11;
		}
	}
//step 3:	Configure uPCTL抯 MCFG register. 
//Allows uPCTL to differentiate between Power Up sequences for different memory protocols (next step).
	owl_writel(parameter->pctl_MCFG       , pctl_base + MCFG);
	
		owl_writel((((((parameter->tFAW)/(parameter->tRRD)+1 -4)+4)*(parameter->tRRD)-(parameter->tFAW))<<8), pctl_base + MCFG1);
	
	if ((ddr_hwinfo->ddr_type&0xf0) == 0x10)
	{
		owl_setbits(pctl_base + MCFG1  , 0x00400000);
	}
//step 4:	Start PHY initialization by accessing relevant PUBL registers.	
	owl_writel(parameter->publ_DCR        , pub_base  + DCR);	
	if((ddr_hwinfo->ddr_type&0xf) == tpy_ddr3)
		owl_writel(parameter->publ_MR0      , pub_base + MR0);
	else //if((ddr_hwinfo->ddr_type ==tpy_lpddr2) || (ddr_hwinfo->ddr_type ==tpy_lpddr3))
		owl_writel(parameter->publ_MR3      , pub_base + MR3);
	owl_writel(parameter->publ_MR1        , pub_base + MR1);
	owl_writel(parameter->publ_MR2        , pub_base + MR2);
	owl_writel(parameter->publ_PTR0       , pub_base + PTR0);
	owl_writel(parameter->publ_PTR1       , pub_base + PTR1);
	owl_writel(parameter->publ_PTR2       , pub_base + PTR2);	
	owl_writel(parameter->publ_DTPR0      , pub_base + DTPR0);
	owl_writel(parameter->publ_DTPR1      , pub_base + DTPR1);
	owl_writel(parameter->publ_DTPR2      , pub_base + DTPR2);
	owl_writel(TRAINADDR                  , pub_base + DTAR);		//Training address	
	if(ddr_hwinfo->rdodt != 0)
	{
		owl_writel(DQSUDR<<4|DQSUDR<<8|0x801, pub_base + DXCCR);
		owl_writel(0x0021                   , pub_base + ODTCR);
		temp_v = ((ddr_hwinfo->rdodt == 1)? 4 : ((ddr_hwinfo->rdodt == 2) ? 2 :((ddr_hwinfo->rdodt == 3) ? 6 : ((ddr_hwinfo->rdodt == 4) ? 12 : 8)))) << 4;
		owl_writel(ddr_hwinfo->zpzq | temp_v  , pub_base + ZQ0CR1);	
		owl_writel(0x0bc03812               , pub_base + ACIOCR);	
	}		
	else{
		owl_writel(0x800                    , pub_base + DXCCR);
		owl_writel(0                        , pub_base + ODTCR);
		owl_writel(ddr_hwinfo->zpzq           , pub_base + ZQ0CR1);	
		owl_writel(0x0bc3f8f6               , pub_base + ACIOCR);	
		owl_clrbits(pub_base + DX0GCR  , 0x00000786);
		owl_clrbits(pub_base + DX1GCR  , 0x00000786);
		owl_clrbits(pub_base + DX2GCR  , 0x00000786);
		owl_clrbits(pub_base + DX3GCR  , 0x00000786);	
	}
	if(ddr_hwinfo->wrodt != 0)
		owl_setbits(pub_base + ODTCR   , 0x00210000);

  if(ddr_hwinfo->ddr_bits_width==x32width)
  {
  	owl_setbits(pub_base + DX2GCR  , 0x1); // enable dq bus[23:16] for x16 width
 		owl_setbits(pub_base + DX3GCR  , 0x1); // enable dq bus[31:24] for x16 width
  }
  else
  {
  	owl_clrbits(pub_base + DX2GCR  , 0x1); // disable dq bus[23:16] for x16 width
 		owl_clrbits(pub_base + DX3GCR  , 0x1); // disable dq bus[31:24] for x16 width
  }
//judge zq 
	temp_v = 100;
	while((owl_readl(pub_base + ZQ0SR0)&0x80000000) != 0x80000000)
	{
		while(!(temp_v--))
		{
			return 0x2a00fa11;
		}
	}
	if(ddr_hwinfo->ddr_type == tpy_lpddr3) //Enable LPDDR3 when pub in LPDDR2 mode
		owl_writel(0x1                      ,pub_base  + LPDDR3_FIX);
	owl_writel((((ddr_hwinfo->ddr_type&0xf)==tpy_ddr3)?0xf2001807:0xf1001807) , pub_base + DSGCR);
//step 5:	Monitor PHY initialization status polling the PUBL register PGSR.IDONE.1
//PHY returns PGSR.IDONE = 1 when PHY initialization is completed.
	temp_v = 100;
	while ((owl_readl(pub_base + PGSR) & 0x7) != 0x7)
	{
		while(!(temp_v--))
		{
			return 0x9A00fa11;
		}
	}
	owl_writel(0x01802e02|(((ddr_hwinfo->rank==1)?1:3)<<18)
			                              , pub_base + PGCR);
	if (ddr_init_param.init_mode == DDR_INIT_FULL)
		owl_writel((((ddr_hwinfo->ddr_type&0xf)==tpy_ddr3)?0x7f:0x5f)
			                              , pub_base + PIR);
	else
		owl_writel(0x3f                     , pub_base + PIR); 
	while (owl_readl(pub_base + PGSR) != 0xF);

	if (ddr_init_param.init_mode == DDR_INIT_RESUME) //precharge 8 times
	{ /*在s2 起来后，phy初始化过程中，对ddr 进行8次 precharge和reflesh。*/
		for (temp_v = 0; temp_v < 8; temp_v++)
		{
			owl_writel((0x80000001|((ddr_hwinfo->rank==1)?0x100000:0x300000)) , PCTL0_BASE + MCMD);
			while ((owl_readl(PCTL0_BASE + MCMD)&0x80000000) == 0x80000000);
		}
		for (temp_v = 0; temp_v < 8; temp_v++)
		{
			owl_writel((0x80000002|((ddr_hwinfo->rank==1)?0x100000:0x300000)) , PCTL0_BASE + MCMD);
			while ((owl_readl(PCTL0_BASE + MCMD)&0x80000000) == 0x80000000);
		}
	}
	owl_writel(0x81                       , pub_base + PIR);
	do{
		temp_v = owl_readl(pub_base + PGSR);
		if(temp_v == 0x3F)
			return 0x1017fa11;
	}while(temp_v != 0x1F);		
//step 6:	Monitor DFI initialization status polling DFISTSTAT0.dfi_init_complete.
//uPCTL returns DFISTSTAT0.dfi_init_complete = 1 when DFI initialization is completed.
	temp_v = 100;
	while (!(owl_readl(pctl_base + DFISTSTAT0) & 0x1))
	{
		while(!(temp_v--))
		{
			return 0xdf00fa11;
		}
	}
//step 7:Configure rest of uPCTL. Program all timing T*registers.
//uPCTL loads initial values into registers.		
	owl_writel(parameter->tMRD           , pctl_base + TMRD);
	owl_writel(parameter->tRFC           , pctl_base + TRFC);
	owl_writel(parameter->tRP            , pctl_base + TRP);
	owl_writel(parameter->tAL            , pctl_base + TAL);
	owl_writel(parameter->tCL            , pctl_base + TCL);
	owl_writel(parameter->tWL            , pctl_base + TCWL);
	owl_writel(parameter->tRAS           , pctl_base + TRAS);
	owl_writel(parameter->tRC            , pctl_base + TRC);
	owl_writel(parameter->tRCD           , pctl_base + TRCD);
	owl_writel(parameter->tRRD           , pctl_base + TRRD);
	owl_writel(parameter->tRTP           , pctl_base + TRTP);
	owl_writel(parameter->tWR            , pctl_base + TWR);
	owl_writel(parameter->tWTR           , pctl_base + TWTR);
	owl_writel(parameter->tEXSR          , pctl_base + TEXSR);
	owl_writel((parameter->tREFI|((RFNUM-1) << 16))
	                                 , pctl_base + TREFI);
	owl_writel(parameter->tREFI_MEM      , pctl_base + TREFI_MEM);
	owl_writel(parameter->tXP            , pctl_base + TXP);
	owl_writel(parameter->tDQS           , pctl_base + TDQS);
	owl_writel(parameter->tRTW           , pctl_base + TRTW);
	owl_writel(parameter->tCKSRE         , pctl_base + TCKSRE);
	owl_writel(parameter->tCKSRX         , pctl_base + TCKSRX);
	owl_writel(parameter->tMOD           , pctl_base + TMOD);
	owl_writel(parameter->tCKE           , pctl_base + TCKE);
	owl_writel(parameter->tMRR           , pctl_base + TMRR);
	owl_writel(parameter->tRSTL          , pctl_base + TRSTL);
	owl_writel(parameter->tZQCS          , pctl_base + TZQCS);
	owl_writel(parameter->tZQCL          , pctl_base + TZQCL);
	owl_writel(parameter->tXPDLL         , pctl_base + TXPDLL);
	owl_writel(parameter->tZQCSI         , pctl_base + TZQCSI);
	owl_writel(parameter->tCKESR         , pctl_base + TCKESR);
	owl_writel(parameter->tDPD           , pctl_base + TDPD); 

//step 9:	memory initialization procedure for ddr
	owl_writel((0x80000000|((ddr_hwinfo->rank==1)?0x100000:0x300000))
		                               , pctl_base + MCMD);
	while ((owl_readl(PCTL0_BASE + MCMD)&0x80000000) == 0x80000000);
	owl_writel((0x80000001|((ddr_hwinfo->rank==1)?0x100000:0x300000))
		                               , pctl_base + MCMD);
	while ((owl_readl(PCTL0_BASE + MCMD)&0x80000000) == 0x80000000);
	
	_set_mode(CONFIG                 ,channel);

	if(S_HDRSEL == sdrmode)
	{
		owl_writel(0xf00                     , pctl_base + SCFG);
		owl_writel(0xf                       , pctl_base + DFITPHYRDLAT);
		owl_writel(0x1                       , pctl_base + DFITPHYWRDATA);
	}
	else
	{
		owl_writel(0x700                     , pctl_base + SCFG);
		owl_writel(0xa                       , pctl_base + DFITPHYRDLAT);
		owl_writel(0x0                       , pctl_base + DFITPHYWRDATA);
	}
	owl_writel(((ddr_hwinfo->ddr_bits_width==x32width)?0x0:0x1)
																	 , pctl_base + PPCFG);
	owl_writel(((ddr_hwinfo->rank == 2)?0x21:0x1), pctl_base + DFIODTRANKMAP);    //ZWT+ 20141125
	owl_writel(0x7                       , pctl_base + DFISTCFG0);
	owl_writel(0x3                       , pctl_base + DFISTCFG1);
	owl_writel(0x3                       , pctl_base + DFISTCFG2);
	owl_writel(0x70101                   , pctl_base + DFILPCFG0);
	owl_writel(0x0                       , pctl_base + ECCCFG);
	
//step 8:	Configure the DFI timing parameter registers
//Ensures compatibility on the read and write paths between the uPCTL and the PHY. 
	owl_writel(parameter->pctl_dfiwrlat  , pctl_base + DFITPHYWRLAT);
	owl_writel(parameter->pctl_dfirden   , pctl_base + DFITPRDDATAEN);
	owl_writel(0x2                       , pctl_base + DFITDRAMCLKDIS);
	owl_writel(0x2                       , pctl_base + DFITDRAMCLKEN);
	owl_writel(0x2                       , pctl_base + DFITCTRLDELAY);
	owl_writel(0x1                       , pctl_base + DFITPHYUPDTYPE0);


	if(ddr_hwinfo->wrodt != 0)
		owl_writel(0x0808                  , pctl_base + DFIODTCFG);
	if(ddr_hwinfo->rdodt != 0)
		owl_setbits(pctl_base + DFIODTCFG , 0x0202);
//step 13: Enable CMDTSTAT register by writing CMDSTAT.cmd_tstat_en=1’b1. 
//Monitor command timers expiration by polling CMDTSTAT.cmd_tstat = 1’b1.
	owl_writel(0x1                       , pctl_base + CMDTSTATEN);
	while ((owl_readl(pctl_base + CMDTSTAT) & 0x1) != 0x1);

	while ((owl_readl(pctl_base + STAT) & 0x7) != 0x1);
	if (ddr_init_param.init_mode == DDR_INIT_RESUME)    //precharge 8 times
	{
		for (temp_v = 0; temp_v < 8; temp_v++)
		{
			owl_writel((0x80000002|((ddr_hwinfo->rank==1)?0x100000:0x300000))           , PCTL0_BASE + MCMD);
			while ((owl_readl(PCTL0_BASE + MCMD)&0x80000000) == 0x80000000);
		}
	}
	if ((ddr_hwinfo->ddr_type&0xf0) == 0x10)
	{
		_set_mode(ACCESS                  ,channel);
	}
	else
	{
		if((ddr_hwinfo->ddr_type&0xf)==tpy_ddr3)
			owl_setbits(pctl_base + MCFG1  , 0x60);
		else
			owl_setbits(pctl_base + MCFG1  , 0x20);
	//step 15: Write GO into SCTL.state_cmd register and poll STAT.ctl_stat = Access.	
		_set_mode(ACCESS                  ,channel);
		while(owl_readl(pctl_base + STAT) != 0x15);
	}
//-------------- config finished, DMM initialize ---------------------------	
	owl_writel(S_HDRSEL               , DMM_HDR);
	temp_v = ((BSTLen*((ddr_hwinfo->ddr_bits_width==x32width)?32:16))<256)?0:1;
	owl_writel(((ddr_hwinfo->rank-1)<<4)|(temp_v<<12)|(((BANK_NUMI==8)?1:0)<<3 )|((ddr_hwinfo->ddr_bits_width==x32width)?1:0)
                                   , DMM_INTERLEAVE_CONFIG);
//zwt	owl_writel(0x00000002                , DMM_AXI_PRIORITY);
//zwt	owl_writel(0x00000006                , DMM_AXI_PRIORITY);
	owl_setbits(DMM_SCH_CTRL        , 0x1 << 30);
	owl_writel(0x2                       , DMM_INTERLEAVE_FRAGMENT);
	temp_v   = (((parameter->tRTP + (parameter->tRP&0x1f) + parameter->tRCD - 1)>>2)+1)<<8;
	temp_v  |= (((parameter->tWL  + parameter->tWR + (parameter->tRP&0x1f) + parameter->tRCD + 3)>>2)+1)<<4;
	temp_v  |= ((parameter->tRC +3 )>>2)+1;
	owl_writel(temp_v                    , DMM_DDR_TIMING);
	owl_setbits(DMM_SCH_CTRL        , 0x1 << 27);
	owl_udelay(100); //waitting for "ACCESS" and DFI/DMM is completed
//-----------------------------------------------------------------------------------
//step end: self adapt get cap、col、row;Start to calibrate columns,rows and cap
	if(ddr_init_param.init_mode == DDR_INIT_FULL)
	{
		ddr_hwinfo->row_col &= 0xcf;
		if((ddr_hwinfo->row_col&0xcc) == 0)
		{
			//ddr_hwinfo->row_col is byte
			//bit[1:0]:select captype,0=Normal cap;1=Abnormal cap but page size equal;2=Abnormal cap and page size unequal.
			//bit[3:2]:columns address limit.
			//bit[7:6]:row address limit.	
			owl_writel(((owl_readl(DMM_INTERLEAVE_CONFIG) & 0xfffe1f99) | 0x00002006), DMM_INTERLEAVE_CONFIG); //cs--row--bank--col
			for(temp_v = 0; temp_v < 4; temp_v++) //get columns,"4" mean [0]:c0~c7;[1]:c0~c8;[2]:c0~c9;[3]:c0~c10
			{
				owl_writel(0,R0ADDROFFS);
				
				if(ddr_hwinfo->ddr_bits_width==x32width)
					owl_writel(R0ADDRDATA,(1<<(temp_v+10))+R0ADDROFFS); //10 = 8(mincol)+2(4byte)
				else
					owl_writew(R0ADDRDATA,(1<<(temp_v+9))+R0ADDROFFS);  //9  = 8(mincol)+1(2byte)				
				if((owl_readl(R0ADDROFFS) == R0ADDRDATA)||(temp_v == 3))
				{
					ddr_hwinfo->row_col |= (temp_v<<2);
					owl_writel(((owl_readl(DMM_INTERLEAVE_CONFIG) & 0xffffff99) | (temp_v<<1)) | 0x60, DMM_INTERLEAVE_CONFIG);//cs--row--bank--col
					for(temp_v = 0; temp_v < 4; temp_v++) //get rows,"4" mean [0]:a0~a12;[1]:a0~a13;[2]:a0~a14;[3]:a0~a15
					{
						owl_writel(0,R0ADDROFFS);
//						temp_w = (temp_v+((ddr_hwinfo->ddr_bits_width==x32width)?26:25)+((ddr_hwinfo->row_col&0xc)>>2));
						if((temp_v+((ddr_hwinfo->ddr_bits_width==x32width)?26:25)+((ddr_hwinfo->row_col&0xc)>>2)) < 31)
						{
							if(ddr_hwinfo->ddr_bits_width==x32width)
								owl_writel(R0ADDRDATA,(1<<(temp_v+26+((ddr_hwinfo->row_col&0xc)>>2)))+R0ADDROFFS); //26 = 8(mincol)+2(4byte)+13(minrow)+3(bank)
							else
								owl_writew(R0ADDRDATA,(1<<(temp_v+25+((ddr_hwinfo->row_col&0xc)>>2)))+R0ADDROFFS); //25 = 8(mincol)+1(2byte)+13(minrow)+3(bank)
						}
						if((owl_readl(R0ADDROFFS) == R0ADDRDATA)||((temp_v+((ddr_hwinfo->ddr_bits_width==x32width)?26:25)+((ddr_hwinfo->row_col&0xc)>>2)) == 31))
						{
							ddr_hwinfo->row_col |= (temp_v<<6);
							break;
						}
						else
						{
							if((temp_v+((ddr_hwinfo->ddr_bits_width==x32width)?26:25)+((ddr_hwinfo->row_col&0xc)>>2)) > 31)
								return 0xf000fa11;
						}
					}
					break;
				}
			}
		}
		temp_v = (((ddr_hwinfo->row_col&0xc)>>2)+((ddr_hwinfo->row_col&0xc0)>>6)+24+((ddr_hwinfo->ddr_bits_width==x32width)?2:1)+(ddr_hwinfo->rank-1));//24=8(mincol)+13(minrow)+3(bank)
		if(temp_v > 31)
			return 0xc000fa11;
		ddr_hwinfo->ddr_cap = ((((ddr_hwinfo->row_col & 0x3) == 0)?(1<<(temp_v-20)):(((1<<(temp_v-20)) * 3) / 4))); 
	}
	if((ddr_hwinfo->row_col & 0x3) == 0)
		owl_writel(((owl_readl(DMM_INTERLEAVE_CONFIG) & 0xfffc1899) | 0x2000 | (((ddr_hwinfo->ddr_cap/256)-1)<<8) | (ddr_hwinfo->row_col>>1)), DMM_INTERLEAVE_CONFIG);//default min cap is 256MByte
	else
		owl_writel(((owl_readl(DMM_INTERLEAVE_CONFIG) & 0xfffc1899) | (((ddr_hwinfo->row_col & 0x3) == 1)?0x02000:0x22000) | (((ddr_hwinfo->ddr_cap/256)-1)<<8) | ((ddr_hwinfo->row_col&0xfc)>>1)), DMM_INTERLEAVE_CONFIG);//default min cap is 256MByte

//------------------------------calibrate columns,rows and cap end---------------------------------------------
	return 0;
}
#if 0
int dcu_dev_timeout_init(void)
{
	owl_clrsetbits(DMM_TO_ETHERNET_DE, 0xFFFF, 0xF801);
	//owl_clrsetbits(DMM_TO_CPU_BISP, 0xFFFFFFFF, 0x0818F808);
	owl_clrsetbits(0xe01b00b4, 0xFFFF, 0x03ff);
	owl_writel(0x3180, DMM_TO_ENABLE);	//NOC_QOS_CTRL
	owl_writel(0xF8010000, DMM_TO_HDE_USB2_0);	//NOC_QOS_CTRL
	owl_clrbits(DMM_SCH_CTRL          , 0x4);
	owl_setbits(DMM_SCH_CTRL          , 0x4);
	return 0;
}
#endif
static void sys_lowtem_preheat(unsigned int amount)
{
	unsigned int amountcnt = 0;
	while(amountcnt<=amount)
	{
		owl_readl(amountcnt * 4);
		amountcnt++;
	}
}
/*
static void debug_ddr(struct owl_ddr_config ddr_param_scan)
{

	owl_printf("ddr_clk = 0x%x ddr_cap = %x \n ddr_bits_width = %x rank = %x ddr_type = %x \n row_col = %x rdodt = %x   wrodt = %x \n zpzq = %x, row_col_detect = %d  \n",
				ddr_param_scan.ddr_clk, ddr_param_scan.ddr_cap,ddr_param_scan.ddr_bits_width,ddr_param_scan.rank,ddr_param_scan.ddr_type,
				ddr_param_scan.row_col,ddr_param_scan.rdodt,ddr_param_scan.wrodt,ddr_param_scan.zpzq, ddr_param_scan.row_col_detect);

}*/

//-------------------------Mem initial--------------------------
static int mem_init(struct owl_ddr_config *ddr_hwinfo)
{
	struct dcu_parameter dram_par;
	unsigned char rank_res   = 0;
	unsigned char width_res  = 0;
	unsigned int ddrclk_res = 0;
	int ret_rst = 0;

	//debug_ddr(*ddr_hwinfo);

	if (ddr_init_param.init_mode == DDR_INIT_FULL)
	{
		rank_res                 = (ddr_hwinfo->rank<=2)?ddr_hwinfo->rank:0;
		ddr_hwinfo->rank           = (ddr_hwinfo->rank == 1)?1:2;
		width_res                = (ddr_hwinfo->ddr_bits_width<2)?ddr_hwinfo->ddr_bits_width:2;
		ddr_hwinfo->ddr_bits_width = (ddr_hwinfo->ddr_bits_width == 1)?1:0;	
		ddr_hwinfo->ddr_clk        = (ddr_hwinfo->ddr_clk/12)*12;
		ddr_hwinfo->ddr_cap        = 2048;
	}
	else if(ddr_init_param.init_mode == DDR_INIT_RESUME)
	{
		if(ddr_init_param.need_preheat)
		{
			ddrclk_res = ddr_hwinfo->ddr_clk;
			ddr_hwinfo->ddr_clk = 240;
		}
	}
	else
	{
		return -1;
	}
	
	do
	{
		if ((ddr_hwinfo->ddr_type&0xf) == tpy_ddr3)
			init_parameter_ddr3(&dram_par, ddr_hwinfo);
		else //if((ddr_hwinfo->ddr_type == tpy_lpddr2) || (ddr_hwinfo->ddr_type == tpy_lpddr3))
			init_parameter_LP(&dram_par, ddr_hwinfo);
		//_par_printf(&dram_par);
		ddr_clkfreq_init(ddr_hwinfo->ddr_clk);
		owl_printf("DRAM init mode%d,type%d,%dcs,x%dbus,rcc0x%x,wrodt%d,rdodt%d,drv%x,%dMHz.@170619_R...",ddr_init_param.init_mode,ddr_hwinfo->ddr_type,ddr_hwinfo->rank,((ddr_hwinfo->ddr_bits_width==0)?32:16),ddr_hwinfo->row_col,ddr_hwinfo->wrodt,ddr_hwinfo->rdodt,ddr_hwinfo->zpzq,ddr_hwinfo->ddr_clk);
		ret_rst = _init_dcu_sequence(ddr_hwinfo, &dram_par,0);
//		owl_printf("DRAM init mode%d,type%d,%dcs,x%d,%dMHz,drv%x,0x%x.@160321_R...",ddr_init_param.init_mode,ddr_hwinfo->ddr_type,ddr_hwinfo->rank,((ddr_hwinfo->ddr_bits_width==0)?32:16),ddr_hwinfo->ddr_clk,owl_readl(DMM_INTERLEAVE_CONFIG));
		if((ret_rst & 0x00ffffff) ==0xfa11 )
		{
			owl_printf("Fail: 0x%x.\n",ret_rst);
			return 0x1747fa11;
		}
		else if(ret_rst == 0x1017fa11)
		{
			owl_printf("Fail:0x%x,0x%x,0x%x,0x%x;\n",owl_readl(0xe01c8000 + DX0GSR0),owl_readl(0xe01c8000 + DX1GSR0),owl_readl(0xe01c8000 + DX2GSR0),owl_readl(0xe01c8000 + DX3GSR0));
			if((rank_res == 0)&&(ddr_hwinfo->rank == 2)&&(ddr_init_param.init_mode == DDR_INIT_FULL))
			{
				ddr_hwinfo->rank--;
			}
			else if((width_res == 2)&&(ddr_hwinfo->ddr_bits_width == 0)&&(ddr_init_param.init_mode == DDR_INIT_FULL))
			{
				ddr_hwinfo->ddr_bits_width++;
				if((rank_res == 0)&&(ddr_hwinfo->rank == 1))
				{
					ddr_hwinfo->rank++;
				}
			}
			else
			{
				if(ddr_init_param.init_mode == DDR_INIT_FULL)
				{
					ddr_hwinfo->rank = rank_res;
					ddr_hwinfo->ddr_bits_width = width_res;
				}
				return 0x1017fa11;
			}
		}
		else
		{
			owl_printf("Ok:DCU_CFG:0x%x,%dMByte Cap.\n",owl_readl(DMM_INTERLEAVE_CONFIG),ddr_hwinfo->ddr_cap);
			if(ddr_init_param.need_preheat)
				ddr_hwinfo->ddr_clk = ddrclk_res;
//			dcu_dev_timeout_init();
			if(ddr_init_param.need_preheat)
				sys_lowtem_preheat(0x500000);
			return 0;
		}
	}while(ret_rst);
	return 0;
}

static void ddr_enter_suspend(/*unsigned int channel*/struct owl_ddr_config *ddr_hwinfo)
{
	unsigned int channel = 0;
	unsigned int pctl_base = (channel == 0) ? PCTL0_BASE : PCTL1_BASE;
	unsigned int pub_base  = (channel == 0) ? PUBL0_BASE : PUBL1_BASE;
	unsigned int val, i = 0;

	_set_mode(ACCESS                  ,0); //switch status to ACCESS
	_set_mode(LP                      ,0); //enter PD
	while(owl_readl(pctl_base + STAT) != 0x45) {
		owl_udelay(5);
		i++;
		if ((i % 100) == 0)
			owl_printf("ch0->LP %d times status %d\n",i, owl_readl(pctl_base + STAT));
		if (i > 1000)
			break;
	}
	
	
//	owl_writel(0x3bfc3f1a, pub_base + ACIOCR);		
	owl_writel(0x1bfc3c18, pub_base + ACIOCR);		
	owl_writel(0x0000080c, pub_base + DXCCR);
	owl_writel(0xc5fc0000, pub_base + DSGCR);
	owl_udelay(10);

	val = owl_readl(DMM_SCH_CTRL);	 //no send CMD to uPCTL
	val |= (1 << 1);
	owl_writel(val, DMM_SCH_CTRL);
	owl_udelay(1);
	val &= 0xefffffff;               //DCU PD indicating signal,A low to high transition by software of this bit will tell analog PHY that VDD will be PD a few momnets later
	owl_writel(val, DMM_SCH_CTRL);
	owl_mdelay(5);
	val |= (1 << 28);
	owl_writel(val, DMM_SCH_CTRL);
	owl_mdelay(5);
}


/* 函数名: ddr_init
 * 输入: 
 *		ddr_hwinfo: ddr初始化参数
 * 输出: 
 *		ddr_hwinfo: 返回值为0时有效，ddr初始化完成后会更新ddr_info中的值
 * 返回值: 
 *		若初始化成功返回0，否则返回errno
 * 说明: 
 *		ddr_init函数会根据ddr_info传入的参数尝试初始化ddr，
 * 	若成功就将try出来的参数写回ddr_info。
 *	下次调用时将更新后的ddr_info传入，可缩短初始化时间
 */
int __ddr_init(struct owl_ddr_config *ddr_hwinfo)
{
	ddr_init_param.init_mode = DDR_INIT_FULL;
	ddr_init_param.need_preheat = 0;
	
	ddr_hwinfo->ddr_clk = 0x1d4;
	ddr_hwinfo->ddr_cap = 0x0;
	ddr_hwinfo->ddr_bits_width = 0x0;
	ddr_hwinfo->rank = 0x0;
	ddr_hwinfo->ddr_type = 0x2;
	ddr_hwinfo->row_col = 0x0;
	ddr_hwinfo->rdodt = 0x1;
	ddr_hwinfo->wrodt = 0x1;
	ddr_hwinfo->zpzq = 0xc;
	
	return mem_init(ddr_hwinfo);
}

/* 函数名: ddr_resume
 * 输入: 
 *		ddr_hwinfo: ddr_init更新后的ddr参数
 *		need_preheat: 若天气寒冷，ddr需要预热则need_preheat设置为1；
 *			若天气温暖，ddr不需要预热则need_preheat设置为0
 * 返回值: 
 *		若resume成功返回0，否则返回errno
 * 说明: 
 *		从自刷新态resume到控制态时调用，将ddr从自刷新态转换为控制态
 */
int ddr_resume(struct owl_ddr_config *ddr_hwinfo, int need_preheat)
{
	int i;
	
	ddr_init_param.init_mode = DDR_INIT_RESUME;
	ddr_init_param.need_preheat = (char)need_preheat;
	if(need_preheat) {
		for (i = 0; i < 10; i++) {
			if (mem_init(ddr_hwinfo) == 0)
				break;
		}
	}
	
	ddr_init_param.need_preheat = 0;
	return mem_init(ddr_hwinfo);
}

/*-------------------------------------------------------------------------
 * 函数名: ddr_suspend
 * 输入: 
 *		ddr_hwinfo: ddr_init更新后的ddr参数
 *		need_preheat: 若天气寒冷，ddr需要预热则need_preheat设置为1；
 *			若天气温暖，ddr不需要预热则need_preheat设置为0
 * 返回值: 
 *		若初始化成功返回0，否则返回errno
 * 说明: 
 *		从控制态suspend到自刷新态时调用，将ddr从控制态转换为自刷新态
 */
int ddr_suspend(struct owl_ddr_config *ddr_hwinfo, int need_preheat)
{
	int i;

	ddr_init_param.init_mode = DDR_INIT_RESUME;
	ddr_init_param.need_preheat = (char)need_preheat;
	if(need_preheat) {
		ddr_enter_suspend(ddr_hwinfo);
		for (i = 0; i < 10; i++) {
			if (mem_init(ddr_hwinfo) == 0)
				break;
		}
		if(i == 10)
			return -1;
	}
	ddr_enter_suspend(ddr_hwinfo);
	return 0;
}


