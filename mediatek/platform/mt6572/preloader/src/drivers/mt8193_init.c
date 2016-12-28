
#include "typedefs.h"
#include "platform.h"
#include "uart.h"
#include "meta.h"
#include "mt8193.h"
#include "mt_i2c.h"


int mt8193_pllgp_en()
{
    printf("mt8193_pllgp_en() enter\n");

    /* PLL1 setup of 75MHZ. AD_PLLGP_CLK=450MHZ*/

    CKGEN_WRITE32(REG_RW_PLL_GPANACFG0, 0xAD300982);

    CKGEN_WRITE32(REG_RW_PLL_GPANACFG3, 0x80008000);

    CKGEN_WRITE32(REG_RW_PLL_GPANACFG2, 0x2500000);

    printf("mt8193_pllgp_en() exit\n");

    return 0;
}

int mt8193_vopll_en()
{
    printf("mt8193_pllgp_en() enter\n");

    /* PLL2 setup of 75MHZ. AD_PLLGP_CLK=450MHZ*/

    IO_WRITE32(0, 0x44c, 0x1);

    // CKGEN_WRITE32(REG_RW_PLL_GPANACFG0, 0xAD300982);

    CKGEN_WRITE32(REG_RW_LVDS_ANACFG2, 0x32215000);

    CKGEN_WRITE32(REG_RW_LVDS_ANACFG3, 0x410c0);

    CKGEN_WRITE32(REG_RW_LVDS_ANACFG4, 0x300);

    printf("mt8193_pllgp_en() exit\n");

    return 0;
}

int mt8193_i2c_init(void)
{   
    u32 ret_code;
    
    printf("mt8193_i2c_init() enter\n");    

    /* Sset I2C speed mode */
    ret_code = mt_i2c_set_speed(I2C2, I2C_CLK_RATE, ST_MODE, MAX_ST_MODE_SPEED);
    if( ret_code !=  I2C_OK)
    {
        printf("[mt8193_i2c_init] mt_i2c_set_speed error (%d)\n", ret_code);
        return ret_code;
    }

    printf("mt8193_i2c_init() exit\n"); 

    return (0);
}



int mt8193_init(void)
{
    printf("mt8193_init() enter\n");

	u32 u4Tmp = 0;

    mt8193_i2c_init();

	
	u4Tmp = CKGEN_READ32(REG_RW_LVDSWRAP_CTRL1);
	u4Tmp |= (CKGEN_LVDSWRAP_CTRL1_NFIPLL_MON_EN | CKGEN_LVDSWRAP_CTRL1_DCXO_POR_MON_EN);
	CKGEN_WRITE32(REG_RW_LVDSWRAP_CTRL1, u4Tmp);
	  
	/* close pad_int trapping function*/
	u4Tmp = 0x0;
	CKGEN_WRITE32(REG_RW_PMUX7, u4Tmp);


#if 0
    /*  dcxo enable */
	u4Tmp = CKGEN_READ32(REG_RW_CKMISC_CTRL);
	u4Tmp &= (~CKGEN_CKMISC_CTRL_DCXO_MODE_EN);
	CKGEN_WRITE32(REG_RW_CKMISC_CTRL, u4Tmp);
#endif
	  
    mt8193_pllgp_en();

    mt8193_vopll_en();
      
	printf("mt8193_init() exit\n");
	  

    return (0);
}

