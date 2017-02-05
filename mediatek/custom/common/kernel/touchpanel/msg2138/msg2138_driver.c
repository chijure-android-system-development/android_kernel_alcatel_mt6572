#include "tpd.h"
#include <linux/interrupt.h>
#include <cust_eint.h>
#include <linux/i2c.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/rtpm_prio.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>

#include <linux/dma-mapping.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <linux/vmalloc.h>


#include "tpd_custom_msg2138.h"

#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>

#include "cust_gpio_usage.h"
#define __TPD_DEBUG__ 

/*Ctp Power Off In Sleep ? */
//#define TPD_CLOSE_POWER_IN_SLEEP
#define REVERSE_X
#define REVERSE_Y


#define GPIO_CTP_RST2138_PIN   GPIO22_CTP_EN_1V8
#define  GPIO_CTP_RST2138_PIN_M_GPIO  GPIO22_CTP_EN_1V8_M_GPIO

 
extern struct tpd_device *tpd;

/*Use For Get CTP Data By I2C*/ 
struct i2c_client *i2c_client = NULL;

/*Use For Firmware Update By I2C*/
//static struct i2c_client     *msg21xx_i2c_client = NULL;

//struct task_struct *thread = NULL;
 
static DECLARE_WAIT_QUEUE_HEAD(waiter);
//static DEFINE_MUTEX(i2c_access);

typedef struct
{
    u16 X;
    u16 Y;
} TouchPoint_t;

/*CTP Data Package*/
typedef struct
{
    u8 nTouchKeyMode;
    u8 nTouchKeyCode;
    u8 nFingerNum;
    TouchPoint_t Point[MAX_TOUCH_FINGER];
} TouchScreenInfo_t;
/*
enum wk_wdt_type {
	WK_WDT_LOC_TYPE,
	WK_WDT_EXT_TYPE,
	WK_WDT_LOC_TYPE_NOLOCK,
	WK_WDT_EXT_TYPE_NOLOCK,
	
};
extern void mtk_wdt_restart(enum wk_wdt_type type);

*/

extern enum wk_wdt_en {
	WK_WDT_DIS=0,
	WK_WDT_EN,
};
extern struct wk_wdt {
	int (*config)(enum wk_wdt_type, enum wk_wdt_mode, int timeout);
	void (*kick_wdt)(enum wk_wdt_type);
	int (*enable)(enum wk_wdt_en);
};

//change version
unsigned char MSG_FIRMWARE_YARISL_VERSION[94*1024] =
{
   //#include "mutto_version1_3.h"
   //#include "Mutto_V1_06.h"
   #include "Tcl_YarisL_Mutto_V1.09_140702.h" //fangjie add 20140703 PR 724767
};



 
static void tpd_eint_interrupt_handler(void);
static struct work_struct    msg21xx_wq;

#ifdef TPD_HAVE_BUTTON 
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_dim_local[TPD_KEY_COUNT][4] = TPD_KEYS_DIM;
#endif

#ifdef MT6575 
 extern void mt65xx_eint_unmask(unsigned int line);
 extern void mt65xx_eint_mask(unsigned int line);
 extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
 extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
 extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
									  kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
									  kal_bool auto_umask);
#else

	extern void mt65xx_eint_unmask(unsigned int line);
	extern void mt65xx_eint_mask(unsigned int line);
	extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
	extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
#endif

 
static int __devinit tpd_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tpd_detect (struct i2c_client *client, struct i2c_board_info *info);
static int __devexit tpd_remove(struct i2c_client *client);
static int touch_event_handler(void *unused);
 

static int tpd_flag = 0;
static int tpd_halt=0;
static int point_num = 0;
static int p_point_num = 0;



#define TPD_OK 0

 
 static const struct i2c_device_id msg2133_tpd_id[] = {{"msg2133",0},{}};

 static struct i2c_board_info __initdata msg2133_i2c_tpd={ I2C_BOARD_INFO("msg2133", (0x26))};
 
 
 static struct i2c_driver tpd_i2c_driver = {
  .driver = {
	 .name = "msg2133",//.name = TPD_DEVICE,
//	 .owner = THIS_MODULE,
  },
  .probe = tpd_probe,
  .remove = __devexit_p(tpd_remove),
  .id_table = msg2133_tpd_id,
  .detect = tpd_detect,
//  .address_data = &addr_data,
 };

static u8 *dma_bufferma = NULL;//FTS_BYTE=u8;
static u32 dma_handlema = NULL;
static struct mutex dma_model_mutexma;

static void I2cDMA_init()
{
	mutex_init(&dma_model_mutexma);
	dma_bufferma= (u8 *)dma_alloc_coherent(NULL, 128, &dma_handlema, GFP_KERNEL);
    	if(!dma_bufferma)
	{
    		printk("[MSG2133A][TSP] dma_alloc_coherent error\n");
	}
}

static void I2cDMA_exit()
{
	if(dma_bufferma)
	{
		dma_free_coherent(NULL, 128, dma_bufferma, dma_handlema);
		dma_bufferma = NULL;
		dma_handlema= 0;
	}
	printk("[MSG2133A][TSP] dma_free_coherent OK\n");
}

 
 //start for update firmware //msz   for update firmware 20121126
#define __FIRMWARE_UPDATE__
#define MSG2133_UPDATE

//#ifdef MSG2133_UPDATE
#ifdef __FIRMWARE_UPDATE__

#define MSG2133_TS_ADDR			0x26
#define MSG2133_FW_ADDR			0x62
#define MSG2133_FW_UPDATE_ADDR   	0x49

static struct i2c_client     *this_client = NULL;
struct class *firmware_class;
struct device *firmware_cmd_dev;
static int update_switch = 0;
static int FwDataCnt;
static  char *fw_version;
static unsigned char temp[94][1024];
static u8 g_dwiic_info_data[1024];   // Buffer for info data

//BEGIN: add by fangjie for create fwversions attribute node. 
unsigned short fw_major_version=0,fw_minor_version=0;
static ssize_t firmware_fwversions_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	TPD_DMESG("*** firmware_version_show fw_version = %s***\n", fw_version);
	return sprintf(buf, "%d.%03d \n", fw_major_version, fw_minor_version);
}
static DEVICE_ATTR(fwversions, 0444, firmware_fwversions_show,NULL);
//END: add by fangjie for create fwversions attribute node. 


static void msg2133_device_power_on()
{
/*
	#ifdef TPD_POWER_SOURCE_CUSTOM
	hwPowerOn(TPD_POWER_SOURCE_CUSTOM, VOL_2800, "TP");
#else
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
#endif
*/
	TPD_DMESG("msg2133: power on\n");
}

static void msg2133_reset()
{
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(10);
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ZERO);  
	msleep(50);
	TPD_DMESG(" msg2133 reset\n");
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(150);

}


/*static bool msg2133_i2c_read(char *pbt_buf, int dw_lenth)
{
    int ret;
    MSG2133_DBG("The msg_i2c_client->addr=0x%x\n",i2c_client->addr);
    ret = i2c_master_recv(this_client, pbt_buf, dw_lenth);

    if(ret <= 0){
        MSG2133_DBG("msg_i2c_read_interface error\n");
        return false;
    }

    return true;
}

static bool msg2133_i2c_write(char *pbt_buf, int dw_lenth)
{
    int ret;
    MSG2133_DBG("The msg_i2c_client->addr=0x%x\n",i2c_client->addr);
    ret = i2c_master_send(this_client, pbt_buf, dw_lenth);

    if(ret <= 0){
        MSG2133_DBG("msg_i2c_read_interface error\n");
        return false;
    }

    return true;
}*/
/*
static void HalTscrCReadI2CSeq(u8 addr, u8* read_data, u16 size)
{
   //according to your platform.
   	int rc;

	struct i2c_msg msgs[] =
    {
		{
			.addr = addr,
			.flags = I2C_M_RD,
			.len = size,
			.buf = read_data,
		},
	};

	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("HalTscrCReadI2CSeq error %d\n", rc);
	}
}

static void HalTscrCDevWriteI2CSeq(u8 addr, u8* data, u16 size)
{
    //according to your platform.
   	int rc;
	struct i2c_msg msgs[] =
    {
		{
			.addr = addr,
			.flags = 0,
			.len = size,
			.buf = data,
		},
	};
	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("HalTscrCDevWriteI2CSeq error %d,addr = %d\n", rc,addr);
	}
}

*/


/*
static bool msg2133_i2c_read(char *pbt_buf, int dw_lenth)
{
    int ret;
    //    pr_ch("The msg_i2c_client->addr=0x%x\n",i2c_client->addr);
    ret = i2c_master_recv(this_client, pbt_buf, dw_lenth);

    if(ret <= 0)
    {
        //pr_tp("msg_i2c_read_interface error\n");
        return false;
    }

    return true;
}

static bool msg2133_i2c_write(char *pbt_buf, int dw_lenth)
{
    int ret;
    //    pr_ch("The msg_i2c_client->addr=0x%x\n",i2c_client->addr);
    ret = i2c_master_send(this_client, pbt_buf, dw_lenth);

    if(ret <= 0)
    {
        //pr_tp("msg_i2c_read_interface error\n");
        return false;
    }

    return true;
}
*/


static void HalTscrCReadI2CSeq(u8 addr, u8* read_data, u8 size)
{
	int ret;
	u16 addrt;
	//mutex_lock(&tp_mutex); 

	addrt=this_client->addr;
	this_client->addr = addr;
	if(size<9){
		this_client->addr = this_client->addr & I2C_MASK_FLAG;	
		ret=i2c_master_recv(this_client, &read_data[0], size);
	}
	else
	{
		mutex_lock(&dma_model_mutexma);
		this_client->addr = this_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
		mutex_unlock(&dma_model_mutexma);
		ret = i2c_master_recv(this_client, dma_handlema,size);

		if(ret<=0)
		{
			this_client->addr=addr;
			TPD_DMESG("[FT6306]i2c_read_byte error line = %d, ret = %d\n", __LINE__, ret);
			return ;
		}
		memcpy(read_data,dma_bufferma,size);
	}

	this_client->addr = addrt;
	if(ret <=  0) {
		printk("addr: %d i2c read interface error!\n",addr);	
	}
	//mutex_unlock(&tp_mutex);
}

static void HalTscrCDevWriteI2CSeq(u8 addr, u8* data, u16 size)
{
	int ret;
	u16 addrt;

	addrt=this_client->addr;
	this_client->addr = addr;
	//mutex_lock(&tp_mutex);
	if(size<9){
		this_client->addr = this_client->addr & I2C_MASK_FLAG;
		ret = i2c_master_send(this_client, &data[0], size);  
	}
	else
	{
		memcpy(dma_bufferma,data,size);
		//TPD_DMESG("[FT6306] I2C  DMA transfer----byte_write------llf\n");
		mutex_lock(&dma_model_mutexma);
		this_client->addr = this_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
		mutex_unlock(&dma_model_mutexma);
		ret = i2c_master_send(this_client, dma_handlema, size);

	}

	this_client->addr = addrt;
	if(ret <=  0) {
		printk("addr: %d i2c write interface error!\n",addr);	
	}  
	//mutex_unlock(&tp_mutex);
}

static void i2c_read_msg2133(unsigned char *pbt_buf, int dw_lenth)
{
    //this_client->addr = MSG2133_FW_ADDR;
	//i2c_master_recv(this_client, pbt_buf, dw_lenth);	//0xC5_8bit
	//this_client->addr = MSG2133_TS_ADDR;
	   	int rc;
	struct i2c_msg msgs[] =
    {
		{
			.addr = MSG2133_FW_ADDR,
			.flags = I2C_M_RD,
			.len = dw_lenth,
			.buf = pbt_buf,
		},
	};
	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("i2c_read_msg2133 error %d,addr = %d\n", rc,MSG2133_FW_ADDR);
	}
	
}

static void i2c_write_msg2133(unsigned char *pbt_buf, int dw_lenth)
{

	//this_client->addr = MSG2133_FW_ADDR;
	//i2c_master_send(this_client, pbt_buf, dw_lenth);		//0xC4_8bit
	//this_client->addr = MSG2133_TS_ADDR;
		   	int rc;
	struct i2c_msg msgs[] =
    {
		{
			.addr = MSG2133_FW_ADDR,
			.flags = 0,
			.len = dw_lenth,
			.buf = pbt_buf,
		},
	};
	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("i2c_write_msg2133 error %d,addr = %d\n", rc,MSG2133_FW_ADDR);
	}
}

static void i2c_read_update_msg2133(unsigned char *pbt_buf, int dw_lenth)
{	

	//this_client->addr = MSG2133_FW_UPDATE_ADDR;
	//i2c_master_recv(this_client, pbt_buf, dw_lenth);	//0x93_8bit
	//this_client->addr = MSG2133_TS_ADDR;
		   	int rc;
	struct i2c_msg msgs[] =
    {
		{
			.addr = MSG2133_FW_UPDATE_ADDR,
			.flags = I2C_M_RD,
			.len = dw_lenth,
			.buf = pbt_buf,
		},
	};
	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("i2c_read_update_msg2133 error %d,addr = %d\n", rc,MSG2133_FW_ADDR);
	}
}

static void i2c_write_update_msg2133(unsigned char *pbt_buf, int dw_lenth)
{	
  //  this_client->addr = MSG2133_FW_UPDATE_ADDR;
	//i2c_master_send(this_client, pbt_buf, dw_lenth);	//0x92_8bit
	//this_client->addr = MSG2133_TS_ADDR;
		//this_client->addr = MSG2133_TS_ADDR;
		   	int rc;
	struct i2c_msg msgs[] =
    {
		{
			.addr = MSG2133_FW_UPDATE_ADDR,
			.flags = 0,
			.len = dw_lenth,
			.buf = pbt_buf,
		},
	};
	rc = i2c_transfer(this_client->adapter, msgs, 1);
	if( rc < 0 )
    {
		printk("i2c_write_update_msg2133 error %d,addr = %d\n", rc,MSG2133_FW_ADDR);
	}
}



void dbbusDWIICEnterSerialDebugMode(void)
{
    unsigned char data[5];
    // Enter the Serial Debug Mode
    data[0] = 0x53;
    data[1] = 0x45;
    data[2] = 0x52;
    data[3] = 0x44;
    data[4] = 0x42;
    i2c_write_msg2133(data, 5);
}

void dbbusDWIICStopMCU(void)
{
    unsigned char data[1];
    // Stop the MCU
    data[0] = 0x37;
    i2c_write_msg2133(data, 1);
}

void dbbusDWIICIICUseBus(void)
{
    unsigned char data[1];
    // IIC Use Bus
    data[0] = 0x35;
    i2c_write_msg2133(data, 1);
}

void dbbusDWIICIICReshape(void)
{
    unsigned char data[1];
    // IIC Re-shape
    data[0] = 0x71;
    i2c_write_msg2133(data, 1);
}

void dbbusDWIICIICNotUseBus(void)
{
    unsigned char data[1];
    // IIC Not Use Bus
    data[0] = 0x34;
    i2c_write_msg2133(data, 1);
}

void dbbusDWIICNotStopMCU(void)
{
    unsigned char data[1];
    // Not Stop the MCU
    data[0] = 0x36;
    i2c_write_msg2133(data, 1);
}

void dbbusDWIICExitSerialDebugMode(void)
{
    unsigned char data[1];
    // Exit the Serial Debug Mode
    data[0] = 0x45;
    i2c_write_msg2133(data, 1);
    // Delay some interval to guard the next transaction
}

void drvISP_EntryIspMode(void)
{
    unsigned char bWriteData[5] =
    {
        0x4D, 0x53, 0x54, 0x41, 0x52
    };
    i2c_write_update_msg2133(bWriteData, 5);
    msleep(10);           // delay about 10ms
}

void drvISP_WriteEnable(void)
{
    unsigned char bWriteData[2] =
    {
        0x10, 0x06
    };
    unsigned char bWriteData1 = 0x12;
    i2c_write_update_msg2133(bWriteData, 2);
    i2c_write_update_msg2133(&bWriteData1, 1);
}

unsigned char drvISP_Read(unsigned char n, unsigned char *pDataToRead)    //First it needs send 0x11 to notify we want to get flash data back.
{
    unsigned char Read_cmd = 0x11;
    unsigned char i = 0;
    unsigned char dbbus_rx_data[16] = {0};
    i2c_write_update_msg2133(&Read_cmd, 1);
    //if (n == 1)
    {
        i2c_read_update_msg2133(&dbbus_rx_data[0], n + 1);

        for(i = 0; i < n; i++)
        {
            *(pDataToRead + i) = dbbus_rx_data[i + 1];
        }
    }
    //else
    {
        //     i2c_read_update_msg2133(pDataToRead, n);
    }
    return 0;
}

unsigned char drvISP_ReadStatus(void)
{
    unsigned char bReadData = 0;
    unsigned char bWriteData[2] =
    {
        0x10, 0x05
    };
    unsigned char bWriteData1 = 0x12;
//    msleep(1);           // delay about 100us
    i2c_write_update_msg2133(bWriteData, 2);
//    msleep(1);           // delay about 100us
    drvISP_Read(1, &bReadData);
//    msleep(10);           // delay about 10ms
    i2c_write_update_msg2133(&bWriteData1, 1);
    return bReadData;
}



void drvISP_BlockErase(unsigned int addr)
{
    unsigned char bWriteData[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char bWriteData1 = 0x12;
    unsigned int timeOutCount=0;
	
    drvISP_WriteEnable();
    //Enable write status register
    bWriteData[0] = 0x10;
    bWriteData[1] = 0x50;
    i2c_write_update_msg2133(bWriteData, 2);
    i2c_write_update_msg2133(&bWriteData1, 1);
    //Write Status
    bWriteData[0] = 0x10;
    bWriteData[1] = 0x01;
    bWriteData[2] = 0x00;
    i2c_write_update_msg2133(bWriteData, 3);
    i2c_write_update_msg2133(&bWriteData1, 1);
    //Write disable
    bWriteData[0] = 0x10;
    bWriteData[1] = 0x04;
    i2c_write_update_msg2133(bWriteData, 2);
    i2c_write_update_msg2133(&bWriteData1, 1);

    timeOutCount=0;
    msleep(1);           // delay about 100us
    while((drvISP_ReadStatus() & 0x01) == 0x01)
    {
        timeOutCount++;
	 if ( timeOutCount > 10000 ) 
            break; /* around 1 sec timeout */
    }

    //pr_ch("The drvISP_ReadStatus3=%d\n", drvISP_ReadStatus());
    drvISP_WriteEnable();
    //pr_ch("The drvISP_ReadStatus4=%d\n", drvISP_ReadStatus());
    bWriteData[0] = 0x10;
    bWriteData[1] = 0xC7;        //Block Erase
    //bWriteData[2] = ((addr >> 16) & 0xFF) ;
    //bWriteData[3] = ((addr >> 8) & 0xFF) ;
    // bWriteData[4] = (addr & 0xFF) ;
    i2c_write_update_msg2133(bWriteData, 2);
    //i2c_write_update_msg2133( &bWriteData, 5);
    i2c_write_update_msg2133(&bWriteData1, 1);

    timeOutCount=0;
    msleep(1);           // delay about 100us
    while((drvISP_ReadStatus() & 0x01) == 0x01)
    {
        timeOutCount++;
	 if ( timeOutCount > 10000 ) 
            break; /* around 1 sec timeout */
    }
}

void drvISP_Program(unsigned short k, unsigned char *pDataToWrite)
{
    unsigned short i = 0;
    unsigned short j = 0;
    //U16 n = 0;
    unsigned char TX_data[133];
    unsigned char bWriteData1 = 0x12;
    unsigned int addr = k * 1024;
#if 1

    for(j = 0; j < 8; j++)    //128*8 cycle
    {
        TX_data[0] = 0x10;
        TX_data[1] = 0x02;// Page Program CMD
        TX_data[2] = (addr + 128 * j) >> 16;
        TX_data[3] = (addr + 128 * j) >> 8;
        TX_data[4] = (addr + 128 * j);

        for(i = 0; i < 128; i++)
        {
            TX_data[5 + i] = pDataToWrite[j * 128 + i];
        }

        while((drvISP_ReadStatus() & 0x01) == 0x01)
        {
            ;    //wait until not in write operation
        }

        drvISP_WriteEnable();
        i2c_write_update_msg2133( TX_data, 133);   //write 133 byte per cycle
        i2c_write_update_msg2133(&bWriteData1, 1);
    }

#else

    for(j = 0; j < 512; j++)    //128*8 cycle
    {
        TX_data[0] = 0x10;
        TX_data[1] = 0x02;// Page Program CMD
        TX_data[2] = (addr + 2 * j) >> 16;
        TX_data[3] = (addr + 2 * j) >> 8;
        TX_data[4] = (addr + 2 * j);

        for(i = 0; i < 2; i++)
        {
            TX_data[5 + i] = pDataToWrite[j * 2 + i];
        }

        while((drvISP_ReadStatus() & 0x01) == 0x01)
        {
            ;    //wait until not in write operation
        }

        drvISP_WriteEnable();
        i2c_write_update_msg2133(TX_data, 7);    //write 7 byte per cycle
        i2c_write_update_msg2133(&bWriteData1, 1);
    }

#endif
}

void drvISP_ExitIspMode(void)
{
    unsigned char bWriteData = 0x24;
    i2c_write_update_msg2133(&bWriteData, 1);
}

#define FW_ADDR_MSG21XX   (0xC4>>1)
#define FW_ADDR_MSG21XX_TP   (0x4C>>1)
#define FW_UPDATE_ADDR_MSG21XX   (0x92>>1)
#define TP_DEBUG	printk//(x)		//x
#define DBUG	printk//(x) //x




static ssize_t firmware_version_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
	printk("tyd-tp: firmware_version_show\n");
    TPD_DMESG("*** firmware_version_show fw_version = %s***\n", fw_version);
    return sprintf(buf, "%s\n", fw_version);
}

