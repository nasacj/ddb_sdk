// CA_STB_DEMODlg.h : header file
//

#ifndef _CA_STB_DECODE_H_
#define _CA_STB_DECODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <windef.h>
#include <winnt.h>
#include "../src/ddbsdk.h"


//#define  PATHOFCAFILE  "\\MMC_Storage\\ddbsdk\\CaFile.data"

long CA_Init(void);
long CA_initEcmEmmPid(int m_nEcmPid, int m_nEmmPid);

long ECM_data_got(WORD ecm_pid, PBYTE ts_buf);
long EMM_data_got(WORD emm_pid, PBYTE ts_buf);
long   Decrypt_data(PBYTE tsBufOut, PBYTE tsBufIn, deDataType dataType);


#if defined (__cplusplus)
}
#endif
#endif