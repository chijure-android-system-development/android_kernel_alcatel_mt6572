
#ifndef _MSDC_IRQ_H_
#define _MSDC_IRQ_H_

#include "mmc_core.h"

#if defined(MSDC_USE_IRQ)
//For CTP Only
extern volatile u32 msdc_irq_sts[];
#endif

#if defined(MMC_MSDC_DRV_LK)
extern void lk_msdc_irq_handler(unsigned int irq);
extern u32 msdc_lk_intr_wait(struct mmc_host *host, u32 intrs);
#endif

extern void msdc_irq_init(struct mmc_host *host);
extern u32 msdc_intr_wait_cmd(struct mmc_host *host, u32 intrs);


#endif
