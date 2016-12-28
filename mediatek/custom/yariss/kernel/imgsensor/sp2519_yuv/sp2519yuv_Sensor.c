
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"
#include "sp2519yuv_Sensor.h"
#include "sp2519yuv_Camera_Sensor_para.h"
#include "sp2519yuv_CameraCustomized.h"

#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT

//#define DEBUG_SENSOR_SP2519  //T-card
#if 0
#define SP2519_24M_72M
#else
#define SP2519_24M_60M
#endif

kal_uint16 SP2519_write_cmos_sensor(kal_uint8 addr, kal_uint8 para);
kal_uint16 SP2519_read_cmos_sensor(kal_uint8 addr);
#define SP2519_SENSOR_ID								0x25

#ifdef DEBUG_SENSOR_SP2519
#define SP2519_OP_CODE_INI		0x00		/* Initial value. */
#define SP2519_OP_CODE_REG		0x01		/* Register */
#define SP2519_OP_CODE_DLY		0x02		/* Delay */
#define SP2519_OP_CODE_END		0x03		/* End of initial setting. */
kal_uint16 fromsd;

typedef struct
{
	u16 init_reg;
	u16 init_val;	/* Save the register value and delay tick */
	u8 op_code;		/* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
} SP2519_initial_set_struct;

SP2519_initial_set_struct SP2519_Init_Reg[1000];
SP2519_initial_set_struct SP2519_Preview_Reg[200];
SP2519_initial_set_struct SP2519_Capture_Reg[200];

u32 strtol(const char *nptr, u8 base)
{
	u8 ret;
	if(!nptr || (base!=16 && base!=10 && base!=8))
	{
		printk("%s(): NULL pointer input\n", __FUNCTION__);
		return -1;
	}
	for(ret=0; *nptr; nptr++)
	{
		if((base==16 && *nptr>='A' && *nptr<='F') || 
				(base==16 && *nptr>='a' && *nptr<='f') || 
				(base>=10 && *nptr>='0' && *nptr<='9') ||
				(base>=8 && *nptr>='0' && *nptr<='7') )
		{
			ret *= base;
			if(base==16 && *nptr>='A' && *nptr<='F')
				ret += *nptr-'A'+10;
			else if(base==16 && *nptr>='a' && *nptr<='f')
				ret += *nptr-'a'+10;
			else if(base>=10 && *nptr>='0' && *nptr<='9')
				ret += *nptr-'0';
			else if(base>=8 && *nptr>='0' && *nptr<='7')
				ret += *nptr-'0';
		}
		else
			return ret;
	}
	return ret;
}

u8 SP2519_Initialize_from_T_Flash()
{
	//FS_HANDLE fp = -1;				/* Default, no file opened. */
	//u8 *data_buff = NULL;
	u8 *curr_ptr = NULL;
	u32 file_size = 0;
	//u32 bytes_read = 0;
	u32 i = 0, j = 0;
	u8 func_ind[4] = {0};	/* REG or DLY */


	struct file *fp; 
	mm_segment_t fs; 
	loff_t pos = 0; 
	static u8 data_buff[10*1024] ;

	fp = filp_open("/mnt/sdcard/sp2519_sd", O_RDONLY , 0); 
	if (IS_ERR(fp)) { 
		printk("create file error\n"); 
		return -1; 
	} 
	fs = get_fs(); 
	set_fs(KERNEL_DS); 

	file_size = vfs_llseek(fp, 0, SEEK_END);
	vfs_read(fp, data_buff, file_size, &pos); 
	//printk("%s %d %d\n", buf,iFileLen,pos); 
	filp_close(fp, NULL); 
	set_fs(fs);





	/* Start parse the setting witch read from t-flash. */
	curr_ptr = data_buff;
	while (curr_ptr < (data_buff + file_size))
	{
		while ((*curr_ptr == ' ') || (*curr_ptr == '\t'))/* Skip the Space & TAB */
			curr_ptr++;				

		if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
		{
			while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
			{
				curr_ptr++;		/* Skip block comment code. */
			}

			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}

		if (((*curr_ptr) == '/') || ((*curr_ptr) == '{') || ((*curr_ptr) == '}'))		/* Comment line, skip it. */
		{
			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}
		/* This just content one enter line. */
		if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
		{
			curr_ptr += 2;
			continue ;
		}
		//printk(" curr_ptr1 = %s\n",curr_ptr);
		memcpy(func_ind, curr_ptr, 3);


		if (strcmp((const char *)func_ind, "REG") == 0)		/* REG */
		{
			curr_ptr += 6;				/* Skip "REG(0x" or "DLY(" */
			SP2519_Init_Reg[i].op_code = SP2519_OP_CODE_REG;

			SP2519_Init_Reg[i].init_reg = strtol((const char *)curr_ptr, 16);
			curr_ptr += 5;	/* Skip "00, 0x" */

			SP2519_Init_Reg[i].init_val = strtol((const char *)curr_ptr, 16);
			curr_ptr += 4;	/* Skip "00);" */

		}
		else									/* DLY */
		{
			/* Need add delay for this setting. */
			curr_ptr += 4;	
			SP2519_Init_Reg[i].op_code = SP2519_OP_CODE_DLY;

			SP2519_Init_Reg[i].init_reg = 0xFF;
			SP2519_Init_Reg[i].init_val = strtol((const char *)curr_ptr,  10);	/* Get the delay ticks, the delay should less then 50 */
		}
		i++;


		/* Skip to next line directly. */
		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
		{
			curr_ptr++;
		}
		curr_ptr += 2;

	}

	/* (0xFFFF, 0xFFFF) means the end of initial setting. */
	SP2519_Init_Reg[i].op_code = SP2519_OP_CODE_END;
	SP2519_Init_Reg[i].init_reg = 0xFF;
	SP2519_Init_Reg[i].init_val = 0xFF;
	i++;
	//for (j=0; j<i; j++)
	//printk(" %x  ==  %x\n",SP2519_Init_Reg[j].init_reg, SP2519_Init_Reg[j].init_val);

	/* Start apply the initial setting to sensor. */
#if 1
	for (j=0; j<i; j++)
	{
		if (SP2519_Init_Reg[j].op_code == SP2519_OP_CODE_END)	/* End of the setting. */
		{
			break ;
		}
		else if (SP2519_Init_Reg[j].op_code == SP2519_OP_CODE_DLY)
		{
			msleep(SP2519_Init_Reg[j].init_val);		/* Delay */
		}
		else if (SP2519_Init_Reg[j].op_code == SP2519_OP_CODE_REG)
		{

			SP2519_write_cmos_sensor(SP2519_Init_Reg[j].init_reg, SP2519_Init_Reg[j].init_val);
		}
		else
		{
			printk("REG ERROR!\n");
		}
	}
#endif
	return 1;	
}


u8 SP2519_Preview_from_T_Flash()
{
	//FS_HANDLE fp = -1;				/* Default, no file opened. */
	//u8 *data_buff = NULL;
	u8 *curr_ptr = NULL;
	u32 file_size = 0;
	//u32 bytes_read = 0;
	u32 i = 0, j = 0;
	u8 func_ind[4] = {0};	/* REG or DLY */


	struct file *fp; 
	mm_segment_t fs; 
	loff_t pos = 0; 
	static u8 data_buff[10*1024] ;

	fp = filp_open("/mnt/sdcard/sp2519_sd_preview", O_RDONLY , 0); 
	if (IS_ERR(fp)) { 
		printk("create file error\n"); 
		return -1; 
	} 
	fs = get_fs(); 
	set_fs(KERNEL_DS); 

	file_size = vfs_llseek(fp, 0, SEEK_END);
	vfs_read(fp, data_buff, file_size, &pos); 
	//printk("%s %d %d\n", buf,iFileLen,pos); 
	filp_close(fp, NULL); 
	set_fs(fs);





	/* Start parse the setting witch read from t-flash. */
	curr_ptr = data_buff;
	while (curr_ptr < (data_buff + file_size))
	{
		while ((*curr_ptr == ' ') || (*curr_ptr == '\t'))/* Skip the Space & TAB */
			curr_ptr++;				

		if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
		{
			while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
			{
				curr_ptr++;		/* Skip block comment code. */
			}

			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}

		if (((*curr_ptr) == '/') || ((*curr_ptr) == '{') || ((*curr_ptr) == '}'))		/* Comment line, skip it. */
		{
			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}
		/* This just content one enter line. */
		if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
		{
			curr_ptr += 2;
			continue ;
		}
		//printk(" curr_ptr1 = %s\n",curr_ptr);
		memcpy(func_ind, curr_ptr, 3);


		if (strcmp((const char *)func_ind, "REG") == 0)		/* REG */
		{
			curr_ptr += 6;				/* Skip "REG(0x" or "DLY(" */
			SP2519_Preview_Reg[i].op_code = SP2519_OP_CODE_REG;

			SP2519_Preview_Reg[i].init_reg = strtol((const char *)curr_ptr, 16);
			curr_ptr += 5;	/* Skip "00, 0x" */

			SP2519_Preview_Reg[i].init_val = strtol((const char *)curr_ptr, 16);
			curr_ptr += 4;	/* Skip "00);" */

		}
		else									/* DLY */
		{
			/* Need add delay for this setting. */
			curr_ptr += 4;	
			SP2519_Preview_Reg[i].op_code = SP2519_OP_CODE_DLY;

			SP2519_Preview_Reg[i].init_reg = 0xFF;
			SP2519_Preview_Reg[i].init_val = strtol((const char *)curr_ptr,  10);	/* Get the delay ticks, the delay should less then 50 */
		}
		i++;


		/* Skip to next line directly. */
		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
		{
			curr_ptr++;
		}
		curr_ptr += 2;
	}

	/* (0xFFFF, 0xFFFF) means the end of initial setting. */
	SP2519_Preview_Reg[i].op_code = SP2519_OP_CODE_END;
	SP2519_Preview_Reg[i].init_reg = 0xFF;
	SP2519_Preview_Reg[i].init_val = 0xFF;
	i++;
	//for (j=0; j<i; j++)
	//printk(" %x  ==  %x\n",SP2519_Init_Reg[j].init_reg, SP2519_Init_Reg[j].init_val);

	/* Start apply the initial setting to sensor. */
#if 1
	for (j=0; j<i; j++)
	{
		if (SP2519_Preview_Reg[j].op_code == SP2519_OP_CODE_END)	/* End of the setting. */
		{
			break ;
		}
		else if (SP2519_Preview_Reg[j].op_code == SP2519_OP_CODE_DLY)
		{
			msleep(SP2519_Preview_Reg[j].init_val);		/* Delay */
		}
		else if (SP2519_Preview_Reg[j].op_code == SP2519_OP_CODE_REG)
		{

			SP2519_write_cmos_sensor(SP2519_Preview_Reg[j].init_reg, SP2519_Preview_Reg[j].init_val);
		}
		else
		{
			printk("REG ERROR!\n");
		}
	}
#endif
	return 1;	
}


u8 SP2519_Capture_from_T_Flash()
{
	//FS_HANDLE fp = -1;				/* Default, no file opened. */
	//u8 *data_buff = NULL;
	u8 *curr_ptr = NULL;
	u32 file_size = 0;
	//u32 bytes_read = 0;
	u32 i = 0, j = 0;
	u8 func_ind[4] = {0};	/* REG or DLY */


	struct file *fp; 
	mm_segment_t fs; 
	loff_t pos = 0; 
	static u8 data_buff[10*1024] ;

	fp = filp_open("/mnt/sdcard/sp2519_sd_capture", O_RDONLY , 0); 
	if (IS_ERR(fp)) { 
		printk("create file error\n"); 
		return -1; 
	} 
	fs = get_fs(); 
	set_fs(KERNEL_DS); 

	file_size = vfs_llseek(fp, 0, SEEK_END);
	vfs_read(fp, data_buff, file_size, &pos); 
	//printk("%s %d %d\n", buf,iFileLen,pos); 
	filp_close(fp, NULL); 
	set_fs(fs);





	/* Start parse the setting witch read from t-flash. */
	curr_ptr = data_buff;
	while (curr_ptr < (data_buff + file_size))
	{
		while ((*curr_ptr == ' ') || (*curr_ptr == '\t'))/* Skip the Space & TAB */
			curr_ptr++;				

		if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
		{
			while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
			{
				curr_ptr++;		/* Skip block comment code. */
			}

			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}

		if (((*curr_ptr) == '/') || ((*curr_ptr) == '{') || ((*curr_ptr) == '}'))		/* Comment line, skip it. */
		{
			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}
		/* This just content one enter line. */
		if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
		{
			curr_ptr += 2;
			continue ;
		}
		//printk(" curr_ptr1 = %s\n",curr_ptr);
		memcpy(func_ind, curr_ptr, 3);


		if (strcmp((const char *)func_ind, "REG") == 0)		/* REG */
		{
			curr_ptr += 6;				/* Skip "REG(0x" or "DLY(" */
			SP2519_Capture_Reg[i].op_code = SP2519_OP_CODE_REG;

			SP2519_Capture_Reg[i].init_reg = strtol((const char *)curr_ptr, 16);
			curr_ptr += 5;	/* Skip "00, 0x" */

			SP2519_Capture_Reg[i].init_val = strtol((const char *)curr_ptr, 16);
			curr_ptr += 4;	/* Skip "00);" */

		}
		else									/* DLY */
		{
			/* Need add delay for this setting. */
			curr_ptr += 4;	
			SP2519_Capture_Reg[i].op_code = SP2519_OP_CODE_DLY;

			SP2519_Capture_Reg[i].init_reg = 0xFF;
			SP2519_Capture_Reg[i].init_val = strtol((const char *)curr_ptr,  10);	/* Get the delay ticks, the delay should less then 50 */
		}
		i++;


		/* Skip to next line directly. */
		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
		{
			curr_ptr++;
		}
		curr_ptr += 2;
	}

	/* (0xFFFF, 0xFFFF) means the end of initial setting. */
	SP2519_Capture_Reg[i].op_code = SP2519_OP_CODE_END;
	SP2519_Capture_Reg[i].init_reg = 0xFF;
	SP2519_Capture_Reg[i].init_val = 0xFF;
	i++;
	//for (j=0; j<i; j++)
	//printk(" %x  ==  %x\n",SP2519_Init_Reg[j].init_reg, SP2519_Init_Reg[j].init_val);

	/* Start apply the initial setting to sensor. */
#if 1
	for (j=0; j<i; j++)
	{
		if (SP2519_Capture_Reg[j].op_code == SP2519_OP_CODE_END)	/* End of the setting. */
		{
			break ;
		}
		else if (SP2519_Capture_Reg[j].op_code == SP2519_OP_CODE_DLY)
		{
			msleep(SP2519_Capture_Reg[j].init_val);		/* Delay */
		}
		else if (SP2519_Capture_Reg[j].op_code == SP2519_OP_CODE_REG)
		{

			SP2519_write_cmos_sensor(SP2519_Capture_Reg[j].init_reg, SP2519_Capture_Reg[j].init_val);
		}
		else
		{
			printk("REG ERROR!\n");
		}
	}
#endif
	return 1;	
}

