/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#if defined(BUILD_LK)
#include <string.h>
#else
#include <linux/string.h>
#endif


#if defined(BUILD_LK)
#include "cust_gpio_usage.h"
#include <platform/mt_gpio.h>
#else
#include "cust_gpio_usage.h"
#include <mach/mt_gpio.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)

#define REGFLAG_DELAY             							0xAB
#define REGFLAG_END_OF_TABLE      							0xAA   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define GPIO_LCM_ID1	GPIO18
#define GPIO_LCM_ID2	GPIO19
#define LCM_TDT			0
#define LCM_BYD			1
bool lcm_vendor=LCM_BYD;	//default to choose byd panel


//set LCM IC ID
#define LCM_ID_ILI9806C 									(0x9816)

//#define LCM_DEBUG


/*--------------------------LCD module explaination begin---------------------------------------*/

//LCD module explaination				//Project		Custom		W&H		Glass	degree	data		HWversion

//LCD_BYD_450_ILI9806C_HSD			//YarisL					480*854		HSD		0				Proto 01
//LCD_TDT_450_ILI9806C_HSD			//YarisL					480*854		HSD		0				Proto 01
/*--------------------------LCD module explaination end----------------------------------------*/

//#define LCD_BYD_450_ILI9806C_HSD_ORIGIN		//the original parm
#define LCD_BYD_450_ILI9806C_HSD_0913		//to resolve the flick issue of 2 dot inversion


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if defined(LCD_BYD_450_ILI9806C_HSD_ORIGIN)
static struct LCM_setting_table lcm_byd_initialization_setting[] = {

	{0xFF,	3,	{0xFF, 0x98, 0x16}},

	{0xBA,	1,	{0x60}},

	{0xB0,	1,	{0x01}},

	{0xBC,	18,	{0x03,0x0D,0x03,0x63,0x01,0x01,0x1b,0x11,0x6E,0x00,0x00,0x00,0x01,0x01,0x16,0x00,0xff,0xf2}},

	{0xBD,	8,	{0x02,0x13,0x45,0x67,0x45,0x67,0x01,0x23}},

	{0xBE,	17,	{0x03,0x22,0x22,0x22,0x22,0xdd,0xcc,0xbb,0xaa,0x66,0x77,0x22,0x22,0x22,0x22,0x22,0x22}},

	{0xED,	2,	{0x7F, 0x0F}},

	{0xF3,	1,	{0x70}},

	{0xB4,	1,	{0x02}},

	{0xC0,	3,	{0x0F, 0x0B, 0x0A}},

	{0xC1,	4,	{0x17,0x88,0x70,0x20}},

	{0xD8,	1,	{0x50}},

	{0xFC,	1,	{0x07}},     
	{0xE0,	16,	{0x00,0x04,0x12,0x11,0x13,0x1E,0xc8,0x08,0x02,0x09,0x03,0x0C,0x0C,0x2D,0x2A,0x00}},

	{0xE1,	16,	{0x00,0x02,0x07,0x0D,0x11,0x16,0x7A,0x09,0x05,0x09,0x06,0x0C,0x0B,0x2F,0x2A,0x00}},

	{0xD5,	8,	{0x0D,0x08,0x08,0x09,0xCB,0xA5,0x01,0x04}},   

	{0xF7,	1,	{0x89}},    

	{0xC7,	1,	{0x7F}},

	{0x11,	1,	{0x00}},
	{REGFLAG_DELAY, 120, {}},

	{0xEE, 9,{0x0A,0x1B,0x5F,0x40,0x00,0x00,0x10,0x00,0x58}},//for esd protect, no need to check sync
	{0xD6, 8,{0xFF,0xA0,0x88,0x14,0x04,0x64,0x28,0x1A}},

	{0x29,	1,	{0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}  

};

#elif defined(LCD_BYD_450_ILI9806C_HSD_0913)
static struct LCM_setting_table lcm_byd_initialization_setting[] = {

