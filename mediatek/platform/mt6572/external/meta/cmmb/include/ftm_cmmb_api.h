
#ifndef FTM_CMMB_API_H
#define FTM_CMMB_API_H
#include "cmmb_errcode.h"
#include "meta_cmmb_para.h"
#ifdef __cplusplus
extern "C" {
#endif

CmmbResult CmmbFtInit();

CmmbResult CmmbFtTerm();

typedef int (*CMMB_AUTOSCAN_CALLBACK)(int channel, int freq);

CmmbResult CmmbFtAutoScan(CMMB_AUTOSCAN_CALLBACK scanProc);

CmmbResult CmmbFtChannelTest(int channel);

CmmbResult CmmbFtSetChannel(int channel);

typedef int (*CMMB_SERVICE_CALLBACK)(int serviceId);

CmmbResult CmmbFtListServices(CMMB_SERVICE_CALLBACK srvProc);

CmmbResult CmmbFtStartService(int serviceId, const char* szMfsFile);

CmmbResult CmmbFtStopService();

#define CMMB_PROP_SNR				1
#define CMMB_PROP_PRE_BER		2
#define CMMB_PROP_RSSI				3


CmmbResult CmmbFtGetProp(CmmbProps* props);


#ifdef __cplusplus
}
#endif

#endif // FTM_CMMB_API_H