#endif

//auto lum
#define SP2519_NORMAL_Y0ffset  0x10
#define SP2519_LOWLIGHT_Y0ffset  0x20

kal_bool   SP2519_MPEG4_encode_mode = KAL_FALSE;
kal_uint16 SP2519_dummy_pixels = 0, SP2519_dummy_lines = 0;
kal_bool   SP2519_MODE_CAPTURE = KAL_FALSE;
kal_bool   SP2519_CAM_BANDING_50HZ = KAL_TRUE; //KAL_FALSE;
kal_bool   SP2519_CAM_Nightmode = 0;
kal_bool	setshutter = KAL_FALSE;
kal_uint32 SP2519_isp_master_clock;
static kal_uint32 SP2519_g_fPV_PCLK = 24;

kal_uint8 SP2519_sensor_write_I2C_address = SP2519_WRITE_ID;
kal_uint8 SP2519_sensor_read_I2C_address = SP2519_READ_ID;

UINT8 SP2519PixelClockDivider=0;

MSDK_SENSOR_CONFIG_STRUCT SP2519SensorConfigData;

//#define SP2519_SET_PAGE0 	SP2519_write_cmos_sensor(0xfd, 0x00)
//#define SP2519_SET_PAGE1 	SP2519_write_cmos_sensor(0xfd, 0x01)

#define PFX "[sp2519]:"
#define SP2519YUV_DEBUG
#ifdef SP2519YUV_DEBUG
//#define SENSORDB(fmt, arg...)  printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define SENSORDB(fmt, arg...)  printk(PFX "%s: " fmt, __FUNCTION__ ,##arg)

#else
#define SENSORDB(x,...)
#endif

#define WINDOW_SIZE_UXGA	0
#define WINDOW_SIZE_720P 	1
#define WINDOW_SIZE_SVGA 	2
#define WINDOW_SIZE_VGA	 	3

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
kal_uint16 SP2519_write_cmos_sensor(kal_uint8 addr, kal_uint8 para)
{
	char puSendCmd[2] = {(char)(addr & 0xFF) , (char)(para & 0xFF)};

	iWriteRegI2C(puSendCmd , 2, SP2519_WRITE_ID);

}
kal_uint16 SP2519_read_cmos_sensor(kal_uint8 addr)
{
	kal_uint16 get_byte=0;
	char puSendCmd = { (char)(addr & 0xFF) };
	iReadRegI2C(&puSendCmd , 1, (u8*)&get_byte, 1, SP2519_WRITE_ID);

	return get_byte;
}

void SP2519_Set_Shutter(kal_uint16 iShutter)
{
	SENSORDB("Ronlus SP2519_Set_Shutter\r\n");
} /* Set_SP2519_Shutter */


kal_uint16 SP2519_Read_Shutter(void)
{
	kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;

	//temp_reg1 = SP2519_read_cmos_sensor(0x04);
	//temp_reg2 = SP2519_read_cmos_sensor(0x03);

	//shutter = (temp_reg1 & 0xFF) | (temp_reg2 << 8);
	SENSORDB("Ronlus SP2519_Read_Shutter\r\n");
	return shutter;
} /* SP2519_read_shutter */


static void SP2519_ae_enable(kal_bool enable)
{	
#if 0
	kal_uint16 temp_AWB_reg = 0;
	SENSORDB("Ronlus SP2519_ae_enable\r\n");
	temp_AWB_reg = SP2519_read_cmos_sensor(0x32);//Ronlus 20120511
	if (enable == 1)
	{
		SP2519_write_cmos_sensor(0x32, (temp_AWB_reg |0x05));
	}
	else if(enable == 0)
	{
		SP2519_write_cmos_sensor(0x32, (temp_AWB_reg & (~0x05)));
	}
#endif
}

static void SP2519_awb_enable(kal_bool enalbe)
{	 
	kal_uint16 temp_AWB_reg = 0;
	SENSORDB("Ronlus SP2519_awb_enable\r\n");
}


static void SP2519_set_hb_shutter(kal_uint32 hb_add,  kal_uint32 shutter)
{
	kal_uint32 hb_ori, hb_total;
	kal_uint32 temp_reg, banding_step;
	SENSORDB("Ronlus SP2519_set_hb_shutter\r\n");
}    /* SP2519_set_dummy */

//void SP2519_config_window(kal_uint16 startx, kal_uint16 starty, kal_uint16 width, kal_uint16 height)//Ronlus
void SP2519_config_window(kal_uint8 index)
{
	SENSORDB("Ronlus SP2519_config_window,index = %d\r\n",index);
	switch(index)
	{
		case WINDOW_SIZE_UXGA:
			break;
		case WINDOW_SIZE_720P:
			break;
		case WINDOW_SIZE_SVGA:
			break;
		case WINDOW_SIZE_VGA:
			break;
		default:
			break;
	}
} /* SP2519_config_window */



kal_uint16 SP2519_SetGain(kal_uint16 iGain)
{
	SENSORDB("Ronlus SP2519_SetGain\r\n");
	return iGain;
}


void SP2519_night_mode(kal_bool bEnable)
{
	//kal_uint16 night = SP2519_read_cmos_sensor(0x20);
	SENSORDB("Ronlus SP2519_night_mode\r\n");//亿新捷陈康sp2519      a8-10 创河田810 3p  
	//sensorlist.cpp kd_imagesensor.h add related files  
	if (bEnable)/*night mode settings*/
	{
		SENSORDB("Ronlus night mode\r\n");
		SP2519_CAM_Nightmode = 1;
		//SP2519_write_cmos_sensor(0xfd,0x00);
		//SP2519_write_cmos_sensor(0xb2,SP2519_LOWLIGHT_Y0ffset);
		if(SP2519_MPEG4_encode_mode == KAL_TRUE) /*video night mode*/
		{
			SENSORDB("Ronlus video night mode\r\n");

			if(SP2519_CAM_BANDING_50HZ == KAL_TRUE)/*video night mode 50hz*/
			{	
#ifdef SP2519_24M_72M		
			// 3X pll   fix 6.0778fps           video night mode 50hz                          
			SP2519_write_cmos_sensor(0xfd , 0x00);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x04 , 0xc2);
			SP2519_write_cmos_sensor(0x05 , 0x00);
			SP2519_write_cmos_sensor(0x06 , 0x00);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x00);
			SP2519_write_cmos_sensor(0x09 , 0x05);
			SP2519_write_cmos_sensor(0x0a , 0xc6);
			SP2519_write_cmos_sensor(0xfd , 0x01);
			SP2519_write_cmos_sensor(0xf0 , 0x00);
			SP2519_write_cmos_sensor(0xf7 , 0x4b);
			SP2519_write_cmos_sensor(0xf8 , 0x3f);
			SP2519_write_cmos_sensor(0x02 , 0x10);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x06 , 0x4b);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x01);                              
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0xfd , 0x02);
			SP2519_write_cmos_sensor(0x3d , 0x14);
			SP2519_write_cmos_sensor(0x3e , 0x3f);
			SP2519_write_cmos_sensor(0x3f , 0x00);
			SP2519_write_cmos_sensor(0x88 , 0xd3); 
			SP2519_write_cmos_sensor(0x89 , 0x20); 
			SP2519_write_cmos_sensor(0x8a , 0x86); 
			SP2519_write_cmos_sensor(0xfd , 0x02);                                       
			SP2519_write_cmos_sensor(0xbe , 0xb0);     
			SP2519_write_cmos_sensor(0xbf , 0x04); 
			SP2519_write_cmos_sensor(0xd0 , 0xb0); 
			SP2519_write_cmos_sensor(0xd1 , 0x04); 
			SP2519_write_cmos_sensor(0xc9 , 0xb0); 
			SP2519_write_cmos_sensor(0xca , 0x04);  
			               
//#else 
			// 2x pll 50hz fix 6.0778FPS maxgain  video night mode 50hz 
			SP2519_write_cmos_sensor(0xfd , 0x00);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x04 , 0xc2);
			SP2519_write_cmos_sensor(0x05 , 0x00);
			SP2519_write_cmos_sensor(0x06 , 0x00);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x00);
			SP2519_write_cmos_sensor(0x09 , 0x02);
			SP2519_write_cmos_sensor(0x0a , 0xa6);
			SP2519_write_cmos_sensor(0xfd , 0x01);
			SP2519_write_cmos_sensor(0xf0 , 0x00);
			SP2519_write_cmos_sensor(0xf7 , 0x4b);
			SP2519_write_cmos_sensor(0xf8 , 0x3f);
			SP2519_write_cmos_sensor(0x02 , 0x10);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x06 , 0x4b);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x01);                              
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0xfd , 0x02);
			SP2519_write_cmos_sensor(0x3d , 0x14);
			SP2519_write_cmos_sensor(0x3e , 0x3f);
			SP2519_write_cmos_sensor(0x3f , 0x00);
			SP2519_write_cmos_sensor(0x88 , 0xd3); 
			SP2519_write_cmos_sensor(0x89 , 0x20); 
			SP2519_write_cmos_sensor(0x8a , 0x86); 
			SP2519_write_cmos_sensor(0xfd , 0x02);                                       
			SP2519_write_cmos_sensor(0xbe , 0xb0);     
			SP2519_write_cmos_sensor(0xbf , 0x04); 
			SP2519_write_cmos_sensor(0xd0 , 0xb0); 
			SP2519_write_cmos_sensor(0xd1 , 0x04); 
			SP2519_write_cmos_sensor(0xc9 , 0xb0); 
			SP2519_write_cmos_sensor(0xca , 0x04);	
#endif 
#ifdef SP2519_24M_60M
//24M fix 8fps 2.5pll 50hz
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x02);
	SP2519_write_cmos_sensor(0x04,0x52);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x02);
	SP2519_write_cmos_sensor(0x0a,0x51);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x63);
	SP2519_write_cmos_sensor(0xf8,0x53);
	SP2519_write_cmos_sensor(0x02,0x0c);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x63);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x53);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x2b);
	SP2519_write_cmos_sensor(0x89,0x2b);
	SP2519_write_cmos_sensor(0x8a,0x65);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xa4);
	SP2519_write_cmos_sensor(0xbf,0x04);
	SP2519_write_cmos_sensor(0xd0,0xa4);
	SP2519_write_cmos_sensor(0xd1,0x04);
	SP2519_write_cmos_sensor(0xc9,0xa4);
	SP2519_write_cmos_sensor(0xca,0x04);

#endif
				SENSORDB("Ronlus video night mode 50hz\r\n");
			}
			else/*video night mode 60hz*/
			{ 
				SENSORDB("Ronlus video night mode 60hz\r\n");     
#ifdef SP2519_24M_72M		

			// 3X pll   fix 6.0292fps    video night mode 60hz                                              
			SP2519_write_cmos_sensor(0xfd , 0x00);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x04 , 0x74);
			SP2519_write_cmos_sensor(0x05 , 0x00);
			SP2519_write_cmos_sensor(0x06 , 0x00);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x00);
			SP2519_write_cmos_sensor(0x09 , 0x05);
			SP2519_write_cmos_sensor(0x0a , 0xd9);
			SP2519_write_cmos_sensor(0xfd , 0x01);
			SP2519_write_cmos_sensor(0xf0 , 0x00);
			SP2519_write_cmos_sensor(0xf7 , 0x3e);
			SP2519_write_cmos_sensor(0xf8 , 0x3e);
			SP2519_write_cmos_sensor(0x02 , 0x14);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x06 , 0x3e);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x01);                              
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0xfd , 0x02);
			SP2519_write_cmos_sensor(0x3d , 0x14);
			SP2519_write_cmos_sensor(0x3e , 0x3e);
			SP2519_write_cmos_sensor(0x3f , 0x00);
			SP2519_write_cmos_sensor(0x88 , 0x42); 
			SP2519_write_cmos_sensor(0x89 , 0x42); 
			SP2519_write_cmos_sensor(0x8a , 0x88); 
			SP2519_write_cmos_sensor(0xfd , 0x02);                                       
			SP2519_write_cmos_sensor(0xbe , 0xd8);     
			SP2519_write_cmos_sensor(0xbf , 0x04); 
			SP2519_write_cmos_sensor(0xd0 , 0xd8); 
			SP2519_write_cmos_sensor(0xd1 , 0x04); 
			SP2519_write_cmos_sensor(0xc9 , 0xd8); 
			SP2519_write_cmos_sensor(0xca , 0x04);
