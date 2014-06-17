/***************************************************************************
 *   File Name: CI_SDK_Typedef.h
 *   Create Date: 2007-01-01 (YYYY-MM-DD)
 *   Vesion: 01.00.00 (xx.xx.xx)
 *   Copyright (C) 2007 by Cidana(ShangHai)                                      
 *   Email: wyvern.zhao@cidana.com                                                     
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *                          
 *   The above copyright notice and this permission notice shall be 
 *   included in all copies or substantial portions of the Software.
 *                                                                         
 *   You should have received a copy of the Cidana License     
 *   along with this program; if not, write to the                         
 *   ShangHai Cidana, Inc.                                       
 ***************************************************************************/


#ifndef _CI_SDK_TYPEDEF_H_
#define _CI_SDK_TYPEDEF_H_

typedef long            ci32s;
typedef char			ci8s;
typedef unsigned char   ci8u;
typedef unsigned short  ci16u;
typedef unsigned long   ci32u;

typedef void            ciVoid;
typedef void*           ciHandle;
typedef ci32s           ciRESULT;
typedef ci32u*          ciCOLORREF;

#define CI_ERR_NONE               0           // no error
#define CI_ERR_FAIL               0x80000001  // failed
#define CI_ERR_INVALID_PARAM      0x80000002  // invalid parameter
#define CI_ERR_NOT_INITED         0x80000003  // sdk not initialized
#define CI_ERR_NOT_SUPPORT        0x80000004  // file format is not supportted
#define CI_ERR_AUDIO_DEVICE_FAIL  0x80000005  // failed to operate audio device
#define CI_ERR_VIDEO_DEVICE_FAIL  0x80000006  // failed to operate video device
#define CI_ERR_TIMEBOMB_EXPIRE    0x80000007  // time bomb expired
#define CI_ERR_SDK_CREATE_FAIL    0x80000008  // Create SDK Fail 
#define CI_ERR_ENGINE_CREATE_FAIL 0x80000009  // Create Engine Fail 
#define FILES_FORMAT_NOT_SUPPORT	(0x80001000)
#define VIDEO_FORMAT_NOT_SUPPORT	(0x80002000)
#define AUDIO_FORMAT_NOT_SUPPORT	(0x80003000)

#define CI_MAX_PID_NUM	4			// For DVBT_ServiceInfo count maximum PID number

#ifdef USE_DLL
#define CI_API __declspec(dllexport)
#else
#define CI_API 
#endif

typedef struct _ECM_Data
{
	ci8u  ecm_buf[4096];
	ci32u ecm_length;
	ci8u  ecm_table_id;
	ci16u cas_ID;
	ci32u isnew;
} ECM_DATA, *PECM_DATA;

typedef enum
{
    DISPLAY_VIDEO_STRETCH,
    DISPLAY_VIDEO_LETTERBOX,    
    DISPLAY_VIDEO_CROPPING,    
}VIDEO_DISPLAY_MODE;

