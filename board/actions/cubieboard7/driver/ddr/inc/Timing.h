/*
=================================================================================     
*                        GL5203 DDR3 Initialize
* FileName: GL5203_DDR3_32bits_init_240MHz.S
* Author  : rain_zhou 
* Version : v1.0
* Chip    : GL5203
* Date    : 2013-3-15
* Description:  GL5203 DDR3 initialization on arm platform
* History :
*   v1.0    create this file    2013-3-15
* DFI use HDR mode. n_clk : interface clk= 1:2
=================================================================================  
*/



#ifndef		_TIMING
#define		_TIMING

/*=================================================================================
*       CPU clock
=================================================================================*/
#if ( COREPLL == 804 )
#define     Core_PLL    0x343
#endif

/*=================================================================================
*      DDR clock config
=================================================================================*/

#if ( DDRCLK_Value == 48 )
#define     DDRCLK     0x104  
#elif ( DDRCLK_Value == 60 )    
#define     DDRCLK    0x105
#elif ( DDRCLK_Value == 72 )       
#define     DDRCLK    0x106
#elif ( DDRCLK_Value == 84 )        
#define     DDRCLK    0x107
#elif ( DDRCLK_Value == 96 )        
#define     DDRCLK    0x108
#elif ( DDRCLK_Value == 108 )         
#define     DDRCLK    0x109
#elif ( DDRCLK_Value == 120 )         
#define     DDRCLK    0x10A
#elif ( DDRCLK_Value == 132 ) 
#define     DDRCLK    0x10B
#elif ( DDRCLK_Value == 144 ) 
#define     DDRCLK    0x10C
#elif ( DDRCLK_Value == 156 ) 
#define     DDRCLK    0x10D
#elif ( DDRCLK_Value == 168 ) 
#define     DDRCLK    0x10E
#elif ( DDRCLK_Value == 180 ) 
#define     DDRCLK    0x10F
#elif ( DDRCLK_Value == 192 ) 
#define     DDRCLK    0x110
#elif ( DDRCLK_Value == 204 ) 
#define     DDRCLK    0x111
#elif ( DDRCLK_Value == 216 ) 
#define     DDRCLK    0x112
#elif ( DDRCLK_Value == 228 ) 
#define     DDRCLK    0x113
#elif ( DDRCLK_Value == 240 ) 
#define     DDRCLK    0x114
#elif ( DDRCLK_Value == 252 ) 
#define     DDRCLK    0x115
#elif ( DDRCLK_Value == 264 ) 
#define     DDRCLK    0x116
#elif ( DDRCLK_Value == 276 ) 
#define     DDRCLK    0x117
#elif ( DDRCLK_Value == 288 ) 
#define     DDRCLK    0x118
#elif ( DDRCLK_Value == 300 ) 
#define     DDRCLK    0x119
#elif ( DDRCLK_Value == 312 ) 
#define     DDRCLK    0x11A
#elif ( DDRCLK_Value == 324 ) 
#define     DDRCLK    0x11B
#elif ( DDRCLK_Value == 336 ) 
#define     DDRCLK    0x11C
#elif ( DDRCLK_Value == 348 ) 
#define     DDRCLK    0x11d
#elif ( DDRCLK_Value == 360 ) 
#define     DDRCLK    0x11e
#elif ( DDRCLK_Value == 372 ) 
#define     DDRCLK    0x11f
#elif ( DDRCLK_Value == 384 ) 
#define     DDRCLK    0x120
#elif ( DDRCLK_Value == 396 ) 
#define     DDRCLK    0x121
#elif ( DDRCLK_Value == 408 ) 
#define     DDRCLK    0x122
#elif ( DDRCLK_Value == 420 ) 
#define     DDRCLK    0x123
#elif ( DDRCLK_Value == 432 ) 
#define     DDRCLK    0x124
#elif ( DDRCLK_Value == 444 ) 
#define     DDRCLK    0x125
#elif ( DDRCLK_Value == 456 ) 
#define     DDRCLK    0x126
#elif ( DDRCLK_Value == 468 ) 
#define     DDRCLK    0x127
#elif ( DDRCLK_Value == 480 ) 
#define     DDRCLK    0x128
#elif ( DDRCLK_Value == 492 ) 
#define     DDRCLK    0x129
#elif ( DDRCLK_Value == 504 ) 
#define     DDRCLK    0x12A
#elif ( DDRCLK_Value == 516 ) 
#define     DDRCLK    0x12b
#elif ( DDRCLK_Value == 528 ) 
#define     DDRCLK    0x12c
#elif ( DDRCLK_Value == 540 ) 
#define     DDRCLK    0x12d
#elif ( DDRCLK_Value == 552 ) 
#define     DDRCLK    0x12e
#elif ( DDRCLK_Value == 564 ) 
#define     DDRCLK    0x12f
#elif ( DDRCLK_Value == 576 ) 
#define     DDRCLK    0x130
#elif ( DDRCLK_Value == 588 ) 
#define     DDRCLK    0x131
#elif ( DDRCLK_Value == 600 ) 
#define     DDRCLK    0x132
#elif ( DDRCLK_Value == 612 ) 
#define     DDRCLK    0x133
#elif ( DDRCLK_Value == 624 ) 
#define     DDRCLK    0x134
#elif ( DDRCLK_Value == 636 ) 
#define     DDRCLK    0x135
#elif ( DDRCLK_Value == 648 ) 
#define     DDRCLK    0x136
#elif ( DDRCLK_Value == 660 ) 
#define     DDRCLK    0x137
#elif ( DDRCLK_Value == 672 ) 
#define     DDRCLK    0x138
#elif ( DDRCLK_Value == 684 ) 
#define     DDRCLK    0x139
#elif ( DDRCLK_Value == 696 ) 
#define     DDRCLK    0x13a
#elif ( DDRCLK_Value == 708 ) 
#define     DDRCLK    0x13b
#elif ( DDRCLK_Value == 720 ) 
#define     DDRCLK    0x13c
#elif ( DDRCLK_Value == 732 ) 
#define     DDRCLK    0x13d
#elif ( DDRCLK_Value == 744 ) 
#define     DDRCLK    0x13e
#elif ( DDRCLK_Value == 756 ) 
#define     DDRCLK    0x13f
#elif ( DDRCLK_Value == 768 ) 
#define     DDRCLK    0x140
#elif ( DDRCLK_Value == 780 ) 
#define     DDRCLK    0x141
#endif