//#else

			// 2X pll   fix 6.0292fps    video night mode 60hz  
			SP2519_write_cmos_sensor(0xfd , 0x00);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x04 , 0x74);
			SP2519_write_cmos_sensor(0x05 , 0x00);
			SP2519_write_cmos_sensor(0x06 , 0x00);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x00);
			SP2519_write_cmos_sensor(0x09 , 0x02);
			SP2519_write_cmos_sensor(0x0a , 0xb3);
			SP2519_write_cmos_sensor(0xfd , 0x01);
			SP2519_write_cmos_sensor(0xf0 , 0x00);
			SP2519_write_cmos_sensor(0xf7 , 0x3e);
			SP2519_write_cmos_sensor(0xf8 , 0x3e);
			SP2519_write_cmos_sensor(0x02 , 0x14);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x06 , 0x3e);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x01);                              
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0xfd , 0x02);
			SP2519_write_cmos_sensor(0x3d , 0x14);
			SP2519_write_cmos_sensor(0x3e , 0x3e);
			SP2519_write_cmos_sensor(0x3f , 0x00);
			SP2519_write_cmos_sensor(0x88 , 0x42); 
			SP2519_write_cmos_sensor(0x89 , 0x42); 
			SP2519_write_cmos_sensor(0x8a , 0x88); 
			SP2519_write_cmos_sensor(0xfd , 0x02);                                       
			SP2519_write_cmos_sensor(0xbe , 0xd8);     
			SP2519_write_cmos_sensor(0xbf , 0x04); 
			SP2519_write_cmos_sensor(0xd0 , 0xd8); 
			SP2519_write_cmos_sensor(0xd1 , 0x04); 
			SP2519_write_cmos_sensor(0xc9 , 0xd8); 
			SP2519_write_cmos_sensor(0xca , 0x04);

#endif
#ifdef SP2519_24M_60M
//24M fix 8fps 2.5pll 60hz 
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x04,0xf2);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x02);
	SP2519_write_cmos_sensor(0x0a,0x48);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x53);
	SP2519_write_cmos_sensor(0xf8,0x53);
	SP2519_write_cmos_sensor(0x02,0x0f);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x53);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x53);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x2b);
	SP2519_write_cmos_sensor(0x89,0x2b);
	SP2519_write_cmos_sensor(0x8a,0x66);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xdd);
	SP2519_write_cmos_sensor(0xbf,0x04);
	SP2519_write_cmos_sensor(0xd0,0xdd);
	SP2519_write_cmos_sensor(0xd1,0x04);
	SP2519_write_cmos_sensor(0xc9,0xdd);
	SP2519_write_cmos_sensor(0xca,0x04);
#endif
			}
		}/*capture night mode*/
		else 
		{   
			SENSORDB("Ronlus capture night mode\r\n");
			if(SP2519_CAM_BANDING_50HZ == KAL_TRUE)/*capture night mode 50hz*/
			{	
				SENSORDB("Ronlus capture night mode 50hz\r\n");
#ifdef SP2519_24M_72M		                          
				// 3X pll   6-12fps    capture night mode 50hz                                              
				SP2519_write_cmos_sensor(0xfd,0x00); 
				SP2519_write_cmos_sensor(0x03,0x03); 
				SP2519_write_cmos_sensor(0x04,0x7e); 
				SP2519_write_cmos_sensor(0x05,0x00); 
				SP2519_write_cmos_sensor(0x06,0x00); 
				SP2519_write_cmos_sensor(0x07,0x00); 
				SP2519_write_cmos_sensor(0x08,0x00); 
				SP2519_write_cmos_sensor(0x09,0x01); 
				SP2519_write_cmos_sensor(0x0a,0x1e); 
				SP2519_write_cmos_sensor(0xfd,0x01); 
				SP2519_write_cmos_sensor(0xf0,0x00); 
				SP2519_write_cmos_sensor(0xf7,0x95); 
				SP2519_write_cmos_sensor(0xf8,0x7c); 
				SP2519_write_cmos_sensor(0x02,0x10); 
				SP2519_write_cmos_sensor(0x03,0x01); 
				SP2519_write_cmos_sensor(0x06,0x95); 
				SP2519_write_cmos_sensor(0x07,0x00); 
				SP2519_write_cmos_sensor(0x08,0x01); 
				SP2519_write_cmos_sensor(0x09,0x00); 
				SP2519_write_cmos_sensor(0xfd,0x02); 
				SP2519_write_cmos_sensor(0x3d,0x14); 
				SP2519_write_cmos_sensor(0x3e,0x7c); 
				SP2519_write_cmos_sensor(0x3f,0x00); 
				SP2519_write_cmos_sensor(0x88,0x6f); 
				SP2519_write_cmos_sensor(0x89,0x21); 
				SP2519_write_cmos_sensor(0x8a,0x43); 
				SP2519_write_cmos_sensor(0xfd,0x02); 
				SP2519_write_cmos_sensor(0xbe,0x50); 
				SP2519_write_cmos_sensor(0xbf,0x09); 
				SP2519_write_cmos_sensor(0xd0,0x50); 
				SP2519_write_cmos_sensor(0xd1,0x09); 
				SP2519_write_cmos_sensor(0xc9,0x50); 
				SP2519_write_cmos_sensor(0xca,0x09); 				                   
//#else                                                                 
				// 2x pll 50hz fix 6.0778FPS maxgain  capture night mode 50hz 
				SP2519_write_cmos_sensor(0xfd , 0x00);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x04 , 0xc2);
				SP2519_write_cmos_sensor(0x05 , 0x00);
				SP2519_write_cmos_sensor(0x06 , 0x00);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x00);
				SP2519_write_cmos_sensor(0x09 , 0x02);
				SP2519_write_cmos_sensor(0x0a , 0xa6);
				SP2519_write_cmos_sensor(0xfd , 0x01);
				SP2519_write_cmos_sensor(0xf0 , 0x00);
				SP2519_write_cmos_sensor(0xf7 , 0x4b);
				SP2519_write_cmos_sensor(0xf8 , 0x3f);
				SP2519_write_cmos_sensor(0x02 , 0x10);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x06 , 0x4b);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x01);                              
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0xfd , 0x02);
				SP2519_write_cmos_sensor(0x3d , 0x14);
				SP2519_write_cmos_sensor(0x3e , 0x3f);
				SP2519_write_cmos_sensor(0x3f , 0x00);
				SP2519_write_cmos_sensor(0x88 , 0xd3); 
				SP2519_write_cmos_sensor(0x89 , 0x20); 
				SP2519_write_cmos_sensor(0x8a , 0x86); 
				SP2519_write_cmos_sensor(0xfd , 0x02);                                       
				SP2519_write_cmos_sensor(0xbe , 0xb0);     
				SP2519_write_cmos_sensor(0xbf , 0x04); 
				SP2519_write_cmos_sensor(0xd0 , 0xb0); 
				SP2519_write_cmos_sensor(0xd1 , 0x04); 
				SP2519_write_cmos_sensor(0xc9 , 0xb0); 
				SP2519_write_cmos_sensor(0xca , 0x04);	
#endif  
#ifdef SP2519_24M_60M
//24M 6-11.2fps 50HZ 2.5pll
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x03);
	SP2519_write_cmos_sensor(0x04,0x42);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0x0a,0x9d);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x8b);
	SP2519_write_cmos_sensor(0xf8,0x74);
	SP2519_write_cmos_sensor(0x02,0x10);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x8b);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x14);
	SP2519_write_cmos_sensor(0x3e,0x74);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0xae);
	SP2519_write_cmos_sensor(0x89,0x69);
	SP2519_write_cmos_sensor(0x8a,0x43);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xb0);
	SP2519_write_cmos_sensor(0xbf,0x08);
	SP2519_write_cmos_sensor(0xd0,0xb0);
	SP2519_write_cmos_sensor(0xd1,0x08);
	SP2519_write_cmos_sensor(0xc9,0xb0);
	SP2519_write_cmos_sensor(0xca,0x08);
#endif
			}
			else/*capture night mode 60hz*/
			{ 
				SENSORDB("Ronlus capture night mode 60hz\r\n");  
#ifdef SP2519_24M_72M		
				// 3X pll   6-12fps    capture night mode 60hz                                              
				SP2519_write_cmos_sensor(0xfd,0x00);
				SP2519_write_cmos_sensor(0x03,0x02);
				SP2519_write_cmos_sensor(0x04,0xe8);
				SP2519_write_cmos_sensor(0x05,0x00);
				SP2519_write_cmos_sensor(0x06,0x00);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x00);
				SP2519_write_cmos_sensor(0x09,0x01);
				SP2519_write_cmos_sensor(0x0a,0x20);
				SP2519_write_cmos_sensor(0xfd,0x01);
				SP2519_write_cmos_sensor(0xf0,0x00);
				SP2519_write_cmos_sensor(0xf7,0x7c);
				SP2519_write_cmos_sensor(0xf8,0x7c);
				SP2519_write_cmos_sensor(0x02,0x14);
				SP2519_write_cmos_sensor(0x03,0x01);
				SP2519_write_cmos_sensor(0x06,0x7c);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x01);
				SP2519_write_cmos_sensor(0x09,0x00);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0x3d,0x14);
				SP2519_write_cmos_sensor(0x3e,0x7c);
				SP2519_write_cmos_sensor(0x3f,0x00);
				SP2519_write_cmos_sensor(0x88,0x21);
				SP2519_write_cmos_sensor(0x89,0x21);
				SP2519_write_cmos_sensor(0x8a,0x44);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0xbe,0xb0);
				SP2519_write_cmos_sensor(0xbf,0x09);
				SP2519_write_cmos_sensor(0xd0,0xb0);
				SP2519_write_cmos_sensor(0xd1,0x09);
				SP2519_write_cmos_sensor(0xc9,0xb0);
				SP2519_write_cmos_sensor(0xca,0x09);

//#else

				// 2X pll   fix 6.0292fps    capture night mode 60hz  
				SP2519_write_cmos_sensor(0xfd , 0x00);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x04 , 0x74);
				SP2519_write_cmos_sensor(0x05 , 0x00);
				SP2519_write_cmos_sensor(0x06 , 0x00);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x00);
				SP2519_write_cmos_sensor(0x09 , 0x02);
				SP2519_write_cmos_sensor(0x0a , 0xb3);
				SP2519_write_cmos_sensor(0xfd , 0x01);
				SP2519_write_cmos_sensor(0xf0 , 0x00);
				SP2519_write_cmos_sensor(0xf7 , 0x3e);
				SP2519_write_cmos_sensor(0xf8 , 0x3e);
				SP2519_write_cmos_sensor(0x02 , 0x14);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x06 , 0x3e);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x01);                              
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0xfd , 0x02);
				SP2519_write_cmos_sensor(0x3d , 0x14);
				SP2519_write_cmos_sensor(0x3e , 0x3e);
				SP2519_write_cmos_sensor(0x3f , 0x00);
				SP2519_write_cmos_sensor(0x88 , 0x42); 
				SP2519_write_cmos_sensor(0x89 , 0x42); 
				SP2519_write_cmos_sensor(0x8a , 0x88); 
				SP2519_write_cmos_sensor(0xfd , 0x02);                                       
				SP2519_write_cmos_sensor(0xbe , 0xd8);     
				SP2519_write_cmos_sensor(0xbf , 0x04); 
				SP2519_write_cmos_sensor(0xd0 , 0xd8); 
				SP2519_write_cmos_sensor(0xd1 , 0x04); 
				SP2519_write_cmos_sensor(0xc9 , 0xd8); 
				SP2519_write_cmos_sensor(0xca , 0x04);
#endif
#ifdef SP2519_24M_60M
//24M 6-11.2fps 2.5pll 60hz
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x02);
	SP2519_write_cmos_sensor(0x04,0xb8);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0x0a,0x9c);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x74);
	SP2519_write_cmos_sensor(0xf8,0x74);
	SP2519_write_cmos_sensor(0x02,0x14);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x74);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x14);
	SP2519_write_cmos_sensor(0x3e,0x74);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x69);
	SP2519_write_cmos_sensor(0x89,0x69);
	SP2519_write_cmos_sensor(0x8a,0x44);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0x10);
	SP2519_write_cmos_sensor(0xbf,0x09);
	SP2519_write_cmos_sensor(0xd0,0x10);
	SP2519_write_cmos_sensor(0xd1,0x09);
	SP2519_write_cmos_sensor(0xc9,0x10);
	SP2519_write_cmos_sensor(0xca,0x09);