	{0xFF,	3,	{0xFF, 0x98, 0x16}},

	{0xBA,	1,	{0x60}},

	{0xB0,	1,	{0x01}},

	{0xBC,	18,	{0x03,0x0D,0x03,0x63,0x01,0x01,0x1b,0x11,0x6E,0x00,0x00,0x00,0x01,0x01,0x16,0x00,0xff,0xf2}},

	{0xBD,	8,	{0x02,0x13,0x45,0x67,0x45,0x67,0x01,0x23}},

	{0xBE,	17,	{0x03,0x22,0x22,0x22,0x22,0xdd,0xcc,0xbb,0xaa,0x66,0x77,0x22,0x22,0x22,0x22,0x22,0x22}},

	{0xED,	2,	{0x7F, 0x0F}},

	{0xF3,	1,	{0x70}},

	{0xB4,	1,	{0x02}},

	{0xC0,	3,	{0x0F, 0x0B, 0x0A}},

	{0xC1,	4,	{0x17,0x88,0x70,0x20}},

	{0xD8,	1,	{0x50}},

	{0xFC,	1,	{0x07}},     
	{0xE0,	16,	{0x00,0x04,0x12,0x11,0x13,0x1E,0xc8,0x08,0x02,0x09,0x03,0x0C,0x0C,0x2D,0x2A,0x00}},

	{0xE1,	16,	{0x00,0x02,0x07,0x0D,0x11,0x16,0x7A,0x09,0x05,0x09,0x06,0x0C,0x0B,0x2F,0x2A,0x00}},

	{0xD5,	8,	{0x0D,0x08,0x08,0x09,0xCB,0xA5,0x01,0x04}},   

	{0xF7,	1,	{0x89}},    

	{0xC7,	1,	{0x78}},

	{0x11,	1,	{0x00}},
	{REGFLAG_DELAY, 120, {}},

	{0xEE, 9,{0x0A,0x1B,0x5F,0x40,0x00,0x00,0x10,0x00,0x58}},//for esd protect, no need to check sync
	{0xD6, 8,{0xFF,0xA0,0x88,0x14,0x04,0x64,0x28,0x1A}},

	{0x29,	1,	{0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}  

};

#endif

static struct LCM_setting_table lcm_byd_initialization_debug1[] = {

	{0xFF,	3,	{0xFF, 0x98, 0x16}},

	{0xBA,	1,	{0x60}},

	{0xB0,	1,	{0x01}},

	{0xBC,	18,	{0x03,0x0D,0x03,0x63,0x01,0x01,0x1b,0x11,0x6E,0x00,0x00,0x00,0x01,0x01,0x16,0x00,0xff,0xf2}},

	{0xBD,	8,	{0x02,0x13,0x45,0x67,0x45,0x67,0x01,0x23}},

	{0xBE,	17,	{0x03,0x22,0x22,0x22,0x22,0xdd,0xcc,0xbb,0xaa,0x66,0x77,0x22,0x22,0x22,0x22,0x22,0x22}},

	{0xED,	2,	{0x7F, 0x0F}},

	{0xF3,	1,	{0x70}},

	{0xB4,	1,	{0x02}},

	{0xC0,	3,	{0x0F, 0x0B, 0x0A}},

	{0xC1,	4,	{0x17,0x88,0x70,0x20}},

	{0xD8,	1,	{0x50}},

	{0xFC,	1,	{0x07}},     
	{0xE0,	16,	{0x00,0x04,0x12,0x11,0x13,0x1E,0xc8,0x08,0x02,0x09,0x03,0x0C,0x0C,0x2D,0x2A,0x00}},

	{0xE1,	16,	{0x00,0x02,0x07,0x0D,0x11,0x16,0x7A,0x09,0x05,0x09,0x06,0x0C,0x0B,0x2F,0x2A,0x00}},

	{0xD5,	8,	{0x0D,0x08,0x08,0x09,0xCB,0xA5,0x01,0x04}},   

