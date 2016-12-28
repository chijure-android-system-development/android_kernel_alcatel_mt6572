
#include <platform/pll.h>

unsigned int mtk_get_bus_freq(void)
{
    kal_uint32 bus_clk = 26000;

    /* For MT6572, check CLK_MUX_SEL(CLK_SEL_0) to get such information */
    kal_uint32 mainpll_con1 = 0, main_diff = 0;
    kal_uint32 clk_sel = 0, pre_div = 1, post_div = 0, vco_div = 1;
    float      n_info = 0, output_freq = 0;

    clk_sel = DRV_Reg32(CLK_SEL_0);

    mainpll_con1 = DRV_Reg32(MAINPLL_CON1);

    post_div = (mainpll_con1 >> 24) & 0x07;
    if(post_div > 0x100)
    {
        output_freq = 0;
    }
    else
    {
        post_div = 0x1 << post_div;
        n_info = (mainpll_con1 & 0x3FFF);
        n_info /= (0x1 << 14);
        n_info += ((mainpll_con1 >> 14) & 0x7F);
        output_freq = 26*n_info*vco_div/pre_div/post_div; //Mhz

        clk_sel = (clk_sel >> 5) & 0x7;
        if( 0x2 == clk_sel )
        {
            bus_clk = output_freq*1000/10;
        }
        else if( 0x4 == clk_sel )
        {
            bus_clk = output_freq*1000/12;
        }
        else
        {
            bus_clk = 26*1000;
        }
    }

    return bus_clk; // Khz
}

void mtk_set_arm_clock(void)
{
    kal_uint32 reg_val = 0;

    /* clock switch - switch AP MCU clock = XTAL */
    reg_val = DRV_Reg32(INFRA_TOPCKGEN_CKMUXSEL);
    reg_val = 0;
    DRV_WriteReg32(INFRA_TOPCKGEN_CKMUXSEL, reg_val);

    /* set ARMPLL */
    DRV_WriteReg32(ARMPLL_CON1, 0x8009A000);

    /* wait 20us (x5) */
    gpt_busy_wait_us(100);

    /* clock switch - switch AP MCU clock = ARMPLL */
    reg_val = DRV_Reg32(INFRA_TOPCKGEN_CKMUXSEL);
    reg_val |= 0x4;
    DRV_WriteReg32(INFRA_TOPCKGEN_CKMUXSEL, reg_val);
}