static ssize_t firmware_version_store(struct device *dev,
                                      struct device_attribute *attr, const char *buf, size_t size)
{
    unsigned char dbbus_tx_data[3];
    unsigned char dbbus_rx_data[4] ;
    unsigned short major = 0, minor = 0;
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    fw_version = kzalloc(sizeof(char), GFP_KERNEL);
    TPD_DMESG("\n");
	TPD_DMESG("tyd-tp: firmware_version_store\n");
    //Get_Chip_Version();
    dbbus_tx_data[0] = 0x53;
    dbbus_tx_data[1] = 0x00;
    dbbus_tx_data[2] = 0x2a;//0x74--msg2133a;  0x2A----msg2133a
    //i2c_write(TOUCH_ADDR_MSG20XX, &dbbus_tx_data[0], 3);
    //i2c_read(TOUCH_ADDR_MSG20XX, &dbbus_rx_data[0], 4);
//    msg2133_i2c_write(&dbbus_tx_data[0], 3);
//    msg2133_i2c_read(&dbbus_rx_data[0], 4);
		HalTscrCDevWriteI2CSeq(FW_ADDR_MSG21XX_TP, &dbbus_tx_data[0], 3);
		HalTscrCReadI2CSeq(FW_ADDR_MSG21XX_TP, &dbbus_rx_data[0], 4);
    major = (dbbus_rx_data[1] << 8) + dbbus_rx_data[0];
    minor = (dbbus_rx_data[3] << 8) + dbbus_rx_data[2];
    TPD_DMESG("***major = %d ***\n", major);
    TPD_DMESG("***minor = %d ***\n", minor);
    sprintf(fw_version, "%03d%03d", major, minor);
    TPD_DMESG("***fw_version = %s ***\n", fw_version);
    return size;
}

static ssize_t firmware_update_show(struct device *dev,
                                    struct device_attribute *attr, char *buf)
{
	printk("tyd-tp: firmware_update_show\n");
    return sprintf(buf, "%s\n", fw_version);
}
#define _FW_UPDATE_C3_



#ifdef _FW_UPDATE_C3_
u8  Fmr_Loader[1024];
    u32 crc_tab[256];

#define _HalTscrHWReset(...) msg2133_reset(__VA_ARGS__)
//#define disable_irq(...) disable_irq_nosync(__VA_ARGS__)

static ssize_t firmware_update_c2(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    unsigned char i;
    unsigned char dbbus_tx_data[4];
    unsigned char dbbus_rx_data[2] = {0};
    update_switch = 1;
    //drvISP_EntryIspMode();
    //drvISP_BlockErase(0x00000);
    //M by cheehwa _HalTscrHWReset();

    //
  //  disable_irq_nosync(this_client->irq);
	
	msg2133_reset();
    //msctpc_LoopDelay ( 100 );        // delay about 100ms*****
    // Enable slave's ISP ECO mode
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    //pr_ch("dbbusDWIICIICReshape\n");
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x08;
    dbbus_tx_data[2] = 0x0c;
    dbbus_tx_data[3] = 0x08;
    // Disable the Watchdog
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Get_Chip_Version();
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11;
    dbbus_tx_data[2] = 0xE2;
    dbbus_tx_data[3] = 0x00;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x60;
    dbbus_tx_data[3] = 0x55;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //pr_ch("update\n");
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x61;
    dbbus_tx_data[3] = 0xAA;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Stop MCU
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x0F;
    dbbus_tx_data[2] = 0xE6;
    dbbus_tx_data[3] = 0x01;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Enable SPI Pad
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 3);
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = (dbbus_rx_data[0] | 0x20);  //Set Bit 5
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x25;
    i2c_write_msg2133(dbbus_tx_data, 3);
    dbbus_rx_data[0] = 0;
    dbbus_rx_data[1] = 0;
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = dbbus_rx_data[0] & 0xFC;  //Clear Bit 1,0
    i2c_write_msg2133(dbbus_tx_data, 4);
    /*
    //------------
    // ISP Speed Change to 400K
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11;
    dbbus_tx_data[2] = 0xE2;
    i2c_write_msg2133( dbbus_tx_data, 3);
    i2c_read_msg2133( &dbbus_rx_data[3], 1);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = dbbus_tx_data[3]&0xf7;  //Clear Bit3
    i2c_write_msg2133( dbbus_tx_data, 4);
    */
    //WP overwrite
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x0E;
    dbbus_tx_data[3] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //set pin high
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x10;
    dbbus_tx_data[3] = 0x08;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbusDWIICIICNotUseBus();
    dbbusDWIICNotStopMCU();
    dbbusDWIICExitSerialDebugMode();
    ///////////////////////////////////////
    // Start to load firmware
    ///////////////////////////////////////
    drvISP_EntryIspMode();
    TPD_DMESG("entryisp\n");
    drvISP_BlockErase(0x00000);
    //msleep(1000);
    TPD_DMESG("FwVersion=2");

    for(i = 0; i < 94; i++)    // total  94 KB : 1 byte per R/W
    {
        //msleep(1);//delay_100us
        TPD_DMESG("drvISP_Program\n");
        drvISP_Program(i, temp[i]);    // program to slave's flash
        //pr_ch("drvISP_Verify\n");
        //drvISP_Verify ( i, temp[i] ); //verify data
    }

    //MSG2133_DBG("update OK\n");
    drvISP_ExitIspMode();
    FwDataCnt = 0;
    msg2133_reset();
    TPD_DMESG("update OK\n");
    update_switch = 0;
    //
    enable_irq(this_client->irq);
    return size;
}

static u32 Reflect ( u32 ref, char ch ) //unsigned int Reflect(unsigned int ref, char ch)
{
    u32 value = 0;
    u32 i = 0;

    for ( i = 1; i < ( ch + 1 ); i++ )
    {
        if ( ref & 1 )
        {
            value |= 1 << ( ch - i );
        }
        ref >>= 1;
    }
    return value;
}

u32 Get_CRC ( u32 text, u32 prevCRC, u32 *crc32_table )
{
    u32  ulCRC = prevCRC;
	ulCRC = ( ulCRC >> 8 ) ^ crc32_table[ ( ulCRC & 0xFF ) ^ text];
    return ulCRC ;
}
static void Init_CRC32_Table ( u32 *crc32_table )
{
    u32 magicnumber = 0x04c11db7;
    u32 i = 0, j;

    for ( i = 0; i <= 0xFF; i++ )
    {
        crc32_table[i] = Reflect ( i, 8 ) << 24;
        for ( j = 0; j < 8; j++ )
        {
            crc32_table[i] = ( crc32_table[i] << 1 ) ^ ( crc32_table[i] & ( 0x80000000L ) ? magicnumber : 0 );
        }
        crc32_table[i] = Reflect ( crc32_table[i], 32 );
    }
}

typedef enum
{
	EMEM_ALL = 0,
	EMEM_MAIN,
	EMEM_INFO,
} EMEM_TYPE_t;

static void drvDB_WriteReg8Bit ( u8 bank, u8 addr, u8 data )
{
    u8 tx_data[4] = {0x10, bank, addr, data};
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, tx_data, 4 );
}

static void drvDB_WriteReg ( u8 bank, u8 addr, u16 data )
{
    u8 tx_data[5] = {0x10, bank, addr, data & 0xFF, data >> 8};
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, tx_data, 5 );
}

static unsigned short drvDB_ReadReg ( u8 bank, u8 addr )
{
    u8 tx_data[3] = {0x10, bank, addr};
    u8 rx_data[2] = {0};

    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, tx_data, 3 );
    HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX, &rx_data[0], 2 );
    return ( rx_data[1] << 8 | rx_data[0] );
}

static int drvTP_erase_emem_c32 ( void )
{
    /////////////////////////
    //Erase  all
    /////////////////////////
    
    //enter gpio mode
    drvDB_WriteReg ( 0x16, 0x1E, 0xBEAF );

    // before gpio mode, set the control pin as the orginal status
    drvDB_WriteReg ( 0x16, 0x08, 0x0000 );
    drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x10 );
    mdelay ( 10 ); //MCR_CLBK_DEBUG_DELAY ( 10, MCU_LOOP_DELAY_COUNT_MS );

    // ptrim = 1, h'04[2]
    drvDB_WriteReg8Bit ( 0x16, 0x08, 0x04 );
    drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x10 );
    mdelay ( 10 ); //MCR_CLBK_DEBUG_DELAY ( 10, MCU_LOOP_DELAY_COUNT_MS );

    // ptm = 6, h'04[12:14] = b'110
    drvDB_WriteReg8Bit ( 0x16, 0x09, 0x60 );
    drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x10 );

    // pmasi = 1, h'04[6]
    drvDB_WriteReg8Bit ( 0x16, 0x08, 0x44 );
    // pce = 1, h'04[11]
    drvDB_WriteReg8Bit ( 0x16, 0x09, 0x68 );
    // perase = 1, h'04[7]
    drvDB_WriteReg8Bit ( 0x16, 0x08, 0xC4 );
    // pnvstr = 1, h'04[5]
    drvDB_WriteReg8Bit ( 0x16, 0x08, 0xE4 );
    // pwe = 1, h'04[9]
    drvDB_WriteReg8Bit ( 0x16, 0x09, 0x6A );
    // trigger gpio load
    drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x10 );

    return ( 1 );
}

static ssize_t firmware_update_c32 ( struct device *dev, struct device_attribute *attr,
                                     const char *buf, size_t size,  EMEM_TYPE_t emem_type )
{
    u8  dbbus_tx_data[4];
    u8  dbbus_rx_data[2] = {0};
      // Buffer for slave's firmware

    u32 i, j, k;
    u32 crc_main, crc_main_tp;
    u32 crc_info, crc_info_tp;
    u16 reg_data = 0;

    crc_main = 0xffffffff;
    crc_info = 0xffffffff;

#if 1
    /////////////////////////
    // Erase  all
    /////////////////////////
    drvTP_erase_emem_c32();
    mdelay ( 1000 ); //MCR_CLBK_DEBUG_DELAY ( 1000, MCU_LOOP_DELAY_COUNT_MS );

    //ResetSlave();
    _HalTscrHWReset();
    //drvDB_EnterDBBUS();
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    mdelay ( 300 );

    // Reset Watchdog
    drvDB_WriteReg8Bit ( 0x3C, 0x60, 0x55 );
    drvDB_WriteReg8Bit ( 0x3C, 0x61, 0xAA );

    /////////////////////////
    // Program
    /////////////////////////

    //polling 0x3CE4 is 0x1C70
    do
    {
        reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
    }
    while ( reg_data != 0x1C70 );


    drvDB_WriteReg ( 0x3C, 0xE4, 0xE38F );  // for all-blocks

    //polling 0x3CE4 is 0x2F43
    do
    {
        reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
    }
    while ( reg_data != 0x2F43 );


    //calculate CRC 32
    Init_CRC32_Table ( &crc_tab[0] );

    for ( i = 0; i < 33; i++ ) // total  33 KB : 2 byte per R/W
    {
        if ( i < 32 )   //emem_main
        {
            if ( i == 31 )
            {
                temp[i][1014] = 0x5A; //Fmr_Loader[1014]=0x5A;
                temp[i][1015] = 0xA5; //Fmr_Loader[1015]=0xA5;

                for ( j = 0; j < 1016; j++ )
                {
                    //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
                    crc_main = Get_CRC ( temp[i][j], crc_main, &crc_tab[0] );
                }
            }
            else
            {
                for ( j = 0; j < 1024; j++ )
                {
                    //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
                    crc_main = Get_CRC ( temp[i][j], crc_main, &crc_tab[0] );
                }
            }
        }
        else  // emem_info
        {
            for ( j = 0; j < 1024; j++ )
            {
                //crc_info=Get_CRC(Fmr_Loader[j],crc_info,&crc_tab[0]);
                crc_info = Get_CRC ( temp[i][j], crc_info, &crc_tab[0] );
            }
        }

        //drvDWIIC_MasterTransmit( DWIIC_MODE_DWIIC_ID, 1024, Fmr_Loader );
        //HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP, temp[i], 1024 );
		for(k=0; k<8; k++)
        {
        	//HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[i*128], 128 );
        	HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &temp[i][k*128], 128 );
			TPD_DMESG ( "firmware_update_c32---g_dwiic_info_data[i*128]: %d\n", i );
			mdelay(50);
        }
        // polling 0x3CE4 is 0xD0BC
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
        }
        while ( reg_data != 0xD0BC );

        drvDB_WriteReg ( 0x3C, 0xE4, 0x2F43 );
    }

    //write file done
    drvDB_WriteReg ( 0x3C, 0xE4, 0x1380 );

    mdelay ( 10 ); //MCR_CLBK_DEBUG_DELAY ( 10, MCU_LOOP_DELAY_COUNT_MS );
    // polling 0x3CE4 is 0x9432
    do
    {
        reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
    }
    while ( reg_data != 0x9432 );

    crc_main = crc_main ^ 0xffffffff;
    crc_info = crc_info ^ 0xffffffff;

    // CRC Main from TP
    crc_main_tp = drvDB_ReadReg ( 0x3C, 0x80 );
    crc_main_tp = ( crc_main_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0x82 );
 
    //CRC Info from TP
    crc_info_tp = drvDB_ReadReg ( 0x3C, 0xA0 );
    crc_info_tp = ( crc_info_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0xA2 );

    TPD_DMESG ( "crc_main=0x%x, crc_info=0x%x, crc_main_tp=0x%x, crc_info_tp=0x%x\n",
               crc_main, crc_info, crc_main_tp, crc_info_tp );

    //drvDB_ExitDBBUS();
    if ( ( crc_main_tp != crc_main ) || ( crc_info_tp != crc_info ) )
    {
        TPD_DMESG ( "update FAILED\n" );
		_HalTscrHWReset();
        FwDataCnt = 0;
    	enable_irq(this_client->irq);		
        return ( 0 );
    }

    TPD_DMESG ( "update OK\n" );
	_HalTscrHWReset();
    FwDataCnt = 0;
	enable_irq(this_client->irq);

    return size;
#endif
}

static int drvTP_erase_emem_c33 ( EMEM_TYPE_t emem_type )
{
    // stop mcu
    drvDB_WriteReg ( 0x0F, 0xE6, 0x0001 );

    //disable watch dog
    drvDB_WriteReg8Bit ( 0x3C, 0x60, 0x55 );
    drvDB_WriteReg8Bit ( 0x3C, 0x61, 0xAA );

    // set PROGRAM password
    drvDB_WriteReg8Bit ( 0x16, 0x1A, 0xBA );
    drvDB_WriteReg8Bit ( 0x16, 0x1B, 0xAB );

    //proto.MstarWriteReg(F1.loopDevice, 0x1618, 0x80);
    drvDB_WriteReg8Bit ( 0x16, 0x18, 0x80 );

    if ( emem_type == EMEM_ALL )
    {
        drvDB_WriteReg8Bit ( 0x16, 0x08, 0x10 ); //mark
    }

    drvDB_WriteReg8Bit ( 0x16, 0x18, 0x40 );
    mdelay ( 10 );

    drvDB_WriteReg8Bit ( 0x16, 0x18, 0x80 );

    // erase trigger
    if ( emem_type == EMEM_MAIN )
    {
        drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x04 ); //erase main
    }
    else
    {
        drvDB_WriteReg8Bit ( 0x16, 0x0E, 0x08 ); //erase all block
    }

    return ( 1 );
}

static int drvTP_read_emem_dbbus_c33 ( EMEM_TYPE_t emem_type, u16 addr, size_t size, u8 *p, size_t set_pce_high )
{
    u32 i;

    // Set the starting address ( must before enabling burst mode and enter riu mode )
    drvDB_WriteReg ( 0x16, 0x00, addr );

    // Enable the burst mode ( must before enter riu mode )
    drvDB_WriteReg ( 0x16, 0x0C, drvDB_ReadReg ( 0x16, 0x0C ) | 0x0001 );

    // Set the RIU password
    drvDB_WriteReg ( 0x16, 0x1A, 0xABBA );

    // Enable the information block if pifren is HIGH
    if ( emem_type == EMEM_INFO )
    {
        // Clear the PCE
        drvDB_WriteReg ( 0x16, 0x18, drvDB_ReadReg ( 0x16, 0x18 ) | 0x0080 );
        mdelay ( 10 );

        // Set the PIFREN to be HIGH
        drvDB_WriteReg ( 0x16, 0x08, 0x0010 );
    }

    // Set the PCE to be HIGH
    drvDB_WriteReg ( 0x16, 0x18, drvDB_ReadReg ( 0x16, 0x18 ) | 0x0040 );
    mdelay ( 10 );

    // Wait pce becomes 1 ( read data ready )
    while ( ( drvDB_ReadReg ( 0x16, 0x10 ) & 0x0004 ) != 0x0004 );

    for ( i = 0; i < size; i += 4 )
    {
        // Fire the FASTREAD command
        drvDB_WriteReg ( 0x16, 0x0E, drvDB_ReadReg ( 0x16, 0x0E ) | 0x0001 );

        // Wait the operation is done
        while ( ( drvDB_ReadReg ( 0x16, 0x10 ) & 0x0001 ) != 0x0001 );

        p[i + 0] = drvDB_ReadReg ( 0x16, 0x04 ) & 0xFF;
        p[i + 1] = ( drvDB_ReadReg ( 0x16, 0x04 ) >> 8 ) & 0xFF;
        p[i + 2] = drvDB_ReadReg ( 0x16, 0x06 ) & 0xFF;
        p[i + 3] = ( drvDB_ReadReg ( 0x16, 0x06 ) >> 8 ) & 0xFF;
    }

    // Disable the burst mode
    drvDB_WriteReg ( 0x16, 0x0C, drvDB_ReadReg ( 0x16, 0x0C ) & ( ~0x0001 ) );

    // Clear the starting address
    drvDB_WriteReg ( 0x16, 0x00, 0x0000 );

    //Always return to main block
    if ( emem_type == EMEM_INFO )
    {
        // Clear the PCE before change block
        drvDB_WriteReg ( 0x16, 0x18, drvDB_ReadReg ( 0x16, 0x18 ) | 0x0080 );
        mdelay ( 10 );
        // Set the PIFREN to be LOW
        drvDB_WriteReg ( 0x16, 0x08, drvDB_ReadReg ( 0x16, 0x08 ) & ( ~0x0010 ) );

        drvDB_WriteReg ( 0x16, 0x18, drvDB_ReadReg ( 0x16, 0x18 ) | 0x0040 );
        while ( ( drvDB_ReadReg ( 0x16, 0x10 ) & 0x0004 ) != 0x0004 );
    }

    // Clear the RIU password
    drvDB_WriteReg ( 0x16, 0x1A, 0x0000 );

    if ( set_pce_high )
    {
        // Set the PCE to be HIGH before jumping back to e-flash codes
        drvDB_WriteReg ( 0x16, 0x18, drvDB_ReadReg ( 0x16, 0x18 ) | 0x0040 );
        while ( ( drvDB_ReadReg ( 0x16, 0x10 ) & 0x0004 ) != 0x0004 );
    }

    return ( 1 );
}


static int drvTP_read_info_dwiic_c33 ( void )
{
    u8  dwiic_tx_data[5];
    u8  dwiic_rx_data[4];
    u16 reg_data=0;
    mdelay ( 300 );

    // Stop Watchdog
    TPD_DMESG ("drvTP_read_info_dwiic_c33---1 \n");
    drvDB_WriteReg8Bit ( 0x3C, 0x60, 0x55 );
    drvDB_WriteReg8Bit ( 0x3C, 0x61, 0xAA );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---2 \n");

    drvDB_WriteReg ( 0x3C, 0xE4, 0xA4AB );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---3 \n");

	drvDB_WriteReg ( 0x1E, 0x04, 0x7d60 );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---4 \n");

    // TP SW reset
    drvDB_WriteReg ( 0x1E, 0x04, 0x829F );
	mdelay ( 100 );
    dwiic_tx_data[0] = 0x10;
    dwiic_tx_data[1] = 0x0F;
    dwiic_tx_data[2] = 0xE6;
    dwiic_tx_data[3] = 0x00;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dwiic_tx_data, 4 );	
    mdelay ( 100 );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---5 \n");

    do{
        reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
    }
    while ( reg_data != 0x5B58 );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---6 \n");

    dwiic_tx_data[0] = 0x72;
    dwiic_tx_data[1] = 0x80;
    dwiic_tx_data[2] = 0x00;
    dwiic_tx_data[3] = 0x04;
    dwiic_tx_data[4] = 0x00;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , dwiic_tx_data, 5 );

    mdelay ( 50 );
	TPD_DMESG ("drvTP_read_info_dwiic_c33---7 \n");

    // recive info data
    
    for(reg_data=0;reg_data<8;reg_data++)
    {
		TPD_DMESG ("drvTP_read_info_dwiic_c33---8---ADDR \n");
		 dwiic_tx_data[1] = 0x80+(((reg_data*128)&0xff00)>>8);            // address High
         dwiic_tx_data[2] = (reg_data*128)&0x00ff;                                   // address low
         HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , dwiic_tx_data, 5 );
         mdelay (10 );
		 TPD_DMESG ("drvTP_read_info_dwiic_c33---8---READ START\n");
    	// recive info data
         HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX_TP, &g_dwiic_info_data[reg_data*128], 128);
		mdelay (200 );
		TPD_DMESG ("drvTP_read_info_dwiic_c33---8---READ END \n");
    }

	
	TPD_DMESG ("drvTP_read_info_dwiic_c33---8 \n");

    return ( 1 );
}

