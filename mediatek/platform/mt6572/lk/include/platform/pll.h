
#ifndef PLL_H
#define PLL_H

#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>

#define AP_PLL_CON0         (APMIXED_BASE+0x000)
#define AP_PLL_CON1         (APMIXED_BASE+0x004)
#define AP_PLL_CON2         (APMIXED_BASE+0x008)
#define AP_PLL_CON3         (APMIXED_BASE+0x00C)
#define AP_PLL_CON4         (APMIXED_BASE+0x010)

#define PLL_HP_CON0         (APMIXED_BASE+0x014)
#define PLL_HP_CON1         (APMIXED_BASE+0x018)

#define CLKSQ_STB_CON0      (APMIXED_BASE+0x01C)

#define PLL_PWR_CON0        (APMIXED_BASE+0x020)
#define PLL_PWR_CON1        (APMIXED_BASE+0x024)
#define PLL_PWR_CON2        (APMIXED_BASE+0x028)
#define PLL_PWR_CON3        (APMIXED_BASE+0x02C)

#define PLL_ISO_CON0        (APMIXED_BASE+0x030)
#define PLL_ISO_CON1        (APMIXED_BASE+0x034)
#define PLL_ISO_CON2        (APMIXED_BASE+0x038)
#define PLL_ISO_CON3        (APMIXED_BASE+0x03C)

#define PLL_EN_CON0         (APMIXED_BASE+0x040)
#define PLL_EN_CON1         (APMIXED_BASE+0x044)

#define PLL_STB_CON0        (APMIXED_BASE+0x048)
#define PLL_STB_CON1        (APMIXED_BASE+0x04C)
#define PLL_STB_CON2        (APMIXED_BASE+0x050)

#define DIV_STB_CON0        (APMIXED_BASE+0x054)

#define PLL_CHG_CON0        (APMIXED_BASE+0x058)
#define PLL_CHG_CON1        (APMIXED_BASE+0x05C)

#define PLL_TEST_CON0       (APMIXED_BASE+0x060)
#define PLL_TEST_CON1       (APMIXED_BASE+0x064)

#define ARMPLL_CON0         (APMIXED_BASE+0x100)
#define ARMPLL_CON1         (APMIXED_BASE+0x104)
#define ARMPLL_CON2         (APMIXED_BASE+0x108)
#define ARMPLL_CON3         (APMIXED_BASE+0x10C)
#define ARMPLL_CON3         (APMIXED_BASE+0x10C)
#define ARMPLL_PWR_CON0     (APMIXED_BASE+0x110)

#define MAINPLL_CON0        (APMIXED_BASE+0x120)
#define MAINPLL_CON1        (APMIXED_BASE+0x124)
#define MAINPLL_CON2        (APMIXED_BASE+0x128)
#define MAINPLL_CON3        (APMIXED_BASE+0x12C)
#define MAINPLL_PWR_CON0    (APMIXED_BASE+0x130)

#define UNIVPLL_CON0        (APMIXED_BASE+0x140)
#define UNIVPLL_CON1        (APMIXED_BASE+0x144)
#define UNIVPLL_CON2        (APMIXED_BASE+0x148)
#define UNIVPLL_CON3        (APMIXED_BASE+0x14C)
#define UNIVPLL_PWR_CON0    (APMIXED_BASE+0x150)

#define AP_ABIST_MON_CON0   (APMIXED_BASE+0xE00)
#define AP_ABIST_MON_CON1   (APMIXED_BASE+0xE04)
#define AP_ABIST_MON_CON2   (APMIXED_BASE+0xE08)
#define AP_ABIST_MON_CON3   (APMIXED_BASE+0xE0C)

#define CLK_SEL_0           (TOPCKGEN_BASE+0x000)
#define CLK_SEL_2           (TOPCKGEN_BASE+0x008)
#define CLK_SEL_3           (TOPCKGEN_BASE+0x00C)
#define CLK_SEL_4           (TOPCKGEN_BASE+0x010)
#define CLK_SWCG_0          (TOPCKGEN_BASE+0x020)
#define CLK_SWCG_1          (TOPCKGEN_BASE+0x024)
#define CLK_SWCG_2          (TOPCKGEN_BASE+0x028)
#define CLK_SWCG_3          (TOPCKGEN_BASE+0x02C)
#define CLK_SWCG_4          (TOPCKGEN_BASE+0x030)
#define CLK_SWCG_5          (TOPCKGEN_BASE+0x034)
#define CLK_SWCG_5          (TOPCKGEN_BASE+0x038)
#define CLK_SETCG_0         (TOPCKGEN_BASE+0x050)
#define CLK_SETCG_1         (TOPCKGEN_BASE+0x054)
#define CLK_SETCG_2         (TOPCKGEN_BASE+0x058)
#define CLK_SETCG_3         (TOPCKGEN_BASE+0x05C)
#define CLK_SETCG_4         (TOPCKGEN_BASE+0x060)
#define CLK_SETCG_5         (TOPCKGEN_BASE+0x064)
#define CLK_SETCG_6         (TOPCKGEN_BASE+0x068)
#define CLK_CLRCG_0         (TOPCKGEN_BASE+0x080)
#define CLK_CLRCG_1         (TOPCKGEN_BASE+0x084)

#define ACLKEN_DIV          (MCUSYS_CFGREG_BASE+0x060)
#define PCLKEN_DIV          (MCUSYS_CFGREG_BASE+0x064)

#define INFRA_TOPCKGEN_CKMUXSEL (INFRA_SYS_CFG_AO_BASE+0x000)

extern unsigned int mtk_get_bus_freq(void);
extern void mtk_set_arm_clock(void);

#endif