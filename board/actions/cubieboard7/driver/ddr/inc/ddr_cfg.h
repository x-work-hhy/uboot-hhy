
#ifndef _SDRAM_TIMING_H_
#define _SDRAM_TIMING_H_


//#define CONFIG_GS705x
//#define CONFIG_S_V500
#define CONFIG_S_V700
struct owl_pmu_config {
	uint16_t bus_id;
	uint16_t bus_mfp;

	uint16_t pmu_id;
	uint16_t vddr_pmu_dcdc_cfg;
	uint16_t io_voltage;
	uint16_t reserved16;
	uint32_t reserved32;
} __attribute__ ((packed));

struct owl_ddr_config {
	uint16_t 	ddr_clk;
	uint16_t 	ddr_cap;	//ddr_initê±éè???a0
	uint8_t		ddr_bits_width;
	uint8_t 	rank;
	uint8_t		ddr_type;
	uint8_t 	row_col;	//ddr_initê±éè???a0
	uint8_t 	rdodt;
	uint8_t 	wrodt;
	uint8_t 	zpzq;
	uint8_t 	row_col_detect;
} __attribute__ ((packed)) ;

//...
//----------------------------CONFIG_GS705x------------------------------
#if defined (CONFIG_GS705x)
	enum ddrinit_mode
	{
		MP             = 0,    //Mass Production
		NS             = 1,    //Normal Startup
		LTSES2         = 2,    //Low Temperature Start enter s2
		LTSXS2         = 3,    //Low Temperature Start exit s2
	};
	#define DDRCLKDIVSTEP 12
	#define DDRCLK_EN     0X8
	#define DMACCLK_EN    0X2
	#define DDRRST_EN     0X4
	#define DMACRST_EN    0X1
#endif
//---------------------------CONFIG_S_V500-------------------------------
#if defined (CONFIG_S_V500)
	enum init_mode 
	{
		NORMAL_BOOT    = 0,
		UPGRADE        = 1,
		LEAVE_LP       = 2, /* leave low power*/
		INIT_STATE_NUM,
	};
	enum ddr_init_mode
	{
		DDR_INIT_FULL,		//íê??3?ê??ˉ
		DDR_INIT_RESUME,	//í?3?S2×′ì?ê±μ?3?ê??ˉ
	};
	#define DDRCLKDIVSTEP 12
	#define DDRCLK_EN     0X8
	#define DDRRST_EN     0X4
#endif
//---------------------------CONFIG_S_V700--------------------------------
#if defined (CONFIG_S_V700)
	enum init_mode 
	{
		NORMAL_BOOT    = 0,
		UPGRADE        = 1,
		LEAVE_LP       = 2, /* leave low power*/
		INIT_STATE_NUM,
	};
	enum ddr_init_mode
	{
		DDR_INIT_FULL  = 0,		//完整初始化
		DDR_INIT_RESUME= 1,	//退出S2状态时的初始化
	};
	#define DDRCLKDIVSTEP 6
	#define DDRCLK_EN     0X100000
	#define DDRRST_EN     0X100000
#endif