static int drvTP_info_updata_C33 ( u16 start_index, u8 *data, u16 size )
{
    // size != 0, start_index+size !> 1024
    u16 i;
    for ( i = 0; i < size; i++ )
    {
        g_dwiic_info_data[start_index] = * ( data + i );
        start_index++;
    }
    return ( 1 );
}

static ssize_t firmware_update_c33 ( struct device *dev, struct device_attribute *attr,
                                     const char *buf, size_t size, EMEM_TYPE_t emem_type )
{
    u8  dbbus_tx_data[4];
    u8  dbbus_rx_data[2] = {0};
    u8  life_counter[2];
    u32 i, j, k;
    u32 crc_main, crc_main_tp;
    u32 crc_info, crc_info_tp;
  
    int update_pass = 1;
    u16 reg_data = 0;
	I2cDMA_init();
	this_client->timing = 100;

    crc_main = 0xffffffff;
    crc_info = 0xffffffff;

    drvTP_read_info_dwiic_c33();
	
    if ( g_dwiic_info_data[0] == 'M' && g_dwiic_info_data[1] == 'S' && g_dwiic_info_data[2] == 'T' && g_dwiic_info_data[3] == 'A' && g_dwiic_info_data[4] == 'R' && g_dwiic_info_data[5] == 'T' && g_dwiic_info_data[6] == 'P' && g_dwiic_info_data[7] == 'C' )
    {
        // updata FW Version
        //drvTP_info_updata_C33 ( 8, &temp[32][8], 5 );

		g_dwiic_info_data[8]=temp[32][8];
		g_dwiic_info_data[9]=temp[32][9];
		g_dwiic_info_data[10]=temp[32][10];
		g_dwiic_info_data[11]=temp[32][11];
        // updata life counter
        life_counter[1] = (( ( (g_dwiic_info_data[13] << 8 ) | g_dwiic_info_data[12]) + 1 ) >> 8 ) & 0xFF;
        life_counter[0] = ( ( (g_dwiic_info_data[13] << 8 ) | g_dwiic_info_data[12]) + 1 ) & 0xFF;
		g_dwiic_info_data[12]=life_counter[0];
		g_dwiic_info_data[13]=life_counter[1];
		TPD_DMESG ( "life_counter[0]=%d life_counter[1]=%d\n",life_counter[0],life_counter[1] );
        //drvTP_info_updata_C33 ( 10, &life_counter[0], 3 );
        drvDB_WriteReg ( 0x3C, 0xE4, 0x78C5 );
		drvDB_WriteReg ( 0x1E, 0x04, 0x7d60 );
        // TP SW reset
        drvDB_WriteReg ( 0x1E, 0x04, 0x829F );

        mdelay ( 50 );

        //polling 0x3CE4 is 0x2F43
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );

        }
        while ( reg_data != 0x2F43 );

        // transmit lk info data---xb.pang for 1024
        //HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[0], 1024 );
		for(i=0;i<8;i++)
        {
        	HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[i*128], 128 );
			TPD_DMESG ( "HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[%*128], 128 ); \n", i);
			mdelay(50);
        }
        //polling 0x3CE4 is 0xD0BC
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
        }
        while ( reg_data != 0xD0BC );

    }

    //erase main
    drvTP_erase_emem_c33 ( EMEM_MAIN );
    mdelay ( 1000 );

    //ResetSlave();
    _HalTscrHWReset();

    //drvDB_EnterDBBUS();
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    mdelay ( 300 );

    /////////////////////////
    // Program
    /////////////////////////

    //polling 0x3CE4 is 0x1C70
    if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
    {
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
        }
        while ( reg_data != 0x1C70 );
    }

    switch ( emem_type )
    {
        case EMEM_ALL:
            drvDB_WriteReg ( 0x3C, 0xE4, 0xE38F );  // for all-blocks
            break;
        case EMEM_MAIN:
            drvDB_WriteReg ( 0x3C, 0xE4, 0x7731 );  // for main block
            break;
        case EMEM_INFO:
            drvDB_WriteReg ( 0x3C, 0xE4, 0x7731 );  // for info block

            drvDB_WriteReg8Bit ( 0x0F, 0xE6, 0x01 );

            drvDB_WriteReg8Bit ( 0x3C, 0xE4, 0xC5 ); //
            drvDB_WriteReg8Bit ( 0x3C, 0xE5, 0x78 ); //

            drvDB_WriteReg8Bit ( 0x1E, 0x04, 0x9F );
            drvDB_WriteReg8Bit ( 0x1E, 0x05, 0x82 );

            drvDB_WriteReg8Bit ( 0x0F, 0xE6, 0x00 );
            mdelay ( 100 );
            break;
    }

    // polling 0x3CE4 is 0x2F43
    do
    {
        reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
    }
    while ( reg_data != 0x2F43 );

    // calculate CRC 32
    Init_CRC32_Table ( &crc_tab[0] );

    for ( i = 0; i < 33; i++ ) // total  33 KB : 2 byte per R/W
    {
        if ( emem_type == EMEM_INFO )
			i = 32;

        if ( i < 32 )   //emem_main
        {
            if ( i == 31 )
            {
                temp[i][1014] = 0x5A; //Fmr_Loader[1014]=0x5A;
                temp[i][1015] = 0xA5; //Fmr_Loader[1015]=0xA5;

                for ( j = 0; j < 1016; j++ )
                {
                    //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
                    crc_main = Get_CRC ( temp[i][j], crc_main, &crc_tab[0] );
                }
            }
            else
            {
                for ( j = 0; j < 1024; j++ )
                {
                    //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
                    crc_main = Get_CRC ( temp[i][j], crc_main, &crc_tab[0] );
                }
            }
        }
        else  //emem_info
        {
            for ( j = 0; j < 1024; j++ )
            {
                //crc_info=Get_CRC(Fmr_Loader[j],crc_info,&crc_tab[0]);
                crc_info = Get_CRC ( g_dwiic_info_data[j], crc_info, &crc_tab[0] );
            }
            if ( emem_type == EMEM_MAIN ) break;
        }

        //drvDWIIC_MasterTransmit( DWIIC_MODE_DWIIC_ID, 1024, Fmr_Loader );
        //HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP, temp[i], 1024 );
		for(k=0; k<8; k++)
        {
        	HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &temp[i][k*128], 128 );
			TPD_DMESG ( "temp[i] \n");
			mdelay(50);
        }
        // polling 0x3CE4 is 0xD0BC
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
        }
        while ( reg_data != 0xD0BC );

        drvDB_WriteReg ( 0x3C, 0xE4, 0x2F43 );
    }

    if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
    {
        // write file done and check crc
        drvDB_WriteReg ( 0x3C, 0xE4, 0x1380 );
    }
    mdelay ( 10 ); //MCR_CLBK_DEBUG_DELAY ( 10, MCU_LOOP_DELAY_COUNT_MS );

    if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
    {
        // polling 0x3CE4 is 0x9432
        do
        {
            reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
			TPD_DMESG("polling \n");
        }while ( reg_data != 0x9432 );
    }

    crc_main = crc_main ^ 0xffffffff;
    crc_info = crc_info ^ 0xffffffff;

    if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
    {
        // CRC Main from TP
        crc_main_tp = drvDB_ReadReg ( 0x3C, 0x80 );
        crc_main_tp = ( crc_main_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0x82 );

        // CRC Info from TP
        crc_info_tp = drvDB_ReadReg ( 0x3C, 0xA0 );
        crc_info_tp = ( crc_info_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0xA2 );
    }
    TPD_DMESG ( "crc_main=0x%x, crc_info=0x%x, crc_main_tp=0x%x, crc_info_tp=0x%x\n",
               crc_main, crc_info, crc_main_tp, crc_info_tp );

    //drvDB_ExitDBBUS();

    update_pass = 1;
	if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
    {
        if ( crc_main_tp != crc_main )
            update_pass = 0;

        if ( crc_info_tp != crc_info )
            update_pass = 0;
    }

    if ( !update_pass )
    {
        TPD_DMESG ( "update FAILED\n" );
		_HalTscrHWReset();
        FwDataCnt = 0;
    	enable_irq(this_client->irq);
        return ( 0 );
    }

    TPD_DMESG ( "update OK\n" );
	_HalTscrHWReset();
    FwDataCnt = 0;
    enable_irq(this_client->irq);

		   	 this_client->timing = 100;
	 this_client->addr = FW_ADDR_MSG21XX_TP;

	   I2cDMA_exit();
    return size;
}

static ssize_t firmware_update_store ( struct device *dev,
                                       struct device_attribute *attr, const char *buf, size_t size )
{
    u8 i;
    u8 dbbus_tx_data[4];
    unsigned char dbbus_rx_data[2] = {0};
	//disable_irq(this_client->irq);

    _HalTscrHWReset();

    // Erase TP Flash first
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    mdelay ( 300 );

    // Disable the Watchdog
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x60;
    dbbus_tx_data[3] = 0x55;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x61;
    dbbus_tx_data[3] = 0xAA;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
    // Stop MCU
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x0F;
    dbbus_tx_data[2] = 0xE6;
    dbbus_tx_data[3] = 0x01;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
    /////////////////////////
    // Difference between C2 and C3
    /////////////////////////
	// c2:2133 c32:2133a(2) c33:2138
    //check id
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0xCC;
    HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 3 );
    HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX, &dbbus_rx_data[0], 2 );
    
        // check version
        dbbus_tx_data[0] = 0x10;
        dbbus_tx_data[1] = 0x3C;
        dbbus_tx_data[2] = 0xEA;
        HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 3 );
        HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX, &dbbus_rx_data[0], 2 );
        TPD_DMESG ( "dbbus_rx version[0]=0x%x", dbbus_rx_data[0] );

		
        return firmware_update_c33 ( dev, attr, buf, size, EMEM_MAIN );
		

    

}

void masterBUT_LoadFwToTarget(unsigned char *temp)

{

   
   u8 dbbus_tx_data[4];
   unsigned char dbbus_rx_data[2] = {0};

   u8  life_counter[2];
    u32 i, j, k;
    u32 crc_main, crc_main_tp;
    u32 crc_info, crc_info_tp;
  EMEM_TYPE_t emem_type;
    int update_pass = 1;
    u16 reg_data = 0;
	I2cDMA_init();  //added by zhengdao


   disable_irq(this_client->irq);

   _HalTscrHWReset();
   this_client->timing = 100;

   // Erase TP Flash first
   dbbusDWIICEnterSerialDebugMode();
   dbbusDWIICStopMCU();
   dbbusDWIICIICUseBus();
   dbbusDWIICIICReshape();
   mdelay ( 300 );

   // Disable the Watchdog
   dbbus_tx_data[0] = 0x10;
   dbbus_tx_data[1] = 0x3C;
   dbbus_tx_data[2] = 0x60;
   dbbus_tx_data[3] = 0x55;
   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
   dbbus_tx_data[0] = 0x10;
   dbbus_tx_data[1] = 0x3C;
   dbbus_tx_data[2] = 0x61;
   dbbus_tx_data[3] = 0xAA;
   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
   // Stop MCU
   dbbus_tx_data[0] = 0x10;
   dbbus_tx_data[1] = 0x0F;
   dbbus_tx_data[2] = 0xE6;
   dbbus_tx_data[3] = 0x01;
   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 4 );
   /////////////////////////
   // Difference between C2 and C3
   /////////////////////////
   // c2:2133 c32:2133a(2) c33:2138
   //check id
   dbbus_tx_data[0] = 0x10;
   dbbus_tx_data[1] = 0x1E;
   dbbus_tx_data[2] = 0xCC;
   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 3 );
   HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX, &dbbus_rx_data[0], 2 );
   
	   // check version
	   dbbus_tx_data[0] = 0x10;
	   dbbus_tx_data[1] = 0x3C;
	   dbbus_tx_data[2] = 0xEA;
	   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX, dbbus_tx_data, 3 );
	   HalTscrCReadI2CSeq ( FW_ADDR_MSG21XX, &dbbus_rx_data[0], 2 );
	   TPD_DMESG ( "dbbus_rx version[0]=0x%x", dbbus_rx_data[0] );

	   
	   crc_main = 0xffffffff;
	   crc_info = 0xffffffff;
	   emem_type= EMEM_MAIN;
	   drvTP_read_info_dwiic_c33();

	   //mtk_wdt_restart(WK_WDT_EXT_TYPE_NOLOCK);//avoid watchdog timeout

//	   g_wk_wdt->enable(WK_WDT_DIS); //added by zhengdao
	   if ( g_dwiic_info_data[0] == 'M' && g_dwiic_info_data[1] == 'S' && g_dwiic_info_data[2] == 'T' && g_dwiic_info_data[3] == 'A' && g_dwiic_info_data[4] == 'R' && g_dwiic_info_data[5] == 'T' && g_dwiic_info_data[6] == 'P' && g_dwiic_info_data[7] == 'C' )
	   {
		   // updata FW Version
		   //drvTP_info_updata_C33 ( 8, &temp[32][8], 5 );
	   
		   g_dwiic_info_data[8]=temp[32*1024+8];
		   g_dwiic_info_data[9]=temp[32*1024+9];
		   g_dwiic_info_data[10]=temp[32*1024+10];
		   g_dwiic_info_data[11]=temp[32*1024+11];
		   // updata life counter
		   life_counter[1] = (( ( (g_dwiic_info_data[13] << 8 ) | g_dwiic_info_data[12]) + 1 ) >> 8 ) & 0xFF;
		   life_counter[0] = ( ( (g_dwiic_info_data[13] << 8 ) | g_dwiic_info_data[12]) + 1 ) & 0xFF;
		   g_dwiic_info_data[12]=life_counter[0];
		   g_dwiic_info_data[13]=life_counter[1];
		   TPD_DMESG ( "life_counter[0]=%d life_counter[1]=%d\n",life_counter[0],life_counter[1] );
		   //drvTP_info_updata_C33 ( 10, &life_counter[0], 3 );
		   drvDB_WriteReg ( 0x3C, 0xE4, 0x78C5 );
		   drvDB_WriteReg ( 0x1E, 0x04, 0x7d60 );
		   // TP SW reset
		   drvDB_WriteReg ( 0x1E, 0x04, 0x829F );
	   
		   mdelay ( 50 );
	   
		   //polling 0x3CE4 is 0x2F43
		   do
		   {
			   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
	   
		   }
		   while ( reg_data != 0x2F43 );
	   
		   // transmit lk info data---xb.pang for 1024
		   //HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[0], 1024 );
		   for(i=0;i<8;i++)
		   {
			   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[i*128], 128 );
			   TPD_DMESG ( "HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &g_dwiic_info_data[%*128], 128 ); \n", i);
			   mdelay(50);
		   }
		   //polling 0x3CE4 is 0xD0BC
		   do
		   {
			   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
		   }
		   while ( reg_data != 0xD0BC );
	   
	   }
	   
	   //erase main
	   drvTP_erase_emem_c33 ( EMEM_MAIN );
	   mdelay ( 1000 );
	   
	   //ResetSlave();
	   _HalTscrHWReset();
	   
	   //drvDB_EnterDBBUS();
	   dbbusDWIICEnterSerialDebugMode();
	   dbbusDWIICStopMCU();
	   dbbusDWIICIICUseBus();
	   dbbusDWIICIICReshape();
	   mdelay ( 300 );
	   
	   /////////////////////////
	   // Program
	   /////////////////////////
	   
	   //polling 0x3CE4 is 0x1C70
	   if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
	   {
		   do
		   {
			   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
		   }
		   while ( reg_data != 0x1C70 );
	   }
	   
	   switch ( emem_type )
	   {
		   case EMEM_ALL:
			   drvDB_WriteReg ( 0x3C, 0xE4, 0xE38F );  // for all-blocks
			   break;
		   case EMEM_MAIN:
			   drvDB_WriteReg ( 0x3C, 0xE4, 0x7731 );  // for main block
			   break;
		   case EMEM_INFO:
			   drvDB_WriteReg ( 0x3C, 0xE4, 0x7731 );  // for info block
	   
			   drvDB_WriteReg8Bit ( 0x0F, 0xE6, 0x01 );
	   
			   drvDB_WriteReg8Bit ( 0x3C, 0xE4, 0xC5 ); //
			   drvDB_WriteReg8Bit ( 0x3C, 0xE5, 0x78 ); //
	   
			   drvDB_WriteReg8Bit ( 0x1E, 0x04, 0x9F );
			   drvDB_WriteReg8Bit ( 0x1E, 0x05, 0x82 );
	   
			   drvDB_WriteReg8Bit ( 0x0F, 0xE6, 0x00 );
			   mdelay ( 100 );
			   break;
	   }
	   
	   // polling 0x3CE4 is 0x2F43
	   do
	   {
		   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
	   }
	   while ( reg_data != 0x2F43 );
	   
	   // calculate CRC 32
	   Init_CRC32_Table ( &crc_tab[0] );
	   
	   for ( i = 0; i < 33; i++ ) // total	33 KB : 2 byte per R/W
	   {
		   if ( emem_type == EMEM_INFO )
			   i = 32;
	   
		   if ( i < 32 )   //emem_main
		   {
			   if ( i == 31 )
			   {
				   temp[i*1024+1014] = 0x5A; //Fmr_Loader[1014]=0x5A;
				   temp[i*1024+1015] = 0xA5; //Fmr_Loader[1015]=0xA5;
	   
				   for ( j = 0; j < 1016; j++ )
				   {
					   //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
					   crc_main = Get_CRC ( temp[i*1024+j], crc_main, &crc_tab[0] );
				   }
			   }
			   else
			   {
				   for ( j = 0; j < 1024; j++ )
				   {
					   //crc_main=Get_CRC(Fmr_Loader[j],crc_main,&crc_tab[0]);
					   crc_main = Get_CRC ( temp[i*1024+j], crc_main, &crc_tab[0] );
				   }
			   }
		   }
		   else  //emem_info
		   {
			   for ( j = 0; j < 1024; j++ )
			   {
				   //crc_info=Get_CRC(Fmr_Loader[j],crc_info,&crc_tab[0]);
				   crc_info = Get_CRC ( g_dwiic_info_data[j], crc_info, &crc_tab[0] );
			   }
			   if ( emem_type == EMEM_MAIN ) break;
		   }
	   
		   //drvDWIIC_MasterTransmit( DWIIC_MODE_DWIIC_ID, 1024, Fmr_Loader );
		  // HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP, &temp[i*1024], 1024 );
		   
		   for(k=0; k<8; k++)
		   {
			   HalTscrCDevWriteI2CSeq ( FW_ADDR_MSG21XX_TP , &temp[i*1024+k*128], 128 );
			   TPD_DMESG ( "temp[i] \n");
			   mdelay(100);
		   }
		   // polling 0x3CE4 is 0xD0BC
		   do
		   {
			   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
		   }
		   while ( reg_data != 0xD0BC );
	   
		   drvDB_WriteReg ( 0x3C, 0xE4, 0x2F43 );
	   }
	   
	   if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
	   {
		   // write file done and check crc
		   drvDB_WriteReg ( 0x3C, 0xE4, 0x1380 );
	   }
	   mdelay ( 10 ); //MCR_CLBK_DEBUG_DELAY ( 10, MCU_LOOP_DELAY_COUNT_MS );
	   
	   if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
	   {
		   // polling 0x3CE4 is 0x9432
		   do
		   {
			   reg_data = drvDB_ReadReg ( 0x3C, 0xE4 );
			   TPD_DMESG("polling \n");
		   }while ( reg_data != 0x9432 );
	   }
	   
	   crc_main = crc_main ^ 0xffffffff;
	   crc_info = crc_info ^ 0xffffffff;
	   
	   if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
	   {
		   // CRC Main from TP
		   crc_main_tp = drvDB_ReadReg ( 0x3C, 0x80 );
		   crc_main_tp = ( crc_main_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0x82 );
	   
		   // CRC Info from TP
		   crc_info_tp = drvDB_ReadReg ( 0x3C, 0xA0 );
		   crc_info_tp = ( crc_info_tp << 16 ) | drvDB_ReadReg ( 0x3C, 0xA2 );
	   }
	   TPD_DMESG ( "crc_main=0x%x, crc_info=0x%x, crc_main_tp=0x%x, crc_info_tp=0x%x\n",
				  crc_main, crc_info, crc_main_tp, crc_info_tp );
	   
	   //drvDB_ExitDBBUS();
	   
	   update_pass = 1;
	   if ( ( emem_type == EMEM_ALL ) || ( emem_type == EMEM_MAIN ) )
	   {
		   if ( crc_main_tp != crc_main )
			   update_pass = 0;
	   
		   if ( crc_info_tp != crc_info )
			   update_pass = 0;
	   }
	   
	   if ( !update_pass )
	   
		   TPD_DMESG ( "update FAILED\n" );
       else
	       TPD_DMESG ( "update OK\n" );
	   
	   _HalTscrHWReset();
	   FwDataCnt = 0;
	   enable_irq(this_client->irq);
	 //  mtk_wdt_restart(WK_WDT_EXT_TYPE_NOLOCK);//avoid watchdog timeout

//	   g_wk_wdt->enable(WK_WDT_EN);  //added by zhengdao
	   	 this_client->timing = 100;
	 this_client->addr = FW_ADDR_MSG21XX_TP;

	   I2cDMA_exit();

	   
	   return ;

   


}



