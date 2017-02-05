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
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>

#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
//#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_DBG_FUNC printk

#define DEBUG_CAMERA_HW_K
#ifdef  DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         printk(KERN_ERR PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_XLOG_INFO(fmt, args...) \
        do {    \
                    xlog_printk(ANDROID_LOG_INFO, "kd_camera_hw", fmt, ##args); \
        } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif

//#ifdef NMIXXX_XTV_SUPPORT
extern bool g_bIsAtvStart;
//#endif

/*yi.zheng.hz modify for FR437080*/
/*
static void MainCameraDigtalPowerCtrl(kal_bool on){
	    if(mt_set_gpio_mode(GPIO_MAIN_CAMERA_12V_POWER_CTRL_PIN,0)){PK_DBG("[[CAMERA SENSOR] Set MAIN CAMERA_DIGITAL POWER_PIN ! \n");}
	    if(mt_set_gpio_dir(GPIO_MAIN_CAMERA_12V_POWER_CTRL_PIN,GPIO_DIR_OUT)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");}
	    if(mt_set_gpio_out(GPIO_MAIN_CAMERA_12V_POWER_CTRL_PIN,on)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");;}
}
*/


/*#ifndef GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN 
#define GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN GPIO37
#endif 
static void MainCameraAnalogPowerCtrl(kal_bool on){
    if(mt_set_gpio_mode(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,0)){PK_DBG("[[CAMERA SENSOR] Set MAIN CAMERA_DIGITAL POWER_PIN ! \n");}
    if(mt_set_gpio_dir(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,GPIO_DIR_OUT)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");}
    if(mt_set_gpio_out(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,on)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");;}
}*/



int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
    u32 pinSetIdx = 0;//default main sensor
    u32 pinSetIdxTmp = 0;

    #define IDX_PS_CMRST 0
    #define IDX_PS_CMPDN 4

    #define IDX_PS_MODE 1
    #define IDX_PS_ON   2
    #define IDX_PS_OFF  3
//zhao.li@tcl add for hi351 begin
	#define IDX_PS_351_PDN_ON 3
	#define IDX_PS_351_PDN_OFF 2
//zhao.li@tcl add for hi351 end
    u32 pinSet[2][8] = {
          //for main sensor
          {GPIO_CAMERA_CMRST_PIN,
              GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
              GPIO_OUT_ONE,                   /* ON state */
              GPIO_OUT_ZERO,                  /* OFF state */
           GPIO_CAMERA_CMPDN_PIN,
              GPIO_CAMERA_CMPDN_PIN_M_GPIO,
/*yi.zheng.hz modify for FR437080 start*/
              GPIO_OUT_ONE,
              GPIO_OUT_ZERO,
          },
          //for sub sensor
          {GPIO_CAMERA_CMRST1_PIN,
           GPIO_CAMERA_CMRST1_PIN_M_GPIO,
              GPIO_OUT_ONE,    //sensor normal work
              GPIO_OUT_ZERO,  //sensor reset
           GPIO_CAMERA_CMPDN1_PIN,
              GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
              GPIO_OUT_ZERO,  // sensor normal work
              GPIO_OUT_ONE,    // sensor power down
/*yi.zheng.hz modify for FR437080 end*/
          }
         };

    if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx){
        pinSetIdx = 0;
    }
    else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx) {
        pinSetIdx = 1;
    }


    //power ON
    if (On) {

       PK_DBG("kdCISModulePowerOn -on:currSensorName=%s;\n",currSensorName);
/*yi.zheng.hz modify for FR437080 start*/
	   //MainCameraDigtalPowerCtrl(1);

	if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5645_MIPI_YUV,currSensorName)))
	{
			PK_DBG("[ON_general 1.8V]sensorIdx:%d \n",SensorIdx);
			if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
			{
				if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
				if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
				if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
				if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
				if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
				if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			}				
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
				//return -EIO;
				goto _kdCISModulePowerOn_exit_;
			} 
			//mdelay(8);	//zrl delete for reduce the camera open time,130801
		   
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
				//return -EIO;
				goto _kdCISModulePowerOn_exit_;
			}
			//mdelay(8);	//zrl delete for reduce the camera open time,130801
		   
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1500,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
				//return -EIO;
				goto _kdCISModulePowerOn_exit_;
			}
			//mdelay(8);	//zrl delete for reduce the camera open time,130801
	#if 0 // do it in sensor driver	   	   
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
				//return -EIO;
				goto _kdCISModulePowerOn_exit_;
			}  
			//mdelay(8);	//zrl delete for reduce the camera open time,130801
	#endif	

			// wait power to be stable 
			mdelay(8); 	//zrl delete for reduce the camera open time,130801
		
			//disable inactive sensor
			if (GPIO_CAMERA_INVALID != pinSet[1-pinSetIdx][IDX_PS_CMRST]) 
			{
				if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
				if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
				if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
				if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
				if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
				if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
			}        
			   
			//enable active sensor
			//RST pin
			if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) 
			{
				if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

				//zrl delete for reduce the camera open time,130801
				//mdelay(20);
				mdelay(2);
				if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
				mdelay(10);
			}  

	}