typedef enum
{
	CI_CFG_PLAYER_MODE,
	CI_CFG_DISPLAY_PARAM,
	CI_CFG_PLAYER_STARTPOS,
	CI_CFG_PLAYER_ENDPOS,
	CI_CFG_COMMON_ROTATE,
	CI_CFG_COMMON_ZOOM,
	CI_CFG_GRAPH_PROPERTY,
	CI_CFG_AUDIO_VOLUME,
	CI_CFG_AUDIO_VOLUME2,
	CI_CFG_AUDIO_RESAMPLERATE,
	CI_CFG_CLIP_MODE,
	CI_CFG_VIDEO_RENDER,
	CI_CFG_AUDIO_RENDER,
	CI_CFG_TIMESHIFTING_PATH,
	CI_CFG_SHOWFIRSTFRAME,
	CI_CFG_AUDIO_EQUALIZER,
	CI_CFG_OPEN_PROGRAM_SELECT,
	CI_CFG_OPEN_SERVICE_ID,
	CI_CFG_CSCCOEF,
	CI_CFG_VIDEO_ASPECTRATIO,
	CI_CFG_VIDEO_PROPERTY,
	CI_CFG_SHOW_SUBTITLE,
	CI_CFG_SUBTITLE_INFO,
	CI_CFG_SUBTITLE_IDX,
	CI_CFG_AUDSTM_INFO,
	CI_CFG_AUDSTM_IDX,
	CI_CFG_AUD_CHLCFG,
	CI_CFG_AV_STREAM_READY,
	CI_CFG_STREAM_TIME,
	CI_CFG_COUNTRY_CODE,
	CI_CFG_TS_PARAMETER,
	CI_CFG_OUTDATEDTIME,
	CI_CFG_AUDRND_THREAD_PRIORITY_OFFSET,
	CI_CFG_VIDRND_THREAD_PRIORITY_OFFSET,
	CI_CFG_VIDEO_RENDER_MODE,
	CI_CFG_VIDEO_RENDER_WND,
	CI_CFG_VIDEO_RENDER_EFFECT,

	// aes decryption
	CI_CFG_SET_AESKEY,
	CI_CFG_REMOVE_AESKEY,

	CI_CFG_BML_AVAILABLE,
    CI_CFG_VIDEO_DISPLAY_MODE,//see VIDEO_DISPLAY_MODE above

	//for debug
	CI_CFG_PERFORMANCE,
	CI_CFG_ADJUST_AVSYNC,

	// stream playback support
	CI_CFG_STREAM_PROGRAM_NUMBER,
	CI_CFG_STREAM_PROGRAM_SERVICEID,
	CI_CFG_STREAM_AUDIO_ID,
	CI_CFG_STREAM_VIDEO_ID,
	CI_CFG_STREAM_GETECM_DATA,

	CI_CMD_SNAPSHOT,
	CI_CMD_SNAPSHOT_WITH_SIZE,
	CI_CMD_TIMESHIFT_START,
	CI_CMD_TIMESHIFT_STOP,
	CI_CFG_TS_PAUSE,
	CI_CFG_TS_SEEK,
#ifdef ANDROID	
	CI_CFG_ANDROID_ISURFACE,
#endif	
	CI_CFG_AUDIO_MUTE,
	
}PLAYER_CFG_TYPE;

/**************************************************************************************************
 * Function declare:
 *	PFNCI_PLAYER_CALLBACK
 * Description:
 *	 This callback function is designed for the host application to inject code between frames during
 *  a lengthy decoding operation. The callback function will be called once per video frame by the
 *  player.The implementation of the callback should avoid heavy-duty works.
 * Parameters:
 *	wParam	[in]:
 *		Param, usually point out the lParam's type.
 *	lParam	[in]:
 *		Param, usually point to a struct.
 *	bHandled		[in/out]:
 *		Does the default callback function need to carry out.
 * Return value:
 *	none
 *************************************************************************************************/
