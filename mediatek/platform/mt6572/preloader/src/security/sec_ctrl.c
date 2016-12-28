
#include "sec_platform.h"
#include "sec_rom_info.h"
#include "sec_ctrl.h"

#define MOD                             "SEC_CTRL"

#define SEC_DEBUG                       (FALSE)
#define SMSG                            print
#if SEC_DEBUG
#define DMSG                            print
#else
#define DMSG 
#endif





void sec_ctrl_init (void)
{    
    COMPILE_ASSERT(AND_SEC_CTRL_SIZE == sizeof(AND_SECCTRL_T));
}
