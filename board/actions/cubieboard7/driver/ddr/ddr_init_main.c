/*
 * (C) Copyright 2012
 * Actions Semi .Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>

#include "../../include/owl_common.h"
#include "../../include/owl_io.h"

#include "inc/ddr_reg.h"
#include "inc/ddr_cfg.h"

/* MFP Register Address */
#define ATC2603C_MFP_BASE		(0xD0)
#define ATC2603C_CHIP_VER		(ATC2603C_MFP_BASE + 0x0C)

#define ATC2603C_PMU_BASE               (0X00)
#define ATC2603C_PMU_DC2_CTL0		(ATC2603C_PMU_BASE + 0x14)

static int ddr_pmu_init(unsigned char ddr_type)
{

	unsigned int temp;
	int ic_ver;
#if 0
	struct owl_pmu_config pmu_config;
	
	owl_bootparam_get_config(OWL_BOOTPARAM_PMU, &pmu_config);

	/*only support 2603C VER_B, 为了兼容之前的配置，12-17 映射1.0-1.25*/
	/*0-11 --> 1.30-1.85, 12-17-->1.00-1.25. step=0.05*/
	temp = pmu_config.vddr_pmu_dcdc_cfg&0x7f;
	if (temp < 12)
		temp += 6;
	else
		temp -=12;
	temp = 0x80ca | (temp << 8);
#endif
	temp = 0x90ca;
	ic_ver = atc260x_reg_read(ATC2603C_CHIP_VER);
	if(ic_ver == 0) { //pmu ver A not support
		owl_printf("pmu ver=a\n");
	}
	
	atc260x_reg_write(ATC2603C_PMU_DC2_CTL0, temp);
	owl_printf("vdd_ddr: val=0x%x, ver=%d\n", temp, ic_ver);
	
	owl_udelay(1000);
	return 0;

}


void ddr_test(unsigned int  ddr_cap_mb)
{
	unsigned int *addr = (unsigned int *)0x0;
	unsigned int i, maxlen;
	maxlen = (ddr_cap_mb << 18);
	owl_printf("====ddr_test(4-addr) %d MB ===\n", ddr_cap_mb);

	for (i = 0x4; i < maxlen; i <<= 1) {
		addr[i] = i;
	}
	for (i = 0x4; i < maxlen; i <<= 1) {
		if (addr[i] != i){
			owl_printf("addr:ddr_test fail %i, 0x%x \n", i, addr[i]);
			return;
		}
	}

	for (i = 0x80000; i < maxlen; i += 0x100000) {
		addr[i] = i;
	}
	for (i = 0x80000; i < maxlen; i += 0x100000) {
		if (addr[i] != i){
			owl_printf("tmp:ddr_test fail %i, 0x%x \n", i, addr[i]);
			return;
		}
	}
	owl_printf("ddr_test ok==\n");	
}



struct owl_ddr_config ddr_param = {
	 .ddr_clk = 0x168,
	 .ddr_cap = 0,
	 .ddr_bits_width = 0,
	 .rank = 0x0,
	 .ddr_type = 0x0,
	 .row_col = 0x0,
	 .wrodt =  0x1,
	 .rdodt = 0x1,
	 .zpzq = 0xc,
	 .row_col_detect = 0,
};
void owl_debug_dump_mem(char *label, void *base, int len)
{
	int i, j;
	char *data = base; 
	owl_printf("%s: dump of %d bytes of data at 0x%llx\n",
			label, len, data);

	for (i = 0; i < len; i += 16) {
		owl_printf("%x: ", i);
		for (j = 0; j < 16; j++) {
			if ((i + j < len))
				owl_printf("%x ", data[i + j]);
			else
				owl_printf("   ");
		} 

		owl_printf("\n");
	}

}

int ddr_init(int mode)
{
	unsigned int temp_v,temp_w;
	struct owl_ddr_config param;


	ddr_pmu_init(ddr_param.ddr_type);
	if ( mode == 2 ) {
		if(ddr_resume(&param, 0) !=0) {
#if 1
			//config_alarm_wakeup_later();
			owl_printf("enter S2 again because of ddr fail!\n");
			//enter_s2(1);
#endif
		}
	} else {
		temp_v = __ddr_init(&param);
		while(temp_v != 0){
			owl_printf("ddr return = 0x%x\n", temp_v);
			owl_mdelay(100);
		}
		ddr_test(param.ddr_cap);
	//owl_bootparam_set_config(OWL_BOOTPARAM_DDR, (void *)&param);
		owl_debug_dump_mem("aa",  &param, 40);
	}

	return 0;
}