#ifndef _PLAY_CALLBACK_ARRAY_
#define _PLAY_CALLBACK_ARRAY_
typedef long (*PFNCI_PLAYER_CALLBACK) (void* pvParam1, void* pvParam2, void* pvParam3, void* pvContext);
typedef struct _PLAY_CALLBACK_ARRAY
{
	PFNCI_PLAYER_CALLBACK	GraphDeviceOpenProc;
	PFNCI_PLAYER_CALLBACK	SoundDeviceOpenProc;
	PFNCI_PLAYER_CALLBACK	GraphDeviceCloseProc;
	PFNCI_PLAYER_CALLBACK 	SoundDeviceCloseProc;
	PFNCI_PLAYER_CALLBACK	DrawBeginProc;
	PFNCI_PLAYER_CALLBACK	DrawEndProc;
	PFNCI_PLAYER_CALLBACK	WaveoutBeginProc;
	PFNCI_PLAYER_CALLBACK	WaveoutEndProc;
	PFNCI_PLAYER_CALLBACK	StatusChangedProc;
	PFNCI_PLAYER_CALLBACK	TimeElaspedChangedProc;
	PFNCI_PLAYER_CALLBACK	GetStreamData;
	PFNCI_PLAYER_CALLBACK	SetStreamData;
	PFNCI_PLAYER_CALLBACK	SetECMData;
	PFNCI_PLAYER_CALLBACK	GetRawESVideo;
	PFNCI_PLAYER_CALLBACK	GetRawESAudio;
	PFNCI_PLAYER_CALLBACK	OutputPcmData;
	PFNCI_PLAYER_CALLBACK	GetMetaData;
	PFNCI_PLAYER_CALLBACK   GetMetaDataExt;
	PFNCI_PLAYER_CALLBACK	OutputRGB565;
	PFNCI_PLAYER_CALLBACK	GetCMMBVideoOpt;
	PFNCI_PLAYER_CALLBACK	GetCMMBAudioOpt;
	PFNCI_PLAYER_CALLBACK	MessageCallBack;
	PFNCI_PLAYER_CALLBACK	OutputYUVData;
	PFNCI_PLAYER_CALLBACK	OutputRGBData;
	PFNCI_PLAYER_CALLBACK	OutputXPEData;
	PFNCI_PLAYER_CALLBACK   OutputEWSInfo;
	PFNCI_PLAYER_CALLBACK   BmlProc;
	PFNCI_PLAYER_CALLBACK  PlayEndCallBack;
	//not necessary, for debug
	PFNCI_PLAYER_CALLBACK	VideoThreadEnterProc; 
	PFNCI_PLAYER_CALLBACK	AudioThreadEnterProc;
	PFNCI_PLAYER_CALLBACK	VideoThreadQuitProc; 
	PFNCI_PLAYER_CALLBACK	AudioThreadQuitProc;

	void*   pCallbackInstance;
}PLAY_CALLBACK_ARRAY, *LP_PLAY_CALLBACK_ARRAY;

typedef struct _tagRECT
{ 
	ci32s left;
	ci32s top;
	ci32s right;
	ci32s bottom;
}CI_RECT;

typedef enum _tagCI_EVENT
{
	CI_EVENT_UNKNOWN = 0,
	CI_EVENT_VIDEO_SRCSIZE_CHANGED,
	CI_EVENT_EPG_UPDATE,
	CI_EVENT_EPG_SDT_COMPLETE,
	CI_EVENT_EPG_EIT_COMPLETE,
	CI_EVENT_CP_VIDEO,
	CI_EVENT_PARENTCTRL_BLOCK,
	CI_EVENT_VIDEO_FIRST_FRAME_SHOWED,
	CI_EVENT_PLAYBACK_BUFFER_INSUFFICIENT,
	CI_EVENT_PLAYBACK_BUFFER_ENOUGH,
}CI_EVENT;

typedef enum _tagCI_PARENTAL_CONTENT_TYPE
{	
	CI_PARENTAL_CONTENT_DRUGS		= (1<<0),
	CI_PARENTAL_CONTENT_VIOLENCE	= (1<<1),
	CI_PARENTAL_CONTENT_SEX			= (1<<2),
}CI_PARENTAL_CONTENT_TYPE;

typedef enum _tagCI_LANGUAGE_TYPE
{
	CI_LAN_UNKNOWN = 0,
	CI_LAN_EN,    // English
	CI_LAN_FR,    // French
	CI_LAN_ZH,    // Chinese
	CI_LAN_JA,    // Japanese
	CI_LAN_PT,    // Portuguese
}CI_LANGUAGE_TYPE;

#define CI_MAX_SUBTITLE_NUM 4
typedef struct _tagCI_SUBTITLE_INFO
{
	ci32u      uNumOfSubtitle;
	CI_LANGUAGE_TYPE   aSubtitleLanguage[CI_MAX_SUBTITLE_NUM];
}CI_SUBTITLE_INFO;

typedef enum _tagCI_CHANNEL_CONFIG
{
	CI_CHLCFG_UNCHANGE = 0,
	CI_CHLCFG_LEFT_ONLY,       // only leave left channel, mute right channel
	CI_CHLCFG_RIGHT_ONLY,      // only leave right channel, mute left channel
	CI_CHLCFG_DUAL_LEFT,       // copy left channel to right
	CI_CHLCFG_DUAL_RIGHT,      // copy right channel to left
	CI_CHLCFG_SWAP,            // swap left and right
}CI_CHANNEL_CONFIG;