#endif
			}
		}
	}
	else /*normal mode settings*/
	{
		SENSORDB("Ronlus normal mode\r\n");
		SP2519_CAM_Nightmode = 0;
              //SP2519_write_cmos_sensor(0xfd,0x00);
	      //SP2519_write_cmos_sensor(0xb2,SP2519_NORMAL_Y0ffset);
		if (SP2519_MPEG4_encode_mode == KAL_TRUE) 
		{
			SENSORDB("Ronlus video normal mode\r\n");
			if(SP2519_CAM_BANDING_50HZ == KAL_TRUE)/*video normal mode 50hz*/
			{
				SENSORDB("Ronlus video normal mode 50hz\r\n");

#ifdef SP2519_24M_72M
				// 3X pll   fix 12fps       video normal mode 50hz 
				SP2519_write_cmos_sensor(0xfd,0x00);
				SP2519_write_cmos_sensor(0x03,0x03);
				SP2519_write_cmos_sensor(0x04,0x7e);
				SP2519_write_cmos_sensor(0x05,0x00);
				SP2519_write_cmos_sensor(0x06,0x00);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x00);
				SP2519_write_cmos_sensor(0x09,0x01);
				SP2519_write_cmos_sensor(0x0a,0x1e);
				SP2519_write_cmos_sensor(0xfd,0x01);
				SP2519_write_cmos_sensor(0xf0,0x00);
				SP2519_write_cmos_sensor(0xf7,0x95);
				SP2519_write_cmos_sensor(0xf8,0x7c);
				SP2519_write_cmos_sensor(0x02,0x08);
				SP2519_write_cmos_sensor(0x03,0x01);
				SP2519_write_cmos_sensor(0x06,0x95);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x01);
				SP2519_write_cmos_sensor(0x09,0x00);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0x3d,0x0a);
				SP2519_write_cmos_sensor(0x3e,0x7c);
				SP2519_write_cmos_sensor(0x3f,0x00);
				SP2519_write_cmos_sensor(0x88,0x6f);
				SP2519_write_cmos_sensor(0x89,0x21);
				SP2519_write_cmos_sensor(0x8a,0x43);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0xbe,0xa8);
				SP2519_write_cmos_sensor(0xbf,0x04);
				SP2519_write_cmos_sensor(0xd0,0xa8);
				SP2519_write_cmos_sensor(0xd1,0x04);
				SP2519_write_cmos_sensor(0xc9,0xa8);
				SP2519_write_cmos_sensor(0xca,0x04);
//#else

				// 2X pll   fix 9.0762fps       video normal mode 50hz     
				SP2519_write_cmos_sensor(0xfd , 0x00);
				SP2519_write_cmos_sensor(0x03 , 0x02);
				SP2519_write_cmos_sensor(0x04 , 0xa0);
				SP2519_write_cmos_sensor(0x05 , 0x00);
				SP2519_write_cmos_sensor(0x06 , 0x00);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x00);
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0x0a , 0x95);
				SP2519_write_cmos_sensor(0xfd , 0x01);
				SP2519_write_cmos_sensor(0xf0 , 0x00);
				SP2519_write_cmos_sensor(0xf7 , 0x70);
				SP2519_write_cmos_sensor(0xf8 , 0x5d);
				SP2519_write_cmos_sensor(0x02 , 0x0b);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x06 , 0x70);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x01);                              
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0xfd , 0x02);
				SP2519_write_cmos_sensor(0x3d , 0x0d);
				SP2519_write_cmos_sensor(0x3e , 0x5d);
				SP2519_write_cmos_sensor(0x3f , 0x00);
				SP2519_write_cmos_sensor(0x88 , 0x92); 
				SP2519_write_cmos_sensor(0x89 , 0x81); 
				SP2519_write_cmos_sensor(0x8a , 0x54); 
				SP2519_write_cmos_sensor(0xfd , 0x02);                                       
				SP2519_write_cmos_sensor(0xbe , 0xd0);     
				SP2519_write_cmos_sensor(0xbf , 0x04); 
				SP2519_write_cmos_sensor(0xd0 , 0xd0); 
				SP2519_write_cmos_sensor(0xd1 , 0x04); 
				SP2519_write_cmos_sensor(0xc9 , 0xd0); 
				SP2519_write_cmos_sensor(0xca , 0x04);	  				
#endif
#ifdef SP2519_24M_60M
//24M fix 10fps 2.5PLL 50HZ
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x02);
	SP2519_write_cmos_sensor(0x04,0xe8);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x01);
	SP2519_write_cmos_sensor(0x0a,0x20);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x7c);
	SP2519_write_cmos_sensor(0xf8,0x67);
	SP2519_write_cmos_sensor(0x02,0x0a);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x7c);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0c);
	SP2519_write_cmos_sensor(0x3e,0x67);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x21);
	SP2519_write_cmos_sensor(0x89,0xf8);
	SP2519_write_cmos_sensor(0x8a,0x44);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xd8);
	SP2519_write_cmos_sensor(0xbf,0x04);
	SP2519_write_cmos_sensor(0xd0,0xd8);
	SP2519_write_cmos_sensor(0xd1,0x04);
	SP2519_write_cmos_sensor(0xc9,0xd8);
	SP2519_write_cmos_sensor(0xca,0x04);
#endif
			}
			else/*video normal mode 60hz*/
			{
				SENSORDB("Ronlus video normal mode 60hz\r\n");  

#ifdef SP2519_24M_72M
			// 3X pll   fix 12fps     video normal mode 60hz
			SP2519_write_cmos_sensor(0xfd,0x00);
			SP2519_write_cmos_sensor(0x03,0x02);
			SP2519_write_cmos_sensor(0x04,0xe8);
			SP2519_write_cmos_sensor(0x05,0x00);
			SP2519_write_cmos_sensor(0x06,0x00);
			SP2519_write_cmos_sensor(0x07,0x00);
			SP2519_write_cmos_sensor(0x08,0x00);
			SP2519_write_cmos_sensor(0x09,0x01);
			SP2519_write_cmos_sensor(0x0a,0x20);
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xf0,0x00);
			SP2519_write_cmos_sensor(0xf7,0x7c);
			SP2519_write_cmos_sensor(0xf8,0x7c);
			SP2519_write_cmos_sensor(0x02,0x0a);
			SP2519_write_cmos_sensor(0x03,0x01);
			SP2519_write_cmos_sensor(0x06,0x7c);
			SP2519_write_cmos_sensor(0x07,0x00);
			SP2519_write_cmos_sensor(0x08,0x01);
			SP2519_write_cmos_sensor(0x09,0x00);
			SP2519_write_cmos_sensor(0xfd,0x02);
			SP2519_write_cmos_sensor(0x3d,0x0a);
			SP2519_write_cmos_sensor(0x3e,0x7c);
			SP2519_write_cmos_sensor(0x3f,0x00);
			SP2519_write_cmos_sensor(0x88,0x21);
			SP2519_write_cmos_sensor(0x89,0x21);
			SP2519_write_cmos_sensor(0x8a,0x44);
			SP2519_write_cmos_sensor(0xfd,0x02);
			SP2519_write_cmos_sensor(0xbe,0xd8);
			SP2519_write_cmos_sensor(0xbf,0x04);
			SP2519_write_cmos_sensor(0xd0,0xd8);
			SP2519_write_cmos_sensor(0xd1,0x04);
			SP2519_write_cmos_sensor(0xc9,0xd8);
			SP2519_write_cmos_sensor(0xca,0x04);
//#else
			// 2X pll   fix 9.0438fps     video normal mode 60hz   
			SP2519_write_cmos_sensor(0xfd , 0x00);
			SP2519_write_cmos_sensor(0x03 , 0x02);
			SP2519_write_cmos_sensor(0x04 , 0x2e);
			SP2519_write_cmos_sensor(0x05 , 0x00);
			SP2519_write_cmos_sensor(0x06 , 0x00);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x00);
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0x0a , 0x99);
			SP2519_write_cmos_sensor(0xfd , 0x01);
			SP2519_write_cmos_sensor(0xf0 , 0x00);
			SP2519_write_cmos_sensor(0xf7 , 0x5d);
			SP2519_write_cmos_sensor(0xf8 , 0x5d);
			SP2519_write_cmos_sensor(0x02 , 0x0d);
			SP2519_write_cmos_sensor(0x03 , 0x01);
			SP2519_write_cmos_sensor(0x06 , 0x5d);
			SP2519_write_cmos_sensor(0x07 , 0x00);
			SP2519_write_cmos_sensor(0x08 , 0x01);                              
			SP2519_write_cmos_sensor(0x09 , 0x00);
			SP2519_write_cmos_sensor(0xfd , 0x02);
			SP2519_write_cmos_sensor(0x3d , 0x0d);
			SP2519_write_cmos_sensor(0x3e , 0x5d);
			SP2519_write_cmos_sensor(0x3f , 0x00);
			SP2519_write_cmos_sensor(0x88 , 0x81); 
			SP2519_write_cmos_sensor(0x89 , 0x81); 
			SP2519_write_cmos_sensor(0x8a , 0x55); 
			SP2519_write_cmos_sensor(0xfd , 0x02);                                       
			SP2519_write_cmos_sensor(0xbe , 0xb9);     
			SP2519_write_cmos_sensor(0xbf , 0x04); 
			SP2519_write_cmos_sensor(0xd0 , 0xb9); 
			SP2519_write_cmos_sensor(0xd1 , 0x04); 
			SP2519_write_cmos_sensor(0xc9 , 0xb9); 
			SP2519_write_cmos_sensor(0xca , 0x04);	
#endif
#ifdef SP2519_24M_60M
//24M fix 10fps 2.5pll 60HZ
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x02);
	SP2519_write_cmos_sensor(0x04,0x6a);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x01);
	SP2519_write_cmos_sensor(0x0a,0x24);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x67);
	SP2519_write_cmos_sensor(0xf8,0x67);
	SP2519_write_cmos_sensor(0x02,0x0c);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x67);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0c);
	SP2519_write_cmos_sensor(0x3e,0x67);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0xf8);
	SP2519_write_cmos_sensor(0x89,0xf8);
	SP2519_write_cmos_sensor(0x8a,0x44);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xd4);
	SP2519_write_cmos_sensor(0xbf,0x04);
	SP2519_write_cmos_sensor(0xd0,0xd4);
	SP2519_write_cmos_sensor(0xd1,0x04);
	SP2519_write_cmos_sensor(0xc9,0xd4);
	SP2519_write_cmos_sensor(0xca,0x04);
#endif
			}
		}
		else/*capture normal mode*/
		{
			SENSORDB("Ronlus capture normal mode\r\n");
			if(SP2519_CAM_BANDING_50HZ == KAL_TRUE)/*capture normal mode 50hz*/
			{
				SENSORDB("Ronlus capture normal mode 50hz\r\n");

#ifdef SP2519_24M_72M 
				// 3X pll   12~8fps       capture normal mode 50hz                 
				SP2519_write_cmos_sensor(0xfd,0x00);
				SP2519_write_cmos_sensor(0x03,0x03);
				SP2519_write_cmos_sensor(0x04,0x7e);
				SP2519_write_cmos_sensor(0x05,0x00);
				SP2519_write_cmos_sensor(0x06,0x00);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x00);
				SP2519_write_cmos_sensor(0x09,0x01);
				SP2519_write_cmos_sensor(0x0a,0x1e);
				SP2519_write_cmos_sensor(0xfd,0x01);
				SP2519_write_cmos_sensor(0xf0,0x00);
				SP2519_write_cmos_sensor(0xf7,0x95);
				SP2519_write_cmos_sensor(0xf8,0x7c);
				SP2519_write_cmos_sensor(0x02,0x0c);
				SP2519_write_cmos_sensor(0x03,0x01);
				SP2519_write_cmos_sensor(0x06,0x95);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x01);
				SP2519_write_cmos_sensor(0x09,0x00);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0x3d,0x0f);
				SP2519_write_cmos_sensor(0x3e,0x7c);
				SP2519_write_cmos_sensor(0x3f,0x00);
				SP2519_write_cmos_sensor(0x88,0x6f);
				SP2519_write_cmos_sensor(0x89,0x21);
				SP2519_write_cmos_sensor(0x8a,0x43);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0xbe,0xfc);
				SP2519_write_cmos_sensor(0xbf,0x06);
				SP2519_write_cmos_sensor(0xd0,0xfc);
				SP2519_write_cmos_sensor(0xd1,0x06);
				SP2519_write_cmos_sensor(0xc9,0xfc);
				SP2519_write_cmos_sensor(0xca,0x06);
//#else

				// 2X pll   fix 9.0762fps       capture normal mode 50hz     
				SP2519_write_cmos_sensor(0xfd , 0x00);
				SP2519_write_cmos_sensor(0x03 , 0x02);
				SP2519_write_cmos_sensor(0x04 , 0xa0);
				SP2519_write_cmos_sensor(0x05 , 0x00);
				SP2519_write_cmos_sensor(0x06 , 0x00);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x00);
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0x0a , 0x95);
				SP2519_write_cmos_sensor(0xfd , 0x01);
				SP2519_write_cmos_sensor(0xf0 , 0x00);
				SP2519_write_cmos_sensor(0xf7 , 0x70);
				SP2519_write_cmos_sensor(0xf8 , 0x5d);
				SP2519_write_cmos_sensor(0x02 , 0x0b);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x06 , 0x70);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x01);                              
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0xfd , 0x02);
				SP2519_write_cmos_sensor(0x3d , 0x0d);
				SP2519_write_cmos_sensor(0x3e , 0x5d);
				SP2519_write_cmos_sensor(0x3f , 0x00);
				SP2519_write_cmos_sensor(0x88 , 0x92); 
				SP2519_write_cmos_sensor(0x89 , 0x81); 
				SP2519_write_cmos_sensor(0x8a , 0x54); 
				SP2519_write_cmos_sensor(0xfd , 0x02);                                       
				SP2519_write_cmos_sensor(0xbe , 0xd0);     
				SP2519_write_cmos_sensor(0xbf , 0x04); 
				SP2519_write_cmos_sensor(0xd0 , 0xd0); 
				SP2519_write_cmos_sensor(0xd1 , 0x04); 
				SP2519_write_cmos_sensor(0xc9 , 0xd0); 
				SP2519_write_cmos_sensor(0xca , 0x04);	  	