#else

static ssize_t firmware_update_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    unsigned char i;
    unsigned char dbbus_tx_data[4];
    unsigned char dbbus_rx_data[2] = {0};
    update_switch = 1;
    //drvISP_EntryIspMode();
    //drvISP_BlockErase(0x00000);
    //M by cheehwa _HalTscrHWReset();

    //
    //disable_irq_nosync(this_client->irq);
	
	msg2133_reset();
    //msctpc_LoopDelay ( 100 );        // delay about 100ms*****
    // Enable slave's ISP ECO mode
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    //pr_ch("dbbusDWIICIICReshape\n");
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x08;
    dbbus_tx_data[2] = 0x0c;
    dbbus_tx_data[3] = 0x08;
    // Disable the Watchdog
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Get_Chip_Version();
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11;
    dbbus_tx_data[2] = 0xE2;
    dbbus_tx_data[3] = 0x00;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x60;
    dbbus_tx_data[3] = 0x55;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //pr_ch("update\n");
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x61;
    dbbus_tx_data[3] = 0xAA;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Stop MCU
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x0F;
    dbbus_tx_data[2] = 0xE6;
    dbbus_tx_data[3] = 0x01;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Enable SPI Pad
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 3);
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = (dbbus_rx_data[0] | 0x20);  //Set Bit 5
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x25;
    i2c_write_msg2133(dbbus_tx_data, 3);
    dbbus_rx_data[0] = 0;
    dbbus_rx_data[1] = 0;
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = dbbus_rx_data[0] & 0xFC;  //Clear Bit 1,0
    i2c_write_msg2133(dbbus_tx_data, 4);
    /*
    //------------
    // ISP Speed Change to 400K
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11;
    dbbus_tx_data[2] = 0xE2;
    i2c_write_msg2133( dbbus_tx_data, 3);
    i2c_read_msg2133( &dbbus_rx_data[3], 1);
    //pr_tp("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = dbbus_tx_data[3]&0xf7;  //Clear Bit3
    i2c_write_msg2133( dbbus_tx_data, 4);
    */
    //WP overwrite
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x0E;
    dbbus_tx_data[3] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //set pin high
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x10;
    dbbus_tx_data[3] = 0x08;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbusDWIICIICNotUseBus();
    dbbusDWIICNotStopMCU();
    dbbusDWIICExitSerialDebugMode();
    ///////////////////////////////////////
    // Start to load firmware
    ///////////////////////////////////////
    drvISP_EntryIspMode();
    TPD_DMESG("entryisp\n");
    drvISP_BlockErase(0x00000);
    //msleep(1000);
    TPD_DMESG("FwVersion=2");

    for(i = 0; i < 94; i++)    // total  94 KB : 1 byte per R/W
    {
        //msleep(1);//delay_100us
        TPD_DMESG("drvISP_Program\n");
        drvISP_Program(i, temp[i]);    // program to slave's flash
        //pr_ch("drvISP_Verify\n");
        //drvISP_Verify ( i, temp[i] ); //verify data
    }

    //MSG2133_DBG("update OK\n");
    drvISP_ExitIspMode();
    FwDataCnt = 0;
    msg2133_reset();
    TPD_DMESG("update OK\n");
    update_switch = 0;
    //
    //enable_irq(this_client->irq);
    return size;
}
#endif
static ssize_t firmware_data_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
	TPD_DMESG("tyd-tp: firmware_data_show\n");
    return FwDataCnt;
}

static ssize_t firmware_data_store(struct device *dev,
                                   struct device_attribute *attr, const char *buf, size_t size)
{
    int i;
    TPD_DMESG("***FwDataCnt = %d ***\n", FwDataCnt);
	TPD_DMESG("tyd-tp: firmware_data_store\n");
   // for(i = 0; i < 1024; i++)
    {
        memcpy(temp[FwDataCnt], buf, 1024);
    }

    FwDataCnt++;
    return size;
}

static ssize_t firmware_clear_show(struct device *dev,
                                   struct device_attribute *attr, char *buf)
{
    unsigned short k = 0, i = 0, j = 0;
    unsigned char bWriteData[5] =
    {
        0x10, 0x03, 0, 0, 0
    };
    unsigned char RX_data[256];
    unsigned char bWriteData1 = 0x12;
    unsigned int addr = 0;
    TPD_DMESG("\n");
	TPD_DMESG("tyd-tp: firmware_clear_show\n");
    for(k = 0; k < 94; i++)    // total  94 KB : 1 byte per R/W
    {
        addr = k * 1024;

        for(j = 0; j < 8; j++)    //128*8 cycle
        {
            bWriteData[2] = (unsigned char)((addr + j * 128) >> 16);
            bWriteData[3] = (unsigned char)((addr + j * 128) >> 8);
            bWriteData[4] = (unsigned char)(addr + j * 128);

            while((drvISP_ReadStatus() & 0x01) == 0x01)
            {
                ;    //wait until not in write operation
            }

            i2c_write_update_msg2133(bWriteData, 5);     //write read flash addr
            drvISP_Read(128, RX_data);
            i2c_write_update_msg2133(&bWriteData1, 1);    //cmd end

            for(i = 0; i < 128; i++)    //log out if verify error{
                if(RX_data[i] != 0xFF){
                    TPD_DMESG("k=%d,j=%d,i=%d===============erase not clean================", k, j, i);
                }
            }
     }
    TPD_DMESG("read finish\n");
    return sprintf(buf, "%s\n", fw_version);
}

static ssize_t firmware_clear_store(struct device *dev,
                                    struct device_attribute *attr, const char *buf, size_t size)
{
    unsigned char dbbus_tx_data[4];
    unsigned char dbbus_rx_data[2] = {0};
    //msctpc_LoopDelay ( 100 );        // delay about 100ms*****
    // Enable slave's ISP ECO mode
    /*
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();*/
    TPD_DMESG("\n");
	TPD_DMESG("tyd-tp: firmware_clear_store\n");
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x08;
    dbbus_tx_data[2] = 0x0c;
    dbbus_tx_data[3] = 0x08;
    // Disable the Watchdog
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Get_Chip_Version();
    //FwVersion  = 2;
    //if (FwVersion  == 2)
    {
        dbbus_tx_data[0] = 0x10;
        dbbus_tx_data[1] = 0x11;
        dbbus_tx_data[2] = 0xE2;
        dbbus_tx_data[3] = 0x00;
        i2c_write_msg2133(dbbus_tx_data, 4);
    }
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x60;
    dbbus_tx_data[3] = 0x55;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x3C;
    dbbus_tx_data[2] = 0x61;
    dbbus_tx_data[3] = 0xAA;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Stop MCU
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x0F;
    dbbus_tx_data[2] = 0xE6;
    dbbus_tx_data[3] = 0x01;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //Enable SPI Pad
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 3);
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    TPD_DMESG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = (dbbus_rx_data[0] | 0x20);  //Set Bit 5
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x25;
    i2c_write_msg2133(dbbus_tx_data, 3);
    dbbus_rx_data[0] = 0;
    dbbus_rx_data[1] = 0;
    i2c_read_msg2133(&dbbus_rx_data[0], 2);
    TPD_DMESG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
    dbbus_tx_data[3] = dbbus_rx_data[2] & 0xFC;  //Clear Bit 1,0
    i2c_write_msg2133(dbbus_tx_data, 4);
    //WP overwrite
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x0E;
    dbbus_tx_data[3] = 0x02;
    i2c_write_msg2133(dbbus_tx_data, 4);
    //set pin high
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x1E;
    dbbus_tx_data[2] = 0x10;
    dbbus_tx_data[3] = 0x08;
    i2c_write_msg2133(dbbus_tx_data, 4);
    dbbusDWIICIICNotUseBus();
    dbbusDWIICNotStopMCU();
    dbbusDWIICExitSerialDebugMode();
    ///////////////////////////////////////
    // Start to load firmware
    ///////////////////////////////////////
    drvISP_EntryIspMode();
    TPD_DMESG("chip erase+\n");
    drvISP_BlockErase(0x00000);
    TPD_DMESG("chip erase-\n");
    drvISP_ExitIspMode();
    return size;
}

static DEVICE_ATTR(version, 0444, firmware_version_show, firmware_version_store);
static DEVICE_ATTR(update, 0444, firmware_update_show, firmware_update_store);
static DEVICE_ATTR(data, 0444, firmware_data_show, firmware_data_store);
static DEVICE_ATTR(clear, 0444, firmware_clear_show, firmware_clear_store);

void msg2133_init_class()
{
	firmware_class = class_create(THIS_MODULE,"ms-touchscreen-msg20xx" );//client->name

	if(IS_ERR(firmware_class))
		pr_err("Failed to create class(firmware)!\n");

	firmware_cmd_dev = device_create(firmware_class,
	                                     NULL, 0, NULL, "device");//device

	if(IS_ERR(firmware_cmd_dev))
		pr_err("Failed to create device(firmware_cmd_dev)!\n");
		
	// version /sys/class/mtk-tpd/device/version
	if(device_create_file(firmware_cmd_dev, &dev_attr_version) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_version.attr.name);

	// update /sys/class/mtk-tpd/device/update
	if(device_create_file(firmware_cmd_dev, &dev_attr_update) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_update.attr.name);

	// data /sys/class/mtk-tpd/device/data
	if(device_create_file(firmware_cmd_dev, &dev_attr_data) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_data.attr.name);

	// clear /sys/class/mtk-tpd/device/clear
	if(device_create_file(firmware_cmd_dev, &dev_attr_clear) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_clear.attr.name);
}

#endif //endif __FIRMWARE_UPDATE__ MSG2133_UPDATE

//end for update firmware

#ifdef DMA_IIC
#include <linux/dma-mapping.h>
static unsigned char *I2CDMABuf_va = NULL;
static volatile unsigned int I2CDMABuf_pa = NULL;
static void _msg_dma_alloc(void)
{
    I2CDMABuf_va = (u8 *)dma_alloc_coherent(NULL, 4096, &I2CDMABuf_pa, GFP_KERNEL);
}
static void _msg_dma_free(void)
{
    if(NULL!=I2CDMABuf_va)
    {
        dma_free_coherent(NULL, 4096, I2CDMABuf_va, I2CDMABuf_pa);
	    I2CDMABuf_va = NULL;
	    I2CDMABuf_pa = 0;
    }
}
#endif


//BEGIN: add by fangjie 
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#define ITO_TEST
#ifdef ITO_TEST
//modify:according to the actual project.
#include "yarisl_open_test_ANA1_MUTTO.h"
#include "yarisl_open_test_ANA2_MUTTO.h"
#include "yarisl_open_test_ANA3_MUTTO.h"
#include "yarisl_open_test_ANA1_B_MUTTO.h"
#include "yarisl_open_test_ANA2_B_MUTTO.h"
#include "yarisl_short_test_ANA1_Mutto.h"
#include "yarisl_short_test_ANA2_Mutto.h"
#include "yarisl_short_test_ANA3_Mutto.h"
#include "yarisl_short_test_ANA4_Mutto.h"
#include <linux/hwmsen_dev.h>

static struct device *compatible_mtp;

u8 bItoTestDebug = 1;
#define ITO_TEST_DEBUG(format, ...) \
{ \
    if(bItoTestDebug) \
    { \
        printk(KERN_ERR "ito_test ***" format "\n", ## __VA_ARGS__); \
        mdelay(5); \
    } \
}
#define ITO_TEST_DEBUG_MUST(format, ...)	printk(KERN_ERR "ito_test ***" format "\n", ## __VA_ARGS__);mdelay(5)

#define MAX_CHNL_NUM (48)
#define PIN_GUARD_RING (46)
#define GPIO_SETTING_SIZE (3)

#define	OPEN_TEST_NON_BORDER_AREA_THRESHOLD (40)        
#define	OPEN_TEST_BORDER_AREA_THRESHOLD     (40)

#define	SHORT_TEST_THRESHOLD                (3500)

#define	ITO_TEST_MODE_OPEN_TEST              (0x01)
#define	ITO_TEST_MODE_SHORT_TEST             (0x02)

s16 s16_raw_data_1[MAX_CHNL_NUM] = {0};
s16 s16_raw_data_2[MAX_CHNL_NUM] = {0};
s16 s16_raw_data_3[MAX_CHNL_NUM] = {0};
s16 s16_raw_data_4[MAX_CHNL_NUM] = {0};
s8 data_flag_1[MAX_CHNL_NUM] = {0};
s8 data_flag_2[MAX_CHNL_NUM] = {0};
s8 data_flag_3[MAX_CHNL_NUM] = {0};
s8 data_flag_4[MAX_CHNL_NUM] = {0};
u8 ito_test_keynum = 0;
u8 ito_test_dummynum = 0;
u8 ito_test_trianglenum = 0;
u8 ito_test_2r = 0;
u8 g_LTP = 1;	
uint16_t *open_1 = NULL;
uint16_t *open_1B = NULL;
uint16_t *open_2 = NULL;
uint16_t *open_2B = NULL;
uint16_t *open_3 = NULL;
u8 *MAP1 = NULL;
u8 *MAP2=NULL;
u8 *MAP3=NULL;
u8 *MAP40_1 = NULL;
u8 *MAP40_2 = NULL;
u8 *MAP40_3 = NULL;
u8 *MAP40_4 = NULL;
u8 *MAP41_1 = NULL;
u8 *MAP41_2 = NULL;
u8 *MAP41_3 = NULL;
u8 *MAP41_4 = NULL;

u16 *short_1 = NULL;
u16 *short_2 = NULL;
u16 *short_3 = NULL;
u16 *short_4 = NULL;
u8 *SHORT_MAP1 = NULL;
u8 *SHORT_MAP2 = NULL;
u8 *SHORT_MAP3 = NULL;
u8 *SHORT_MAP4 = NULL;
u16 *short_1_GPO = NULL;
u16 *short_2_GPO = NULL;
u16 *short_3_GPO = NULL;
u16 *short_4_GPO = NULL;


static u8 g_fail_channel[MAX_CHNL_NUM] = {0};
static int fail_channel_count = 0;
static u8 ito_test_mode = 0;

#define ITO_TEST_ADDR_TP  (0x4C>>1)
#define ITO_TEST_ADDR_REG (0xC4>>1)
#define REG_INTR_FIQ_MASK           0x04
#define FIQ_E_FRAME_READY_MASK      ( 1 << 8 )

#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT5  (1<<5)
#define BIT11 (1<<11)
#define BIT15 (1<<15)

static int ito_test_i2c_read(U8 addr, U8* read_data, U16 size)//modify : according to the project to change  g_I2cClient to this_client.
{
#if 0
    int rc;
    U8 addr_before = i2c_client->addr;
    i2c_client->addr = addr;

    #ifdef DMA_IIC
    if(size>8&&NULL!=I2CDMABuf_va)
    {
        int i = 0;
        this_client->ext_flag = this_client->ext_flag | I2C_DMA_FLAG ;
        rc = i2c_master_recv(this_client, (unsigned char *)I2CDMABuf_pa, size);
        for(i = 0; i < size; i++)
   		{
        	read_data[i] = I2CDMABuf_va[i];
    	}
    }
    else
    {
        rc = i2c_master_recv(this_client, read_data, size);
    }
    this_client->ext_flag = this_client->ext_flag & (~I2C_DMA_FLAG);	
    #else
    rc = i2c_master_recv(i2c_client, read_data, size);
    #endif

    i2c_client->addr = addr_before;
    if( rc < 0 )
    {
        ITO_TEST_DEBUG_MUST("ito_test_i2c_read error %d,addr=%d\n", rc,addr);
    }
    return rc;
#endif
	int ret;
	u16 addrt;
	//mutex_lock(&tp_mutex); 

	addrt=this_client->addr;
	this_client->addr = addr;
	if(size<9){
		this_client->addr = this_client->addr & I2C_MASK_FLAG;	
		ret=i2c_master_recv(this_client, &read_data[0], size);
	}
	else
	{
		mutex_lock(&dma_model_mutexma);
		this_client->addr = this_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
		mutex_unlock(&dma_model_mutexma);
		ret = i2c_master_recv(this_client, dma_handlema,size);

		if(ret<=0)
		{
			this_client->addr=addr;
			TPD_DMESG("[FT6306]i2c_read_byte error line = %d, ret = %d\n", __LINE__, ret);
			return ;
		}
		memcpy(read_data,dma_bufferma,size);
	}

	this_client->addr = addrt;
	if(ret <=  0) {
		printk("addr: %d i2c read interface error!\n",addr);	
	}
	//mutex_unlock(&tp_mutex);
	return ret;
}

static int ito_test_i2c_write(U8 addr, U8* data, U16 size)//modify : according to the project to change  g_I2cClient to this_client.
{ 
#if 0
    int rc;
    U8 addr_before = i2c_client->addr;
    i2c_client->addr = addr;

#ifdef DMA_IIC
    if(size>8&&NULL!=I2CDMABuf_va)
	{
	    int i = 0;
	    for(i=0;i<size;i++)
    	{
    		 I2CDMABuf_va[i]=data[i];
    	}
		this_client->ext_flag = this_client->ext_flag | I2C_DMA_FLAG ;
		rc = i2c_master_send(this_client, (unsigned char *)I2CDMABuf_pa, size);
	}
	else
	{
		rc = i2c_master_send(this_client, data, size);
	}
    this_client->ext_flag = this_client->ext_flag & (~I2C_DMA_FLAG);	
#else
    rc = i2c_master_send(i2c_client, data, size);
#endif

    i2c_client->addr = addr_before;
    if( rc < 0 )
    {
        ITO_TEST_DEBUG_MUST("ito_test_i2c_write error %d,addr = %d,data[0]=%d\n", rc, addr,data[0]);
    }
    return rc;
#endif
	int ret;
	u16 addrt;

	addrt=this_client->addr;
	this_client->addr = addr;
	//mutex_lock(&tp_mutex);
	if(size<9){
		this_client->addr = this_client->addr & I2C_MASK_FLAG;
		ret = i2c_master_send(this_client, &data[0], size);  
	}
	else
	{
		memcpy(dma_bufferma,data,size);
		//TPD_DMESG("[FT6306] I2C  DMA transfer----byte_write------llf\n");
		mutex_lock(&dma_model_mutexma);
		this_client->addr = this_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
		mutex_unlock(&dma_model_mutexma);
		ret = i2c_master_send(this_client, dma_handlema, size);

	}

	this_client->addr = addrt;
	if(ret <=  0) {
		printk("addr: %d i2c write interface error!\n",addr);	
	}  
	return ret;
	//mutex_unlock(&tp_mutex);
}

static void ito_test_reset(void)//modify for the actual yarisl project.
{
    	ITO_TEST_DEBUG("reset tp\n");
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(5);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ZERO);
	msleep(100);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(100);
}

static void ito_test_disable_irq(void) //modify for the actual yarisl project.
{
	//disable_irq_nosync(irq_msg21xx);
	mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
}

static void ito_test_enable_irq(void) //modify for the actual yarisl project.
{
	//enable_irq(irq_msg21xx);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
}

static void ito_test_set_iic_rate(u32 iicRate) //modify for the actual yarisl project must larger than 50K
{
	#if 0 //fangjie modify
	#ifdef CONFIG_I2C_SPRD//展讯平台
        	sprd_i2c_ctl_chg_clk(this_client->adapter->nr, iicRate);
        	mdelay(100);
	#endif
    	#ifdef MTK//MTK平台
        	this_client->timing = iicRate/1000;
    	#endif
	#endif 
	 i2c_client->timing = iicRate/1000;
}

static void ito_test_WriteReg( u8 bank, u8 addr, u16 data )
{
    u8 tx_data[5] = {0x10, bank, addr, data & 0xFF, data >> 8};
    ito_test_i2c_write( ITO_TEST_ADDR_REG, &tx_data[0], 5 );
}

static void ito_test_WriteReg8Bit( u8 bank, u8 addr, u8 data )
{
    u8 tx_data[4] = {0x10, bank, addr, data};
    ito_test_i2c_write ( ITO_TEST_ADDR_REG, &tx_data[0], 4 );
}

static unsigned short ito_test_ReadReg( u8 bank, u8 addr )
{
    u8 tx_data[3] = {0x10, bank, addr};
    u8 rx_data[2] = {0};

    ito_test_i2c_write( ITO_TEST_ADDR_REG, &tx_data[0], 3 );
    ito_test_i2c_read ( ITO_TEST_ADDR_REG, &rx_data[0], 2 );
    return ( rx_data[1] << 8 | rx_data[0] );
}

static u32 ito_test_get_TpType(void)
{
    u8 tx_data[3] = {0};
    u8 rx_data[4] = {0};
    u32 Major = 0, Minor = 0;

    ITO_TEST_DEBUG("GetTpType\n");
        
    tx_data[0] = 0x53;
    tx_data[1] = 0x00;
    tx_data[2] = 0x2A;
    ito_test_i2c_write(ITO_TEST_ADDR_TP, &tx_data[0], 3);
    mdelay(50);
    ito_test_i2c_read(ITO_TEST_ADDR_TP, &rx_data[0], 4);
    Major = (rx_data[1]<<8) + rx_data[0];
    Minor = (rx_data[3]<<8) + rx_data[2];

    ITO_TEST_DEBUG("***TpTypeMajor = %d ***\n", Major);
    ITO_TEST_DEBUG("***TpTypeMinor = %d ***\n", Minor);
    
    return Major;
}

//modify:  Note the ctp's number of yarisl
#define TP_OF_LIANCHUANG    (2)
#define TP_OF_JUNDA         (4)
#define TP_OF_MUTTO  (1) //add by fangjie.

