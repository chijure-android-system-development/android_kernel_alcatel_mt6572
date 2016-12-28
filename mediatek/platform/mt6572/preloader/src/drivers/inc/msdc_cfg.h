
#ifndef _MSDC_CFG_H_
#define _MSDC_CFG_H_

//#define __FPGA__

//#define MMC_MSDC_DRV_CTP
#define MMC_MSDC_DRV_PRELOADER
//#define MMC_MSDC_DRV_LK

#if !defined(MMC_MSDC_DRV_CTP) && !defined(MMC_MSDC_DRV_PRELOADER) && !defined(MMC_MSDC_DRV_LK)
    #error Please define of MMC_MSDC_DRV_CTP, MMC_MSDC_DRV_PRELOADER, MMC_MSDC_DRV_LK
#elif defined(MMC_MSDC_DRV_CTP) && defined(MMC_MSDC_DRV_PRELOADER)
    #error Please define only one of MMC_MSDC_DRV_CTP, MMC_MSDC_DRV_PRELOADER, MMC_MSDC_DRV_LK
#elif defined(MMC_MSDC_DRV_CTP) && defined(MMC_MSDC_DRV_LK)
    #error Please define only one of MMC_MSDC_DRV_CTP, MMC_MSDC_DRV_PRELOADER, MMC_MSDC_DRV_LK
#elif defined(MMC_MSDC_DRV_PRELOADER) && defined(MMC_MSDC_DRV_LK)
    #error Please define only one of MMC_MSDC_DRV_CTP, MMC_MSDC_DRV_PRELOADER, MMC_MSDC_DRV_LK
#endif


/*--------------------------------------------------------------------------*/
/* Common Definition                  */
/*--------------------------------------------------------------------------*/
#define MMC_DEBUG				(0)
#define MSDC_DEBUG				(0)
#define MSDC_HW_DEBUG			(0) //Introduced by MT6582?

//#define MMC_TEST

//#define MSDC_USE_SDXC_FPGA		(0)	//Introduced by MT6582?
#define MSDC_USE_REG_OPS_DUMP   (0)

//#define MSDC_USE_DMA_MODE		    //Original exist in MT6577/89
//#define MSDC_USE_MMC_STREAM       //Introdcued by ?

//#define MSDC_USE_IRQ			    //For CTP Only, but not necessary for CTP
#define MSDC_USE_CLKDIV_IN_DATCRC (1) // use clk div method for data crc
									//Introduced by MT6582?
#define MSDC_USE_CLKSRC_IN_DATCRC (0)
#define MSDC_USE_EMMC45_POWER	(0) //Introduced by MT6582?

#define MSDC_MODE_DEFAULT_PIO
//#define MSDC_MODE_DEFAULT_DMA_BASIC
//#define MSDC_MODE_DEFAULT_DMA_DESC
//#define MSDC_MODE_DEFAULT_DMA_ENHANCED
//#define MSDC_MODE_DEFAULT_DMA_STREAM

//#define MSDC_WITH_DEINIT

//#define FEATURE_MMC_SDIO			//Original exist in MT6577/89
#define FEATURE_MMC_HS				//Original exist in MT6577/89
//#define FEATURE_MMC_UHS1			//Original exist in MT6577/89
#define FEATURE_MMC_BOOT_MODE		//Original exist in MT6577/89
//#define FEATURE_MMC_CARD_DETECT	//Original exist in MT6577/89
#define FEATURE_MMC_STRIPPED		//Original exist in MT6577/89
#define FEATURE_MMC_RD_TUNING       /* use read tuning */
//#define FEATURE_MMC_WR_TUNING    /* use write tuning */
#define FEATURE_MMC_CM_TUNING       /* use command tuning */
//#define MSDC_TUNE_LOG (1)			//Original exist in MT6577/89
//#define FEATURE_MSDC_ENH_DMA_MODE   //For CTP Only
//#define FEATURE_MMC_MEM_PRESERVE_MODE //For LK only. To dump memory to SD card
#define FEATURE_MMC_ADDR_TRANS      //For LK/Preloader Only. LK and Preload can turn it off for UT or for memory preserved mode

#define USE_2WAY_ACMD41_CMD1		//Introduced by MT6572 BROM
//#define USE_2WAY_ACMD41_FAKE_EFUSE  FALSE //Introduced by MT6572 BROM
#define USE_2WAY_ACMD41_FAKE_EFUSE TRUE

//#if MSDC_DEBUG
//#define MSG_DEBUG
//#endif

//#define MSDC_INLINE_UTILS
//Candidate options
//#define MMC_PROFILING
//#define MSDC_RW_FAKE_FAIL //UT option
//#define MSDC_CMD_FAKE_FAIL  //UT option
//#define MSDC_CMD12_FAKE_FAIL  //UT option
//#define MSDC_CMD_FAKE_TUNE_FAIL     //UT option, let tune always fail
//#define MSDC_CMD_RW_FAKE_TMO    //UT option, let cmd timeout
//#define MSDC_DAT_FAKE_TMO    //UT option, let cmd timeout

#endif /* end of _CONFIG_H_ */