#endif
#ifdef SP2519_24M_60M
//24M 2.5pll 8-11.2fps 50HZ Maxgain=0xf8
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x03);
	SP2519_write_cmos_sensor(0x04,0x3c);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0x0a,0xa3);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x8a);
	SP2519_write_cmos_sensor(0xf8,0x73);
	SP2519_write_cmos_sensor(0x02,0x0c);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x8a);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x73);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0xb5);
	SP2519_write_cmos_sensor(0x89,0x73);
	SP2519_write_cmos_sensor(0x8a,0x43);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0x78);
	SP2519_write_cmos_sensor(0xbf,0x06);
	SP2519_write_cmos_sensor(0xd0,0x78);
	SP2519_write_cmos_sensor(0xd1,0x06);
	SP2519_write_cmos_sensor(0xc9,0x78);
	SP2519_write_cmos_sensor(0xca,0x06);
#endif
			}
			else/*capture normal mode 60hz*/
			{
				SENSORDB("Ronlus capture normal mode 60hz\r\n"); /*72M 8~12fps 60hz*/

#ifdef SP2519_24M_72M
				// 3X pll   12~8fps       capture normal mode 60hz                 
				SP2519_write_cmos_sensor(0xfd,0x00);
				SP2519_write_cmos_sensor(0x03,0x02);
				SP2519_write_cmos_sensor(0x04,0xe8);
				SP2519_write_cmos_sensor(0x05,0x00);
				SP2519_write_cmos_sensor(0x06,0x00);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x00);
				SP2519_write_cmos_sensor(0x09,0x01);
				SP2519_write_cmos_sensor(0x0a,0x20);
				SP2519_write_cmos_sensor(0xfd,0x01);
				SP2519_write_cmos_sensor(0xf0,0x00);
				SP2519_write_cmos_sensor(0xf7,0x7c);
				SP2519_write_cmos_sensor(0xf8,0x7c);
				SP2519_write_cmos_sensor(0x02,0x0f);
				SP2519_write_cmos_sensor(0x03,0x01);
				SP2519_write_cmos_sensor(0x06,0x7c);
				SP2519_write_cmos_sensor(0x07,0x00);
				SP2519_write_cmos_sensor(0x08,0x01);
				SP2519_write_cmos_sensor(0x09,0x00);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0x3d,0x0f);
				SP2519_write_cmos_sensor(0x3e,0x7c);
				SP2519_write_cmos_sensor(0x3f,0x00);
				SP2519_write_cmos_sensor(0x88,0x21);
				SP2519_write_cmos_sensor(0x89,0x21);
				SP2519_write_cmos_sensor(0x8a,0x44);
				SP2519_write_cmos_sensor(0xfd,0x02);
				SP2519_write_cmos_sensor(0xbe,0x44);
				SP2519_write_cmos_sensor(0xbf,0x07);
				SP2519_write_cmos_sensor(0xd0,0x44);
				SP2519_write_cmos_sensor(0xd1,0x07);
				SP2519_write_cmos_sensor(0xc9,0x44);
				SP2519_write_cmos_sensor(0xca,0x07);

//#else
				//  2X pll   fix 9.0438fps    capture normal mode 60hz   
				SP2519_write_cmos_sensor(0xfd , 0x00);
				SP2519_write_cmos_sensor(0x03 , 0x02);
				SP2519_write_cmos_sensor(0x04 , 0x2e);
				SP2519_write_cmos_sensor(0x05 , 0x00);
				SP2519_write_cmos_sensor(0x06 , 0x00);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x00);
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0x0a , 0x99);
				SP2519_write_cmos_sensor(0xfd , 0x01);
				SP2519_write_cmos_sensor(0xf0 , 0x00);
				SP2519_write_cmos_sensor(0xf7 , 0x5d);
				SP2519_write_cmos_sensor(0xf8 , 0x5d);
				SP2519_write_cmos_sensor(0x02 , 0x0d);
				SP2519_write_cmos_sensor(0x03 , 0x01);
				SP2519_write_cmos_sensor(0x06 , 0x5d);
				SP2519_write_cmos_sensor(0x07 , 0x00);
				SP2519_write_cmos_sensor(0x08 , 0x01);                              
				SP2519_write_cmos_sensor(0x09 , 0x00);
				SP2519_write_cmos_sensor(0xfd , 0x02);
				SP2519_write_cmos_sensor(0x3d , 0x0d);
				SP2519_write_cmos_sensor(0x3e , 0x5d);
				SP2519_write_cmos_sensor(0x3f , 0x00);
				SP2519_write_cmos_sensor(0x88 , 0x81); 
				SP2519_write_cmos_sensor(0x89 , 0x81); 
				SP2519_write_cmos_sensor(0x8a , 0x55); 
				SP2519_write_cmos_sensor(0xfd , 0x02);                                       
				SP2519_write_cmos_sensor(0xbe , 0xb9);     
				SP2519_write_cmos_sensor(0xbf , 0x04); 
				SP2519_write_cmos_sensor(0xd0 , 0xb9); 
				SP2519_write_cmos_sensor(0xd1 , 0x04); 
				SP2519_write_cmos_sensor(0xc9 , 0xb9); 
				SP2519_write_cmos_sensor(0xca , 0x04);
#endif
#ifdef SP2519_24M_60M
//24M 8-11.2fps 2.5PLL 60HZ 
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x02);
	SP2519_write_cmos_sensor(0x04,0xb8);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0x0a,0x9c);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x74);
	SP2519_write_cmos_sensor(0xf8,0x74);
	SP2519_write_cmos_sensor(0x02,0x0f);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x74);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x74);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x69);
	SP2519_write_cmos_sensor(0x89,0x69);
	SP2519_write_cmos_sensor(0x8a,0x44);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xcc);
	SP2519_write_cmos_sensor(0xbf,0x06);
	SP2519_write_cmos_sensor(0xd0,0xcc);
	SP2519_write_cmos_sensor(0xd1,0x06);
	SP2519_write_cmos_sensor(0xc9,0xcc);
	SP2519_write_cmos_sensor(0xca,0x06);

#endif
			}
		}
	}
} /* SP2519_NightMode */



void SP2519_Sensor_Init(void)
{
     //Kavie 20140305 
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x1b,0x30);
	SP2519_write_cmos_sensor(0x1c,0x17);
	SP2519_write_cmos_sensor(0x30,0x00);
#ifdef SP2519_24M_72M
	SP2519_write_cmos_sensor(0x2f,0x08);
//#else//2pll
	SP2519_write_cmos_sensor(0x2f,0x04);
#endif
#ifdef SP2519_24M_60M
	SP2519_write_cmos_sensor(0x2f,0x11);
#endif
	SP2519_write_cmos_sensor(0x09,0x01);
	SP2519_write_cmos_sensor(0x0c,0x55);
	SP2519_write_cmos_sensor(0x27,0xa5);
	SP2519_write_cmos_sensor(0x1a,0x4b);
	SP2519_write_cmos_sensor(0x20,0x2f);
	SP2519_write_cmos_sensor(0x22,0x5a);
	SP2519_write_cmos_sensor(0x25,0x8f);
	SP2519_write_cmos_sensor(0x21,0x10);
	SP2519_write_cmos_sensor(0x28,0x0b);
	SP2519_write_cmos_sensor(0x1d,0x00);
	SP2519_write_cmos_sensor(0x7a,0x41);
	SP2519_write_cmos_sensor(0x70,0x41);
	SP2519_write_cmos_sensor(0x74,0x40);
	SP2519_write_cmos_sensor(0x75,0x40);
	SP2519_write_cmos_sensor(0x15,0x3e);
	SP2519_write_cmos_sensor(0x71,0x3f);
	SP2519_write_cmos_sensor(0x7c,0x3f);
	SP2519_write_cmos_sensor(0x76,0x3f);
	SP2519_write_cmos_sensor(0x7e,0x29);
	SP2519_write_cmos_sensor(0x72,0x29);
	SP2519_write_cmos_sensor(0x77,0x28);
	SP2519_write_cmos_sensor(0x1e,0x01);
	SP2519_write_cmos_sensor(0x1c,0x1f);
	SP2519_write_cmos_sensor(0x2e,0xcf);
	SP2519_write_cmos_sensor(0x1f,0xf0);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x32,0x00);
	SP2519_write_cmos_sensor(0xfb,0x25);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x85,0x00);
#ifdef SP2519_24M_72M 
	// 3X pll   12~8fps       capture normal mode 50hz                 
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x03);
	SP2519_write_cmos_sensor(0x04,0x7e);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x01);
	SP2519_write_cmos_sensor(0x0a,0x1e);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x95);
	SP2519_write_cmos_sensor(0xf8,0x7c);
	SP2519_write_cmos_sensor(0x02,0x0c);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x95);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x7c);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0x6f);
	SP2519_write_cmos_sensor(0x89,0x21);
	SP2519_write_cmos_sensor(0x8a,0x43);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0xfc);
	SP2519_write_cmos_sensor(0xbf,0x06);
	SP2519_write_cmos_sensor(0xd0,0xfc);
	SP2519_write_cmos_sensor(0xd1,0x06);
	SP2519_write_cmos_sensor(0xc9,0xfc);
	SP2519_write_cmos_sensor(0xca,0x06);
//#else

	// 2X pll   fix 9.0762fps       capture normal mode 50hz     
	SP2519_write_cmos_sensor(0xfd , 0x00);
	SP2519_write_cmos_sensor(0x03 , 0x02);
	SP2519_write_cmos_sensor(0x04 , 0xa0);
	SP2519_write_cmos_sensor(0x05 , 0x00);
	SP2519_write_cmos_sensor(0x06 , 0x00);
	SP2519_write_cmos_sensor(0x07 , 0x00);
	SP2519_write_cmos_sensor(0x08 , 0x00);
	SP2519_write_cmos_sensor(0x09 , 0x00);
	SP2519_write_cmos_sensor(0x0a , 0x95);
	SP2519_write_cmos_sensor(0xfd , 0x01);
	SP2519_write_cmos_sensor(0xf0 , 0x00);
	SP2519_write_cmos_sensor(0xf7 , 0x70);
	SP2519_write_cmos_sensor(0xf8 , 0x5d);
	SP2519_write_cmos_sensor(0x02 , 0x0b);
	SP2519_write_cmos_sensor(0x03 , 0x01);
	SP2519_write_cmos_sensor(0x06 , 0x70);
	SP2519_write_cmos_sensor(0x07 , 0x00);
	SP2519_write_cmos_sensor(0x08 , 0x01);                              
	SP2519_write_cmos_sensor(0x09 , 0x00);
	SP2519_write_cmos_sensor(0xfd , 0x02);
	SP2519_write_cmos_sensor(0x3d , 0x0d);
	SP2519_write_cmos_sensor(0x3e , 0x5d);
	SP2519_write_cmos_sensor(0x3f , 0x00);
	SP2519_write_cmos_sensor(0x88 , 0x92); 
	SP2519_write_cmos_sensor(0x89 , 0x81); 
	SP2519_write_cmos_sensor(0x8a , 0x54); 
	SP2519_write_cmos_sensor(0xfd , 0x02);                                       
	SP2519_write_cmos_sensor(0xbe , 0xd0);     
	SP2519_write_cmos_sensor(0xbf , 0x04); 
	SP2519_write_cmos_sensor(0xd0 , 0xd0); 
	SP2519_write_cmos_sensor(0xd1 , 0x04); 
	SP2519_write_cmos_sensor(0xc9 , 0xd0); 
	SP2519_write_cmos_sensor(0xca , 0x04);
#endif
#ifdef SP2519_24M_60M
//24M 2.5pll 8-11.2fps 50HZ Maxgain=0xf8
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x03,0x03);
	SP2519_write_cmos_sensor(0x04,0x3c);
	SP2519_write_cmos_sensor(0x05,0x00);
	SP2519_write_cmos_sensor(0x06,0x00);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0x0a,0xa3);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf0,0x00);
	SP2519_write_cmos_sensor(0xf7,0x8a);
	SP2519_write_cmos_sensor(0xf8,0x73);
	SP2519_write_cmos_sensor(0x02,0x0c);
	SP2519_write_cmos_sensor(0x03,0x01);
	SP2519_write_cmos_sensor(0x06,0x8a);
	SP2519_write_cmos_sensor(0x07,0x00);
	SP2519_write_cmos_sensor(0x08,0x01);
	SP2519_write_cmos_sensor(0x09,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x3d,0x0f);
	SP2519_write_cmos_sensor(0x3e,0x73);
	SP2519_write_cmos_sensor(0x3f,0x00);
	SP2519_write_cmos_sensor(0x88,0xb5);
	SP2519_write_cmos_sensor(0x89,0x73);
	SP2519_write_cmos_sensor(0x8a,0x43);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xbe,0x78);
	SP2519_write_cmos_sensor(0xbf,0x06);
	SP2519_write_cmos_sensor(0xd0,0x78);
	SP2519_write_cmos_sensor(0xd1,0x06);
	SP2519_write_cmos_sensor(0xc9,0x78);
	SP2519_write_cmos_sensor(0xca,0x06);