static u32 ito_test_choose_TpType(void)
{
    	u32 tpType = 0;
    	u8 i = 0;
    	open_1 = NULL;
    	open_1B = NULL;
    	open_2 = NULL;
    	open_2B = NULL;
    	open_3 = NULL;
    	MAP1 = NULL;
    	MAP2 = NULL;
    	MAP3 = NULL;
    	MAP40_1 = NULL;
    	MAP40_2 = NULL;
    	MAP40_3 = NULL;
    	MAP40_4 = NULL;
    	MAP41_1 = NULL;
    	MAP41_2 = NULL;
    	MAP41_3 = NULL;
    	MAP41_4 = NULL;
    short_1 = NULL;
    short_2 = NULL;
    short_3 = NULL;
    short_4 = NULL;
    SHORT_MAP1 = NULL;
    SHORT_MAP2 = NULL;
    SHORT_MAP3 = NULL;
    SHORT_MAP4 = NULL;
    short_1_GPO = NULL;
    short_2_GPO = NULL;
    short_3_GPO = NULL;
    short_4_GPO = NULL;
    	ito_test_keynum = 0;
    	ito_test_dummynum = 0;
    	ito_test_trianglenum = 0;
    	ito_test_2r = 0;

    	for(i=0;i<10;i++)
    	{
       		 tpType = ito_test_get_TpType();
        	ITO_TEST_DEBUG("tpType=%d;i=%d;\n",tpType,i);
		if (0x0001 == tpType)
		{
			tpType = 1;
		}
		if (TP_OF_MUTTO ==  tpType)
		{
			break;
		}        	
        	else if(i<5)
        	{
            		mdelay(100);  
        	}
        	else
        	{
            		ito_test_reset();
        	}
	}
	    
	if(TP_OF_MUTTO==tpType) //modify for the actual yarisl project.
	{
	        open_1 = open_1_MUTTO;
	        open_1B = open_1B_MUTTO;
	        open_2 = open_2_MUTTO;
	        open_2B = open_2B_MUTTO;
	        open_3 = open_3_MUTTO;
	        MAP1 = MAP1_MUTTO;
	        MAP2 = MAP2_MUTTO;
	        MAP3 = MAP3_MUTTO;
	        MAP40_1 = MAP40_1_MUTTO;
	        MAP40_2 = MAP40_2_MUTTO;
	        MAP40_3 = MAP40_3_MUTTO;
	        MAP40_4 = MAP40_4_MUTTO;
	        MAP41_1 = MAP41_1_MUTTO;
	        MAP41_2 = MAP41_2_MUTTO;
	        MAP41_3 = MAP41_3_MUTTO;
	        MAP41_4 = MAP41_4_MUTTO;
        short_1 = short_1_MUTTO;
        short_2 = short_2_MUTTO;
        short_3 = short_3_MUTTO;
        short_4 = short_4_MUTTO;
        SHORT_MAP1 = SHORT_MAP1_MUTTO;
        SHORT_MAP2 = SHORT_MAP2_MUTTO;
        SHORT_MAP3 = SHORT_MAP3_MUTTO;
        SHORT_MAP4 = SHORT_MAP4_MUTTO;
        short_1_GPO = short_1_MUTTO_GPO;
        short_2_GPO = short_2_MUTTO_GPO;
        short_3_GPO = short_3_MUTTO_GPO;
        short_4_GPO = short_4_MUTTO_GPO;
	        ito_test_keynum = NUM_KEY_MUTTO;
	        ito_test_dummynum = NUM_DUMMY_MUTTO;
	        ito_test_trianglenum = NUM_SENSOR_MUTTO;
	        ito_test_2r = ENABLE_2R_MUTTO;
	}	
	 else
	{
		tpType = 0;
	}
	return tpType;
}

static void ito_test_EnterSerialDebugMode(void)
{
    u8 data[5];

    data[0] = 0x53;
    data[1] = 0x45;
    data[2] = 0x52;
    data[3] = 0x44;
    data[4] = 0x42;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, &data[0], 5);

    data[0] = 0x37;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, &data[0], 1);

    data[0] = 0x35;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, &data[0], 1);

    data[0] = 0x71;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, &data[0], 1);
}

static void ito_test_disable_filter_noise_detect(void)
{
    u16 reg_value;

    ITO_TEST_DEBUG("ito_test_disable_filter_noise_detect()\n");
     
    // Disable DIG/ANA drop
    reg_value = ito_test_ReadReg( 0x13, 0x02 ); 
      
    ito_test_WriteReg( 0x13, 0x02, reg_value & (~(BIT2 | BIT1 | BIT0)) );      
}

static uint16_t ito_test_get_num( void )
{
    uint16_t    num_of_sensor,i;
    uint16_t 	RegValue1,RegValue2;
 
    num_of_sensor = 0;
        
    RegValue1 = ito_test_ReadReg( 0x11, 0x4A); //bank:ana, addr:h0025  
    ITO_TEST_DEBUG("ito_test_get_num,RegValue1=%d\n",RegValue1);
    if ( ( RegValue1 & BIT1) == BIT1 )
    {
    	RegValue1 = ito_test_ReadReg( 0x12, 0x0A); //bank:ana2, addr:h0005  			
    	RegValue1 = RegValue1 & 0x0F;
    	
    	RegValue2 = ito_test_ReadReg( 0x12, 0x16); //bank:ana2, addr:h000b    		
    	RegValue2 = (( RegValue2 >> 1 ) & 0x0F) + 1;
    	
    	num_of_sensor = RegValue1 * RegValue2;
    }
	else
	{
	    for(i=0;i<4;i++)
	    {
	        num_of_sensor+=(ito_test_ReadReg( 0x12, 0x0A)>>(4*i))&0x0F; //bank:ana2, addr:h0005  
	    }
	}
    ITO_TEST_DEBUG("ito_test_get_num() num_of_sensor=%d\n", num_of_sensor);
    return num_of_sensor;        
}

static void ito_test_polling( void )
{
    uint16_t    reg_int = 0x0000;
    uint16_t    reg_value;


    reg_int = 0;

    ito_test_WriteReg( 0x13, 0x0C, BIT15 ); //bank:fir, addr:h0006         
    ito_test_WriteReg( 0x12, 0x14, (ito_test_ReadReg(0x12,0x14) | BIT0) ); //bank:ana2, addr:h000a        
            
    ITO_TEST_DEBUG("polling start\n");

    do
    {
        reg_int = ito_test_ReadReg(0x3D, 0x18); //bank:intr_ctrl, addr:h000c
    } while( ( reg_int & FIQ_E_FRAME_READY_MASK ) == 0x0000 );

    ITO_TEST_DEBUG("polling end\n");
    reg_value = ito_test_ReadReg( 0x3D, 0x18 ); 
    ito_test_WriteReg( 0x3D, 0x18, reg_value & (~FIQ_E_FRAME_READY_MASK) );      
}

static uint16_t ito_test_get_data_out( int16_t* s16_raw_data )
{
    uint8_t     i,dbbus_tx_data[8];
    uint16_t    raw_data[MAX_CHNL_NUM]={0};
    uint16_t    num_of_sensor;
    uint16_t    reg_int;
    uint8_t		dbbus_rx_data[MAX_CHNL_NUM*2]={0};
  
    num_of_sensor = ito_test_get_num();
    if(num_of_sensor*2>MAX_CHNL_NUM*2)
    {
        ITO_TEST_DEBUG("danger, num_of_sensor=%d\n", num_of_sensor);
        return num_of_sensor;
    }

    reg_int = ito_test_ReadReg( 0x3d, REG_INTR_FIQ_MASK<<1 ); 
    ito_test_WriteReg( 0x3d, REG_INTR_FIQ_MASK<<1, (reg_int & (uint16_t)(~FIQ_E_FRAME_READY_MASK) ) ); 
    ito_test_polling();
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x13; //bank:fir, addr:h0020 
    dbbus_tx_data[2] = 0x40;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 3);
    mdelay(20);
    ito_test_i2c_read(ITO_TEST_ADDR_REG, &dbbus_rx_data[0], (num_of_sensor * 2));
    mdelay(100);
    
    for(i=0;i<num_of_sensor * 2;i++)
    {
        ITO_TEST_DEBUG("dbbus_rx_data[%d]=%d\n",i,dbbus_rx_data[i]);
    }
 
    reg_int = ito_test_ReadReg( 0x3d, REG_INTR_FIQ_MASK<<1 ); 
    ito_test_WriteReg( 0x3d, REG_INTR_FIQ_MASK<<1, (reg_int | (uint16_t)FIQ_E_FRAME_READY_MASK ) ); 

    for( i = 0; i < num_of_sensor; i++ )
    {
        raw_data[i] = ( dbbus_rx_data[ 2 * i + 1] << 8 ) | ( dbbus_rx_data[2 * i] );
        s16_raw_data[i] = ( int16_t )raw_data[i];
    }
    
    return num_of_sensor;
}

static void ito_test_send_data_in( uint8_t step )
{
    uint16_t	i;
    uint8_t 	dbbus_tx_data[512];
    uint16_t 	*Type1=NULL;        

    ITO_TEST_DEBUG("ito_test_send_data_in step=%d\n",step);

    if( step == 0 ) // 39-4 (2R)
    {
        Type1 = &short_4[0];  
    }
    else if( step == 1 ) // 39-1
    {
        Type1 = &short_1[0];      	
    }
    else if( step == 2 ) // 39-2
    {
        Type1 = &short_2[0];      	
    }
    else if( step == 3 ) // 39-3
    {
        Type1 = &short_3[0];        
    }
    else if( step == 4 )
    {
        Type1 = &open_1[0];        
    }
    else if( step == 5 )
    {
        Type1 = &open_2[0];      	
    }
    else if( step == 6 )
    {
        Type1 = &open_3[0];      	
    }
    else if( step == 9 )
    {
        Type1 = &open_1B[0];        
    }
    else if( step == 10 )
    {
        Type1 = &open_2B[0];      	
    } 
     
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11; //bank:ana, addr:h0000
    dbbus_tx_data[2] = 0x00;    
    for( i = 0; i <= 0x3E ; i++ )
    {
        dbbus_tx_data[3+2*i] = Type1[i] & 0xFF;
        dbbus_tx_data[4+2*i] = ( Type1[i] >> 8 ) & 0xFF;    	
    }
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 3+0x3F*2);
 
    dbbus_tx_data[2] = 0x7A * 2; //bank:ana, addr:h007a
    for( i = 0x7A; i <= 0x7D ; i++ )
    {
        dbbus_tx_data[3+2*(i-0x7A)] = 0;
        dbbus_tx_data[4+2*(i-0x7A)] = 0;    	    	
    }
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 3+8);  
    
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x12; //bank:ana2, addr:h0005
      
    dbbus_tx_data[2] = 0x05 * 2;
    dbbus_tx_data[3] = Type1[128+0x05] & 0xFF;
    dbbus_tx_data[4] = ( Type1[128+0x05] >> 8 ) & 0xFF;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);
    
    dbbus_tx_data[2] = 0x0B * 2; //bank:ana2, addr:h000b
    dbbus_tx_data[3] = Type1[128+0x0B] & 0xFF;
    dbbus_tx_data[4] = ( Type1[128+0x0B] >> 8 ) & 0xFF;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);
    
    dbbus_tx_data[2] = 0x12 * 2; //bank:ana2, addr:h0012
    dbbus_tx_data[3] = Type1[128+0x12] & 0xFF;
    dbbus_tx_data[4] = ( Type1[128+0x12] >> 8 ) & 0xFF;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);
    
    dbbus_tx_data[2] = 0x15 * 2; //bank:ana2, addr:h0015
    dbbus_tx_data[3] = Type1[128+0x15] & 0xFF;
    dbbus_tx_data[4] = ( Type1[128+0x15] >> 8 ) & 0xFF;
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);        
/*
#if 1//for AC mod --showlo
        dbbus_tx_data[1] = 0x13;
        dbbus_tx_data[2] = 0x12 * 2;
        dbbus_tx_data[3] = 0X30;
        dbbus_tx_data[4] = 0X30;
        ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);        

        
        dbbus_tx_data[2] = 0x14 * 2;
        dbbus_tx_data[3] = 0X30;
        dbbus_tx_data[4] = 0X30;
        ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, 5);     

        
        dbbus_tx_data[1] = 0x12;
             for (i = 0x0D; i <= 0x10;i++ )//for AC noise(++)
	{
	    dbbus_tx_data[2] = i * 2;
	    dbbus_tx_data[3] = Type1[128+i] & 0xFF;
	    dbbus_tx_data[4] = ( Type1[128+i] >> 8 ) & 0xFF;
	    ito_test_i2c_write( ITO_TEST_ADDR_REG,  dbbus_tx_data,5 );  
	}

       for (i = 0x16; i <= 0x18; i++)//for AC noise
	{
	    dbbus_tx_data[2] = i * 2;
	    dbbus_tx_data[3] = Type1[128+i] & 0xFF;
	    dbbus_tx_data[4] = ( Type1[128+i] >> 8 ) & 0xFF;
	    ito_test_i2c_write( ITO_TEST_ADDR_REG, dbbus_tx_data,5 );  
	}
#endif
*/
}

static void ito_test_set_v( uint8_t Enable, uint8_t Prs)	
{
    uint16_t    u16RegValue;        
    
    u16RegValue = ito_test_ReadReg( 0x12, 0x08); //bank:ana2, addr:h0004
    u16RegValue = u16RegValue & 0xF1; 							
    if ( Prs == 0 )
    {
    	ito_test_WriteReg( 0x12, 0x08, u16RegValue| 0x0C); 		
    }
    else if ( Prs == 1 )
    {
    	ito_test_WriteReg( 0x12, 0x08, u16RegValue| 0x0E); 		     	
    }
    else
    {
    	ito_test_WriteReg( 0x12, 0x08, u16RegValue| 0x02); 			
    }    
    
    if ( Enable )
    {
        u16RegValue = ito_test_ReadReg( 0x11, 0x06); //bank:ana, addr:h0003  
        ito_test_WriteReg( 0x11, 0x06, u16RegValue| 0x03);   	
    }
    else
    {
        u16RegValue = ito_test_ReadReg( 0x11, 0x06);    
        u16RegValue = u16RegValue & 0xFC;					
        ito_test_WriteReg( 0x11, 0x06, u16RegValue);         
    }
}

static void ito_test_set_c( uint8_t Csub_Step )
{
    uint8_t i;
    uint8_t dbbus_tx_data[MAX_CHNL_NUM+3];
    uint8_t HighLevel_Csub = false;
    uint8_t Csub_new;
     
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x11; //bank:ana, addr:h0042       
    dbbus_tx_data[2] = 0x84;        
    for( i = 0; i < MAX_CHNL_NUM; i++ )
    {
		Csub_new = Csub_Step;        
        HighLevel_Csub = false;   
        if( Csub_new > 0x1F )
        {
            Csub_new = Csub_new - 0x14;
            HighLevel_Csub = true;
        }
           
        dbbus_tx_data[3+i] = Csub_new & 0x1F;        
        if( HighLevel_Csub == true )
        {
            dbbus_tx_data[3+i] |= BIT5;
        }
    }
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, MAX_CHNL_NUM+3);

    dbbus_tx_data[2] = 0xB4; //bank:ana, addr:h005a        
    ito_test_i2c_write(ITO_TEST_ADDR_REG, dbbus_tx_data, MAX_CHNL_NUM+3);
}

static void ito_test_sw( void )
{
    ito_test_WriteReg( 0x11, 0x00, 0xFFFF ); //bank:ana, addr:h0000
    ito_test_WriteReg( 0x11, 0x00, 0x0000 );
    mdelay( 50 );
}

static void ito_open_test_first(uint8_t item_id, int16_t* s16_raw_data, s8* data_flag)		
{
    uint8_t     loop;
    uint8_t     i, j;
    int16_t     s16_raw_data_tmp[MAX_CHNL_NUM] = {0};
    uint8_t     num_of_sensor, num_of_sensor2, total_sensor = 0;
    uint16_t	u16RegValue;
    uint8_t 	*pMapping = NULL;
    
    num_of_sensor = 0;
    num_of_sensor2 = 0;	
	
    ITO_TEST_DEBUG("ito_open_test_first() item_id=%d\n", item_id);
    // stop cpu
    ito_test_WriteReg( 0x0F, 0xE6, 0x01 ); //bank:mheg5, addr:h0073

    ito_test_WriteReg( 0x1E, 0x24, 0x0500 ); //bank:chip, addr:h0012
    ito_test_WriteReg( 0x1E, 0x2A, 0x0000 ); //bank:chip, addr:h0015
    ito_test_WriteReg( 0x1E, 0xE6, 0x6E00 ); //bank:chip, addr:h0073
    ito_test_WriteReg( 0x1E, 0xE8, 0x0071 ); //bank:chip, addr:h0074
	    
    if ( item_id == 40 )    			
    {
        pMapping = &MAP1[0];
        if ( ito_test_2r )
        {
            total_sensor = ito_test_trianglenum/2; 
        }
        else
        {
            total_sensor = ito_test_trianglenum/2 + ito_test_keynum + ito_test_dummynum;
        }
    }
    else if( item_id == 41 )    		
    {
        pMapping = &MAP2[0];
        if ( ito_test_2r )
        {
            total_sensor = ito_test_trianglenum/2; 
        }
        else
        {
            total_sensor = ito_test_trianglenum/2 + ito_test_keynum + ito_test_dummynum;
        }
    }
    else if( item_id == 42 )    		
    {
        pMapping = &MAP3[0];      
        total_sensor = ito_test_trianglenum + ito_test_keynum+ ito_test_dummynum; 
    }
        	    
    loop = 1;
    if ( item_id != 42 )
    {
	      if(total_sensor>11)
        {
            loop = 2;
        }
    }	
    
    ITO_TEST_DEBUG("loop=%d\n", loop);
	
    for ( i = 0; i < loop; i ++ )
    {
        if ( i == 0 )
        {
            ito_test_send_data_in( item_id - 36 );
        }
        else
        { 
            if ( item_id == 40 )
            { 
                ito_test_send_data_in( 9 );
            }
            else
            { 		
                ito_test_send_data_in( 10 );
            }
        }
        
        ito_test_disable_filter_noise_detect();
	
        ito_test_set_v(1,0);    
        u16RegValue = ito_test_ReadReg( 0x11, 0x0E ); //bank:ana, addr:h0007   			
        ito_test_WriteReg( 0x11, 0x0E, u16RegValue | BIT11 );				 		
	
        if ( g_LTP == 1 )
        {
	    	    ito_test_set_c( 32 );
	    }	    	
        else
        {	    	
	    	    ito_test_set_c( 0 );
        }
        
        ito_test_sw();
		
        if ( i == 0 )	 
        {      
            num_of_sensor=ito_test_get_data_out(  s16_raw_data_tmp );
            ITO_TEST_DEBUG("num_of_sensor=%d;\n",num_of_sensor);
        }
        else	
        {      
            num_of_sensor2=ito_test_get_data_out(  &s16_raw_data_tmp[num_of_sensor] );
            ITO_TEST_DEBUG("num_of_sensor=%d;num_of_sensor2=%d\n",num_of_sensor,num_of_sensor2);
        }
    }
    
    for ( j = 0; j < total_sensor; j ++ )
    {
        if ( g_LTP == 1 )
        {
            s16_raw_data[pMapping[j]] = s16_raw_data_tmp[j] + 4096;
            data_flag[pMapping[j]] = 1;
        }
        else
        {
            s16_raw_data[pMapping[j]] = s16_raw_data_tmp[j];	
            data_flag[pMapping[j]] = 1;
        }
    }	

    return;
}

typedef enum
{
	ITO_TEST_OK = 0,
	ITO_TEST_FAIL,
	ITO_TEST_GET_TP_TYPE_ERROR,
	ITO_TEST_UNDEFINED_ERROR

} ITO_TEST_RET;