/*=================================================================================
*                  MCU ODT and output impedance config
=================================================================================*/
#if     (Mcu_ODT == 30)
#define     ZPODT       0x8
#elif   (Mcu_ODT == 40)
#define     ZPODT       0x6
#elif   (Mcu_ODT == 48)
#define     ZPODT       0x5
#elif   (Mcu_ODT == 60)
#define     ZPODT       0x4
#elif   (Mcu_ODT == 80)
#define     ZPODT       0x3
#elif   (Mcu_ODT == 120)
#define     ZPODT       0x2
#endif

#define     ZPZQ        0xC //21Ω output impedance  could be changed
#define     RP_ZQnCR1      ZPZQ|(ZPODT<<4)

#define     ZDEN       1
#define     RP_ZQnCR0   0x14A|(ZDEN<<28)   

/*=================================================================================
*                  UPCTL_TEFI 
=================================================================================*/
#if ( DDR_Device_Cap_Single == 128 )
#define		tRFC	    1     //单位是100ns
#define	    tRFC_C    ((110*DDRCLK_Value)/1000+1)
#elif ( DDR_Device_Cap_Single == 256 )
#define		tRFC	    2
#define	    tRFC_C    ((160*DDRCLK_Value)/1000+1)
#elif ( DDR_Device_Cap_Single == 512 )
#define		tRFC	    3
#define	    tRFC_C    ((260*DDRCLK_Value)/1000+1)
#elif ( DDR_Device_Cap_Single == 1024 )
#define		tRFC	   4
#define	    tRFC_C    ((350*DDRCLK_Value)/1000+1)
#endif

    

#define     vRFN    1   //同时可发refresh数目，最大不能大于8
#define     R_PTEFI     ((vRFN-1)<<16)|tRFC
/*=================================================================================
*                  UPCTL_MCFG
=================================================================================*/
#define     DDR3_P      1
#define     DDR2_P      0

/*=================================================================================
*                  ODT
=================================================================================*/
#define     Rank    0x2     //仅读的时候使能       
#if (CS_NUM==1)
#define R_DFICFG    Rank
#elif (CS_NUM==2)
#define R_DFICFG    Rank<<8|Rank
#elif (CS_NUM==3)
#define R_DFICFG    Rank<<16|Rank<<8|Rank
#elif (CS_NUM==4)
#define R_DFICFG    Rank<<24|Rank<<16|Rank<<8|Rank
#endif