enum dcu_mode {
	INIT           = 0,
	CONFIG         = 1,
	ACCESS         = 2,
	LP             = 3,
	WAKEUP         = 4,
};
enum sdram_type
{
	tpy_ddr3       = 0,
	tpy_lpddr2     = 1,
	tpy_lpddr3     = 2,
	tpy_ddr3_rdc   = 0x10,
};
enum dqbuswidth
{
	x16width       = 1,
	x32width       = 0,
};
enum s_hdrmode
{
	sdrmode       = 0,
	hdrmode       = 1,
};
enum dllstute
{
	dllen         = 0,
	dlloff        = 1,
};
static struct  dcu_parameter{
//-------------------timing---------------
	unsigned int tREFI;
	unsigned int tMRD;
	unsigned int tRFC;
	unsigned int tRP;	
	unsigned int tRTW;
	unsigned int tAL;
	unsigned int tCL;
	unsigned int tWL;
	unsigned int tRAS;
	unsigned int tRC;
	unsigned int tRCD;
	unsigned int tRRD;
	unsigned int tRTP;
	unsigned int tWR;
	unsigned int tWTR;
	unsigned int tEXSR;	
	unsigned int tXP;
	unsigned int tXPDLL;
	unsigned int tZQCS;
	unsigned int tZQCSI;
	unsigned int tDQS;
	unsigned int tCKSRE;
	unsigned int tCKSRX;
	unsigned int tCKE;
	unsigned int tMOD;
	unsigned int tRSTL;	
	unsigned int tZQCL;	
	unsigned int tFAW;	
	unsigned int tMRR;
	unsigned int tCKESR;
	unsigned int tDPD;
	unsigned int tREFI_MEM;
//-------------------upctl---------------
	unsigned int pctl_TINIT;
	unsigned int pctl_TRSTH;
	unsigned int pctl_MCFG;
	unsigned int pctl_dfiwrlat;
	unsigned int pctl_dfirden;   
//-------------------publ---------------
	unsigned int publ_DCR;
	unsigned int publ_PTR0;
	unsigned int publ_PTR1;
	unsigned int publ_PTR2;
	unsigned int publ_MR0;
	unsigned int publ_MR1;
	unsigned int publ_MR2;
	unsigned int publ_MR3;	    
	unsigned int publ_DTPR0;
	unsigned int publ_DTPR1;  
	unsigned int publ_DTPR2;  
};

#define BANK_NUMI  8  
#define BSTLen     8 
#define DRIVSTREN  3            // Drive strength,1:34ohm  2:40ohm  3:48ohm  4:60ohm  6:80ohm   7:120ohm
#define RFNUM      8   // refresh num: 1 ~ 8times per
#define DLLEN      0   // DLL Enable/Disable: Enable (0) or disable (1) the DLL. DLL must be enabled for normal operation.
#define ODIC       0x2  //Output Driver Impedance Control: Controls the output drive strength.
                        //0x0: RZQ/6
                        //0x2: RZQ/7
                        //other: reserved
//#define ODTRTT     0x4  // On Die Termination: Selects the effective resistance for SDRAM on die termination. 
                       //0x  0: ODT disabled
                       //0x  4: RZQ/4
                       //0x 40: RZQ/2
                       //0x 44: RZQ/6
                       //0x200: RZQ/12
                       //0x204: RZQ/8
                       //other: reserved
#define PCAL      0x0  //Posted CAS Additive Latency: Setting additive latency that allows read and write commands to 
                       //be issued to the SDRAM earlier than normal (refer to SDRAM datasheet for details).
                       //0x0: = 0 (AL disabled)
                       //0x1:CL - 1
                       //0x2:CL - 2
                       //other: reserved
#define WLEVEL     0    //Write Leveling Enable: Enables write-leveling when set "1".

#define ASREN      1    //Auto Self-Refresh: When enabled (‘1’), SDRAM automatically provides self-refresh 
                        //power management functions for all supported operating temperature values. 
                        //Otherwise the SRT bit must be programmed to indicate the temperature range.
#define RTTWR      1    //Dynamic ODT: Selects RTT for dynamic ODT. Valid values are:
												//01 = RZQ/4
												//02 = RZQ/2
												//other: reserved
#define DRV_ST     3    //1:34.3Ohm
												//2:40ohm
												//3:48ohm
												//4:60ohm
												//6:80ohm
												//7:120ohm
#define DQSUDR     1    //000 = Open: On-die resistor disconnected
                        //001 = 688 ohms
                        //010 = 611 ohms
                        //011 = 550 ohms
                        //100 = 500 ohms
                        //101 = 458 ohms
                        //110 = 393 ohms
                        //111 = 344 ohms
#define TRAINADDR       0x20000000
#define R0ADDROFFS      0x4
#define R0ADDRDATA      0x6996
#define S_HDRSEL        1 //0:SDR mode;1:HDR mode



#endif
