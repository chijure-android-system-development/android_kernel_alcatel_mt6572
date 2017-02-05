#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_hw.h"
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/xlog.h>
#include <mach/upmu_common.h>


#include <mach/mt_pwm.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>
#include <mach/upmu_common.h>


/******************************************************************************
 * Debug configuration
******************************************************************************/
// availible parameter
// ANDROID_LOG_ASSERT
// ANDROID_LOG_ERROR
// ANDROID_LOG_WARNING
// ANDROID_LOG_INFO
// ANDROID_LOG_DEBUG
// ANDROID_LOG_VERBOSE
#define TAG_NAME "leds_strobe.c"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_WARN(fmt, arg...)        xlog_printk(ANDROID_LOG_WARNING, TAG_NAME, KERN_WARNING  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_NOTICE(fmt, arg...)      xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME, KERN_NOTICE  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_INFO(fmt, arg...)        xlog_printk(ANDROID_LOG_INFO   , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_TRC_FUNC(f)              xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME,  "<%s>\n", __FUNCTION__);
#define PK_TRC_VERBOSE(fmt, arg...) xlog_printk(ANDROID_LOG_VERBOSE, TAG_NAME,  fmt, ##arg)
#define PK_ERROR(fmt, arg...)       xlog_printk(ANDROID_LOG_ERROR  , TAG_NAME, KERN_ERR "%s: " fmt, __FUNCTION__ ,##arg)


#define DEBUG_LEDS_STROBE
#ifdef  DEBUG_LEDS_STROBE
	#define PK_DBG PK_DBG_FUNC
	#define PK_VER PK_TRC_VERBOSE
	#define PK_ERR PK_ERROR
#else
	#define PK_DBG(a,...)
	#define PK_VER(a,...)
	#define PK_ERR(a,...)
#endif

/******************************************************************************
 * local variables
******************************************************************************/
static DEFINE_SPINLOCK(g_strobeSMPLock);
static u32 strobe_Res = 0;
static u32 strobe_Timeus = 0;
static BOOL g_strobe_On = 0;
static u32 strobe_width = 100; 

static int g_duty=-1;
static int g_step=-1;
static int g_timeOutTimeMs=0;
extern int flash_mode;
static struct work_struct workTimeOut;
/*****************************************************************************
Functions
*****************************************************************************/
static void work_timeOutFunc(struct work_struct *data);

static struct pwm_easy_config pwm_setting;
static int fl_pwm_num = PWM1;                                   
unsigned int fl_div = CLK_DIV4;

extern void mt_power_on (U32 pwm_no);
extern void mt_power_off (U32 pwm_no);
extern S32 mt_set_pwm_enable ( U32 pwm_no ) ;
extern S32 mt_set_pwm_disable ( U32 pwm_no ) ;
extern void mt_pwm_power_off (U32 pwm_no);



int FL_Enable(void)
{
	if(flash_mode == 2)
	{
		mt_set_gpio_out(GPIO16, GPIO_OUT_ZERO);
	}
	else 
	{
		mt_set_gpio_out(GPIO16, GPIO_OUT_ONE);
	}
	mt_set_gpio_out(GPIO128, GPIO_OUT_ONE);
	printk("%s\n", __func__);
    return 0;
}

int FL_Disable(void)
{
	mt_set_gpio_out(GPIO16, GPIO_OUT_ZERO);
	mt_set_gpio_out(GPIO128, GPIO_OUT_ZERO);
	printk("%s\n", __func__);
    return 0;
}

int FL_dim_duty(kal_uint32 duty)
{
	//upmu_set_flash_dim_duty(duty);
    return 0;
}

int FL_step(kal_uint32 step)
{
	int sTab[8]={0,2,4,6,9,11,13,15};
	//upmu_set_flash_sel(sTab[step]);
    return 0;
}

int FL_Init(void)
{
	//upmu_set_flash_dim_duty(0);
	//upmu_set_flash_sel(0);
	printk("%s\n", __func__);
	mt_set_gpio_mode(GPIO16, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO16, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO16, GPIO_OUT_ZERO);
    mt_set_gpio_mode(GPIO128, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO128, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO128, GPIO_OUT_ZERO);
	
	FL_Disable();
	INIT_WORK(&workTimeOut, work_timeOutFunc);
	  
    return 0;
}
int FL_Uninit(void)
{
	FL_Disable();
    return 0;
}

/*add for LED Torch blink by qiangbo.ni @s */
int TORCH_Level(u32 level)
{

	if(g_strobe_On)
	{
		mt_set_gpio_mode(128, GPIO_MODE_01);

		pwm_setting.pwm_no = fl_pwm_num;
		pwm_setting.clk_div = fl_div;
		pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;

		pwm_setting.duty = level;
		pwm_setting.duration = 0;
		pwm_set_easy_config(&pwm_setting);
	}
	return 0;
}
/*add for LED Torch blink by qiangbo.ni @e */

/*add for LED Torch by qiangbo.ni @s*/
int TORCH_Enable(void)
{
	mt_set_gpio_out(GPIO16, GPIO_OUT_ZERO);
	mt_set_gpio_mode(128, GPIO_MODE_01);
	
	pwm_setting.pwm_no = fl_pwm_num;
	pwm_setting.clk_div = fl_div;
	pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;
	
	pwm_setting.duty = strobe_width;
	pwm_setting.duration = 0;
	pwm_set_easy_config(&pwm_setting);

	return 0;
}