#define     tODT_burst8     6
#define     tODT_latr       CL-CWL
#define     R_DFICFIG1        (tODT_burst8<<24)|(tODT_latr<<8)
  
#define     CKEN    0x7
#define     RP_PGCR     (0x01802000|(CKEN<<9)|(1<<18))  //|(DQS_GAT<<1) 

/*=================================================================================
*                  MR0 DLL on/off ;CL;read bust type;burst length;tWR
=================================================================================*/
//#define CL     ((15*DDRCLK_Value/1000)+1)       //是否考虑手动输入大于700M error
#if (DDRCLK_Value<404)
#define CL      6
#elif (DDRCLK_Value<540)
#define CL      7
#elif (DDRCLK_Value<670)
#define CL      9
#elif (DDRCLK_Value<804) 
#define CL      11
#else
#define CL      13   //800M
#endif
//#define CL    CL_num                      //cas read latency
#define BLEN      0 //0 ---fix8
#define BT      0 //0 sequential 1 interleave
#define TM      0 //0 operating 1 test mode
#define DR      1 //reset DLL

#if (tWR<9)  
#define WR      (tWR-4)
#elif(tWR<11) 
#define WR      5   //10
#else
#define WR      6 //12
#endif

#define PD      1 // power-down slow exit
#define RP_MR0  BLEN|((CL-4)<<4)|(BT<<3)|(TM<<7)|(DR<<8)|(WR<<9)|(PD<<12)

/*=================================================================================
*                  MR1  Write leveling;DLL enable; Rout;AL;ODT;TQS;Q off
=================================================================================*/
#define AL      0
#if(DDRCLK_Value<200)
#define DE      0
#else
#define DE      0 //DLL enable
#endif

#define DIC     1 //RZQ/7
#define RTT     1 //disable rtt when non-write
#define AL      0
#define QOFF    0

#define RP_MR1      DE|(DIC<<1)|(RTT<<2)|(AL<<3)|(QOFF<<12)


/*=================================================================================
*                  MR2  CWL;auto selfresh;selfresh temp;dynamic ODT;
=================================================================================*/

#if ( DDRCLK_Value < 396 )
#define     CWL		5
#elif ( DDRCLK_Value < 524 )
#define     CWL		6
#elif ( DDRCLK_Value < 660 )
#define     CWL		7
#elif ( DDRCLK_Value < 804 )
#define     CWL		8
#else
#define 		CWL   8//800M
#endif

#define  PASR       0 //full array
#define  ASR        1 //auto self-refresh
#define  SRT        0 // self-refresh
#define  RTTWR      1 //60ohm

#define     RP_MR2      PASR|((CWL-5)<<3)|(SRT<<6)|(SRT<<7)|(RTTWR<<9)

/*=================================================================================
*                  MR3(MPR)
=================================================================================*/
#define MPRLOC      0//pattern type
#define MPR         0//disable

#define RP_MR3        MPRLOC|(MPR<<2)  

/*=================================================================================
*                 Memory   Timing
=================================================================================*/
#if (DDRCLK_Value < 400)
#define     tRP     6
#elif (DDRCLK_Value < 900)
#define     tRP     (15*DDRCLK_Value/1000)
#endif

#define     tCCD     5
#define     tRTW    (CL+tCCD-CWL+3)         // read to write timing

#if (DDRCLK_Value < 400)
#define     tRAS     15
#elif (DDRCLK_Value < 900)
#define     tRAS    (40*DDRCLK_Value/1000) //37.5
#endif

#define     tRC     tRP+tRAS         	//52.5ns
#if (DDRCLK_Value < 300 )
#define     tRCD    5
#elif (DDRCLK_Value < 900 )
#define     tRCD    ((15*DDRCLK_Value/1000)+1)
#endif
#define     tRRD        6

#if (DDRCLK_Value < 500)
#define     tRTP        5
#elif(DDRCLK_Value < 804)  
#define     tRTP    ((8*DDRCLK_Value/1000)+1)
#else
#define     tRTP        8
#endif

#if (DDRCLK_Value < 400)
#define     tWR         6
#else
#define     tWR         ((15*DDRCLK_Value/1000)+1)
#endif

#if (DDRCLK_Value < 500)
#define     tWTR        5
#elif(DDRCLK_Value < 804)//800M
#define     tWTR    (   (8*DDRCLK_Value/1000)+1)
#else
#define     tWTR        8
#endif

