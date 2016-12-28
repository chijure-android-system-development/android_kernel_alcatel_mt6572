
#include <linux/types.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/sync_write.h>

#define GPIO_WR32(addr, data)   mt65xx_reg_sync_writel(data, addr)
#define GPIO_RD32(addr)         __raw_readl(addr)

#define GPIO_MBIST_CFG_6 0xF0000220
#define GPIO_MBIST_CFG_7 0xF0000224
#define GPIO_MBIST_CFG_2 0xF0000210
#define GPIO_CLK_CFG_3	 0xF000014C
//#define GPIO_MD_TOP_CLKO_MODE 0x8000050C
//static unsigned long clockm_base;
int mt_set_clock_output(unsigned long num, unsigned long src, unsigned long div)
{
/*FIXME*/
	GPIOERR("GPIO clock out module not implement yet!\n");
	return RSUCCESS;
}
EXPORT_SYMBOL(mt_set_clock_output);

int mt_get_clock_output(unsigned long num, unsigned long * src, unsigned long *div)
{
	return RSUCCESS;
}
EXPORT_SYMBOL(mt_get_clock_output);


struct mt_gpio_modem_info {
	char name[40];
	int num;
};

static struct mt_gpio_modem_info mt_gpio_info[]={
	{"GPIO_MD_TEST",800},
#ifdef GPIO_AST_CS_PIN
	{"GPIO_AST_HIF_CS",GPIO_AST_CS_PIN},
#endif
#ifdef GPIO_AST_CS_PIN_NCE
	{"GPIO_AST_HIF_CS_ID",GPIO_AST_CS_PIN_NCE},
#endif
#ifdef GPIO_AST_RST_PIN
	{"GPIO_AST_Reset",GPIO_AST_RST_PIN},
#endif
#ifdef GPIO_AST_CLK32K_PIN
	{"GPIO_AST_CLK_32K",GPIO_AST_CLK32K_PIN},
#endif
#ifdef GPIO_AST_CLK32K_PIN_CLK
	{"GPIO_AST_CLK_32K_CLKM",GPIO_AST_CLK32K_PIN_CLK},
#endif
#ifdef GPIO_AST_WAKEUP_PIN
	{"GPIO_AST_Wakeup",GPIO_AST_WAKEUP_PIN},
#endif
#ifdef GPIO_AST_INTR_PIN
	{"GPIO_AST_INT",GPIO_AST_INTR_PIN},
#endif
#ifdef GPIO_AST_WAKEUP_INTR_PIN
	{"GPIO_AST_WAKEUP_INT",GPIO_AST_WAKEUP_INTR_PIN},
#endif
#ifdef GPIO_AST_AFC_SWITCH_PIN
	{"GPIO_AST_AFC_Switch",GPIO_AST_AFC_SWITCH_PIN},
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_1ST_PIN
	{"GPIO_FDD_Band_Support_Detection_1",GPIO_FDD_BAND_SUPPORT_DETECT_1ST_PIN},
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_2ND_PIN
	{"GPIO_FDD_Band_Support_Detection_2",GPIO_FDD_BAND_SUPPORT_DETECT_2ND_PIN},
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_3RD_PIN
	{"GPIO_FDD_Band_Support_Detection_3",GPIO_FDD_BAND_SUPPORT_DETECT_3RD_PIN},
#endif
#ifdef GPIO_SIM_SWITCH_CLK_PIN
	{"GPIO_SIM_SWITCH_CLK",GPIO_SIM_SWITCH_CLK_PIN},
#endif
#ifdef GPIO_SIM_SWITCH_DAT_PIN
	{"GPIO_SIM_SWITCH_DAT",GPIO_SIM_SWITCH_DAT_PIN},
#endif

/*if you have new GPIO pin add bellow*/

};
int mt_get_md_gpio(char * gpio_name, int len)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(mt_gpio_info); i++)
	{
		if (!strncmp (gpio_name, mt_gpio_info[i].name, len))
		{
			GPIOMSG("Modern get number=%d, name:%s\n", mt_gpio_info[i].num, gpio_name);
			return (mt_gpio_info[i].num);
		}
	}
	GPIOERR("Modem gpio name can't match!!!\n");
	return -1;
}
EXPORT_SYMBOL(mt_get_md_gpio);

int mt_get_md_gpio_debug(char * str)
{
	if(strcmp(str,"ALL")==0){
		int i;
		for(i=0;i<ARRAY_SIZE(mt_gpio_info);i++){
			GPIOMSG("GPIO number=%d,%s\n", mt_gpio_info[i].num, mt_gpio_info[i].name);
		}
	}else{
		GPIOMSG("GPIO number=%d,%s\n",mt_get_md_gpio(str,strlen(str)),str);
	}
}
EXPORT_SYMBOL(mt_get_md_gpio_debug);

