 
#include "mt_emi.h"
#include "mt_mmmfg_sram_repair.h"
#include "mt_mmmfg_on_off.h"

int MFG_MM_SRAM_repair(void)
{
   int value = 0;
   /// power/clock on MFG and MM sub sys   
   mmsys_on();
   mfg_on();  
   
   /// do the SRAM repair flow
   value = SRAM_repair(MFG_MMSYS);   
   
   /// power/clock off MFG and MM sub sys
   mfg_off();
   mmsys_off();
      
   return value;
}
