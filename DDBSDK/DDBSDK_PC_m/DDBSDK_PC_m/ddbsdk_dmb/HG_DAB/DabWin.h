/**********************************************************
* Copyright: Silicon Motion (China), Inc.
* All rights reserved.
*
* Program: DabWin.h
* Description:	The API for the DabWin.dll
*				
* Last Modified: 07/04/2010
* History:
* Date		By				Description
* --------	---------------	----------------------------
* 07/04/2010    MOTO			Created
***********************************************************/

#ifndef __DAB_WIN_H__
#define __DAB_WIN_H__

#include "DAB_public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAB_WIN_EXTERN	__declspec(dllexport)
//#define DAB_WIN_EXTERN	extern


DAB_WIN_EXTERN int DAB_OpenDevice( void* parameter );
DAB_WIN_EXTERN int DAB_Scan( unsigned int EnsFreq );
DAB_WIN_EXTERN SERVINFORM DAB_GetInfo( BYTE gPlayList );		//the return is not pointer,please use the DAB_GetSrvInfo instead 
DAB_WIN_EXTERN int DAB_GetSrvInfo( BYTE gPlayList, SERVINFORM* pServiceInform );
DAB_WIN_EXTERN int DAB_SelService( BYTE gPlayList );
DAB_WIN_EXTERN int DAB_GetVersion( void );
DAB_WIN_EXTERN int DAB_CloseDevice(void);


DAB_WIN_EXTERN int vstream_open(void* parameter);
DAB_WIN_EXTERN int DAB_ReadMSC(char* buffer, int max_length );
DAB_WIN_EXTERN int DAB_ClrRecord(void);


#ifdef __cplusplus
}
#endif

#endif /* __DAB_WIN_H__ */