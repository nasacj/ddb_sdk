#ifndef __DAB_PUBLIC_DEF_h__
#define __DAB_PUBLIC_DEF_h__
#include "fci_types.h"

#define  SERVICE_LABEL
#define  SAVED_IN_FILE
#define  SAVED_IN_RESV

typedef enum
{
   DAB_RTN_OK                      = 0x00,                                              /*!< OK */
   DAB_RTN_FAIL                    = 0x01,                                              /*!< Failed */
   DAB_RTN_INVALID_ARG             = 0x02,                                              /*!< Invalid arguments */
   DAB_RTN_NOT_IMPLEMENTED         = 0x03,                                              /*!< Not implemented in the function */
   DAB_RTN_NOT_SUPPORTED           = 0x04,                                              /*!< Not supported in the function */
   DAB_RTN_UNDEFINED_ERR           = 0x05,                                              /*!< Undefined Error */
   DAB_RTN_SERVICE_OUT_OF_CAPACITY = 0x06,                                              /*!<out of service capacity */
   DAB_RTN_SERVICE_ALREADY_STARTED = 0x07,                                              /*!<service already started */
   DAB_RTN_SERVICE_NOT_STARTED     = 0x08,                                              /*!<service not started*/
   DAB_RTN_TUNER_SETUP_FAIL        = 0x09,                                              /*!<service not started*/
   DAB_RTN_OFDM_SYNC_FAIL          = 0x0a,                                              /*!<Ofdm sync lost*/
   DAB_RTN_SYNC_INT_FAIL           = 0x0b,                                              /*!<Ofdm sync lost*/
} DAB_RTN_CODE_E;


#define MAX_ESB_NUM      1
#define MAX_SVC_NUM      40
#define MAX_SC_NUM       40
#define MAX_SUBCH_NUM    64
#define MAX_DIDP_NUM     40
#define MAX_sSVC_NUM    40

#define MX_LABEL_SIZE    32

#define MAX_AFS_NUM      8                     //according to the spec of DAB


#define MAX_AUDIO_NUM    40
#define MAX_VIDEO_NUM    10
#define MAX_DATA_NUM     10

#ifndef IN
#define IN
#define OUT
#endif

#define DAB_PLUS_SUPPORT      1                 //	MOTO add in H1124 for DAB+ project

#define SERVICE_AUDIO_MP2		0x0201
#define SERVICE_AUDIO_AAC		0x0202
#define SERVICE_VIDEO_H264		0x0401
#define SERVICE_DATA_TS			0x0801
#define SERVICE_FIC     		0x1001
#define SERVICE_UNSUPPORT		0x0000


#define ALL_INFO              0xFFFF
#define ENSEMBLE_INFO         0x0100
#define SERVICE_AUDIO_INFO    0x0200
#define SERVICE_VIDEO_INFO    0x0400
#define SERVICE_DATA_INFO     0x0800




typedef struct _SMI_SERVICE_SUBCH_INFO
{
   UINT8  label[MX_LABEL_SIZE];
   UINT16 SrvType;                        // audio, video or else
   UINT8  SrvCharSet;                     // the label's format: Unicode or UTF8 or else
   UINT16 bCharAbbreviatedFlag;           //    16bits AbbreviatedFlag
   UINT8  SubChId;
   UINT16 BitRateKhz;
   //UINT8	IsFavourite;
   //UINT8	GUI_RSV[32-MX_LABEL_SIZE-5];
   UINT8  nscps;
   UINT8  sSubChId;
    UINT16 sDataLenPer24ms;
   UINT8  service_driver[32];
    UINT16 sPacketAddress;
} SMI_SERVICE_SUBCH_INFO;       // if need   // if need ,it  can be fixed to 64Byte, Driver is 32B, the GUI is 32B.

typedef struct _SMI_ENSEMBLE_INFO
{
   UINT32 FreqKhz;
   UINT8  EnsembleName[MX_LABEL_SIZE];
   UINT8  EnCharSet;
   UINT16 bCharAbbreviatedFlag;          //    16bits AbbreviatedFlag
   UINT8  TotalServiceNum;
   UINT8  TotalAudioNum;
   UINT8  TotalVideoNum;
   UINT8  TotalDataNum;
   UINT8  ensemble_driver[32];
} SMI_ENSEMBLE_INFO;

typedef struct _BBM_SERVICE_DRV_INFO
{
   UINT32 FreqKhz;
   UINT8  DrvVID[3];
   UINT8  SubChId;
   UINT32 SId;
   UINT16 SrvType;              // audio, video or else  //H1126	add to support DAB+
   UINT16 DataLenPer24ms;
   u8 nscps;
   UINT8  sSubChId;
    UINT16 sDataLenPer24ms;
   UINT16 sPacketAddress;
} BBM_SERVICE_DRV_INFO;


typedef struct _SMI_SORT_INFO
{
   UINT32 SrcNum;
   UINT32 FreqKhz;
   UINT8  label[MX_LABEL_SIZE];
   UINT32 DstNum;
} SMI_SORT_INFO;


typedef struct
{
   UINT8  hour;
   UINT8  minute;
   UINT8  second;
   UINT16 year;
   UINT8  month;
   UINT8  day;
   UINT8  updated;
} runtime_info;

typedef struct _RTC_TIME
{
   UINT8  Sec08;
   UINT8  Min08;
   UINT8  Hour08;
   UINT8  Week08;
   UINT8  Date08;
   UINT8  Month08;
   UINT16 Year16;
   UINT8  Mode08;
} Time_Info, *PTime_Info;

extern runtime_info run_timer;

typedef struct _SERVINFORM_
{
	BYTE serviceLable[32];
	BYTE EnsembleName[32];
	BYTE SubChannleId;
	WORD ServiceType;
	WORD BitRateKhz;
	BYTE  SrvCharSet;
	BYTE  TotalAudioNum;
	BYTE  TotalVideoNum;
	BYTE  TotalDataNum;
	Time_Info Clndr;
}SERVINFORM;


#define DAB_ERR_MSC                0x0001
#define DAB_ERR_FIC                0x0002
#define DAB_ERR_INT                0x0004

#define DAB_MSG_ERROR_LEVEL        0x0001
#define DAB_MSG_DLS                0x0002
#define DAB_MSG_PTY                0x0004
#define DAB_MSG_CH_MODE            0x0008

#define DAB_STATUS_PLAY            0x00
#define DAB_STATUS_STOP            0x01
#define DAB_STATUS_NO_SIGNAL       0x02
#define DAB_STATUS_SIGNA_RESYNC    0x03
#define DAB_STATUS_PLAY_OK         0x04                                         // Add in 2009.6.2
#define DAB_STATUS_STOP_OK         0x05
extern BYTE g_BBMStatusForDAB;

extern BYTE         g_bySetTimeFlag;
extern BYTE         g_byLTOFlag;


#endif /* __DAB_API_DEF_h__ */