int TORCH_Disable(void) 
{
		mt_set_gpio_out(GPIO16, GPIO_OUT_ZERO);
		mt_set_pwm_disable(fl_pwm_num);
		mt_pwm_power_off(fl_pwm_num);

   	return 0;
}
/*add for LED Torch by qiangbo.ni @e*/
/*****************************************************************************
User interface
*****************************************************************************/

static void work_timeOutFunc(struct work_struct *data)
{
	FL_Disable();
    PK_DBG("ledTimeOut_callback\n");
    //printk(KERN_ALERT "work handler function./n");
}



enum hrtimer_restart ledTimeOutCallback(struct hrtimer *timer)
{
    schedule_work(&workTimeOut);
    return HRTIMER_NORESTART;
}
static struct hrtimer g_timeOutTimer;
void timerInit(void)
{
	g_timeOutTimeMs=1000; //1s
	hrtimer_init( &g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
	g_timeOutTimer.function=ledTimeOutCallback;

}



static int constant_flashlight_ioctl(MUINT32 cmd, MUINT32 arg)
{
	int i4RetValue = 0;
	int ior_shift;
	int iow_shift;
	int iowr_shift;
	ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC,0, int));
	iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC,0, int));
	iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC,0, int));
	PK_DBG("constant_flashlight_ioctl() line=%d ior_shift=%d, iow_shift=%d iowr_shift=%d arg=%d\n",__LINE__, ior_shift, iow_shift, iowr_shift, arg);
    switch(cmd)
    {

		case FLASH_IOC_SET_TIME_OUT_TIME_MS:
			PK_DBG("FLASH_IOC_SET_TIME_OUT_TIME_MS: %d\n",arg);
			g_timeOutTimeMs=arg;
		break;


    	case FLASH_IOC_SET_DUTY :
    		PK_DBG("FLASHLIGHT_DUTY: %d\n",arg);
    		g_duty=arg;
    		FL_dim_duty(arg);
    		break;


    	case FLASH_IOC_SET_STEP:
    		PK_DBG("FLASH_IOC_SET_STEP: %d\n",arg);
    		g_step=arg;
    		FL_step(arg);
    		break;

    	case FLASH_IOC_SET_ONOFF :
    		PK_DBG("FLASHLIGHT_ONOFF: %d\n",arg);
    		if(arg==1)
    		{
				if(g_timeOutTimeMs!=0)
	            {
	            	ktime_t ktime;
					ktime = ktime_set( 0, g_timeOutTimeMs*1000000 );
					hrtimer_start( &g_timeOutTimer, ktime, HRTIMER_MODE_REL );
	            }
    			FL_Enable();
    			g_strobe_On=1;
    		}
    		else
    		{
    			FL_Disable();
				hrtimer_cancel( &g_timeOutTimer );
    		}
    		break;
/* add for LED Torch by qiangbo.ni @s*/
	    case FLASHLIGHTIOC_X_SET_FLASHLEVEL:
					strobe_width = arg;
	        printk("level:%d \n",(int)arg);
					TORCH_Level(strobe_width);
		      break;
		  case FLASHLIGHTIOC_ENABLE_STATUS:
			  	printk("**********g_strobe_on = %d \n", g_strobe_On);
			    copy_to_user((void __user *) arg , (void*)&g_strobe_On , sizeof(int));
			    break;
		  case FLASHLIGHT_TORCH_SELECT:
					printk("@@@@@@FLASHLIGHT_TORCH_SELECT@@@@@@\n");
					if (arg){
							TORCH_Enable();
							g_strobe_On = TRUE;
					}
					else 
					{
							TORCH_Disable();
	            g_strobe_On = FALSE;
	        }
					break;
/* add for LED Torch by qiangbo.ni @e*/
			default :
	    		PK_DBG(" No such command \n");
	    		i4RetValue = -EPERM;
	    		break;
	    }
	    return i4RetValue;
}




static int constant_flashlight_open(void *pArg)
{
    int i4RetValue = 0;
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	if (0 == strobe_Res)
	{
	    FL_Init();
		timerInit();
	}
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);
	spin_lock_irq(&g_strobeSMPLock);

    if(strobe_Res)
    {
        PK_ERR(" busy!\n");
        i4RetValue = -EBUSY;
    }
    else
    {
        strobe_Res += 1;
    }


    spin_unlock_irq(&g_strobeSMPLock);
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

    return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{
    PK_DBG(" constant_flashlight_release\n");

    if (strobe_Res)
    {
        spin_lock_irq(&g_strobeSMPLock);

        strobe_Res = 0;
        strobe_Timeus = 0;

        /* LED On Status */
        g_strobe_On = FALSE;

        spin_unlock_irq(&g_strobeSMPLock);

    	FL_Uninit();
    }

    PK_DBG(" Done\n");

    return 0;

}


FLASHLIGHT_FUNCTION_STRUCT	constantFlashlightFunc=
{
	constant_flashlight_open,
	constant_flashlight_release,
	constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
    if (pfFunc != NULL)
    {
        *pfFunc = &constantFlashlightFunc;
    }
    return 0;
}



/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{

    return 0;
}

EXPORT_SYMBOL(strobe_VDIrq);