#endif
	SP2519_write_cmos_sensor(0xb8,0x90);
	SP2519_write_cmos_sensor(0xb9,0x85);
	SP2519_write_cmos_sensor(0xba,0x30);
	SP2519_write_cmos_sensor(0xbb,0x45);
	SP2519_write_cmos_sensor(0xbc,0xc0);
	SP2519_write_cmos_sensor(0xbd,0x60);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x77,0x48);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xe0,0x48);
	SP2519_write_cmos_sensor(0xe1,0x38);
	SP2519_write_cmos_sensor(0xe2,0x30);
	SP2519_write_cmos_sensor(0xe3,0x2c);
	SP2519_write_cmos_sensor(0xe4,0x2c);
	SP2519_write_cmos_sensor(0xe5,0x2a);
	SP2519_write_cmos_sensor(0xe6,0x2a);
	SP2519_write_cmos_sensor(0xe7,0x28);
	SP2519_write_cmos_sensor(0xe8,0x28);
	SP2519_write_cmos_sensor(0xe9,0x28);
	SP2519_write_cmos_sensor(0xea,0x26);
	SP2519_write_cmos_sensor(0xf3,0x26);
	SP2519_write_cmos_sensor(0xf4,0x26);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x04,0xc0);
	SP2519_write_cmos_sensor(0x05,0x26);
	SP2519_write_cmos_sensor(0x0a,0x48);
	SP2519_write_cmos_sensor(0x0b,0x26);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xf2,0x09);
	SP2519_write_cmos_sensor(0xeb,0x78);
	SP2519_write_cmos_sensor(0xec,0x78);
	SP2519_write_cmos_sensor(0xed,0x06);
	SP2519_write_cmos_sensor(0xee,0x0a);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x52,0xff);
	SP2519_write_cmos_sensor(0x53,0x60);
	SP2519_write_cmos_sensor(0x94,0x20);
	SP2519_write_cmos_sensor(0x54,0x00);
	SP2519_write_cmos_sensor(0x55,0x00);
	SP2519_write_cmos_sensor(0x56,0x80);
	SP2519_write_cmos_sensor(0x57,0x80);
	SP2519_write_cmos_sensor(0x95,0x80);
	SP2519_write_cmos_sensor(0x58,0x00);
	SP2519_write_cmos_sensor(0x59,0x00);
	SP2519_write_cmos_sensor(0x5a,0xf6);
	SP2519_write_cmos_sensor(0x5b,0x00);
	SP2519_write_cmos_sensor(0x5c,0x88);
	SP2519_write_cmos_sensor(0x5d,0x00);
	SP2519_write_cmos_sensor(0x96,0x68);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x8a,0x00);
	SP2519_write_cmos_sensor(0x8b,0x00);
	SP2519_write_cmos_sensor(0x8c,0xff);
	SP2519_write_cmos_sensor(0x22,0xff);
	SP2519_write_cmos_sensor(0x23,0xff);
	SP2519_write_cmos_sensor(0x24,0xff);
	SP2519_write_cmos_sensor(0x25,0xff);
	SP2519_write_cmos_sensor(0x5e,0xff);
	SP2519_write_cmos_sensor(0x5f,0xff);
	SP2519_write_cmos_sensor(0x60,0xff);
	SP2519_write_cmos_sensor(0x61,0xff);
	SP2519_write_cmos_sensor(0x62,0x00);
	SP2519_write_cmos_sensor(0x63,0x00);
	SP2519_write_cmos_sensor(0x64,0x00);
	SP2519_write_cmos_sensor(0x65,0x00);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x21,0x00);
	SP2519_write_cmos_sensor(0x22,0x00);
	SP2519_write_cmos_sensor(0x26,0x60);
	SP2519_write_cmos_sensor(0x27,0x14);
	SP2519_write_cmos_sensor(0x28,0x05);
	SP2519_write_cmos_sensor(0x29,0x00);
	SP2519_write_cmos_sensor(0x2a,0x01);

	SP2519_write_cmos_sensor(0xfd,0x01);                                                                  
	SP2519_write_cmos_sensor(0xa1,0x1f);                                                                  
	SP2519_write_cmos_sensor(0xa2,0x1b);                                                                  
	SP2519_write_cmos_sensor(0xa3,0x25);                                                                  
	SP2519_write_cmos_sensor(0xa4,0x1d);                                                                  
	SP2519_write_cmos_sensor(0xa5,0x19);                                                                  
	SP2519_write_cmos_sensor(0xa6,0x15);                                                                  
	SP2519_write_cmos_sensor(0xa7,0x1F);                                                                  
	SP2519_write_cmos_sensor(0xa8,0x15);                                                                  
	SP2519_write_cmos_sensor(0xa9,0x17);                                                                  
	SP2519_write_cmos_sensor(0xaa,0x13);                                                                  
	SP2519_write_cmos_sensor(0xab,0x1A);                                                                  
	SP2519_write_cmos_sensor(0xac,0x14);                                                                  
	SP2519_write_cmos_sensor(0xad,0x02);                                                                  
	SP2519_write_cmos_sensor(0xae,0x08);                                                                  
	SP2519_write_cmos_sensor(0xaf,0x02);                                                                  
	SP2519_write_cmos_sensor(0xb0,0x04);                                                                  
	SP2519_write_cmos_sensor(0xb1,0x05);                                                                  
	SP2519_write_cmos_sensor(0xb2,0x07);                                                                  
	SP2519_write_cmos_sensor(0xb3,0x05);                                                                  
	SP2519_write_cmos_sensor(0xb4,0x04);                                                                  
	SP2519_write_cmos_sensor(0xb5,0x08);                                                                  
	SP2519_write_cmos_sensor(0xb6,0x08);                                                                  
	SP2519_write_cmos_sensor(0xb7,0x09);                                                                  
	SP2519_write_cmos_sensor(0xb8,0x08);                                                                   

	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x26,0xa0);
	SP2519_write_cmos_sensor(0x27,0x96);
	SP2519_write_cmos_sensor(0x28,0xcc);
	SP2519_write_cmos_sensor(0x29,0x01);
	SP2519_write_cmos_sensor(0x2a,0x00);
	SP2519_write_cmos_sensor(0x2b,0x00);
	SP2519_write_cmos_sensor(0x2c,0x20);
	SP2519_write_cmos_sensor(0x2d,0xdc);
	SP2519_write_cmos_sensor(0x2e,0x20);
	SP2519_write_cmos_sensor(0x2f,0x96);
	SP2519_write_cmos_sensor(0x1b,0x80);
	SP2519_write_cmos_sensor(0x1a,0x80);
	SP2519_write_cmos_sensor(0x18,0x16);
	SP2519_write_cmos_sensor(0x19,0x26);
	SP2519_write_cmos_sensor(0x1d,0x04); 
	SP2519_write_cmos_sensor(0x1f,0x06);

	SP2519_write_cmos_sensor(0x66,0x30);
	SP2519_write_cmos_sensor(0x67,0x50);
	SP2519_write_cmos_sensor(0x68,0xbb);
	SP2519_write_cmos_sensor(0x69,0xdf);
	SP2519_write_cmos_sensor(0x6a,0xa5);

	SP2519_write_cmos_sensor(0x7c,0x0d);
	SP2519_write_cmos_sensor(0x7d,0x2f);
	SP2519_write_cmos_sensor(0x7e,0xe0);
	SP2519_write_cmos_sensor(0x7f,0x05);
	SP2519_write_cmos_sensor(0x80,0xa6);

	SP2519_write_cmos_sensor(0x70,0x0c);
	SP2519_write_cmos_sensor(0x71,0x28);
	SP2519_write_cmos_sensor(0x72,0x0b);
	SP2519_write_cmos_sensor(0x73,0x28);
	SP2519_write_cmos_sensor(0x74,0xaa);

	SP2519_write_cmos_sensor(0x6b,0x00);
	SP2519_write_cmos_sensor(0x6c,0x0b);
	SP2519_write_cmos_sensor(0x6d,0x10);
	SP2519_write_cmos_sensor(0x6e,0x24);
	SP2519_write_cmos_sensor(0x6f,0xaa);

	SP2519_write_cmos_sensor(0x61,0xd2);
	SP2519_write_cmos_sensor(0x62,0xf0);
	SP2519_write_cmos_sensor(0x63,0x30);
	SP2519_write_cmos_sensor(0x64,0x4d);
	SP2519_write_cmos_sensor(0x65,0x5a);

	SP2519_write_cmos_sensor(0x75,0x00);
	SP2519_write_cmos_sensor(0x76,0x09);
	SP2519_write_cmos_sensor(0x77,0x02);
	SP2519_write_cmos_sensor(0x0e,0x16);
	SP2519_write_cmos_sensor(0x3b,0x09);

	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x02,0x00);
	SP2519_write_cmos_sensor(0x03,0x63);

	SP2519_write_cmos_sensor(0x04,0xf0);

	SP2519_write_cmos_sensor(0xf5,0x9a);
	SP2519_write_cmos_sensor(0xf6,0x70);
	SP2519_write_cmos_sensor(0xf7,0xa0);
	SP2519_write_cmos_sensor(0xf8,0x70);

	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x08,0x00);
	SP2519_write_cmos_sensor(0x09,0x04);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xdd,0x0f);
	SP2519_write_cmos_sensor(0xde,0x0f);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x57,0x30);
	SP2519_write_cmos_sensor(0x58,0x10);
	SP2519_write_cmos_sensor(0x59,0xe0);
	SP2519_write_cmos_sensor(0x5a,0x00);
	SP2519_write_cmos_sensor(0x5b,0x0d);

	SP2519_write_cmos_sensor(0xcb,0x04);
	SP2519_write_cmos_sensor(0xcc,0x0b);
	SP2519_write_cmos_sensor(0xcd,0x10);
	SP2519_write_cmos_sensor(0xce,0x1a);

	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x87,0x04);
	SP2519_write_cmos_sensor(0x88,0x08);
	SP2519_write_cmos_sensor(0x89,0x10);
	SP2519_write_cmos_sensor(0xfd,0x02);

	SP2519_write_cmos_sensor(0xe8,0x68);
	SP2519_write_cmos_sensor(0xec,0x78);
	SP2519_write_cmos_sensor(0xe9,0x68);
	SP2519_write_cmos_sensor(0xed,0x78);
	SP2519_write_cmos_sensor(0xea,0x68);
	SP2519_write_cmos_sensor(0xee,0x70);
	SP2519_write_cmos_sensor(0xeb,0x58);
	SP2519_write_cmos_sensor(0xef,0x50);

	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xdc,0x04);
	SP2519_write_cmos_sensor(0x05,0x6f);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xf4,0x30);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x97,0x98);
	SP2519_write_cmos_sensor(0x98,0x88);
	SP2519_write_cmos_sensor(0x99,0x88);
	SP2519_write_cmos_sensor(0x9a,0x80);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xe4,0xff);
	SP2519_write_cmos_sensor(0xe5,0xff);
	SP2519_write_cmos_sensor(0xe6,0xff);
	SP2519_write_cmos_sensor(0xe7,0xff);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x72,0x10);
	SP2519_write_cmos_sensor(0x73,0x28);
	SP2519_write_cmos_sensor(0x74,0x28);
	SP2519_write_cmos_sensor(0x75,0x30);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x78,0x20);
	SP2519_write_cmos_sensor(0x79,0x20);
	SP2519_write_cmos_sensor(0x7a,0x14);
	SP2519_write_cmos_sensor(0x7b,0x08);
	SP2519_write_cmos_sensor(0x81,0x02);
	SP2519_write_cmos_sensor(0x82,0x20);
	SP2519_write_cmos_sensor(0x83,0x20);
	SP2519_write_cmos_sensor(0x84,0x08);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x7e,0x03);
	SP2519_write_cmos_sensor(0x7f,0x0d);
	SP2519_write_cmos_sensor(0x80,0x10);
	SP2519_write_cmos_sensor(0x81,0x16);
	SP2519_write_cmos_sensor(0x7c,0xff);
	SP2519_write_cmos_sensor(0x82,0x64);
	SP2519_write_cmos_sensor(0x83,0x43);
	SP2519_write_cmos_sensor(0x84,0x00);
	SP2519_write_cmos_sensor(0x85,0x20);
	SP2519_write_cmos_sensor(0x86,0x40);
	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x66,0x18);
	SP2519_write_cmos_sensor(0x67,0x28);
	SP2519_write_cmos_sensor(0x68,0x20);
	SP2519_write_cmos_sensor(0x69,0x88);
	SP2519_write_cmos_sensor(0x9b,0x18);
	SP2519_write_cmos_sensor(0x9c,0x28);
	SP2519_write_cmos_sensor(0x9d,0x20);   

	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x8b,0x00); 
	SP2519_write_cmos_sensor(0x8c,0x0d); 
	SP2519_write_cmos_sensor(0x8d,0x19); 
	SP2519_write_cmos_sensor(0x8e,0x26); 
	SP2519_write_cmos_sensor(0x8f,0x30); 
	SP2519_write_cmos_sensor(0x90,0x43); 
	SP2519_write_cmos_sensor(0x91,0x53); 
	SP2519_write_cmos_sensor(0x92,0x5e); 
	SP2519_write_cmos_sensor(0x93,0x6a); 
	SP2519_write_cmos_sensor(0x94,0x7d); 
	SP2519_write_cmos_sensor(0x95,0x8d); 
	SP2519_write_cmos_sensor(0x96,0x9e); 
	SP2519_write_cmos_sensor(0x97,0xac); 
	SP2519_write_cmos_sensor(0x98,0xba); 
	SP2519_write_cmos_sensor(0x99,0xc6); 
	SP2519_write_cmos_sensor(0x9a,0xd1); 
	SP2519_write_cmos_sensor(0x9b,0xda); 
	SP2519_write_cmos_sensor(0x9c,0xe4); 
	SP2519_write_cmos_sensor(0x9d,0xeb); 
	SP2519_write_cmos_sensor(0x9e,0xf2); 
	SP2519_write_cmos_sensor(0x9f,0xf9); 
	SP2519_write_cmos_sensor(0xa0,0xff); 
 
	SP2519_write_cmos_sensor(0xfd,0x02); 
	SP2519_write_cmos_sensor(0x15,0xa9); 
	SP2519_write_cmos_sensor(0x16,0x84); 

	SP2519_write_cmos_sensor(0xa0,0x97); 
	SP2519_write_cmos_sensor(0xa1,0xea); 
	SP2519_write_cmos_sensor(0xa2,0xff); 
	SP2519_write_cmos_sensor(0xa3,0x0e); 
	SP2519_write_cmos_sensor(0xa4,0x78); 
	SP2519_write_cmos_sensor(0xa5,0xfa); 
	SP2519_write_cmos_sensor(0xa6,0x08); 
	SP2519_write_cmos_sensor(0xa7,0xcb); 
	SP2519_write_cmos_sensor(0xa8,0xad); 
	SP2519_write_cmos_sensor(0xa9,0x3c); 
	SP2519_write_cmos_sensor(0xaa,0x30); 
	SP2519_write_cmos_sensor(0xab,0x0c); 

	SP2519_write_cmos_sensor(0xac,0x7f);
	SP2519_write_cmos_sensor(0xad,0x08);
	SP2519_write_cmos_sensor(0xae,0xf8);
	SP2519_write_cmos_sensor(0xaf,0xff);
	SP2519_write_cmos_sensor(0xb0,0x6e);
	SP2519_write_cmos_sensor(0xb1,0x13);
	SP2519_write_cmos_sensor(0xb2,0xd2);
	SP2519_write_cmos_sensor(0xb3,0x6e);
	SP2519_write_cmos_sensor(0xb4,0x40);
	SP2519_write_cmos_sensor(0xb5,0x30);
	SP2519_write_cmos_sensor(0xb6,0x03);
	SP2519_write_cmos_sensor(0xb7,0x1f);

	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xd2,0x2d);
	SP2519_write_cmos_sensor(0xd1,0x38);
	SP2519_write_cmos_sensor(0xdd,0x3f);
	SP2519_write_cmos_sensor(0xde,0x37);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xc1,0x40);
	SP2519_write_cmos_sensor(0xc2,0x40);
	SP2519_write_cmos_sensor(0xc3,0x40);
	SP2519_write_cmos_sensor(0xc4,0x40);
	SP2519_write_cmos_sensor(0xc5,0x80);
	SP2519_write_cmos_sensor(0xc6,0x60);
	SP2519_write_cmos_sensor(0xc7,0x00);
	SP2519_write_cmos_sensor(0xc8,0x00);  

	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xd3,0xa0);//0x98 20140306
	SP2519_write_cmos_sensor(0xd4,0x98);//0x90  20140306
	SP2519_write_cmos_sensor(0xd5,0x88);//0x80  20140306
	SP2519_write_cmos_sensor(0xd6,0x88);//0x80  20140306

	SP2519_write_cmos_sensor(0xd7,0xa0);//0x98  20140306
	SP2519_write_cmos_sensor(0xd8,0x98);//0x90  20140306
	SP2519_write_cmos_sensor(0xd9,0x88);//0x80  20140306
	SP2519_write_cmos_sensor(0xda,0x88);//0x80  20140306

	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x76,0x0a);
	SP2519_write_cmos_sensor(0x7a,0x40);
	SP2519_write_cmos_sensor(0x7b,0x40);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xc2,0xaa);
	SP2519_write_cmos_sensor(0xc3,0xaa);
	SP2519_write_cmos_sensor(0xc4,0x66);
	SP2519_write_cmos_sensor(0xc5,0x66);

	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xcd,0x08);
	SP2519_write_cmos_sensor(0xce,0x18);

	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x32,0x60);
	SP2519_write_cmos_sensor(0x35,0x60);
	SP2519_write_cmos_sensor(0x37,0x13);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0xdb,0x00);

	SP2519_write_cmos_sensor(0x10,0x88);
	SP2519_write_cmos_sensor(0x11,0x88);
	SP2519_write_cmos_sensor(0x12,0x88);
	SP2519_write_cmos_sensor(0x13,0x8a);

	SP2519_write_cmos_sensor(0x14,0xba);
	SP2519_write_cmos_sensor(0x15,0xba);
	SP2519_write_cmos_sensor(0x16,0xa8);
	SP2519_write_cmos_sensor(0x17,0xa0);

	SP2519_write_cmos_sensor(0xfd,0x03);
	SP2519_write_cmos_sensor(0x00,0x80);
	SP2519_write_cmos_sensor(0x03,0x68);
	SP2519_write_cmos_sensor(0x06,0xd8);
	SP2519_write_cmos_sensor(0x07,0x28);
	SP2519_write_cmos_sensor(0x0a,0xfd);
	SP2519_write_cmos_sensor(0x01,0x16);
	SP2519_write_cmos_sensor(0x02,0x16);
	SP2519_write_cmos_sensor(0x04,0x16);
	SP2519_write_cmos_sensor(0x05,0x16);
	SP2519_write_cmos_sensor(0x0b,0x40);
	SP2519_write_cmos_sensor(0x0c,0x50);
	SP2519_write_cmos_sensor(0x0d,0x50);
	SP2519_write_cmos_sensor(0x0e,0x50);
	SP2519_write_cmos_sensor(0x08,0x0c);
	SP2519_write_cmos_sensor(0x09,0x0c);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x8e,0x0a);
	SP2519_write_cmos_sensor(0x90,0x40);
	SP2519_write_cmos_sensor(0x91,0x40);
	SP2519_write_cmos_sensor(0x92,0x60);
	SP2519_write_cmos_sensor(0x93,0x80);
	SP2519_write_cmos_sensor(0x9e,0x44);
	SP2519_write_cmos_sensor(0x9f,0x44);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x85,0x00);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x00,0x00);
	SP2519_write_cmos_sensor(0xfb,0x25);
	SP2519_write_cmos_sensor(0x32,0x15);
	SP2519_write_cmos_sensor(0x33,0xef);
	SP2519_write_cmos_sensor(0x34,0xef);

	SP2519_write_cmos_sensor(0x35,0x40);
	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x3f,0x03);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x50,0x00);
	SP2519_write_cmos_sensor(0x66,0x00);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xd6,0x0f);
	SP2519_write_cmos_sensor(0xfd,0x00);


	SP2519_write_cmos_sensor(0xfd,0x00);
	SP2519_write_cmos_sensor(0x1b,0x30);
	SP2519_write_cmos_sensor(0x1d,0x00);
	SP2519_write_cmos_sensor(0xfd,0x01);
	SP2519_write_cmos_sensor(0x36,0x00);

	SP2519_write_cmos_sensor(0xfd,0x02);

	SP2519_write_cmos_sensor(0x40,0x00);
	SP2519_write_cmos_sensor(0x41,0x40);
	SP2519_write_cmos_sensor(0x42,0x00);
	SP2519_write_cmos_sensor(0x43,0x40);
	SP2519_write_cmos_sensor(0x44,0x02);
	SP2519_write_cmos_sensor(0x45,0x58);
	SP2519_write_cmos_sensor(0x46,0x03);
	SP2519_write_cmos_sensor(0x47,0x20);
	SP2519_write_cmos_sensor(0x0f,0x01);
	SP2519_write_cmos_sensor(0x8f,0x02);
}