	{0xF7,	1,	{0x89}},    

	{REGFLAG_END_OF_TABLE, 0x00, {}}  

};

static struct LCM_setting_table lcm_byd_initialization_debug2[] = {

	{0x11,	1,	{0x00}},
	{REGFLAG_DELAY, 120, {}},

	{0xEE, 9,{0x0A,0x1B,0x5F,0x40,0x00,0x00,0x10,0x00,0x58}},//for esd protect, no need to check sync
	{0xD6, 8,{0xFF,0xA0,0x88,0x14,0x04,0x64,0x28,0x1A}},

	{0x29,	1,	{0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}  

};


static struct LCM_setting_table lcm_tdt_initialization_setting[] = {

	{0xFF,	3,	{0xFF, 0x98, 0x16}},// EXTC Command Set enable register


	{0xBA,	1,	{0x60}},// SPI Interface Setting

	{0xB0,	1,	{0x01}},// Interface Mode Control

	{0xBC,	18,	{0x03, 0x0D, 0x61, 0x69, 0x16, 0x16, 0x1B, 0x11, 0x70, 0x00, 0x00,0x00, 0x16, 0x16, 0x09, 0x00, 0xFF, 0xF0}},// GIP 1


	{0xBD,	8,	{0x01, 0x45, 0x45, 0x67, 0x01, 0x23, 0x45, 0x67}},// GIP 2

	{0xBE,	17,	{0x13, 0x22, 0x11, 0x00, 0x66, 0x77, 0x22, 0x22, 0xBA, 0xDC, 0xCB,0xAD, 0x22, 0x22, 0x22, 0x22, 0x22}},// GIP 3


	{0xED,	2,	{0x7F, 0x0F}},// en_volt_reg measure VGMP

	{0xF3,	1,	{0x70}},


	{0xB4,	1,	{0x00}},// Display Inversion Control

	{0xC0,	3,	{0x0F, 0x0B, 0x0A}},// Power Control 1

	{0xC1,	4,	{0x17, 0x86, 0x7E, 0x20}},// Power Control 2


	{0xD8,	1,	{0x50}},// VGLO Selection

	{0xFC,	1,	{0x07}},// VGLO Selection

	{0xE0,	16,	{0x00, 0x0F, 0x18, 0x0A, 0x0F, 0x14, 0xCD, 0x0D, 0x07, 0x09, 0x07,0x0B, 0x0E, 0x2D, 0x2C, 0x00}},// Positive Gamma Control

	{0xE1,	16,	{0x00, 0x13, 0x20, 0x13, 0x13, 0x17, 0x74, 0x03, 0x01, 0x08, 0x07,0x0E, 0x0C, 0x23, 0x1D, 0x00}},// Negative Gamma Control

	{0xD5,	8,	{0x0F, 0x0A, 0x07, 0x0A, 0xCB, 0xA5, 0x01, 0x04}},// Source Timing Adjust

	{0xF7,	1,	{0x89}},// Resolution

	{0xC7,	1,	{0x27}},// Vcom

	{0x36,	1,	{0x00}},// 00=>FW; 03=>BW

	{0x11,	1,	{0x00}},// Exit Sleep
	{REGFLAG_DELAY, 120, {}},

	{0xEE, 9,{0x0A,0x1B,0x5F,0x40,0x00,0x00,0x10,0x00,0x58}},//for esd protect, no need to check sync
	{0xD6, 8,{0xFF,0xA0,0x88,0x14,0x04,0x64,0x28,0x1A}},

