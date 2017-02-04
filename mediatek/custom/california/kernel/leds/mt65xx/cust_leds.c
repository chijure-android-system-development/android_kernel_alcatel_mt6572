
#include <cust_leds.h>
#include <mach/mt_pwm.h>

#include <linux/kernel.h>
#include <mach/pmic_mt6329_hw_bank1.h> 
#include <mach/pmic_mt6329_sw_bank1.h> 
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>
#include <mach/mt_gpio.h>

#define GPIO_LED_DRV GPIO15

//#define GPIO_LED_DRV GPIO14



//extern int mtkfb_set_backlight_level(unsigned int level);
//extern int mtkfb_set_backlight_pwm(int div);
extern int disp_bls_set_backlight(unsigned int level);
/*
#define ERROR_BL_LEVEL 0xFFFFFFFF

unsigned int brightness_mapping(unsigned int level)
{  
	return ERROR_BL_LEVEL;
}
*/
unsigned int brightness_mapping(unsigned int level)
{
    unsigned int mapped_level;
/* modify by zhiping.liu for expand brightness dynamic range @start */
    if (level < 8)
        mapped_level = 2;
    else
        mapped_level = (level + 1) >> 2;
/* modify by zhiping.liu for expand brightness dynamic range @end */
	return mapped_level;
}

unsigned int Cust_SetBacklight(int level, int div)
{
    //mtkfb_set_backlight_pwm(div);
    //mtkfb_set_backlight_level(brightness_mapping(level));
    disp_bls_set_backlight(brightness_mapping(level));
    return 0;
}


static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
//	{"red",               MT65XX_LED_MODE_NONE, -1, {0}},
    {"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK3, {0}},  //changed by zhengdao
	{"green",             MT65XX_LED_MODE_NONE, -1, {0}},
	{"blue",              MT65XX_LED_MODE_NONE, -1, {0}},
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1, {0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1, {0}},
	{"button-backlight",  MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK1, {0}},
	{"lcd-backlight",     MT65XX_LED_MODE_PWM, PWM2, {1,CLK_DIV1,16,16}},
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

void led_hw_control_switch(int enable)
{
	mt_set_gpio_mode(GPIO_LED_DRV, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LED_DRV, GPIO_DIR_OUT);

	if(enable)
		mt_set_gpio_out(GPIO_LED_DRV, GPIO_OUT_ZERO);
	else
		mt_set_gpio_out(GPIO_LED_DRV, GPIO_OUT_ONE);
}

