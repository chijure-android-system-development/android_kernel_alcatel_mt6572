

#include <platform/mt8193.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_i2c.h>


int mt8193_io_init(void)
{
    printf("lk mt8193_io_init() enter\n");

    u32 u4Tmp = 0;

    /* Modify some pad multi function as function 1*/
 
    u4Tmp = CKGEN_READ32(REG_RW_PMUX1);
    u4Tmp |= (1<<PMUX1_PAD_G0_FUNC);
    u4Tmp |= (1<<PMUX1_PAD_B5_FUNC);
    u4Tmp |= (1<<PMUX1_PAD_B4_FUNC);
    CKGEN_WRITE32(REG_RW_PMUX1, u4Tmp);

    u4Tmp = CKGEN_READ32(REG_RW_PMUX2);
    u4Tmp |= (1<<PMUX2_PAD_B3_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_B2_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_B1_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_B0_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_DE_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_VCLK_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_HSYNC_FUNC);
    u4Tmp |= (1<<PMUX2_PAD_VSYNC_FUNC);
    CKGEN_WRITE32(REG_RW_PMUX2, u4Tmp);
    

    printf("lk mt8193_io_init() exit\n");

    return 0;
}


int mt8193_init(void)
{
	  printf("uboot mt8193_init() enter\n");
	  
      mt8193_io_init();
      
	  printf("uboot mt8193_init() exit\n");
	  
    return (0);
}