#define CI_MAX_AUDIO_STREAM_NUM 4
typedef struct _tagCI_AUDIO_STREAM_INFO
{
	ci32u      uNumOfAudioStream;
	CI_LANGUAGE_TYPE   aAudioStreamLanguage[CI_MAX_AUDIO_STREAM_NUM];
}CI_AUDIO_STREAM_INFO;

typedef struct _tagRawESPacket
{
	unsigned long  uTimeStamp;
	unsigned long  uESPacketLength;
	unsigned char*  pESPacket;
}RawESPacket;

typedef struct _tag_yuv_data
{    
	unsigned char*	yuv[3];
	unsigned int	stride[3];
	unsigned int	height[3];
	unsigned int	width[3];
}CI_YUV_DATA , *LPCI_YUV_DATA;  

typedef struct _tagRGB_DATA 
{
	unsigned int    nWidth;
	unsigned int    nHeight;
	long            rgb_type;
	unsigned char  *pData;
	unsigned int    nDatalen;

}CI_RGB_DATA, *LPCI_RGB_DATA;

typedef struct _tagExternAudioDeviceStatus
{
	unsigned long  uAudioStreamID;
	unsigned long  uBufferedBytes;
	unsigned char  acSpecBar[32];
}ExternAudioDeviceStatus;

typedef struct _tagEqualizerConfig
{
	unsigned long  cbSize;      // sizeof(EqualizerConfig)
	bool           bEnable;     // If bEnable is set false, asGain[] will be ignored.
	short          asGain[10];  // From -12 to +12
}EqualizerConfig;

typedef struct _tagEpgChange
{
	unsigned long  uNewProgramCount;
	unsigned long  uNewEventCount[64];
}EpgChange;

typedef struct _tagBMLInitInfo
{
	unsigned int network_id;
	unsigned int ts_id;
	unsigned int service_id;

	unsigned int reserved[16];
}BMLInitInfo;

typedef enum _tagBMLCallbackType
{
	TS_PACKET,
	PMT_SECTION,
	BML_INIT_INFO
} BMLCallbackType;

typedef struct tagTIMESHIFT_PARAMETER
{
	void*			pPath;

	unsigned long	uTimeshiftLength; //in second
	unsigned long	uCurrentMFID;

	unsigned long	uTotalLen; //in second
	unsigned long	uGapToLiving; //in second

	int				nErrorCode;
	int				nReserved[10];
}TIMESHIFT_PARAMETER, *PTIMESHIFT_PARAMETER;

typedef enum tagPreviewFormat
{
	PRV_RGB565,
	PRV_RGB888,
	PRV_RGB32,
	PRV_YUV420,
	PRV_YUV422,
} PreviewFormat;

typedef struct tagPreviewOption
{
	ciVoid			*filename;
	unsigned long	ulPreviewWidth;
	unsigned long   ulPreviewHeight;
	PreviewFormat   format;
	unsigned long	ulPicMilliSec;
} PreviewOption, *LPPreviewOption;

struct SnapShotOpt
{
	unsigned long BitsPerPixel;
	unsigned long width;
	unsigned long height;
};

#endif  // ~_PLAY_CALLBACK_ARRAY_


/****************************************************************************************************
 ****************************************************************************************************
	Data structure
 ****************************************************************************************************/

typedef struct _tag_clip_info 
{
	ci32u dwDuration;
	ci32s dwHasVideo;
	ci32s dwHasAudio;
	ci32s dwHasMData;
	ci32s dwHasSubtitle;
	ci32u dwWidth;
	ci32u dwHeight;
	ci32u dwBitrate;
	ci32u dwIntervalTime;
	ci32u dwSamplesPerSec;
	ci32u dwChannels;
	ci32u dwBitsPerSample;
	ci32u dwPCMSizePerFrame;
	ci8s  sFileType[64];
	ci8s  sAudioType[64];
	ci8s  sVideoType[64];
} CI_CLIP_INFO, *LPCI_CLIP_INFO;

typedef struct _DeviceInfo
{
	ci32s screen_width;
	ci32s screen_height;
}CI_DEVICE_INFO, *LPCI_DEVICE_INFO;