#define     tXS         tRFC_C+(10*DDRCLK_Value/1000)
#define     tXSDLL      512
#define     tXP         5
#define     tXPLL       15
#define     tZQCS       64
#define     tZQCSI      ((550*10)/DDRCLK_Value)/tRFC 
//#define     tZQCSI      0
#define     tDQS        1  //不同rank的command反应时间

#if (DDRCLK_Value < 500)
#define     tCKSRE      5
#else
#define     tCKSRE      ((15*DDRCLK_Value/1000)+1)
#endif  

#if (DDRCLK_Value < 500)
#define     tCKSRX      5
#else
#define     tCKSRX      ((15*DDRCLK_Value/1000)+1)
#endif  

#define     tMRR        2//LPDDR2 mode register read
#define     tCKE        5
#define     tMOD        15      //12clk
#define     tDPD        0
//#define     tRSTL       (150*DDRCLK_Value/1000)  //100ns
#define     tZQSCL      530
#define     tCKESR      6

#define     tRSTL       (150*DDRCLK_Value/1000)     //min 100ns
#define     tZQCL       550     //init 512;oper 256
#define     tXPR        tRFC_C+(10*DDRCLK_Value/1000)

#define     tMRD            3
#define     tCCD_V          1
#define     tDTPR0          tMRD|(6<<2)|(6<<5)|(tRP<<8)|(9<<12)|(tRAS<<16)|(tRRD<<21)|(tRC<<25)|(tCCD_V<<31)

#define     tRTW_V          0 //standard bus
#if(DDRCLK_Value<600)
#define     tFAW            (50*DDRCLK_Value/1000)
#else
#define     tFAW            31
#endif
#define     tRTODT          0 //ODT turn on immediately
#define     tDTPR1          (tRTW_V<<2)|(tFAW<<3)|(tMOD<<9)|(tRTODT<<11)|(tRFC<<16)

#if(tXS>tXSDLL)
#define     tX              tXS 
#else
#define     tX              tXSDLL
#endif
#if(DDRCLK_Value<400)
#define     tXPDLL          10
#else
#define     tXPDLL          (30*DDRCLK_Value/1000)
#endif
#define     tDLLK           512
#define     tDTPR2          tX|(tXPDLL<<10)|(tCKE<<15)|(tDLLK<<19)   


#define     tEXSR           512                             

/*=================================================================================
*                 Memory   config
=================================================================================*/
#define   ZCKSEL        2


/*=================================================================================
*                 PHY   Timing
=================================================================================*/
#define     tDLLSRST     (80*DDRCLK_Value/1000)  //min 8 cycle
#define     tDLLLOCK     (5120*DDRCLK_Value/1000)  //?
#define     tITMSRST     10     //default 8
#define     tPTR0        tDLLSRST|(tDLLLOCK<<6)|(tITMSRST<<18)

#define     tDINIT0         (510*DDRCLK_Value)
#define     tDINIT1         tXPR
#define     tPTR1           tDINIT0|(tDINIT1<<19)

#define     tDINIT2         (200*DDRCLK_Value)
#define     tDINIT3         DDRCLK_Value  //uesed by LPDDR2
//#define     tDINIT3         534 //uesed by LPDDR2 default value
#define     tPTR2           tDINIT2|(tDINIT3<<17)

/*=================================================================================
*                 DLL   config
=================================================================================*/



/*****************DMM config***************/
#if(DDR_Device_Cap_Single==128)
#define     row_add         1
#elif(DDR_Device_Cap_Single==256)
#define     row_add         2
#else
#define    row_add          3
#endif

/*=================================================================================
*                 DFI  config
=================================================================================*/
#define     R_DFITPHYWRLAT      (CWL-1)/2
#define     R_DFITPRDDATAEN     (CL-2)/2
    

#define     R_DMMCONFIG          (Bus_width|(2<<1)|(1<<3)|(CS_NUM<<4)|(row_add<<5)|(((DDR_Device_Cap_Single/256)-1)<<8))

/*=================================================================================
*                 DFI  config
=================================================================================*/
#define     tRDR            ((tRTP+tRP+tRCD-4+3)/4)
#define     tWDR            ((CL+tWR+tRP+tRCD+3)/4)
#define     tRC_DMM         ((tRC+3)/4)

#define     R_DDR_TIM       ((tRDR<<8)|(tWDR<<4)|tRC_DMM)


#endif
