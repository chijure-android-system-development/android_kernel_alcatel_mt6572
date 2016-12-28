
#ifndef MT_EMI_H
#define MT_EMI_H

#include "typedefs.h"

//typedef unsigned int            kal_uint32;
//typedef int			kal_int32;

//extern void mt6516_set_emi (void);
//extern void mt6516_256M_mem_setting (void);
int get_dram_rank_nr (void);
void get_dram_rank_size (int dram_rank_size[]);
#define DDR1  1
#define DDR2  2
#define DDR3  3
typedef struct
{
    int  type;                /* 0x0000 : Invalid
                                 0x0001 : Discrete DDR1
                                 0x0002 : Discrete DDR2
                                 0x0003 : Discrete DDR3
                                 0x0101 : MCP(NAND+LPDDR1)
                                 0x0102 : MCP(NAND+LPDDR2)
                                 0x0103 : MCP(NAND+LPDDR3)
                                 0x0201 : MCP(eMMC+LPDDR1)
                                 0x0202 : MCP(eMMC+LPDDR2)
                                 0x0203 : MCP(eMMC+LPDDR3)
                              */
    char  ID[12];
    int   id_length;              // EMMC and NAND ID/FW ID checking length
    kal_uint32 EMI_Freq;              //200 / 266 /333 Mhz
    kal_uint32 EMI_DRVA_value;
    kal_uint32 EMI_DRVB_value;
 
    kal_uint32 EMI_ODLA_value;
    kal_uint32 EMI_ODLB_value;
    kal_uint32 EMI_ODLC_value;
    kal_uint32 EMI_ODLD_value;
    kal_uint32 EMI_ODLE_value;
    kal_uint32 EMI_ODLF_value;
    kal_uint32 EMI_ODLG_value;
    kal_uint32 EMI_ODLH_value;
    kal_uint32 EMI_ODLI_value;
    kal_uint32 EMI_ODLJ_value;
    kal_uint32 EMI_ODLK_value;
    kal_uint32 EMI_ODLL_value;
    kal_uint32 EMI_ODLM_value;
    kal_uint32 EMI_ODLN_value;
 
    kal_uint32 EMI_CONI_value;
    kal_uint32 EMI_CONJ_value;
    kal_uint32 EMI_CONK_value;
    kal_uint32 EMI_CONL_value;
    kal_uint32 EMI_CONN_value;
 
    kal_uint32 EMI_DUTA_value;
    kal_uint32 EMI_DUTB_value;
    kal_uint32 EMI_DUTC_value;
 
    kal_uint32 EMI_DUCA_value;
    kal_uint32 EMI_DUCB_value;
    kal_uint32 EMI_DUCE_value;
 
    kal_uint32 EMI_IOCL_value;

    kal_uint32 EMI_GEND_value;

    int   DRAM_RANK_SIZE[4];    
    int   reserved[2];
} EMI_SETTINGS;


int mt_get_dram_type (void); 
/* 0: invalid */
/* 1: mDDR1 */
/* 2: mDDR2 */
/* 3: mDDR3 */

/* SRAM repair HW module*/
typedef enum
{
	MFG_MMSYS = 0,
	MDSYS,
	HSPA,
	TDDSYS	
}REPAIR_MODULE;

int SRAM_repair(REPAIR_MODULE module);

#endif
