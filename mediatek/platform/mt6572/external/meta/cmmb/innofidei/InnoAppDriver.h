
#ifndef INNOAPPDRIVER_H
#define INNOAPPDRIVER_H

// InnoAppDriver.h - Innofidei CMMB API functions
//
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char BYTE;

#define STATTYPE_SYNC_STATE			0x00
#define STATTYPE_SIGNAL_STRENGTH	0x01
#define STATTYPE_LDPC_TOTAL_COUNT	0x02
#define STATTYPE_LDPC_ERR_COUNT		0x03
#define STATTYPE_RS_TOTAL_COUNT		0x04
#define STATTYPE_RS_ERR_COUNT		0x05
#define STATTYPE_SIGNAL_QUALITY		0x06                                     //xingyu add
#define FW_ERR_STATUS			              0x07                                    //xingyu add
#define STATTYPE_BER_COUNT                        0x08                                    //xingyu add
#define STATTYPE_SNR_COUNT                        0x09                                   //xingyu add

// INNO_GET_SYS_STATE command parameter
struct inno_sys_state{
	BYTE	stattype;
	union{
		BYTE sync_state;
		int	signal_strength;
		int	ldpc_total_count;
		int   ldpc_err_count;
		int	rs_total_count;
		int	rs_err_count;
		int	signal_quality;
		int    fw_err_status;                  //xingyu 0317 uam debug
		int    BER;
		int    SNR;
	} statdata;
};

typedef enum{
	INNO_NO_ERROR = 0,
	INNO_GENERAL_ERROR = 1,
	INNO_TIMEOUT_ERROR = 2,
	INNO_FW_OPERATION_ERROR = 3,
	INNO_FW_DOWNLOAD_ERROR = 4,
	INNO_PARAMETER_ERROR = 5,
} INNO_RET;

typedef void (*DemuxFrameCallBack)(BYTE *buffer, int buf_size, BYTE channel_id);

INNO_RET InnoAppDriverInit(DemuxFrameCallBack data_callback);


INNO_RET InnoAppDriverDeinit();

INNO_RET InnoMfsMemset(int flag);

INNO_RET InnoSetTunerFrequency(BYTE freq_dot);


INNO_RET InnoSetChannelConfig(BYTE channel_id, BYTE ts_start, BYTE ts_count, BYTE demod,BYTE subframe_ID);


INNO_RET InnoGetChannelConfig(BYTE channel_id, BYTE *ts_start, BYTE *ts_count, BYTE *demod,BYTE* subframe_ID);


INNO_RET InnoGetSysStatus(struct inno_sys_state* sys_state);


INNO_RET InnoCloseChannel(BYTE channel_id);


INNO_RET InnoUamInit();


INNO_RET InnoUamReset(BYTE *pATRValue, unsigned int *pATRLen);


INNO_RET InnoUamTransfer(BYTE *pBufIn, unsigned int bufInLen, BYTE *pBufOut, unsigned short*pBufOutLen, unsigned short *sw);

typedef struct _CMBBMS_SK
{
	unsigned char ISMACrypSalt[18];
	unsigned char SKLength;
}CMBBMS_SK;

typedef struct _CMBBMS_ISMA
{
	unsigned char MBBMS_ECMDataType;
	CMBBMS_SK ISMACrypAVSK[2];

}CMBBMS_ISMA;

#define CMD_SET_CARD_ENV			0x07
#define CMD_SET_MBBMS_ISMA 			0xc8
#define CMD_SET_UAM_OVER 			0xED
#define CMD_SET_UAM_AID_3G			0xc9

typedef enum{
	CAS_OK = 0x00,
	NO_MATCHING_CAS = 0x15,
	CARD_OP_ERROR = 0x17,
	MAC_ERR = 0x80,
	GSM_ERR = 0x81,
	KEY_ERR	= 0x82,
	KS_NOT_FIND	= 0x83,
	KEY_NOT_FIND	= 0x84,
	CMD_ERR	= 0x85,
}ERR_STATUS;
INNO_RET Inno_SetUAMOver(void);
INNO_RET Inno_SetCardEnv(unsigned char airnetwork);

INNO_RET Inno_Set_MBBMS_ISMA(unsigned char isBase64, CMBBMS_ISMA mbbms_isma);
INNO_RET Inno_Set_UAM_AID_3G(unsigned char *uam_aid, unsigned char aid_len);

void Inno_ReadErrStatus(unsigned long *errstatus);
#ifdef __cplusplus
}
#endif

#endif // INNOAPPDRIVER_H
