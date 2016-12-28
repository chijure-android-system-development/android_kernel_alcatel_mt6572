
#ifndef MMC_DBG_H
#define MMC_DBG_H

#include "mmc_core.h"

extern void mmc_dump_card_status(u32 card_status);
extern void mmc_dump_ocr_reg(u32 resp);
extern void mmc_dump_rca_resp(u32 resp);
extern void mmc_dump_io_resp(u32 resp);
extern void mmc_dump_tuning_blk(u8 *buf);
extern void mmc_dump_csd(struct mmc_card *card);
extern void mmc_dump_ext_csd(struct mmc_card *card);
extern void mmc_prof_card_init(void *data, ulong id, ulong counts);
extern void mmc_prof_read(void *data, ulong id, ulong counts);
extern void mmc_prof_write(void *data, ulong id, ulong counts);

#endif /* MMC_DBG_H */

