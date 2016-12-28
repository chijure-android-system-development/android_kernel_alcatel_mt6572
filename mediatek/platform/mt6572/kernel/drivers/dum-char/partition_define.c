#include <linux/module.h>
#include "partition_define.h"
struct excel_info PartInfo[PART_NUM]={
			{"preloader",16777216,0x0, EMMC, 0,BOOT_1},
			{"mbr",524288,0x1000000, EMMC, 0,USER},
			{"ebr1",524288,0x1080000, EMMC, 1,USER},
			{"pro_info",3145728,0x1100000, EMMC, 0,USER},
			{"nvram",5242880,0x1400000, EMMC, 0,USER},
			{"protect_f",10485760,0x1900000, EMMC, 2,USER},
			{"protect_s",10485760,0x2300000, EMMC, 3,USER},
			{"seccfg",131072,0x2d00000, EMMC, 0,USER},
			{"uboot",393216,0x2d20000, EMMC, 0,USER},
			{"bootimg",6291456,0x2d80000, EMMC, 0,USER},
			{"recovery",6291456,0x3380000, EMMC, 0,USER},
			{"sec_ro",6291456,0x3980000, EMMC, 4,USER},
			{"misc",524288,0x3f80000, EMMC, 0,USER},
			{"logo",3145728,0x4000000, EMMC, 0,USER},
			{"ebr2",524288,0x4300000, EMMC, 0,USER},
			{"custpack",642777088,0x4380000, EMMC, 5,USER},
			{"mobile_info",8388608,0x2a880000, EMMC, 6,USER},
			{"expdb",10485760,0x2b080000, EMMC, 0,USER},
			{"android",681574400,0x2ba80000, EMMC, 7,USER},
			{"cache",153092096,0x54480000, EMMC, 8,USER},
			{"usrdata",1073741824,0x5d680000, EMMC, 9,USER},
			{"otp",45088768,0xFFFF0200, EMMC, 0,USER},
			{"bmtpool",22020096,0xFFFF00a8, EMMC, 0,USER},
 };
EXPORT_SYMBOL(PartInfo);

#ifdef  MTK_EMMC_SUPPORT
struct MBR_EBR_struct MBR_EBR_px[MBR_COUNT]={
	{"mbr", {1, 2, 3, 4, }},
	{"ebr1", {5, 6, 7, }},
	{"ebr2", {8, 9, }},
};

EXPORT_SYMBOL(MBR_EBR_px);
#endif