	{0x29,	1,	{0x00}},// Display On

	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};


#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif


static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 150, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	//{0x2C, 1, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out	
	{0x11, 1, {0x00}},
	{0x29, 1, {0x00}},

	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 150, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	{0x2C, 1, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
    {REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
		dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		params->dsi.packet_size=256;

		// Video mode setting	

		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.intermediat_buffer_num = 2;	
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.word_count=480*3;

		//here is for esd protect by legen
		params->dsi.noncont_clock = true;
		params->dsi.noncont_clock_period=2;
		params->dsi.lcm_ext_te_enable=true;
		//for esd protest end by legen

		//delete by zhuqiang 2013.3.4 
		//	params->dsi.word_count=FRAME_WIDTH*3;	
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.vertical_sync_active=4;  
		params->dsi.vertical_backporch=16;
		params->dsi.vertical_frontporch=20;
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.vertical_active_line=FRAME_HEIGHT;

		//delete by zhuqiang 2013.3.4 
		//	params->dsi.line_byte=2180;		
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.horizontal_sync_active=10;  
		
//zrl modify for improve the TP reort point begin,130916
		params->dsi.horizontal_backporch=50;      
		params->dsi.horizontal_frontporch=50;  
//zrl modify for improve the TP reort point end,130916
		  
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;	//added by zhuqiang 2013.3.4 
		
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.pll_div1=0;         //  div1=0,1,2,3;  div1_real=1,2,4,4
		params->dsi.pll_div2=2;         // div2=0,1,2,3;div2_real=1,2,4,4
		
//zrl modify for improve the TP reort point begin,130916
        params->dsi.fbk_div =32;              // fref=26MHz,  fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
//zrl modify for improve the TP reort point end,130916

		// add by zhuqiang for FR437058 at 2013.4.25 end
}

//legen add for detect lcm vendor
static bool lcm_select_panel(void)
{
	int value=0;
	
	mt_set_gpio_mode(GPIO_LCM_ID1,GPIO_MODE_00);
	mt_set_gpio_mode(GPIO_LCM_ID2,GPIO_MODE_00);

	mt_set_gpio_pull_enable(GPIO_LCM_ID1, GPIO_PULL_DISABLE);
	mt_set_gpio_pull_enable(GPIO_LCM_ID2, GPIO_PULL_DISABLE);

	mt_set_gpio_dir(GPIO_LCM_ID1, GPIO_DIR_IN);
	mt_set_gpio_dir(GPIO_LCM_ID2, GPIO_DIR_IN);

	value+=mt_get_gpio_in(GPIO_LCM_ID1);
	value+=mt_get_gpio_in(GPIO_LCM_ID2);
	if(value)
		return LCM_TDT;
	
	return LCM_BYD;
}
//legen add end 

static int first_init=0;
static void lcm_init(void)
{
    unsigned int data_array[16];

#if defined(BUILD_LK)
  	lcm_vendor=lcm_select_panel();
#else
  	if(!first_init)
  	{
  		first_init=1;
		lcm_vendor=lcm_select_panel();
  	}
#endif

#ifdef BUILD_LK
	printf("[%s]lk,ili9806c,zrl choose lcm vendor:%d-%s\n",__func__,lcm_vendor,lcm_vendor?"BYD":"TDT");
#else
	printk("[%s]kernel,ili9806c,zrl choose lcm vendor:%d-%s\n",__func__,lcm_vendor,lcm_vendor?"BYD":"TDT");
#endif

    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);

    if(lcm_vendor == LCM_BYD)
		push_table(lcm_byd_initialization_setting, sizeof(lcm_byd_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    else
		push_table(lcm_tdt_initialization_setting, sizeof(lcm_tdt_initialization_setting) / sizeof(struct LCM_setting_table), 1);

    //push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


#if 0

static void lcm_resume(void)
{
	static int temp = 0x46;
	unsigned int data_array[16];

	//zrl add for debug for Cut  screen, 121015
	push_table(lcm_byd_initialization_debug1, sizeof(lcm_byd_initialization_debug1) / sizeof(struct LCM_setting_table), 1);

	data_array[0]= 0x00023902;
	data_array[1]= 0x00|0x00|(temp<<8)|0xC7;
	dsi_set_cmdq(&data_array, 2, 1);

	push_table(lcm_byd_initialization_debug2, sizeof(lcm_byd_initialization_debug2) / sizeof(struct LCM_setting_table), 1);

#if defined(BUILD_LK)
	printf("zrl lcm_resume,temp = 0x%x,data_array[1] = 0x%x\n",temp,data_array[1]);
#else
	printk("zrl lcm_resume,temp = 0x%x,data_array[1] = 0x%x\n",temp,data_array[1]);
#endif

	//x1=temp%256;
	//x2=temp/256;
	temp+=2;

}

#else

static void lcm_resume(void)
{

	//lcm_init();  
	//MDELAY(200);

	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#endif


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}


static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK

	unsigned char buffer[4];
	unsigned int array[16];

#if defined(LCM_DEBUG)
		printk("ili9806c: lcm_esd_check enter\n");
#endif

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x0A, buffer, 1);
	
#if defined(LCM_DEBUG)
	printk("lcm_esd_check  0x0A = %x\n",buffer[0]);
#endif

	if(buffer[0] != 0x9C)
	{
		return 1;
	}

	array[0] = 0x00043700;
	dsi_set_cmdq(array,1,1);
	read_reg_v2(0x09, buffer, 4);

#if defined(LCM_DEBUG)
	printk("lcm_esd_check  0x09(bit0~3) = %x, %x, %x, %x\n",buffer[0],buffer[1],buffer[2],buffer[3]);
#endif

	//if ((buffer[0]==0x80)&&(buffer[1]==0x73)&&(buffer[2]==0x04)&&(buffer[3]==0x00))
	if ((buffer[0]==0x80)&&(buffer[1]==0x73)&&(buffer[2]==0x04)&&((buffer[3]==0x00) || (buffer[3]==0x01)))
	{
	#if defined(LCM_DEBUG)
		printk("ili9806c: lcm_esd_check exit\n");
	#endif
		return 0;
	}
	else
	{
		return 1;
	}
	
#endif
}


static unsigned int lcm_esd_recover(void)
{
#ifndef BUILD_LK

   printk("ili9806c: lcm_esd_recover enter");
   lcm_init();
   return 1;

 #endif 
}


// ---------------------------------------------------------------------------
//  Get LCM ID Information
// ---------------------------------------------------------------------------
static unsigned int lcm_compare_id();
static unsigned int lcm_compare_id()
{
	 unsigned int id=0;
	 unsigned char buffer[2];
	 unsigned int array[16];  
	 
	 SET_RESET_PIN(1);	//NOTE:should reset LCM firstly
	 MDELAY(10);
	 SET_RESET_PIN(0);
	 MDELAY(10);
	 SET_RESET_PIN(1);
	 MDELAY(120);	
	 


	 array[0]=0x00043902;
	 array[1]=0x1698FFFF;
	 dsi_set_cmdq(array, 2, 1);
	 MDELAY(10); 

	 array[0] = 0x00033700;// set return byte number
	 dsi_set_cmdq(array, 1, 1);
	 //MDELAY(10); 
	 
	 read_reg_v2(0xD3, buffer, 3);

	 //zrl modify for ili9806 read ID,121113	   
	 id = buffer[1]<<8 |buffer[2];

#if defined(BUILD_LK)
	 printf("zrl_lk -- ili9806c 0x%x , 0x%x , 0x%x \n",buffer[0],buffer[1],id);
#else
	 printk("zrl_kernel -- ili9806c 0x%x , 0x%x , 0x%x \n",buffer[0],buffer[1],id);
#endif
	 
	 return (id == LCM_ID_ILI9806C)?1:0;
	 //zrl end,121113
} 
//zrl end,130225

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9806c_6572_dsi_vdo_lcm_drv =
{
    .name			= "ili9806c_6572_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
	
//	.set_backlight	= lcm_setbacklight,
//	.set_pwm        = lcm_setpwm,
//	.get_pwm        = lcm_getpwm,
   
	.esd_check   = lcm_esd_check,
   	.esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
};