typedef struct _tag_element_info
{
	ci16u wElementType;
	ci16u wElementStreamID;
}CI_ELEMENT_INFO, *LPCI_ELEMENT_INFO;

typedef struct _tag_program_info
{
	ci16u wProgramNumber;
	ci16u wElementCount;
	LPCI_ELEMENT_INFO pElements;
}CI_PROGRAM_INFO, *LPCI_PROGRAM_INFO;

typedef struct _tag_stream_info
{
	ci16u wStreamID;
	ci16u wProgramCount;
	LPCI_PROGRAM_INFO pPrograms;
}CI_STREAM_INFO, *LPCI_STREAM_INFO;

typedef struct _tag_audio_info 
{
	ci32u   dwAudioType;   
	ci32u   dwDuration;    
	ci16u   wChannel;    
	ci16u   wBitsPerSample;   
	ci16u   wBlockAlign;    
	ci32u   dwSamplingRate;   
	ci32u   dwBitrate;    
}CI_AUDIO_INFO ,  *LPCI_AUDIO_INFO; 

typedef struct _tag_video_info
{
	ci32u   dwVideoType;   
	ci32u   dwDuration;   
	ci32u   dwPicWidth;   
	ci32u   dwPicHeight;   
	ci32u   dwFPS;    
	ci32u   dwBitrate;   
}CI_VIDEO_INFO,*LPCI_VIDEO_INFO; 

typedef struct _tag_player_callback_data
{
	ci32u     dwStatus;     
	ciRESULT  resStatus;     
	ci32u     dwStatusData1;    
	ci32u     dwStatusData2;    
	ci8u*     pFrameBuf;     
	ci32u     dwFrameLen;      
	ci32u     dwCSType;     
	ci32u     dwWidth;     
	ci32u     dwHeight;     
	ci32s     bDraw;      
}CI_PLAYER_CALLBACK_DATA, *LPCI_PLAYER_CALLBACK_DATA; 

typedef struct _tag_player_mode
{
	ci32u  dwMode;     
	ci32s  lParam;     
}CI_PLAYER_MODE , *LPCI_PLAYER_MODE;  

typedef struct _tag_display_param 
{
	ciHandle     hDisplayDeviceHandle;   
	CI_RECT      rectScreen;     
	CI_RECT      rectClip;     
	CI_RECT      rectSrcClip;
	ciCOLORREF   colorBackground;    
	ci32s        dwDeviceType;	
	ci32s        dwResampleMode;
	ci32s        dwBPP;
	ci32u        dwFourCC;
	ci32s        dwCopyProtected;
}CI_DISPLAY_PARAM , *LPCI_DISPLAY_PARAM;   

enum CI_STATUS
{
	CI_INVALID = 0,
	CI_STOPPED,
	CI_PAUSED,
	CI_RUNNING,
	CI_SHIFTING,
	CI_FASTFORWARD,
};

typedef struct _tag_IDTag3_Data
{
	long version_v1; 
	long Track_v1; 
	char Title_v1[32];
	char Artist_v1[32];
	char Album_v1[32];
	char Year_v1[16];
	char Comment_v1[32];	
	char Genre_v1[32];

	long version_v2; 
	long Track_v2; 
	// The first element of the following array shows the encode type of the string.
	// If the first element is 0, the string is ASCII code.
	// If the first element is 1, the string is UTF-16 encoded.
	char Title_v2[64];
	char Artist_v2[64];
	char Album_v2[64];
	char Year_v2[16];
	char Comment_v2[1024];
	char Genre_v2[32];
	char * pPic;
	unsigned int Pic_Size;
	char pic_type[10];
}CI_IDTag3_Data , *LPCI_IDTag3_Data;

enum CI_ID3_STATUS
{
	CI_ID3_ERROR = 0,               //both v1 and  v2 data not exist
	CI_ID3V1_EXIST = 0x01,          //only v1 data exists
	CI_ID3V2_EXIST = 0x02,          //only v2 data exists
	CI_ID3V1V2_EXIST = 0x03,        //both v1 and v2 data exist
};


typedef struct  
{
	int iYear;
	int iMonth;
	int iDay;
}tOutdatedTime;

#endif  // ~_CI_SDK_TYPEDEF_H_