void SP2519_Write_More_Registers(void)
{
	//Ronlus this function is used for FAE debug
}



static void SP2519_PV_setting(void)
{
	SENSORDB("Ronlus SP2519_PV_setting\r\n");
} /* SP2519_PV_setting */


static void SP2519_CAP_setting(void)
{
	kal_uint16 corr_r_offset,corr_g_offset,corr_b_offset,temp = 0;
	SENSORDB("Ronlus SP2519_CAP_setting\r\n");	
} /* SP2519_CAP_setting */
UINT32 SP2519Open(void)
{
	kal_uint16 sensor_id=0;
	int i;
	SENSORDB("Ronlus SP2519Open\r\n");
	// check if sensor ID correct
	for(i = 0; i < 3; i++)
	{
		SP2519_write_cmos_sensor(0xfd,0x00);
		sensor_id = SP2519_read_cmos_sensor(0x02);
		SENSORDB("Ronlus SP2519 Sensor id = %x\n", sensor_id);
		if (sensor_id == SP2519_SENSOR_ID)
		{
			break;
		}
	}
	mdelay(50);
	if(sensor_id != SP2519_SENSOR_ID)
	{
		SENSORDB("SP2519 Sensor id read failed, ID = %x\n", sensor_id);
		return ERROR_SENSOR_CONNECT_FAIL;
	}

#ifdef DEBUG_SENSOR_SP2519  //gepeiwei   120903
	//判断手机对应目录下是否有名为sp2528_sd 的文件,没有默认参数

	//介于各种原因，本版本初始化参数在_s_fmt中。
	struct file *fp; 
	mm_segment_t fs; 
	loff_t pos = 0; 
	static char buf[10*1024] ;

	fp = filp_open("/mnt/sdcard/sp2519_sd", O_RDONLY , 0); 
	if (IS_ERR(fp)) { 
		fromsd = 0;   
		printk("open file error\n");

	} 
	else 
	{
		fromsd = 1;
		printk("open file ok\n");

		//SP2519_Initialize_from_T_Flash();


		filp_close(fp, NULL); 
		set_fs(fs);
	}

	if(fromsd == 1)//是否从SD读取//gepeiwei   120903
	{
		printk("________________from t!\n");
		SP2519_Initialize_from_T_Flash();//从SD卡读取的主要函数
	}
	else
	{
		SP2519_Sensor_Init();
		SP2519_Write_More_Registers();//added for FAE to debut
	}
#else  
	//RETAILMSG(1, (TEXT("Sensor Read ID OK \r\n")));
	// initail sequence write in
	SP2519_Sensor_Init();
	SP2519_Write_More_Registers();//added for FAE to debut
#endif
	return ERROR_NONE;
} /* SP2519Open */


UINT32 SP2519Close(void)
{
	SENSORDB("Ronlus SP2519Close\r\n");
	return ERROR_NONE;
} /* SP2519Close */


UINT32 SP2519Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	kal_uint32 iTemp;
	kal_uint16 iStartX = 0, iStartY = 1;
	SENSORDB("Ronlus SP2519Preview fun start\r\n");
	setshutter = KAL_FALSE;
	if(sensor_config_data->SensorOperationMode == MSDK_SENSOR_OPERATION_MODE_VIDEO)		// MPEG4 Encode Mode
	{
		SENSORDB("Ronlus video preview\r\n");
		SP2519_MPEG4_encode_mode = KAL_TRUE;
	}
	else
	{
		SENSORDB("Ronlus capture preview\r\n");
		SP2519_MPEG4_encode_mode = KAL_FALSE;
	}
	//SP2519_config_window(WINDOW_SIZE_SVGA);//add zch test(use this for SVGA)
#ifdef DEBUG_SENSOR_SP2519
	SP2519_Preview_from_T_Flash();//add zch 2013-11-26
#else
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xcc,0x0b);
	SP2519_write_cmos_sensor(0xcd,0x10);
	SP2519_write_cmos_sensor(0xe8,0x68);
	SP2519_write_cmos_sensor(0xec,0x78);
	SP2519_write_cmos_sensor(0xe9,0x78);
	SP2519_write_cmos_sensor(0xed,0x88);
	SP2519_write_cmos_sensor(0xea,0x78);
	SP2519_write_cmos_sensor(0xee,0x80);
	SP2519_write_cmos_sensor(0xeb,0x68);
	SP2519_write_cmos_sensor(0xef,0x60);
	
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x0f,0x01);
	SP2519_write_cmos_sensor(0x57,0x40);
#endif	
	
	image_window->GrabStartX= 1;
	image_window->GrabStartY= 1;
	image_window->ExposureWindowWidth = 800-8;//IMAGE_SENSOR_FULL_WIDTH; //modify by sp_yjp,20130918
	image_window->ExposureWindowHeight =600-6;//IMAGE_SENSOR_FULL_HEIGHT;//modify by sp_yjp,20130918
	// copy sensor_config_data
	memcpy(&SP2519SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));//rotation
	SENSORDB("Ronlus SP2519Preview fun end\r\n");
	return ERROR_NONE;
} /* SP2519Preview */


UINT32 SP2519Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	SENSORDB("Ronlus SP2519Capture fun start\r\n");
	SP2519_MODE_CAPTURE=KAL_TRUE;