ITO_TEST_RET ito_open_test_second(u8 item_id)
{
    ITO_TEST_RET ret = ITO_TEST_OK;
	u8 i = 0;
    
	s32  s16_raw_data_jg_tmp1 = 0;
	s32  s16_raw_data_jg_tmp2 = 0;
	s32  jg_tmp1_avg_Th_max =0;
	s32  jg_tmp1_avg_Th_min =0;
	s32  jg_tmp2_avg_Th_max =0;
	s32  jg_tmp2_avg_Th_min =0;

    ITO_TEST_DEBUG("ito_open_test_second() item_id=%d\n", item_id);

	if ( item_id == 40 )    			
    {
        for (i=0; i<(ito_test_trianglenum/2)-2; i++)
        {
			s16_raw_data_jg_tmp1 += s16_raw_data_1[MAP40_1[i]];
		}
		for (i=0; i<2; i++)
        {
			s16_raw_data_jg_tmp2 += s16_raw_data_1[MAP40_2[i]];
		}
    }
    else if( item_id == 41 )    		
    {
        for (i=0; i<(ito_test_trianglenum/2)-2; i++)
        {
			s16_raw_data_jg_tmp1 += s16_raw_data_2[MAP41_1[i]];
		}
		for (i=0; i<2; i++)
        {
			s16_raw_data_jg_tmp2 += s16_raw_data_2[MAP41_2[i]];
		}
    }

	    jg_tmp1_avg_Th_max = (s16_raw_data_jg_tmp1 / ((ito_test_trianglenum/2)-2)) * ( 100 + OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp1_avg_Th_min = (s16_raw_data_jg_tmp1 / ((ito_test_trianglenum/2)-2)) * ( 100 - OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
        jg_tmp2_avg_Th_max = (s16_raw_data_jg_tmp2 / 2) * ( 100 + OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp2_avg_Th_min = (s16_raw_data_jg_tmp2 / 2 ) * ( 100 - OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
	
        ITO_TEST_DEBUG("item_id=%d;sum1=%d;max1=%d;min1=%d;sum2=%d;max2=%d;min2=%d\n",item_id,s16_raw_data_jg_tmp1,jg_tmp1_avg_Th_max,jg_tmp1_avg_Th_min,s16_raw_data_jg_tmp2,jg_tmp2_avg_Th_max,jg_tmp2_avg_Th_min);

	if ( item_id == 40 ) 
	{
		for (i=0; i<(ito_test_trianglenum/2)-2; i++)
		{
			if (s16_raw_data_1[MAP40_1[i]] > jg_tmp1_avg_Th_max || s16_raw_data_1[MAP40_1[i]] < jg_tmp1_avg_Th_min)
			{
				g_fail_channel[fail_channel_count] = MAP40_1[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		}
		
		for (i=0; i<(ito_test_trianglenum/2)-3; i++)//modify: 
		{
            if (s16_raw_data_1[MAP40_1[i]] > s16_raw_data_1[MAP40_1[i+1]])
            { 
                g_fail_channel[fail_channel_count] = MAP40_1[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
		}
		
		for (i=0; i<2; i++)
		{
			if (s16_raw_data_1[MAP40_2[i]] > jg_tmp2_avg_Th_max || s16_raw_data_1[MAP40_2[i]] < jg_tmp2_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP40_2[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		} 
	}

	if ( item_id == 41 ) 
	{
		for (i=0; i<(ito_test_trianglenum/2)-2; i++)
		{
			if (s16_raw_data_2[MAP41_1[i]] > jg_tmp1_avg_Th_max || s16_raw_data_2[MAP41_1[i]] < jg_tmp1_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP41_1[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}	
		}
    
        for (i=0; i<(ito_test_trianglenum/2)-3; i++)//modify: 
        {
            if (s16_raw_data_2[MAP41_1[i]] < s16_raw_data_2[MAP41_1[i+1]])
            { 
                g_fail_channel[fail_channel_count] = MAP41_1[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
        }

		for (i=0; i<2; i++)
		{
			if (s16_raw_data_2[MAP41_2[i]] > jg_tmp2_avg_Th_max || s16_raw_data_2[MAP41_2[i]] < jg_tmp2_avg_Th_min)
			{ 
				g_fail_channel[fail_channel_count] = MAP41_2[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		} 
	}

	return ret;
}

ITO_TEST_RET ito_open_test_second_2r (u8 item_id)
{
    ITO_TEST_RET ret = ITO_TEST_OK;
	u8 i = 0;
    
	s32  s16_raw_data_jg_tmp1 = 0;
	s32  s16_raw_data_jg_tmp2 = 0;
	s32  s16_raw_data_jg_tmp3 = 0;
	s32  s16_raw_data_jg_tmp4 = 0;
	
	s32  jg_tmp1_avg_Th_max =0;
	s32  jg_tmp1_avg_Th_min =0;
	s32  jg_tmp2_avg_Th_max =0;
	s32  jg_tmp2_avg_Th_min =0;
	s32  jg_tmp3_avg_Th_max =0;
	s32  jg_tmp3_avg_Th_min =0;
	s32  jg_tmp4_avg_Th_max =0;
	s32  jg_tmp4_avg_Th_min =0;

	if ( item_id == 40 )    			
  {
    for (i=0; i<(ito_test_trianglenum/4)-2; i++)
    {
      s16_raw_data_jg_tmp1 += s16_raw_data_1[MAP40_1[i]];  //first region: non-border 
		}
		
		for (i=0; i<2; i++)
		{
		  s16_raw_data_jg_tmp2 += s16_raw_data_1[MAP40_2[i]];  //first region: border
		}

		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
		  s16_raw_data_jg_tmp3 += s16_raw_data_1[MAP40_3[i]];  //second region: non-border
		}
		
		for (i=0; i<2; i++)
		{
		  s16_raw_data_jg_tmp4 += s16_raw_data_1[MAP40_4[i]];  //second region: border
		}
  }
  else if( item_id == 41 )    		
  {
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
		  s16_raw_data_jg_tmp1 += s16_raw_data_2[MAP41_1[i]];  //first region: non-border
		}
		
		for (i=0; i<2; i++)
		{
		  s16_raw_data_jg_tmp2 += s16_raw_data_2[MAP41_2[i]];  //first region: border
		}
		
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
		  s16_raw_data_jg_tmp3 += s16_raw_data_2[MAP41_3[i]];  //second region: non-border
		}
		
		for (i=0; i<2; i++)
		{
		  s16_raw_data_jg_tmp4 += s16_raw_data_2[MAP41_4[i]];  //second region: border
		}
	}

	    jg_tmp1_avg_Th_max = (s16_raw_data_jg_tmp1 / ((ito_test_trianglenum/4)-2)) * ( 100 + OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp1_avg_Th_min = (s16_raw_data_jg_tmp1 / ((ito_test_trianglenum/4)-2)) * ( 100 - OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
        jg_tmp2_avg_Th_max = (s16_raw_data_jg_tmp2 / 2) * ( 100 + OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp2_avg_Th_min = (s16_raw_data_jg_tmp2 / 2) * ( 100 - OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
		jg_tmp3_avg_Th_max = (s16_raw_data_jg_tmp3 / ((ito_test_trianglenum/4)-2)) * ( 100 + OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp3_avg_Th_min = (s16_raw_data_jg_tmp3 / ((ito_test_trianglenum/4)-2)) * ( 100 - OPEN_TEST_NON_BORDER_AREA_THRESHOLD) / 100 ;
        jg_tmp4_avg_Th_max = (s16_raw_data_jg_tmp4 / 2) * ( 100 + OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
	    jg_tmp4_avg_Th_min = (s16_raw_data_jg_tmp4 / 2) * ( 100 - OPEN_TEST_BORDER_AREA_THRESHOLD) / 100 ;
		
	
        ITO_TEST_DEBUG("item_id=%d;sum1=%d;max1=%d;min1=%d;sum2=%d;max2=%d;min2=%d;sum3=%d;max3=%d;min3=%d;sum4=%d;max4=%d;min4=%d;\n",item_id,s16_raw_data_jg_tmp1,jg_tmp1_avg_Th_max,jg_tmp1_avg_Th_min,s16_raw_data_jg_tmp2,jg_tmp2_avg_Th_max,jg_tmp2_avg_Th_min,s16_raw_data_jg_tmp3,jg_tmp3_avg_Th_max,jg_tmp3_avg_Th_min,s16_raw_data_jg_tmp4,jg_tmp4_avg_Th_max,jg_tmp4_avg_Th_min);


	if ( item_id == 40 ) 
	{
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
			if (s16_raw_data_1[MAP40_1[i]] > jg_tmp1_avg_Th_max || s16_raw_data_1[MAP40_1[i]] < jg_tmp1_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP40_1[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		}
		
		for (i=0; i<2; i++)
		{
			if (s16_raw_data_1[MAP40_2[i]] > jg_tmp2_avg_Th_max || s16_raw_data_1[MAP40_2[i]] < jg_tmp2_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP40_2[i];				
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}	
		} 
		
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
			if (s16_raw_data_1[MAP40_3[i]] > jg_tmp3_avg_Th_max || s16_raw_data_1[MAP40_3[i]] < jg_tmp3_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP40_3[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		}
		
		for (i=0; i<2; i++)
		{
			if (s16_raw_data_1[MAP40_4[i]] > jg_tmp4_avg_Th_max || s16_raw_data_1[MAP40_4[i]] < jg_tmp4_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP40_4[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		} 
	}

	if ( item_id == 41 ) 
	{
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
			if (s16_raw_data_2[MAP41_1[i]] > jg_tmp1_avg_Th_max || s16_raw_data_2[MAP41_1[i]] < jg_tmp1_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP41_1[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}	
		}
		
		for (i=0; i<2; i++)
		{
			if (s16_raw_data_2[MAP41_2[i]] > jg_tmp2_avg_Th_max || s16_raw_data_2[MAP41_2[i]] < jg_tmp2_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP41_2[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}
		}
		
		for (i=0; i<(ito_test_trianglenum/4)-2; i++)
		{
			if (s16_raw_data_2[MAP41_3[i]] > jg_tmp3_avg_Th_max || s16_raw_data_2[MAP41_3[i]] < jg_tmp3_avg_Th_min) 
			{	
				g_fail_channel[fail_channel_count] = MAP41_3[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}	
		}
		
		for (i=0; i<2; i++)
		{
			if (s16_raw_data_2[MAP41_4[i]] > jg_tmp4_avg_Th_max || s16_raw_data_2[MAP41_4[i]] < jg_tmp4_avg_Th_min) 
			{
				g_fail_channel[fail_channel_count] = MAP41_4[i];
				fail_channel_count ++; 
				ret = ITO_TEST_FAIL;
			}	
		} 
	}

	return ret;
}

static ITO_TEST_RET ito_open_test_interface(void)
{
    ITO_TEST_RET ret1 = ITO_TEST_OK, ret2 = ITO_TEST_OK, ret3 = ITO_TEST_OK;
    uint16_t i = 0;
#ifdef DMA_IIC
    _msg_dma_alloc();
#endif
	I2cDMA_init();
    ITO_TEST_DEBUG("open test start\n");

    ito_test_set_iic_rate(50000);
    ito_test_disable_irq();
    ito_test_reset();
    if(!ito_test_choose_TpType())
    {
        ITO_TEST_DEBUG("choose tpType fail\n");
        ret1 = ITO_TEST_GET_TP_TYPE_ERROR;
        goto ITO_TEST_END;
    }
    ito_test_EnterSerialDebugMode();
    mdelay(100);
    ITO_TEST_DEBUG("EnterSerialDebugMode\n");
    // stop cpu
    ito_test_WriteReg8Bit ( 0x0F, 0xE6, 0x01 ); //bank:mheg5, addr:h0073
    // stop watch dog
    ito_test_WriteReg ( 0x3C, 0x60, 0xAA55 ); //bank:reg_PIU_MISC_0, addr:h0030
    ITO_TEST_DEBUG("stop mcu and disable watchdog V.005\n");   
    mdelay(50);
    
    for(i = 0;i < MAX_CHNL_NUM;i++)
    {
        s16_raw_data_1[i] = 0;
        s16_raw_data_2[i] = 0;
        s16_raw_data_3[i] = 0;
        data_flag_1[i] = 0;
        data_flag_2[i] = 0;
        data_flag_3[i] = 0;
    }	
	
    fail_channel_count = 0; // Reset fail_channel_count to 0 before test start
	
    ito_open_test_first(40, s16_raw_data_1, data_flag_1);
    ITO_TEST_DEBUG("40 get s16_raw_data_1\n");
    if(ito_test_2r)
    {
        ret2=ito_open_test_second_2r(40);
    }
    else
    {
        ret2=ito_open_test_second(40);
    }
    
    ito_open_test_first(41, s16_raw_data_2, data_flag_2);
    ITO_TEST_DEBUG("41 get s16_raw_data_2\n");
    if(ito_test_2r)
    {
        ret3=ito_open_test_second_2r(41);
    }
    else
    {
        ret3=ito_open_test_second(41);
    }
    
    //ito_open_test_first(42, s16_raw_data_3, data_flag_3);
    //ITO_TEST_DEBUG("42 get s16_raw_data_3\n");
    
    ITO_TEST_END:
#ifdef DMA_IIC
    _msg_dma_free();
#endif
	I2cDMA_exit();
    ito_test_set_iic_rate(100000);
    ito_test_reset();
    ito_test_enable_irq();
    ITO_TEST_DEBUG("open test end\n");
    
    if ((ret1 != ITO_TEST_OK) && (ret2 == ITO_TEST_OK) && (ret3 == ITO_TEST_OK))
    {
        return ITO_TEST_GET_TP_TYPE_ERROR;		
    }
    else if ((ret1 == ITO_TEST_OK) && ((ret2 != ITO_TEST_OK) || (ret3 != ITO_TEST_OK)))
    {
        return ITO_TEST_FAIL;	
    }
    else
    {
        return ITO_TEST_OK;	
    }
}

static void ito_short_test_change_GPO_setting(u8 item_id)
{
    u8 dbbus_tx_data[3+GPIO_SETTING_SIZE*2] = {0};
    u16 gpoSettings[3] = {0};
    u32 i;
    
    ITO_TEST_DEBUG("ito_short_test_change_GPO_setting() item_id=%d\n", item_id);
    
    if (item_id == 0) // 39-4
    {
        gpoSettings[0] = short_4_GPO[0];		
        gpoSettings[1] = short_4_GPO[1];		
        gpoSettings[2] = short_4_GPO[2];		
        gpoSettings[2] |= (1 << (int)(PIN_GUARD_RING % 16));
    }
    else if (item_id == 1) // 39-1
    {
        gpoSettings[0] = short_1_GPO[0];		
        gpoSettings[1] = short_1_GPO[1];		
        gpoSettings[2] = short_1_GPO[2];		
        gpoSettings[2] |= (1 << (int)(PIN_GUARD_RING % 16));
    }
    else if (item_id == 2) // 39-2
    {
        gpoSettings[0] = short_2_GPO[0];		
        gpoSettings[1] = short_2_GPO[1];		
        gpoSettings[2] = short_2_GPO[2];		
        gpoSettings[2] |= (1 << (int)(PIN_GUARD_RING % 16));
    }
    else if (item_id == 3) // 39-3
    {
        gpoSettings[0] = short_3_GPO[0];		
        gpoSettings[1] = short_3_GPO[1];		
        gpoSettings[2] = short_3_GPO[2];		
        gpoSettings[2] |= (1 << (int)(PIN_GUARD_RING % 16));
    }
    else
    {
        ITO_TEST_DEBUG("Invalid item id for changing GPIO setting of short test.\n");

        return;
    }

    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x12;
    dbbus_tx_data[2] = 0x48;

    for (i = 0; i < GPIO_SETTING_SIZE; i ++)
    {
        dbbus_tx_data[3+2*i] = gpoSettings[i] & 0xFF;
        dbbus_tx_data[4+2*i] = (gpoSettings[i] >> 8) & 0xFF;    	
    }

    ito_test_i2c_write(ITO_TEST_ADDR_REG, &dbbus_tx_data[0], 3+GPIO_SETTING_SIZE*2);    
}

static void ito_short_test_change_Rmode_setting(uint8_t mode)
{
    uint8_t dbbus_tx_data[6];

    ITO_TEST_DEBUG("ito_short_test_change_Rmode_setting() mode=%d\n", mode);

    // AFE R-mode enable(Bit-12)
    ito_test_WriteReg8Bit( 0x11, 0x03, 0x10 );

    // drv_mux_OV (Bit-8 1:enable)
    ito_test_WriteReg8Bit( 0x11, 0x07, 0x55 );
    
    if (mode == 1) // P_CODE: 0V
    {
        ito_test_WriteReg( 0x11, 0x0E, 0x073A );
    }
    else if (mode == 0) // N_CODE: 2.4V
    {
        ito_test_WriteReg( 0x11, 0x0E, 0x073B );
    }

    // SW2 rising & SW3 rising return to 0
    ito_test_WriteReg8Bit( 0x12, 0x27, 0x01 );
    // turn off the chopping
    ito_test_WriteReg8Bit( 0x12, 0x08, 0x0C );
    // idle driver ov
    ito_test_WriteReg8Bit( 0x12, 0x41, 0xC0 );
	  
	  // AFE ov
    dbbus_tx_data[0] = 0x10;
    dbbus_tx_data[1] = 0x12;
    dbbus_tx_data[2] = 0x44;
    dbbus_tx_data[3] = 0xFF;
    dbbus_tx_data[4] = 0xFF;
    dbbus_tx_data[5] = 0xFF;

    ito_test_i2c_write(ITO_TEST_ADDR_REG, &dbbus_tx_data[0], 6);        
}	

static void ito_short_test_first(uint8_t item_id , int16_t* s16_raw_data, s8* data_flag)		
{
    uint8_t     i;
    int16_t     s16_raw_data_tmp[MAX_CHNL_NUM] = {0};
    int16_t     s16_raw_data_tmp2[MAX_CHNL_NUM] = {0};
    uint8_t     num_of_sensor, num_of_sensor2, num_of_sensor_mapping_1, num_of_sensor_mapping_2, sensor_count = 0;
    uint8_t 	*pMapping = NULL;
    

    ITO_TEST_DEBUG("ito_short_test_first() item_id=%d\n", item_id);
    // stop cpu
    ito_test_WriteReg( 0x0F, 0xE6, 0x01 ); //bank:mheg5, addr:h0073
    // chip top op0
    ito_test_WriteReg( 0x1E, 0x24, 0x0500 ); //bank:chip, addr:h0012
    ito_test_WriteReg( 0x1E, 0x2A, 0x0000 ); //bank:chip, addr:h0015
    ito_test_WriteReg( 0x1E, 0xE6, 0x6E00 ); //bank:chip, addr:h0073
    ito_test_WriteReg( 0x1E, 0xE8, 0x0071 ); //bank:chip, addr:h0074
	    
    if ((ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) % 2 != 0)
    {
        num_of_sensor_mapping_1 = (ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) / 2 + 1;
        num_of_sensor_mapping_2 = num_of_sensor_mapping_1;
    }
    else
    {
        num_of_sensor_mapping_1 = (ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) / 2;
        num_of_sensor_mapping_2 = num_of_sensor_mapping_1;
        if (num_of_sensor_mapping_2 % 2 != 0)
        {	
            num_of_sensor_mapping_2 ++;
        }
    }        

    if ( item_id == 0 ) // 39-4 (2R)    			
    {
        pMapping = &SHORT_MAP4[0];
        sensor_count = ito_test_trianglenum/2; 
    }
    else if( item_id == 1 ) // 39-1    			    		
    {
        pMapping = &SHORT_MAP1[0];
        sensor_count = num_of_sensor_mapping_1; 
    }
    else if( item_id == 2 ) // 39-2   		
    {
        pMapping = &SHORT_MAP2[0];      
        sensor_count = num_of_sensor_mapping_2; 
    }
    else if( item_id == 3 ) // 39-3    		
    {
        pMapping = &SHORT_MAP3[0];      
        sensor_count = ito_test_trianglenum; 
    }
    ITO_TEST_DEBUG("sensor_count=%d\n", sensor_count);
        	    
    ito_test_send_data_in( item_id );
    
    ito_test_disable_filter_noise_detect();

    ito_short_test_change_Rmode_setting(1);
    ito_short_test_change_GPO_setting(item_id);
    ito_test_sw();

    num_of_sensor = ito_test_get_data_out(  s16_raw_data_tmp );
    ITO_TEST_DEBUG("num_of_sensor=%d\n", num_of_sensor);

    ito_short_test_change_Rmode_setting(0);
    ito_short_test_change_GPO_setting(item_id);
    ito_test_sw();

    num_of_sensor2 = ito_test_get_data_out(  s16_raw_data_tmp2 );
    ITO_TEST_DEBUG("num_of_sensor2=%d\n", num_of_sensor2);
    
    for ( i = 0; i < sensor_count; i ++ )
    {
        s16_raw_data[pMapping[i]] = s16_raw_data_tmp[i] - s16_raw_data_tmp2[i];	
        data_flag[pMapping[i]] = 1;
    }	
}

static ITO_TEST_RET ito_short_test_second(u8 item_id)
{
    ITO_TEST_RET ret = ITO_TEST_OK;
    u8 i;
    u8 num_of_sensor_mapping_1, num_of_sensor_mapping_2, sensor_count = 0;
	
    ITO_TEST_DEBUG("ito_short_test_second() item_id=%d\n", item_id);

    if ((ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) % 2 != 0)
    {
        num_of_sensor_mapping_1 = (ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) / 2 + 1;
        num_of_sensor_mapping_2 = num_of_sensor_mapping_1;
    }
    else
    {
        num_of_sensor_mapping_1 = (ito_test_trianglenum + ito_test_keynum + ito_test_dummynum) / 2;
        num_of_sensor_mapping_2 = num_of_sensor_mapping_1;
        if (num_of_sensor_mapping_2 % 2 != 0)
        {	
            num_of_sensor_mapping_2 ++;
        }
    }        

    if ( item_id == 0 ) // 39-4 (2R)   
    {
        sensor_count = ito_test_trianglenum/2;
        
        for (i = 0; i < sensor_count; i ++)
        {
            if (s16_raw_data_4[SHORT_MAP4[i]] > SHORT_TEST_THRESHOLD)
            {
                g_fail_channel[fail_channel_count] = SHORT_MAP4[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
        }
    }
    else if ( item_id == 1 ) // 39-1
    {
        sensor_count = num_of_sensor_mapping_1;
        
        for (i = 0; i < sensor_count; i ++)
        {
            if (s16_raw_data_1[SHORT_MAP1[i]] > SHORT_TEST_THRESHOLD)
            {
                g_fail_channel[fail_channel_count] = SHORT_MAP1[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
        }
    }
    else if ( item_id == 2 ) // 39-2
    {
        sensor_count = num_of_sensor_mapping_2;
        
        for (i = 0; i < sensor_count; i ++)
        {
            if (s16_raw_data_2[SHORT_MAP2[i]] > SHORT_TEST_THRESHOLD)
            {
                g_fail_channel[fail_channel_count] = SHORT_MAP2[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
        }
    }
    else if ( item_id == 3 ) // 39-3
    {
        sensor_count = ito_test_trianglenum;
        
        for (i = 0; i < sensor_count; i ++)
        {
            if (s16_raw_data_3[SHORT_MAP3[i]] > SHORT_TEST_THRESHOLD)
            {
                g_fail_channel[fail_channel_count] = SHORT_MAP3[i];
                fail_channel_count ++; 
                ret = ITO_TEST_FAIL;
            }
        }
    }
    ITO_TEST_DEBUG("sensor_count=%d\n", sensor_count);

    return ret;
}

static ITO_TEST_RET ito_short_test_interface(void)
{
    ITO_TEST_RET ret1 = ITO_TEST_OK, ret2 = ITO_TEST_OK, ret3 = ITO_TEST_OK, ret4 = ITO_TEST_OK, ret5 = ITO_TEST_OK;
    u16 i = 0;
#ifdef DMA_IIC
    _msg_dma_alloc();
#endif
I2cDMA_init();

    ITO_TEST_DEBUG("short test start\n");

    ito_test_set_iic_rate(50000);
    
    ito_test_disable_irq();
    ito_test_reset();
    if(!ito_test_choose_TpType())
    {
        ITO_TEST_DEBUG("choose tpType fail\n");
        ret1 = ITO_TEST_GET_TP_TYPE_ERROR;
        goto ITO_TEST_END;
    }
    ito_test_EnterSerialDebugMode();
    mdelay(100);
    ITO_TEST_DEBUG("EnterSerialDebugMode\n");
    // stop cpu
    ito_test_WriteReg8Bit ( 0x0F, 0xE6, 0x01 ); //bank:mheg5, addr:h0073
    // stop watch dog
    ito_test_WriteReg ( 0x3C, 0x60, 0xAA55 ); //bank:reg_PIU_MISC_0, addr:h0030
    ITO_TEST_DEBUG("stop mcu and disable watchdog V.005\n");   
    mdelay(50);
    
    for(i = 0; i < MAX_CHNL_NUM; i ++)
    {
        s16_raw_data_1[i] = 0;
        s16_raw_data_2[i] = 0;
        s16_raw_data_3[i] = 0;
        s16_raw_data_4[i] = 0;
        data_flag_1[i] = 0;
        data_flag_2[i] = 0;
        data_flag_3[i] = 0;
        data_flag_4[i] = 0;
    }	
	
    fail_channel_count = 0; // Reset fail_channel_count to 0 before test start
	
    ito_short_test_first(1, s16_raw_data_1, data_flag_1);
    ITO_TEST_DEBUG("1 get s16_raw_data_1\n");
    ret2 = ito_short_test_second(1);
    
    ito_short_test_first(2, s16_raw_data_2, data_flag_2);
    ITO_TEST_DEBUG("2 get s16_raw_data_2\n");
    ret3 = ito_short_test_second(2);

    ito_short_test_first(3, s16_raw_data_3, data_flag_3);
    ITO_TEST_DEBUG("3 get s16_raw_data_3\n");
    ret4 = ito_short_test_second(3);
    
    if(ito_test_2r)
    {
        ito_short_test_first(0, s16_raw_data_4, data_flag_4);
        ITO_TEST_DEBUG("0 get s16_raw_data_4\n");
        ret5 = ito_short_test_second(0);
    }

    ITO_TEST_END:
#ifdef DMA_IIC
    _msg_dma_free();
#endif
I2cDMA_exit();
    ito_test_set_iic_rate(100000);
    ito_test_reset();
    ito_test_enable_irq();
    ITO_TEST_DEBUG("short test end\n");
    
    if ((ret1 != ITO_TEST_OK) && (ret2 == ITO_TEST_OK) && (ret3 == ITO_TEST_OK) && (ret4 == ITO_TEST_OK) && (ret5 == ITO_TEST_OK))
    {
        return ITO_TEST_GET_TP_TYPE_ERROR;		
    }
    else if ((ret1 == ITO_TEST_OK) && ((ret2 != ITO_TEST_OK) || (ret3 != ITO_TEST_OK) || (ret4 != ITO_TEST_OK) || (ret5 != ITO_TEST_OK)))
    {
        return ITO_TEST_FAIL;	
    }
    else
    {
        return ITO_TEST_OK;	
    }
}

#include <linux/proc_fs.h>
#define ITO_TEST_AUTHORITY 0777 
static struct proc_dir_entry *msg_ito_test = NULL;
static struct proc_dir_entry *debug = NULL;
static struct proc_dir_entry *debug_on_off = NULL;
static struct proc_dir_entry *open_test = NULL;
static struct proc_dir_entry *short_test = NULL;
static struct proc_dir_entry *fail_channel = NULL;
static struct proc_dir_entry *data = NULL;
#define PROC_MSG_ITO_TEST      "msg-ito-test"
#define PROC_ITO_TEST_DEBUG      "debug"
#define PROC_ITO_TEST_DEBUG_ON_OFF     "debug-on-off"
#define PROC_ITO_TEST_OPEN     "open"
#define PROC_ITO_TEST_SHORT     "short"
#define PROC_ITO_TEST_FAIL_CHANNEL     "fail-channel"
#define PROC_ITO_TEST_DATA      "data"
ITO_TEST_RET g_ito_test_ret = ITO_TEST_OK;

static int ito_test_proc_read_debug(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt = 0;
    
    cnt = sprintf(page, "%d", g_ito_test_ret);

    *eof = 1;

    return cnt;
}

static int ito_test_proc_write_debug(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    u32 mode = 0;
    u32 i = 0;
    
    ITO_TEST_DEBUG_MUST("buffer = %s\n", buffer);

    if (buffer != NULL)
    {
        sscanf(buffer, "%x", &mode);   

        ITO_TEST_DEBUG_MUST("Mp Test Mode = 0x%x\n", mode);

        if (mode == ITO_TEST_MODE_OPEN_TEST) //open test
        {
            ito_test_mode = ITO_TEST_MODE_OPEN_TEST;
            g_ito_test_ret = ito_open_test_interface();
        }
        else if (mode == ITO_TEST_MODE_SHORT_TEST) //short test
        {
            ito_test_mode = ITO_TEST_MODE_SHORT_TEST;
            g_ito_test_ret = ito_short_test_interface();
        }
        else
        {
            ITO_TEST_DEBUG_MUST("*** Undefined MP Test Mode ***\n");

            g_ito_test_ret = ITO_TEST_UNDEFINED_ERROR;
        }
    }

    if(ITO_TEST_OK==g_ito_test_ret)
    {
        ITO_TEST_DEBUG_MUST("ITO_TEST_OK");
    }
    else if(ITO_TEST_FAIL==g_ito_test_ret)
    {
        ITO_TEST_DEBUG_MUST("ITO_TEST_FAIL");
    }
    else if(ITO_TEST_GET_TP_TYPE_ERROR==g_ito_test_ret)
    {
        ITO_TEST_DEBUG_MUST("ITO_TEST_GET_TP_TYPE_ERROR");
    }
    else if(ITO_TEST_UNDEFINED_ERROR==g_ito_test_ret)
    {
        ITO_TEST_DEBUG_MUST("ITO_TEST_UNDEFINED_ERROR");
    }

    ITO_TEST_DEBUG_MUST("ito_test_ret = %d",g_ito_test_ret);
    mdelay(5);

    for(i=0;i<MAX_CHNL_NUM;i++)
    {
        ITO_TEST_DEBUG_MUST("data_1[%d]=%d;\n",i,s16_raw_data_1[i]);
    }
    mdelay(5);
    for(i=0;i<MAX_CHNL_NUM;i++)
    {
        ITO_TEST_DEBUG_MUST("data_2[%d]=%d;\n",i,s16_raw_data_2[i]);
    }
    mdelay(5);
    for(i=0;i<MAX_CHNL_NUM;i++)
    {
        ITO_TEST_DEBUG_MUST("data_3[%d]=%d;\n",i,s16_raw_data_3[i]);
    }
    mdelay(5);
    
    if (ito_test_mode == ITO_TEST_MODE_SHORT_TEST && ito_test_2r == 1)
    {
        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_4[%d]=%d;\n",i,s16_raw_data_4[i]);
        }
        mdelay(5);
    }

    return count;
}

static int ito_test_proc_read_debug_on_off(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt= 0;
    
    bItoTestDebug = 1;
    ITO_TEST_DEBUG_MUST("on debug bItoTestDebug = %d",bItoTestDebug);
    
    *eof = 1;
    return cnt;
}

static int ito_test_proc_write_debug_on_off(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    bItoTestDebug = 0;
    ITO_TEST_DEBUG_MUST("off debug bItoTestDebug = %d",bItoTestDebug);
    return count;
}

static int ito_test_proc_read_open(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt = 0;

    ITO_TEST_DEBUG_MUST("ito_test_proc_read_open()\n");
    
    cnt = sprintf(page, "%d", g_ito_test_ret);

    *eof = 1;

    return cnt;
}

static int ito_test_proc_write_open(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    u32 i = 0;
    
    ITO_TEST_DEBUG_MUST("ito_test_proc_write_open()\n");
    ITO_TEST_DEBUG_MUST("buffer = %s\n", buffer);

    if (buffer != NULL)
    {
        ITO_TEST_DEBUG_MUST("ITO Open Test\n");

        ito_test_mode = ITO_TEST_MODE_OPEN_TEST;
        g_ito_test_ret = ito_open_test_interface();

        if(ITO_TEST_OK==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_OK");
        }
        else if(ITO_TEST_FAIL==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_FAIL");
        }
        else if(ITO_TEST_GET_TP_TYPE_ERROR==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_GET_TP_TYPE_ERROR");
        }
        else if(ITO_TEST_UNDEFINED_ERROR==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_UNDEFINED_ERROR");
        }

        ITO_TEST_DEBUG_MUST("ito_test_ret = %d",g_ito_test_ret);
        mdelay(5);

        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_1[%d]=%d;\n",i,s16_raw_data_1[i]);
        }
        mdelay(5);
        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_2[%d]=%d;\n",i,s16_raw_data_2[i]);
        }
        mdelay(5);
        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_3[%d]=%d;\n",i,s16_raw_data_3[i]);
        }
        mdelay(5);
    }

    return count;
}

static int ito_test_proc_read_short(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt = 0;

    ITO_TEST_DEBUG_MUST("ito_test_proc_read_short()\n");
    
    cnt = sprintf(page, "%d", g_ito_test_ret);

    *eof = 1;

    return cnt;
}

static int ito_test_proc_write_short(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    u32 i = 0;
    
    ITO_TEST_DEBUG_MUST("ito_test_proc_write_short()\n");
    ITO_TEST_DEBUG_MUST("buffer = %s\n", buffer);

    if (buffer != NULL)
    {
        ITO_TEST_DEBUG_MUST("ITO Short Test\n");

        ito_test_mode = ITO_TEST_MODE_SHORT_TEST;
        g_ito_test_ret = ito_short_test_interface();

        if(ITO_TEST_OK==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_OK");
        }
        else if(ITO_TEST_FAIL==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_FAIL");
        }
        else if(ITO_TEST_GET_TP_TYPE_ERROR==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_GET_TP_TYPE_ERROR");
        }
        else if(ITO_TEST_UNDEFINED_ERROR==g_ito_test_ret)
        {
            ITO_TEST_DEBUG_MUST("ITO_TEST_UNDEFINED_ERROR");
        }

        ITO_TEST_DEBUG_MUST("ito_test_ret = %d",g_ito_test_ret);
        mdelay(5);

        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_1[%d]=%d;\n",i,s16_raw_data_1[i]);
        }
        mdelay(5);
        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_2[%d]=%d;\n",i,s16_raw_data_2[i]);
        }
        mdelay(5);
        for(i=0;i<MAX_CHNL_NUM;i++)
        {
            ITO_TEST_DEBUG_MUST("data_3[%d]=%d;\n",i,s16_raw_data_3[i]);
        }
        mdelay(5);
    
        if (ito_test_mode == ITO_TEST_MODE_SHORT_TEST && ito_test_2r == 1)
        {
            for(i=0;i<MAX_CHNL_NUM;i++)
            {
                ITO_TEST_DEBUG_MUST("data_4[%d]=%d;\n",i,s16_raw_data_4[i]);
            }
            mdelay(5);
        }
    }

    return count;
}

static int ito_test_proc_read_fail_channel(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt = 0;
    int i;

    ITO_TEST_DEBUG_MUST("ito_test_proc_read_fail_channel()\n");
    ITO_TEST_DEBUG_MUST("fail_channel_count = %d\n", fail_channel_count);
    
    for (i = 0; i < fail_channel_count; i ++)
    {
    	  page[i] = g_fail_channel[i];
    }

    *eof = 1;

    cnt = fail_channel_count;

    return cnt;
}

static int ito_test_proc_write_fail_channel(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    ITO_TEST_DEBUG_MUST("ito_test_proc_write_fail_channel()\n");

    return count;
}

static int ito_test_proc_read_data(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt = 0;
    int i;
    u8 high_byte, low_byte;

    ITO_TEST_DEBUG_MUST("ito_test_proc_read_data()\n");
    
    if (ito_test_mode == ITO_TEST_MODE_OPEN_TEST)
    {
        for (i = 0; i < MAX_CHNL_NUM; i ++)
        {
            high_byte = (s16_raw_data_1[i] >> 8) & 0xFF;
            low_byte = (s16_raw_data_1[i]) & 0xFF;
    	  
            if (data_flag_1[i] == 1)
            {
                page[i*4] = 1; // indicate it is a on-use channel number
            }
            else
            {
                page[i*4] = 0; // indicate it is a non-use channel number
            }
            
            if (s16_raw_data_1[i] >= 0)
            {
                page[i*4+1] = 0; // + : a positive number
            }
            else
            {
                page[i*4+1] = 1; // - : a negative number
            }
			
            page[i*4+2] = high_byte;
            page[i*4+3] = low_byte;
        }

        for (i = 0; i < MAX_CHNL_NUM; i ++)
        {
            high_byte = (s16_raw_data_2[i] >> 8) & 0xFF;
            low_byte = (s16_raw_data_2[i]) & 0xFF;
        
            if (data_flag_2[i] == 1)
            {
                page[i*4+MAX_CHNL_NUM*4] = 1; // indicate it is a on-use channel number
            }
            else
            {
                page[i*4+MAX_CHNL_NUM*4] = 0; // indicate it is a non-use channel number
            }

            if (s16_raw_data_2[i] >= 0)
            {
                page[(i*4+1)+MAX_CHNL_NUM*4] = 0; // + : a positive number
            }
            else
            {
                page[(i*4+1)+MAX_CHNL_NUM*4] = 1; // - : a negative number
            }

            page[(i*4+2)+MAX_CHNL_NUM*4] = high_byte;
            page[(i*4+3)+MAX_CHNL_NUM*4] = low_byte;
        }

        cnt = MAX_CHNL_NUM*8;
    }
    else if (ito_test_mode == ITO_TEST_MODE_SHORT_TEST)
    {
        for (i = 0; i < MAX_CHNL_NUM; i ++)
        {
            high_byte = (s16_raw_data_1[i] >> 8) & 0xFF;
            low_byte = (s16_raw_data_1[i]) & 0xFF;

            if (data_flag_1[i] == 1)
            {
                page[i*4] = 1; // indicate it is a on-use channel number
            }
            else
            {
                page[i*4] = 0; // indicate it is a non-use channel number
            }

            if (s16_raw_data_1[i] >= 0)
            {
                page[i*4+1] = 0; // + : a positive number
            }
            else
            {
                page[i*4+1] = 1; // - : a negative number
            }
			
            page[i*4+2] = high_byte;
            page[i*4+3] = low_byte;
        }

        for (i = 0; i < MAX_CHNL_NUM; i ++)
        {
            high_byte = (s16_raw_data_2[i] >> 8) & 0xFF;
            low_byte = (s16_raw_data_2[i]) & 0xFF;
        
            if (data_flag_2[i] == 1)
            {
                page[i*4+MAX_CHNL_NUM*4] = 1; // indicate it is a on-use channel number
            }
            else
            {
                page[i*4+MAX_CHNL_NUM*4] = 0; // indicate it is a non-use channel number
            }

            if (s16_raw_data_2[i] >= 0)
            {
                page[(i*4+1)+MAX_CHNL_NUM*4] = 0; // + : a positive number
            }
            else
            {
                page[(i*4+1)+MAX_CHNL_NUM*4] = 1; // - : a negative number
            }

            page[i*4+2+MAX_CHNL_NUM*4] = high_byte;
            page[i*4+3+MAX_CHNL_NUM*4] = low_byte;
        }

        for (i = 0; i < MAX_CHNL_NUM; i ++)
        {
            high_byte = (s16_raw_data_3[i] >> 8) & 0xFF;
            low_byte = (s16_raw_data_3[i]) & 0xFF;
        
            if (data_flag_3[i] == 1)
            {
                page[i*4+MAX_CHNL_NUM*8] = 1; // indicate it is a on-use channel number
            }
            else
            {
                page[i*4+MAX_CHNL_NUM*8] = 0; // indicate it is a non-use channel number
            }

            if (s16_raw_data_3[i] >= 0)
            {
                page[(i*4+1)+MAX_CHNL_NUM*8] = 0; // + : a positive number
            }
            else
            {
                page[(i*4+1)+MAX_CHNL_NUM*8] = 1; // - : a negative number
            }

            page[(i*4+2)+MAX_CHNL_NUM*8] = high_byte;
            page[(i*4+3)+MAX_CHNL_NUM*8] = low_byte;
        }

        if (ito_test_2r)
        {
            for (i = 0; i < MAX_CHNL_NUM; i ++)
            {
                high_byte = (s16_raw_data_4[i] >> 8) & 0xFF;
                low_byte = (s16_raw_data_4[i]) & 0xFF;
        
                if (data_flag_4[i] == 1)
                {
                    page[i*4+MAX_CHNL_NUM*12] = 1; // indicate it is a on-use channel number
                }
                else
                {
                    page[i*4+MAX_CHNL_NUM*12] = 0; // indicate it is a non-use channel number
                }

                if (s16_raw_data_4[i] >= 0)
                {
                    page[(i*4+1)+MAX_CHNL_NUM*12] = 0; // + : a positive number
                }
                else
                {
                    page[(i*4+1)+MAX_CHNL_NUM*12] = 1; // - : a negative number
                }

                page[(i*4+2)+MAX_CHNL_NUM*12] = high_byte;
                page[(i*4+3)+MAX_CHNL_NUM*12] = low_byte;
            }
        }
        
        cnt = MAX_CHNL_NUM*16;
    }
    else 
    {
        ITO_TEST_DEBUG_MUST("*** Undefined MP Test Mode ***\n");
    }

    *eof = 1;
    
    return cnt;
}

static int ito_test_proc_write_data(struct file *file, const char *buffer, unsigned long count, void *data)
{    
    ITO_TEST_DEBUG_MUST("ito_test_proc_write_data()\n");

    return count;
}

static void ito_test_create_entry(void)
{
	// create /proc/msg-ito-test/debug , authority =0777
	// create /proc/msg-ito-test/debug-on-off, authority =0777
	// create /proc/msg-ito-test/fail-channel, authority =0777
	// create /proc/msg-ito-test/data, authority =0777
	msg_ito_test = proc_mkdir(PROC_MSG_ITO_TEST, NULL);
    	debug = create_proc_entry(PROC_ITO_TEST_DEBUG, ITO_TEST_AUTHORITY, msg_ito_test);
    	debug_on_off= create_proc_entry(PROC_ITO_TEST_DEBUG_ON_OFF, ITO_TEST_AUTHORITY, msg_ito_test);
    	open_test = create_proc_entry(PROC_ITO_TEST_OPEN, ITO_TEST_AUTHORITY, msg_ito_test);
    	short_test = create_proc_entry(PROC_ITO_TEST_SHORT, ITO_TEST_AUTHORITY, msg_ito_test);
	fail_channel = create_proc_entry(PROC_ITO_TEST_FAIL_CHANNEL, ITO_TEST_AUTHORITY, msg_ito_test);
    	data = create_proc_entry(PROC_ITO_TEST_DATA, ITO_TEST_AUTHORITY, msg_ito_test);

    if (NULL==debug) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST DEBUG failed\n");
    } 
    else 
    {
        debug->read_proc = ito_test_proc_read_debug;
        debug->write_proc = ito_test_proc_write_debug;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST DEBUG OK\n");
    }

    if (NULL==debug_on_off) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST ON OFF failed\n");
    } 
    else 
    {
        debug_on_off->read_proc = ito_test_proc_read_debug_on_off;
        debug_on_off->write_proc = ito_test_proc_write_debug_on_off;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST ON OFF OK\n");
    }


    if (NULL==open_test) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST OPEN failed\n");
    } 
    else 
    {
        open_test->read_proc = ito_test_proc_read_open;
        open_test->write_proc = ito_test_proc_write_open;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST OPEN OK\n");
    }

    if (NULL==short_test) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST SHORT failed\n");
    } 
    else 
    {
        short_test->read_proc = ito_test_proc_read_short;
        short_test->write_proc = ito_test_proc_write_short;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST SHORT OK\n");
    }

    if (NULL==fail_channel) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST FAIL CHANNEL failed\n");
    } 
    else 
    {
        fail_channel->read_proc = ito_test_proc_read_fail_channel;
        fail_channel->write_proc = ito_test_proc_write_fail_channel;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST FAIL CHANNEL OK\n");
    }

    if (NULL==data) 
    {
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST DATA failed\n");
    } 
    else 
    {
        data->read_proc = ito_test_proc_read_data;
        data->write_proc = ito_test_proc_write_data;
        ITO_TEST_DEBUG_MUST("create_proc_entry ITO TEST DATA OK\n");
    }
}
#endif
//END: fangjie


 static u8 Calculate_8BitsChecksum( u8 *msg, s32 s32Length )
 {
	 s32 s32Checksum = 0;
	 s32 i;
 
	 for( i = 0 ; i < s32Length; i++ )
	 {
		 s32Checksum += msg[i];
	 }
 
	 return ( u8 )( ( -s32Checksum ) & 0xFF );
 }

 static int tpd_touchinfo(TouchScreenInfo_t *touchData)
 {

    u8 val[8] = {0};
    u8 Checksum = 0;
    u8 i;
    u32 delta_x = 0, delta_y = 0;
    u32 u32X = 0;
    u32 u32Y = 0;
    

    TPD_DEBUG(KERN_ERR "[msg2133]==tpd_touchinfo() \n");


#ifdef SWAP_X_Y
    int tempx;
    int tempy;
#endif

    /*Get Touch Raw Data*/
    i2c_master_recv( i2c_client, &val[0], REPORT_PACKET_LENGTH );
    TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--val[0]:%x, REPORT_PACKET_LENGTH:%x \n",val[0], REPORT_PACKET_LENGTH);
    Checksum = Calculate_8BitsChecksum( &val[0], 7 ); //calculate checksum
    TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--Checksum:%x, val[7]:%x, val[0]:%x \n",Checksum, val[7], val[0]);

    if( ( Checksum == val[7] ) && ( val[0] == 0x52 ) ) //check the checksum  of packet
    {
        u32X = ( ( ( val[1] & 0xF0 ) << 4 ) | val[2] );   //parse the packet to coordinates
        u32Y = ( ( ( val[1] & 0x0F ) << 8 ) | val[3] );

        delta_x = ( ( ( val[4] & 0xF0 ) << 4 ) | val[5] );
        delta_y = ( ( ( val[4] & 0x0F ) << 8 ) | val[6] );
		TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--u32X:%d, u32Y:%d, delta_x:%d, delta_y:%d \n",u32X, u32Y,delta_x, delta_y);

#ifdef SWAP_X_Y
        tempy = u32X;
        tempx = u32Y;
        u32X = tempx;
        u32Y = tempy;

        tempy = delta_x;
        tempx = delta_y;
        delta_x = tempx;
        delta_y = tempy;
#endif
#ifdef REVERSE_X
        u32X = 2047 - u32X;
        delta_x = 4095 - delta_x;
#endif
#ifdef REVERSE_Y
        u32Y = 2047 - u32Y;
        delta_y = 4095 - delta_y;
#endif

		TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--u32X:%d, u32Y:%d, delta_x:%d, delta_y:%d \n",u32X, u32Y,delta_x, delta_y);

        if( ( val[1] == 0xFF ) && ( val[2] == 0xFF ) && ( val[3] == 0xFF ) && ( val[4] == 0xFF ) && ( val[6] == 0xFF ) )
        {  
            touchData->Point[0].X = 0; // final X coordinate
            touchData->Point[0].Y = 0; // final Y coordinate

            if( ( val[5] == 0x0 ) || ( val[5] == 0xFF ) )
            {
                touchData->nFingerNum = 0; //touch end
                touchData->nTouchKeyCode = 0; //TouchKeyMode
                touchData->nTouchKeyMode = 0; //TouchKeyMode
            }
            else
            {
                touchData->nTouchKeyMode = 1; //TouchKeyMode
                touchData->nTouchKeyCode = val[5]; //TouchKeyCode
                touchData->nFingerNum = 1;
            }
        }
        else
        {
            touchData->nTouchKeyMode = 0; //Touch on screen...

            if(
#ifdef REVERSE_X
                ( delta_x == 4095 )
#else
                ( delta_x == 0 )
#endif
                &&
#ifdef REVERSE_Y
                ( delta_y == 4095 )
#else
                ( delta_y == 0 )
#endif
            )
            {
                touchData->nFingerNum = 1; //one touch
                touchData->Point[0].X = ( u32X * MS_TS_MSG21XX_X_MAX ) / 2048;
                touchData->Point[0].Y = ( u32Y * MS_TS_MSG21XX_Y_MAX ) / 2048;
				TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--FingerNum = 1 \n");
				TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--touchData->Point[0].X = %d,  touchData->Point[0].Y = %d \n",touchData->Point[0].X,touchData->Point[0].Y);
            }
            else
            {
                u32 x2, y2;

                touchData->nFingerNum = 2; //two touch

                /* Finger 1 */
                touchData->Point[0].X = ( u32X * MS_TS_MSG21XX_X_MAX ) / 2048;
                touchData->Point[0].Y = ( u32Y * MS_TS_MSG21XX_Y_MAX ) / 2048;

                /* Finger 2 */
                if( delta_x > 2048 )    //transform the unsigh value to sign value
                {
                    delta_x -= 4096;
                }
                if( delta_y > 2048 )
                {
                    delta_y -= 4096;
                }

                x2 = ( u32 )( u32X + delta_x );
                y2 = ( u32 )( u32Y + delta_y );

                touchData->Point[1].X = ( x2 * MS_TS_MSG21XX_X_MAX ) / 2048;
                touchData->Point[1].Y = ( y2 * MS_TS_MSG21XX_Y_MAX ) / 2048;
				TPD_DEBUG(KERN_ERR"[tpd_touchinfo]--FingerNum = 2 \n");
            }
        }

       
    }
    else
    {
        //DBG("Packet error 0x%x, 0x%x, 0x%x", val[0], val[1], val[2]);
        //DBG("             0x%x, 0x%x, 0x%x", val[3], val[4], val[5]);
        //DBG("             0x%x, 0x%x, 0x%x", val[6], val[7], Checksum);
        TPD_DEBUG( KERN_ERR "err status in tp\n" );
		return false;

		
    }

    //enable_irq( msg21xx_irq );
  ///
	 return true;

 };
 
 static  void tpd_down(int x, int y, int p) {
 	
	 // input_report_abs(tpd->dev, ABS_PRESSURE, p);
	  input_report_key(tpd->dev, BTN_TOUCH, 1);
	  input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 25);
	  input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
	  input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);

	  /* track id Start 0 */
		//input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, p); 
	  input_mt_sync(tpd->dev);
	  if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
	  {   
		tpd_button(x, y, 1);  
	  }
	  if(y > TPD_RES_Y) //virtual key debounce to avoid android ANR issue
	  {
		 // msleep(50);
		  TPD_DEBUG("D virtual key \n");
	  }
	  TPD_EM_PRINT(x, y, x, y, p-1, 1);
  }
  
 static  void tpd_up(int x, int y,int *count) {

	  input_report_key(tpd->dev, BTN_TOUCH, 0);
	  input_mt_sync(tpd->dev);
	  TPD_EM_PRINT(x, y, x, y, 0, 0);
		  
	  if(FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
	  {   
	  	 TPD_DEBUG(KERN_ERR "[msg2133]--tpd_up-BOOT MODE--X:%d, Y:%d; \n", x, y);
		 tpd_button(x, y, 0); 
	  } 		  
 
  }

 static int touch_event_handler(void *unused)
 {
  
    TouchScreenInfo_t touchData;
	u8 touchkeycode = 0;
	static u32 preKeyStatus = 0;
	int i=0;
 
    TPD_DEBUG(KERN_ERR "[msg2133]touch_event_handler() do while \n");

	touchData.nFingerNum = 0;
	TPD_DEBUG(KERN_ERR "[msg2133]touch_event_handler() do while \n");
	 
	if (tpd_touchinfo(&touchData)) 
	{
	 
		TPD_DEBUG(KERN_ERR "[msg2133]--KeyMode:%d, KeyCode:%d, FingerNum =%d \n", touchData.nTouchKeyMode, touchData.nTouchKeyCode, touchData.nFingerNum );
	 
		//key...
		if( touchData.nTouchKeyMode )
		{
	    	//key mode change virtual key mode
			touchData.nFingerNum = 1;
			if( touchData.nTouchKeyCode == 2 )
			{
				//touchkeycode = KEY_MENU;
				touchData.Point[0].X = 400;
				touchData.Point[0].Y = 900;
			}
			if( touchData.nTouchKeyCode == 4 )
			{
				//touchkeycode = KEY_HOMEPAGE ;
				touchData.Point[0].X = 240;
				touchData.Point[0].Y = 900;

			}
			if( touchData.nTouchKeyCode == 1 )
			{
				//touchkeycode = KEY_BACK;
				touchData.Point[0].X = 80;
				touchData.Point[0].Y = 900;

			}

					
		}
				//report
		{
	 
			if( ( touchData.nFingerNum ) == 0 ) //touch end
			{
				TPD_DEBUG("------UP------ \n");
				TPD_DEBUG(KERN_ERR "[msg2133]---X:%d, Y:%d; \n", touchData.Point[0].X, touchData.Point[0].Y);
				tpd_up(touchData.Point[0].X, touchData.Point[0].Y, 0);
				input_sync( tpd->dev );
			}
			else //touch on screen
			{
	 
				for( i = 0; i < ( (int)touchData.nFingerNum ); i++ )
				{
				    TPD_DEBUG("------DOWN------ \n");
					tpd_down(touchData.Point[i].X, touchData.Point[i].Y, 1);
					TPD_DEBUG(KERN_ERR "[msg2133]---X:%d, Y:%d; i=%d \n", touchData.Point[i].X, touchData.Point[i].Y, i);
				}
	 
				input_sync( tpd->dev );
			}
		}//end if(touchData->nTouchKeyMode)
	 
			}

     mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM); 
	 return 0;
 }
 
 static int tpd_detect (struct i2c_client *client, struct i2c_board_info *info) 
 {
	 strcpy(info->type, TPD_DEVICE);	
	  return 0;
 }
 
 static void tpd_eint_interrupt_handler(void)
 {
	 mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	 schedule_work( &msg21xx_wq );
 }


//BEGIN: fangjie add for ctp compatible design for MMITest rawdata test. 20140626
#if 1 //#ifdef SYS_COMPATIBLE
static ssize_t firmware_ctpid_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
	unsigned char *strid=NULL;
	
	printk("*** The tp vendor  = %s***\n", fw_major_version);
	strid="M1";
	return sprintf(buf, "%s\n", strid);
}
static DEVICE_ATTR(ctpid, S_IRUGO | S_IWUSR, firmware_ctpid_show, NULL);
#endif
//END: fangjie add for ctp compatible design for MMITest rawdata test. 20140626


 static int __devinit tpd_probe(struct i2c_client *client, const struct i2c_device_id *id)
 {	 
 
	int retval = TPD_OK;
	char data;
	u8 report_rate=0;
	int reset_count = 0;
	int error;
	u8 test[0] = {0x45};
    unsigned char dbbus_tx_data[3];
	unsigned char dbbus_rx_data[4] ;
	unsigned short current_major_version=0, current_minor_version=0;
	char version[2][10] = {"MUTTO", "JUNDA"};

	i2c_client = client;
	//msg21xx_i2c_client = client;
	this_client = client;
	/*reset I2C clock*/
    //i2c_client->timing = 0;
    
   INIT_WORK( &msg21xx_wq, touch_event_handler );
//power on, need confirm with SA
/*
#ifdef TPD_POWER_SOURCE_CUSTOM
	hwPowerOn(TPD_POWER_SOURCE_CUSTOM, VOL_2800, "TP");
#else
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
#endif
*/
#ifdef TPD_POWER_SOURCE_1800
	hwPowerOn(TPD_POWER_SOURCE_1800, VOL_1800, "TP");
#endif 


#ifdef TPD_CLOSE_POWER_IN_SLEEP	 
	hwPowerDown(TPD_POWER_SOURCE,"TP");
	hwPowerOn(TPD_POWER_SOURCE,VOL_2800,"TP");
	msleep(100);
#else

	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(10);
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ZERO);  
	msleep(50);
	TPD_DMESG(" msg2133 reset\n");
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(50);
	
#endif
	

	mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
   	mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_DISABLE);

    msleep(10);

	mt65xx_eint_set_sens(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_SENSITIVE);
	mt65xx_eint_set_hw_debounce(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_TOUCH_PANEL_POLARITY, tpd_eint_interrupt_handler, 1); 
	msleep(50);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	msleep(200);
/*
	 char dbbus_tx_data[3];
	 char dbbus_rx_data[2]; 
	 dbbusDWIICEnterSerialDebugMode();
	 dbbusDWIICStopMCU();
	 dbbusDWIICIICUseBus();
	 dbbusDWIICIICReshape();
	 dbbus_tx_data[0] = 0x10;
	 dbbus_tx_data[1] = 0x3A;
	 dbbus_tx_data[2] = 0x08; 
	 dbbus_rx_data[0]=0xFF;
	 dbbus_rx_data[1]=0xFF; 
	 HalTscrCDevWriteI2CSeq(0XC4, &dbbus_tx_data[0], 3);
	 HalTscrCReadI2CSeq(0XC4, &dbbus_rx_data[0], 2);
	 dbbusDWIICIICNotUseBus();
	 dbbusDWIICNotStopMCU();
	 dbbusDWIICExitSerialDebugMode();

	 if(dbbus_rx_data[0]==0||dbbus_rx_data[0]==0xff)
	 {
		 return -1;//	
	 }

	*/
	dbbus_tx_data[0] = 0x53;
	dbbus_tx_data[1] = 0x00;
	dbbus_tx_data[2] = 0x2a;
	error = i2c_master_send(i2c_client, &dbbus_tx_data[0], 3);
	if(error < 0)						//add error handing about tp not exsit which is used for meta test
	{
		i2c_client->addr = FW_ADDR_MSG21XX;
		error = i2c_master_send(i2c_client, &test[0], 1);
		if(error < 0){
			printk("the device of tp do not exist.\n");
			return -1;
		}
		else{
	//		masterBUT_LoadFwToTarget(MSG_FIRMWARE_MEGANE_V204,sizeof(MSG_FIRMWARE_MEGANE_V204));
	        i2c_client->addr = FW_ADDR_MSG21XX_TP;
	        i2c_client->addr |= I2C_ENEXT_FLAG; //I2C_HS_FLAG;
			dbbus_tx_data[0] = 0x53;
			dbbus_tx_data[1] = 0x00;
			dbbus_tx_data[2] = 0x2a;
			i2c_master_send(i2c_client, &dbbus_tx_data[0], 3);
		}
	}
	i2c_master_recv(i2c_client, &dbbus_rx_data[0], 4);


	current_major_version = (dbbus_rx_data[1]<<8)+dbbus_rx_data[0];
	current_minor_version = (dbbus_rx_data[3]<<8)+dbbus_rx_data[2];

	//BEGIN: add by fangjie for create fwversions attribute node. 
	fw_major_version = current_major_version;
	fw_minor_version = current_minor_version;
	//END: add by fangjie for create fwversions attribute node. 

	printk("current_major_version = 0x%04x\ncurrent_minor_version = 0x%04x\n",current_major_version,current_minor_version);

	if(current_major_version == 0x0001) /*MUTTO*/
	{
		printk("the VID is %s\n",version[0]);
		if(current_minor_version < 0x0009) //fangjie modify for update firmware to V1.09,PR724767
		{
			masterBUT_LoadFwToTarget(MSG_FIRMWARE_YARISL_VERSION);
		}
	}
	else //yarisl only has one mstar module (mutto) 
	{
		printk("the VID is unknown\n");
		//BEGIN: add by fangjie 
		//because yarisl  only has one mstar module (mutto), 
		//so , if when TP IC is msg2138, but the current_major_version != 0x01, 
		//means that the firmware is broken, then need force to upgrade.
		masterBUT_LoadFwToTarget(MSG_FIRMWARE_YARISL_VERSION);
		//END: add by fangjie.
	}
	
    tpd_load_status = 1;

	TPD_DMESG("msg2133 Touch Panel Device Probe %s\n", (retval < TPD_OK) ? "FAIL" : "PASS");
	
	TPD_DEBUG("msg2133--frameware upgrade \n");

    /*frameware upgrade*/	
#ifdef __FIRMWARE_UPDATE__
		firmware_class = class_create( THIS_MODULE, "ms-touchscreen-msg20xx" );
		if( IS_ERR( firmware_class ) )
			pr_err( "Failed to create class(firmware)!\n" );
		firmware_cmd_dev = device_create( firmware_class,
										  NULL, 0, NULL, "device" );
		if( IS_ERR( firmware_cmd_dev ) )
			pr_err( "Failed to create device(firmware_cmd_dev)!\n" );
	
		// version
		if( device_create_file( firmware_cmd_dev, &dev_attr_version ) < 0 )
			pr_err( "Failed to create device file(%s)!\n", dev_attr_version.attr.name );
		// update
		if( device_create_file( firmware_cmd_dev, &dev_attr_update ) < 0 )
			pr_err( "Failed to create device file(%s)!\n", dev_attr_update.attr.name );
		// data
		if( device_create_file( firmware_cmd_dev, &dev_attr_data ) < 0 )
			pr_err( "Failed to create device file(%s)!\n", dev_attr_data.attr.name );
		// clear
		if( device_create_file( firmware_cmd_dev, &dev_attr_clear ) < 0 )
			pr_err( "Failed to create device file(%s)!\n", dev_attr_clear.attr.name );

		//BEGIN: add by fangjie for create fwversions attribute node. 
		if( device_create_file( firmware_cmd_dev, &dev_attr_fwversions ) < 0 )
			pr_err( "Failed to create device file(%s)!\n", dev_attr_fwversions.attr.name );
		//END: add by fangjie for create fwversions attribute node. 
	
		dev_set_drvdata( firmware_cmd_dev, NULL );
#endif

	//BEGIN: fangjie add for MMITest.apk Rawdata test. 20140626	
	#ifdef ITO_TEST
		ito_test_create_entry();
			//BEGIN: fangjie add for ctp compatible design for MMITest rawdata test. 20140626
		#if 1 //#ifdef SYS_COMPATIBLE
		compatible_mtp=hwmsen_get_compatible_dev();
		if (device_create_file(compatible_mtp, &dev_attr_ctpid) < 0)
		{
			pr_err("Failed to create device file(%s)!\n", dev_attr_ctpid.attr.name);
		}
		dev_set_drvdata(compatible_mtp, NULL);
		#endif
		//END: fangjie add for ctp compatible design for MMITest rawdata test. 20140626
	#endif	
	//END: fangjie add for MMITest.apk Rawdata test. 20140626
      return 0;   
 }

 static int __devexit tpd_remove(struct i2c_client *client)
 
 {
   
	 TPD_DEBUG("TPD removed\n");
 
   return 0;
 }
 
 
 static int tpd_local_init(void)
 {

 
  	TPD_DMESG("Mstar msg2133 I2C Touchscreen Driver (Built %s @ %s)\n", __DATE__, __TIME__);
 
 
    if(i2c_add_driver(&tpd_i2c_driver)!=0)
   	{
  		TPD_DMESG("msg2133 unable to add i2c driver.\n");
      	return -1;
    }
    if(tpd_load_status == 0) 
    {
    	TPD_DMESG("msg2133 add error touch panel driver.\n");
    	i2c_del_driver(&tpd_i2c_driver);
    	return -1;
    }
	
#ifdef TPD_HAVE_BUTTON     
    tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
#endif   
  
//#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))    
//WARP CHECK IS NEED --XB.PANG
//#endif 

	TPD_DMESG("end %s, %d\n", __FUNCTION__, __LINE__);  
		
    return 0; 
 }

 static void tpd_resume( struct early_suspend *h )
 {
 
   TPD_DMESG("TPD wake up\n");
#ifdef TPD_CLOSE_POWER_IN_SLEEP	
	hwPowerOn(TPD_POWER_SOURCE,VOL_2800,"TP");
#endif
	msleep(100);
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ZERO);  
	msleep(50);
	TPD_DMESG(" msg2133 reset\n");
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ONE);
	msleep(200);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	TPD_DMESG("TPD wake up done\n");
	
 }

 static void tpd_suspend( struct early_suspend *h )
 {
 	
	TPD_DMESG("TPD enter sleep\n");
	mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	
	mt_set_gpio_mode(GPIO_CTP_RST2138_PIN, GPIO_CTP_RST2138_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_CTP_RST2138_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_RST2138_PIN, GPIO_OUT_ZERO);  
	 
#ifdef TPD_CLOSE_POWER_IN_SLEEP	
	hwPowerDown(TPD_POWER_SOURCE,"TP");
#else
	//TP enter sleep mode----XB.PANG NEED CHECK
	//if have sleep mode
#endif
    TPD_DMESG("TPD enter sleep done\n");
 } 


 static struct tpd_driver_t tpd_device_driver = {
		 .tpd_device_name = "msg2133",
		 .tpd_local_init = tpd_local_init,
		 .suspend = tpd_suspend,
		 .resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
		 .tpd_have_button = 1,
#else
		 .tpd_have_button = 0,
#endif		
 };
 /* called when loaded into kernel */
 static int __init tpd_driver_init(void) {
	 TPD_DEBUG("MediaTek MSG2133 touch panel driver init\n");
	   i2c_register_board_info(1, &msg2133_i2c_tpd, 1);
		 if(tpd_driver_add(&tpd_device_driver) < 0)
			 TPD_DMESG("add MSG2133 driver failed\n");
	 return 0;
 }
 
 /* should never be called */
 static void __exit tpd_driver_exit(void) {
	 TPD_DMESG("MediaTek MSG2133 touch panel driver exit\n");
	 tpd_driver_remove(&tpd_device_driver);
 }
 
 module_init(tpd_driver_init);
 module_exit(tpd_driver_exit);


