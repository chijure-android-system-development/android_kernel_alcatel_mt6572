
#include "sec_platform.h"
#include "sec_rom_info.h"
#include "sec_ctrl.h"

#define MOD                             "SEC_FLASHTOOL_CFG"

#define SMSG                            dbg_print





void sec_flashtool_cfg_init (void)
{    
    COMPILE_ASSERT(FLASHTOOL_CFG_SIZE == sizeof(FLASHTOOL_SECCFG_T));
    COMPILE_ASSERT(FLASHTOOL_NON_SLA_FORBID_CFG_SIZE == sizeof(FLASHTOOL_FORBID_DOWNLOAD_NSLA_T));
}
