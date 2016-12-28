


#include "ft_main.h"
#include "meta.h"
#include "FT_Cmd_Para.h"
#include "FT_Public.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FT_PREFIX   "FT: "
#define FT_LOG(fmt, arg ...) META_LOG(FT_PREFIX fmt, ##arg)
//#define FT_LOG(fmt, arg...) printf(FT_PREFIX fmt, ##arg)

int g_fdUsbComPort = -1;

void FTMuxPrimitiveData(META_RX_DATA *pMuxBuf);


int FTT_Init(int dwContext)
{
    FT_Module_Init();
    g_fdUsbComPort = dwContext;
    FT_LOG("[FTT_Drv:] FT Init... ");
    return 1;
}

int FTT_Deinit(int hDeviceContext)
{
    FT_Module_Deinit();
    return 1;
}

void FT_DispatchMessage(void *pLocalBuf, void *pPeerBuf, int local_len, int peer_len)
{
    FT_H  *ft_header;
    ft_header =(FT_H *)pLocalBuf;
    kal_int16 ft_peer_len = peer_len -8;	//del the size of peer buf header
    char *pft_PeerBuf = (char *)pPeerBuf;
    
    pft_PeerBuf += 8; // skip the header of peer buffer
    
    FT_LOG("[FTT_Drv:] FTMainThread ID : %d ", ft_header->id);
    
    switch (ft_header->id)
    {
        case FT_IS_ALIVE_REQ_ID:				//test alive
            FT_TestAlive((FT_IS_ALIVE_REQ *)pLocalBuf);
            break;
        #ifdef FT_WIFI_FEATURE
        case FT_WIFI_REQ_ID:					//wifi test
            FT_WIFI_OP((FT_WM_WIFI_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        #endif
        case FT_SDCARD_REQ_ID:                  //sdcard test
            FT_SDcard_OP((SDCARD_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_READ_REQ_ID:				//nvram read
            FT_APEditorRead_OP((FT_AP_Editor_read_req *)pLocalBuf);
            break;
        case FT_NVRAM_WRITE_REQ_ID:				//nvram write
            FT_APEditorWrite_OP((FT_AP_Editor_write_req *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_RESET_REQ_ID:				//nvram reset
            FT_APEditorReset_OP((FT_AP_Editor_reset_req *)pLocalBuf);
            break;
        case FT_GPIO_REQ_ID:
            FT_GPIO_OP((GPIO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_VER_INFO_REQ_ID:				//get version info
            FT_GetVersionInfo((FT_VER_INFO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        /*
        case FT_CHECK_META_VER_REQ_ID:			//require meta dll version
            FT_CheckMetaDllVersion((FT_CHECK_META_VER_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        */
        case FT_POWER_OFF_REQ_ID:				//power off target
            FT_PowerOff((FT_POWER_OFF_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_REG_WRITE_ID:					//baseband reg wirte
            FT_CPURegW_OP((FT_REG_WRITE_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_REG_READ_ID:					//baseband reg read
            FT_CPURegR_OP((FT_REG_READ_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_CCT_REQ_ID:
            FT_CCAP_OP((FT_CCT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_UTILITY_COMMAND_REQ_ID:			//utility command
            FT_Peripheral_OP((FT_UTILITY_COMMAND_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_PMIC_REG_READ_ID:				//pmic read
            FT_PMICRegR_OP((FT_PMIC_REG_READ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_PMIC_REG_WRITE_ID:				//pmic write
            FT_PMICRegW_OP((FT_PMIC_REG_WRITE *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
    #ifdef FT_BT_FEATURE 
        case FT_BT_REQ_ID:						//bt test
            FT_BT_OP((BT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#endif
        /*
        case FT_WM_DVB_REQ_ID:					//dvb test
            FT_DVBT_OP((FT_DVB_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_WM_BAT_REQ_ID:
            FT_BAT_OP((FT_BATT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        */
        case FT_ADC_GETMEADATA_ID:				//adc test
            FT_AUXADC_OP((AUXADC_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#ifdef FT_GPS_FEATURE		
        case FT_GPS_REQ_ID:
            FT_GPS_OP((GPS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
    #endif
        case FT_BATT_READ_INFO_REQ_ID:
            FT_BAT_FW_OP((FT_BATT_READ_INFO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_BAT_CHIPUPDATE_REQ_ID:
            FT_BAT_ChipUpdate_OP((FT_BATT_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#ifdef FT_FM_FEATURE 	
        case FT_FM_REQ_ID:
            FT_FM_OP((FM_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
	#endif
        case FT_L4AUD_REQ_ID:
            FT_L4AUDIO_OP((FT_L4AUD_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_LOW_POWER_REQ_ID:
            FT_LOW_POWER_OP((FT_LOW_POWER_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_BACKUP_REQ_ID:
            FT_NVRAM_Backup_OP((FT_NVRAM_BACKUP_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_NVRAM_RESTORE_REQ_ID:
            FT_NVRAM_Restore_OP((FT_NVRAM_RESTORE_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GSENSOR_REQ_ID:
            FT_GSENSOR_OP((GS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_META_MODE_LOCK_REQ_ID:
            FT_META_MODE_LOCK((FT_META_MODE_LOCK_REQ *)pLocalBuf);
            break;
        case FT_MATV_CMD_REQ_ID:
        #ifdef FT_MATV_FEATURE		
            FT_MATV_OP((FT_MATV_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
		#else
            FT_MATV_CNF matv_cnf;
	       memset(&matv_cnf, 0, sizeof(FT_MATV_CNF));
           matv_cnf.header.id = FT_MATV_CMD_CNF_ID;
	       matv_cnf.header.token = ((FT_MATV_REQ *)pLocalBuf)->header.token;
           matv_cnf.status = META_NOT_SUPPORT;
           matv_cnf.type = ((FT_MATV_REQ *)pLocalBuf)->type;
		  WriteDataToPC(&matv_cnf, sizeof(matv_cnf), NULL, 0);
        #endif
            break;
        // Reboot device
        case FT_REBOOT_REQ_ID:
        FT_Reboot((FT_META_REBOOT_REQ *)pLocalBuf);
        break;
        case FT_CUSTOMER_REQ_ID:
            FT_CUSTOMER_OP((FT_CUSTOMER_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GET_CHIPID_REQ_ID:
            FT_GET_CHIPID_OP((FT_GET_CHIPID_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_MSENSOR_REQ_ID:
            FT_MSENSOR_OP((FT_MSENSOR_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_ALSPS_REQ_ID:
            FT_ALSPS_OP((FT_ALSPS_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
        case FT_GYROSCOPE_REQ_ID:
            FT_GYROSENSOR_OP((GYRO_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
            break;
		// Touch panel
		case FT_CTP_REQ_ID:
			FT_CTP_OP((Touch_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
		break;
	// Get version info V2
    	case FT_VER_INFO_V2_REQ_ID:
        	FT_GetVersionInfoV2((FT_VER_INFO_V2_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
        	break;
        	
        #ifdef FT_NFC_FEATURE
        case FT_NFC_REQ_ID:
               FT_NFC_OP((NFC_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
               break;
        #endif
    #ifdef FT_EMMC_FEATURE
		case FT_EMMC_REQ_ID:
			FT_CLR_EMMC_OP((FT_EMMC_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		case FT_CRYPTFS_REQ_ID:
			FT_CRYPTFS_OP((FT_CRYPTFS_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		#endif
		
		case FT_BUILD_PROP_REQ_ID:
			FT_BUILD_PROP_OP((FT_BUILD_PROP_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		case FT_MODEM_REQ_ID:
			FT_MODEM_INFO_OP((FT_MODEM_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;

		case FT_SIM_NUM_REQ_ID:
			FT_SIM_NUM_OP((FT_GET_SIM_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
		case FT_DFO_REQ_ID:
			FT_DFO_OP((FT_DFO_REQ *)pLocalBuf,(char *)pft_PeerBuf, ft_peer_len);
			break;
                case FT_ADC_REQ_ID:
			FT_ADC_OP((ADC_REQ *)pLocalBuf, (char *)pft_PeerBuf, ft_peer_len);
			break;
        default:
            //printf((TEXT("[FTT_Drv:] FTMainThread Error:!!! ID: %hu "), ft_header->id));
            FT_LOG("[FTT_Drv:] FTMainThread Error:!!! ID ");
        break;
    }
}

void FTMuxPrimitiveData(META_RX_DATA *pMuxBuf)
{
    /* This primitive is logged by TST */
    unsigned char *pTempBuf = NULL;
    unsigned char *pTempDstBuf = NULL;
    unsigned char *pMamptrBase = NULL;
    unsigned char *pDestptrBase = NULL;
    int iCheckNum = 0;
    int dest_index=0;
    unsigned char cCheckSum = 0;
    int cbWriten = 0;
    int cbTxBuffer = 0;
    int i=0;
    
    if(pMuxBuf == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData pMuxBuf is NULL");
        return;
    }
    
    cbTxBuffer = pMuxBuf->LocalLen + pMuxBuf->PeerLen + 9;
    if (cbTxBuffer>FrameMaxSize)
    {
        FT_LOG("[TST_Drv:] FTMuxPrimitiveData: error frame size is too big!! ");
        return;
    }
    else
        FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Type-%d Local_len-%d, Peer_len-%d", pMuxBuf->eFrameType, pMuxBuf->LocalLen, pMuxBuf->PeerLen);
    
    FT_LOG("[TST_Drv:] FTMuxPrimitiveData: total size is -%d", cbTxBuffer);
    pMamptrBase = (unsigned char *)malloc(cbTxBuffer);
    //Wayne add MAX_TST_TX_BUFFER_LENGTH
    if(pMamptrBase == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData malloc pMamptrBase Fail");
        return;
    }
    pDestptrBase = (unsigned char *)malloc(MAX_TST_TX_BUFFER_LENGTH);//2048);
    if(pDestptrBase == NULL)
    {
        FT_LOG("Err: FTMuxPrimitiveData malloc pDestptrBase Fail");
        free(pMamptrBase);
        return;
    }
    
    
    pTempDstBuf = pDestptrBase;
    pTempBuf = pMamptrBase;
    
    /* fill the frameheader */
    *pTempBuf++ = 0x55;
    *pTempBuf++=((pMuxBuf->LocalLen + pMuxBuf->PeerLen +5)&0xff00)>>8;
    *pTempBuf++= (pMuxBuf->LocalLen + pMuxBuf->PeerLen +5)&0xff;
    *pTempBuf++ = 0x60;
    
    /*fill the local and peer data u16Length and its data */
    *pTempBuf++ = ((pMuxBuf->LocalLen)&0xff); /// pMuxBuf->LocalLen ;
    *pTempBuf++ = ((pMuxBuf->LocalLen)&0xff00)>>8;
    *pTempBuf++ = (pMuxBuf->PeerLen )&0xff;   ///pMuxBuf->PeerLen ;
    *pTempBuf++ = ((pMuxBuf->PeerLen)&0xff00)>>8;
    
    memcpy((pTempBuf), pMuxBuf->uData, pMuxBuf->LocalLen + pMuxBuf->PeerLen);
    
    pTempBuf = pMamptrBase;
    
    /* 0x5a is start data, so we use 0x5a and 0x01 inidcate 0xa5, use 0x5a and 0x5a indicate 0x5a
    the escape is just for campatiable with feature phone */
    while (iCheckNum != (cbTxBuffer-1))
    {
        cCheckSum ^= *pTempBuf;
        *pTempDstBuf = *pTempBuf;
        iCheckNum++;
        
        if (*pTempBuf ==0xA5 )
        {
            *pTempDstBuf++ = 0x5A;
            *pTempDstBuf++ = 0x01;
            dest_index++;		//do the escape, dest_index should add for write to uart or usb
        }
        else if (*pTempBuf ==0x5A )
        {
            *pTempDstBuf++ = 0x5A;
            *pTempDstBuf++ = 0x5A;
            dest_index++;		//do the escape, dest_index should add for write to uart or usb
        }
        else
            pTempDstBuf++;
        
        dest_index++;
        pTempBuf++;
    }
    
    /* 0x5a is start data, so we use 0x5a and 0x01 inidcate 0xa5 for check sum, use 0x5a and 0x5a indicate 0x5a
    the escape is just for campatiable with feature phone */
    if ( cCheckSum ==0xA5 )
    {
        dest_index++;		//do the escape, dest_index should add for write to uart or usb
        //Wayne replace 2048 with MAX_TST_RECEIVE_BUFFER_LENGTH
        if ((dest_index) > MAX_TST_TX_BUFFER_LENGTH)//2048)
        {
            FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Data is too big: index-%d cbTxBuffer-%d ",dest_index, cbTxBuffer);
            goto TSTMuxError;
        }
        
        *pTempDstBuf++= 0x5A;
        *pTempDstBuf = 0x01;
    }
    else if ( cCheckSum ==0x5A )
    {
        dest_index++;		//do the escape, dest_index should add for write to uart or usb
        if ((dest_index) > MAX_TST_TX_BUFFER_LENGTH)//2048)
        {
            FT_LOG("[TST_Drv:] FTMuxPrimitiveData: Data is too big: index-%d cbTxBuffer-%d ",dest_index, cbTxBuffer);
            goto TSTMuxError;
        }
        *pTempDstBuf++= 0x5A;
        *pTempDstBuf = 0x5A;
    }
    else
        *pTempDstBuf =(char )cCheckSum;
    
    dest_index++;
    
    //write to PC
    cbWriten = write(g_fdUsbComPort, (void *)pDestptrBase, dest_index);
    pTempDstBuf = pDestptrBase;
    /*
    for (i =0; i<cbWriten; i++)
    {
    FT_LOG("%2x ",*(pTempDstBuf+i));
    
    if ((i+1)%16 ==0)
    FT_LOG("");
    
    }
    */
    //FT_LOG("");
    
    FT_LOG("[TST_Drv:] FTMuxPrimitiveData: %d  %d %d  cChecksum: %d ",cbWriten, cbTxBuffer, dest_index,cCheckSum);
    
    TSTMuxError:
    
    free(pMamptrBase);
    free(pDestptrBase);
}


#ifdef __cplusplus
}
#endif


int WriteDataToPC(void *Local_buf,unsigned short Local_len,void *Peer_buf,unsigned short Peer_len)
{
    META_RX_DATA sTempRxbuf ;
    unsigned char *cPeerbuf = &sTempRxbuf.uData[Local_len+8];
    
    memset(&sTempRxbuf, 0, sizeof(sTempRxbuf));
    
    //add the header of frame type
    sTempRxbuf.eFrameType = AP_FRAME;
    sTempRxbuf.LocalLen = Local_len ;
    
    //check the buffer size.
    if (Peer_len >0)
        sTempRxbuf.PeerLen =  Peer_len +8;
    else
        sTempRxbuf.PeerLen = 0;
    
    if (((Local_len + Peer_len)> FTMaxSize)||(Peer_len >PeerBufMaxlen))
    {
        FT_LOG("[FTT_Drv:] WriteDataToPC Error: Local_len-%hu Peer_len- %hu", Local_len,Peer_len);
        return 0;
    }
    
    if ((Local_len == 0) && (Local_buf == NULL))
    {
        FT_LOG("[FTT_Drv:] WriteDataToPC Error: Local_len-%hu Peer_len- %hu", Local_len,Peer_len);
        return 0;
    }
    
    // copy to the temp buffer, and send it to the tst task.
    memcpy(sTempRxbuf.uData, Local_buf, Local_len);
    if ((Peer_len >0)&&(Peer_buf !=NULL))
        memcpy(cPeerbuf, Peer_buf, Peer_len);
    
    FTMuxPrimitiveData(&sTempRxbuf);
    return 1;
}