#if 1
	if ((image_window->ImageTargetWidth<=IMAGE_SENSOR_SVGA_WIDTH)&&
			(image_window->ImageTargetHeight<=IMAGE_SENSOR_SVGA_HEIGHT))
	{    /* Less than PV Mode */
		image_window->GrabStartX = IMAGE_SENSOR_SVGA_GRAB_PIXELS;
		image_window->GrabStartY = IMAGE_SENSOR_SVGA_GRAB_LINES;
		image_window->ExposureWindowWidth= IMAGE_SENSOR_PV_WIDTH;
		image_window->ExposureWindowHeight = IMAGE_SENSOR_PV_HEIGHT;
	}
	else
#endif

	{
		kal_uint32 shutter, cap_dummy_pixels = 0; 
		if(!setshutter)
		{
			//SP2519_ae_enable(KAL_FALSE);
			//SP2519_awb_enable(KAL_FALSE);
			//shutter = SP2519_Read_Shutter();
			//SP2519_CAP_setting();
			//SP2519_set_hb_shutter(cap_dummy_pixels, shutter);
		}	
		//SP2519_config_window(WINDOW_SIZE_UXGA);//add zch test(user this for SVGA)
		image_window->GrabStartX = IMAGE_SENSOR_UXGA_GRAB_PIXELS;
		image_window->GrabStartY = IMAGE_SENSOR_UXGA_GRAB_LINES;
		image_window->ExposureWindowWidth= IMAGE_SENSOR_FULL_WIDTH;
		image_window->ExposureWindowHeight = IMAGE_SENSOR_FULL_HEIGHT;
	}
#ifdef DEBUG_SENSOR_SP2519
	SP2519_Capture_from_T_Flash();//add zch 2013-11-26
#else
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0xcb,0x04);
	SP2519_write_cmos_sensor(0xcc,0x1a); 
	SP2519_write_cmos_sensor(0xcd,0x1a);
	SP2519_write_cmos_sensor(0xce,0x1a);
	SP2519_write_cmos_sensor(0xe8,0x58);
	SP2519_write_cmos_sensor(0xec,0x68);
	SP2519_write_cmos_sensor(0xe9,0x58);
	SP2519_write_cmos_sensor(0xed,0x60);
	SP2519_write_cmos_sensor(0xea,0x50);
	SP2519_write_cmos_sensor(0xee,0x58);
	SP2519_write_cmos_sensor(0xeb,0x48);
	SP2519_write_cmos_sensor(0xef,0x40);
	SP2519_write_cmos_sensor(0xfd,0x02);
	SP2519_write_cmos_sensor(0x0f,0x00);
	SP2519_write_cmos_sensor(0x57,0x40);
#endif
	// copy sensor_config_data
	memcpy(&SP2519SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	SENSORDB("Ronlus SP2519Capture fun end\r\n");
	return ERROR_NONE;
} /* SP2519_Capture() */



UINT32 SP2519GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
	SENSORDB("Ronlus SP2519GetResolution\r\n");
	pSensorResolution->SensorFullWidth=1600-8;
	pSensorResolution->SensorFullHeight=1200-6;
	pSensorResolution->SensorPreviewWidth=800-8;//IMAGE_SENSOR_FULL_WIDTH;//IMAGE_SENSOR_SVGA_WIDTH;(user this for SVGA)
	pSensorResolution->SensorPreviewHeight=600-6;//IMAGE_SENSOR_FULL_HEIGHT;//IMAGE_SIMAGE_SENSOR_SVGA_HEIGHT;(use this for SVGA)
	pSensorResolution->SensorVideoWidth =800-8;//IMAGE_SENSOR_FULL_WIDTH;//IMAGE_SENSOR_SVGA_WIDTH;(user this for SVGA)
	pSensorResolution->SensorVideoHeight=600-6;//IMAGE_SENSOR_FULL_HEIGHT;//IMAGE_SIMAGE_SENSOR_SVGA_HEIGHT;(use this for SVGA)
	return ERROR_NONE;
} /* SP2519GetResolution() */


UINT32 SP2519GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
		MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
		MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	SENSORDB("Ronlus SP2519GetInfo\r\n");
	pSensorInfo->SensorPreviewResolutionX=800-8;//IMAGE_SENSOR_UXGA_WIDTH;//IMAGE_SENSOR_PV_WIDTH(use this for SVGA);
	pSensorInfo->SensorPreviewResolutionY=600-6;//IMAGE_SENSOR_UXGA_HEIGHT;//IMAGE_SENSOR_PV_HEIGHT(user this for SVGA);
	pSensorInfo->SensorFullResolutionX=1600-8;
	pSensorInfo->SensorFullResolutionY=1200-6;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=10;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=1;
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_PARALLEL;

	pSensorInfo->CaptureDelayFrame = 3;
	pSensorInfo->PreviewDelayFrame = 1;
	pSensorInfo->VideoDelayFrame = 4;
	pSensorInfo->SensorMasterClockSwitch = 0;
	pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_2MA;

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_SVGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_SVGA_GRAB_LINES;

			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			//  case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount= 3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_UXGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_UXGA_GRAB_LINES;
			break;
		default:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount= 3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_SVGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_SVGA_GRAB_LINES;
			break;
	}
	SP2519PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &SP2519SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
} /* SP2519GetInfo() */


UINT32 SP2519Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
		MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	SENSORDB("Ronlus SP2519Control\r\n");
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			SP2519Preview(pImageWindow, pSensorConfigData);
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			//case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			SP2519Capture(pImageWindow, pSensorConfigData);
			break;
	}


	return TRUE;
}	/* SP2519Control() */

BOOL SP2519_set_param_wb(UINT16 para)
{
	SENSORDB("Ronlus SP2519_set_param_wb\r\n");
	switch (para)
	{
		case AWB_MODE_OFF:
			//SP2519_write_cmos_sensor(0xfd,0x00);
			// SP2519_write_cmos_sensor(0x32,0x05);
			break; 					
		case AWB_MODE_AUTO:

			break;
		case AWB_MODE_CLOUDY_DAYLIGHT: //cloudy

			break;
		case AWB_MODE_DAYLIGHT: //sunny
	
			break;
		case AWB_MODE_INCANDESCENT: //office

			break;
		case AWB_MODE_TUNGSTEN: //home

			break;
		case AWB_MODE_FLUORESCENT:

		default:
			return FALSE;
	}  
	return TRUE;
} /* SP2519_set_param_wb */


BOOL SP2519_set_param_effect(UINT16 para)
{
	SENSORDB("Ronlus SP2519_set_param_effect\r\n");
	switch (para)
	{
		case MEFFECT_OFF:

			break;
		case MEFFECT_SEPIA:

			break;
		case MEFFECT_NEGATIVE://----datasheet
	
			break;
		case MEFFECT_SEPIAGREEN://----datasheet aqua
	
			break;
		case MEFFECT_SEPIABLUE:

			break;
		case MEFFECT_MONO: //----datasheet black & white
		
			break;
		default:
			return FALSE;
	}
	return TRUE;
} /* SP2519_set_param_effect */

UINT8 index = 1;
BOOL SP2519_set_param_banding(UINT16 para)
{
	//UINT16 buffer = 0;
	SENSORDB("Ronlus SP2519_set_param_banding para = %d ---- index = %d\r\n",para,index); 
	SENSORDB("Ronlus SP2519_set_param_banding ---- SP2519_MPEG4_encode_mode = %d\r\n",SP2519_MPEG4_encode_mode);
	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
			SP2519_CAM_BANDING_50HZ = KAL_TRUE;
			break;
		case AE_FLICKER_MODE_60HZ:
			SP2519_CAM_BANDING_50HZ = KAL_FALSE;
			break;
		default:
			SP2519_CAM_BANDING_50HZ = KAL_TRUE;
			return TRUE;
	}

	return TRUE;
} /* SP2519_set_param_banding */


BOOL SP2519_set_param_exposure(UINT16 para)
{
	SENSORDB("Ronlus SP2519_set_param_exposure\r\n");
	switch (para)
	{
		case AE_EV_COMP_n13:              /* EV -2 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0xe0);
			break;
		case AE_EV_COMP_n10:              /* EV -1.5 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0xe8);
			break;
		case AE_EV_COMP_n07:              /* EV -1 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0xf0);
			break;
		case AE_EV_COMP_n03:              /* EV -0.5 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0xf8);
			break;
		case AE_EV_COMP_00:                /* EV 0 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0x00);
			break;
		case AE_EV_COMP_03:              /* EV +0.5 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0x08);			
			break;
		case AE_EV_COMP_07:              /* EV +1 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0x10);
			break;
		case AE_EV_COMP_10:              /* EV +1.5 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0x18);
			break;
		case AE_EV_COMP_13:              /* EV +2 */
			SP2519_write_cmos_sensor(0xfd,0x01);
			SP2519_write_cmos_sensor(0xdb,0x20);
			break;
		default:
			return FALSE;
	}
	return TRUE;
} /* SP2519_set_param_exposure */


UINT32 SP2519YUVSensorSetting(FEATURE_ID iCmd, UINT16 iPara)
{

#ifdef DEBUG_SENSOR_SP2519
	return TRUE;
#endif
	SENSORDB("Ronlus SP2519YUVSensorSetting\r\n");
	switch (iCmd) 
	{
		case FID_SCENE_MODE:	    
			if (iPara == SCENE_MODE_OFF)
			{
				SP2519_night_mode(0); 
			}
			else if (iPara == SCENE_MODE_NIGHTSCENE)
			{
				SP2519_night_mode(1); 
			}	    
			break; 	    
		case FID_AWB_MODE:
			SP2519_set_param_wb(iPara);
			break;
		case FID_COLOR_EFFECT:
			SP2519_set_param_effect(iPara);
			break;
		case FID_AE_EV:
			SP2519_set_param_exposure(iPara);
			break;
		case FID_AE_FLICKER:
			SP2519_set_param_banding(iPara);
			SP2519_night_mode(SP2519_CAM_Nightmode); 
			break;
		default:
			break;
	}
	return TRUE;
}
/* SP2519YUVSensorSetting */


UINT32 sp2519_get_sensor_id(UINT32 *sensorID) 
{
	volatile signed char i;
	kal_uint16 sensor_id=0;
	SENSORDB("xieyang SP2519GetSensorID ");
	//SENSORDB("xieyang in GPIO_CAMERA_CMPDN_PIN=%d,GPIO_CAMERA_CMPDN1_PIN=%d", 
	//	mt_get_gpio_out(GPIO_CAMERA_CMPDN_PIN),mt_get_gpio_out(GPIO_CAMERA_CMPDN1_PIN));

	for(i=0;i<3;i++)
	{
		SP2519_write_cmos_sensor(0xfd, 0x00); 
		sensor_id = SP2519_read_cmos_sensor(0x02);
		SENSORDB("%s sensor_id=%d\n", __func__, sensor_id);

		if (sensor_id == SP2519_SENSOR_ID)
		{
			break;
		}
	}

	if(sensor_id != SP2519_SENSOR_ID)
	{
		*sensorID = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	else
	{
		*sensorID = SP2519_SENSOR_ID;
	}

	return ERROR_NONE;
}


UINT32 SP2519FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
		UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	UINT32 SP2519SensorRegNumber;
	UINT32 i;
	SENSORDB("Ronlus SP2519FeatureControl.---FeatureId = %d\r\n",FeatureId);
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

	RETAILMSG(1, (_T("gaiyang SP2519FeatureControl FeatureId=%d\r\n"), FeatureId));

	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=UXGA_PERIOD_PIXEL_NUMS;
			*pFeatureReturnPara16=UXGA_PERIOD_LINE_NUMS;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=(SVGA_PERIOD_PIXEL_NUMS)+SP2519_dummy_pixels;
			*pFeatureReturnPara16=(SVGA_PERIOD_LINE_NUMS)+SP2519_dummy_lines;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			*pFeatureReturnPara32 = SP2519_g_fPV_PCLK;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_SET_ESHUTTER:
			break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
#ifndef DEBUG_SENSOR_SP2519		
			SP2519_night_mode((BOOL) *pFeatureData16);
#endif
			break;
		case SENSOR_FEATURE_SET_GAIN:
		case SENSOR_FEATURE_SET_FLASHLIGHT:
			break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			SP2519_isp_master_clock=*pFeatureData32;
			break;
		case SENSOR_FEATURE_SET_REGISTER:
			SP2519_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
			break;
		case SENSOR_FEATURE_GET_REGISTER:
			pSensorRegData->RegData = SP2519_read_cmos_sensor(pSensorRegData->RegAddr);
			break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			memcpy(pSensorConfigData, &SP2519SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			*pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
			break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		case SENSOR_FEATURE_GET_GROUP_COUNT:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
			break;
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			sp2519_get_sensor_id(pFeatureReturnPara32); 
			break; 
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_SET_YUV_CMD:
			SP2519YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
			break;
		default:
			break;
	}
	return ERROR_NONE;
}	/* SP2519FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncSP2519YUV=
{
	SP2519Open,
	SP2519GetInfo,
	SP2519GetResolution,
	SP2519FeatureControl,
	SP2519Control,
	SP2519Close
};


UINT32 SP2519_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	SENSORDB("Ronlus SP2519_YUV_SensorInit\r\n");
	if (pfFunc!=NULL)
	{
		SENSORDB("Ronlus SP2519_YUV_SensorInit fun_config success\r\n");
		*pfFunc=&SensorFuncSP2519YUV;
	}
	return ERROR_NONE;
} /* SensorInit() */