#if 1//zhao.li@tcl open this for sub camera //remove GC0329 front camera code
       else if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_GC0329_YUV,currSensorName)))
       	{

		if (g_bIsAtvStart){
				printk("***********Now TV is on.\n");
			}else{
			
	        if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
		     if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
	            if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
	            if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
	            if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
	            if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
		     if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}    
	        }
		
	   	PK_DBG("current open camera is gc0329 sensor\n\r");
       		if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IO VDD
		{
			PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");		            
			goto _kdCISModulePowerOn_exit_;
		} 

		if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD
		{
		      	PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
		      	goto _kdCISModulePowerOn_exit_;
		}

		msleep(5);

	        if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {

	            if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
		     mdelay(2);
	            if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
	            mdelay(2);      
	        }

/*
	          //disable inactive sensor
	          if(pinSetIdx == 0) {//disable sub
	             pinSetIdxTmp = 1;
	          }
	          else{
	             pinSetIdxTmp = 0;
	          }
	          if (GPIO_CAMERA_INVALID != pinSet[pinSetIdxTmp][IDX_PS_CMRST]) {
	             if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
	             if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
	             if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
	             if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
	             if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
	             if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
	          }
*/		}	    
	}
#endif
	else
/*yi.zheng.hz modify for FR437080 end*/
       if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_S5K5CAGX_YUV,currSensorName)))
       {
			   if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			   if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			   
			   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))
			   {
				   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
				   //return -EIO;
				   goto _kdCISModulePowerOn_exit_;
			   }
		     mdelay(1);
			   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
			   {
				   PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
				   //return -EIO;
				   goto _kdCISModulePowerOn_exit_;
			   }
			   mdelay(1);
			   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
			   {
				   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");//2.8 ->1.8
				   //return -EIO;
				   goto _kdCISModulePowerOn_exit_;
			   }
			   mdelay(1);

		   if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) 
		   {
				 //PDN pin
				 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
				 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
				 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
				 mdelay(1);
		   
				 //RST pin
				 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
				 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
				 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
				 msleep(3);
		   }   
		   
	   }
      
       
    }
    else {//power OFF

       PK_DBG("kdCISModulePowerOn -off:currSensorName=%s\n",currSensorName);
		   if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5645_MIPI_YUV,currSensorName)))
		  {
			  PK_DBG("kdCISModulePower--off get in---SENSOR_DRVNAME_OV5645_MIPI_RAW \n");
  
			  //reset pull down
			  if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) 
			  {
				  if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
				  if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
				  if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
				  if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			  	  if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			  	  if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			  }	
			   if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D,mode_name)) {
				  PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
				  //return -EIO;
				  goto _kdCISModulePowerOn_exit_;
			  }
			   mdelay(2);
			  if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A, mode_name)) {
				  PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
				  //return -EIO;
				  goto _kdCISModulePowerOn_exit_;
			  }	
			  mdelay(2);
			  if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
			  {
				  PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
				  //return -EIO;
				  goto _kdCISModulePowerOn_exit_;
			  }
			  mdelay(2);
	#if 0 //do it in sensor driver		  
		       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
		       {
		           PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
		           //return -EIO;
		           goto _kdCISModulePowerOn_exit_;
		       }
	#endif		   
       
	       }

#if 1//zhao.li@tcl open this for sub camera //remove GC0329 code
	if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_GC0329_YUV,currSensorName)))
	{      
		if (g_bIsAtvStart){
				printk("***********Now TV is off.\n");
		}else{
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			  
	       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
	           PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
	           //return -EIO;
	           goto _kdCISModulePowerOn_exit_;
	       }
	       
	       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
	       {
	           PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
	           //return -EIO;
	           goto _kdCISModulePowerOn_exit_;
	       }
		} 
	}
	else
#endif
			if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_S5K5CAGX_YUV,currSensorName)))
			{
			       if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
					   mdelay(1);
					   if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
					   
					   if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2, mode_name)) {
			           PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
			           goto _kdCISModulePowerOn_exit_;
			       }
			       
			       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
			           PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
			           goto _kdCISModulePowerOn_exit_;
			       }
			       
			       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D,mode_name))
			       {
			           PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			           goto _kdCISModulePowerOn_exit_;
			       }
			}

  }

  return 0;
_kdCISModulePowerOn_exit_:
    return -EIO;
}


EXPORT_SYMBOL(kdCISModulePowerOn);

//!--
//