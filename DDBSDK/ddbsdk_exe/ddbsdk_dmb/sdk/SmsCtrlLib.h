/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2005,2006 Siano Mobile Silicon Ltd. All rights reserved */
/*                                                                       */
/* PROPRIETARY RIGHTS of Siano Mobile Silicon are involved in the        */
/* subject matter of this material.  All manufacturing, reproduction,    */
/* use, and sales rights pertaining to this subject matter are governed  */
/* by the license agreement.  The recipient of this software implicitly  */
/* accepts the terms of the license.                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*                                                                       */
/*      SmsCtrlLib.h		                                             */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      Siano Host Control Library API									 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*************************************************************************/

#ifndef SMS_CTRL_LIB_H
#define SMS_CTRL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OS_WIN32) || defined(OS_WINCE)
	#ifdef SMSHOSTLIB_EXPORTS
		#define SMSHOSTLIB_API __declspec(dllexport)
	#else
		#define SMSHOSTLIB_API __declspec(dllimport)
	#endif
#else
	#define SMSHOSTLIB_API  
#endif

#include <windef.h>
#include <stdlib.h>
#include <winnt.h>
#include <basetsd.h>
#include <types.h>

/*************************************************************************
*			 Defines
*************************************************************************/

#define SMSHOSTLIB_MAX_LABEL_SIZE 							(  16 )
#define SMSHOSTLIB_MAX_SCAN_FREQ 							(  10 )
#define SMSHOSTLIB_MAX_FILTERS_LIST_LEN 					(  32 )
#define SMSHOSTLIB_DAB_MAX_SERVICES							(  20 )
#define SMSHOSTLIB_DAB_MAX_SERVICE_COMPONENTS_IN_SERVICE	(   4 )
#define SMSHOSTLIB_MAX_NUM_LOG_PARAMETERS                   (   8 )
#define SMSHOSTLIB_MAX_USER_APPLICATIONS					(  12 )	//12 in V1.3.3; 6 in V1.4.1
#define SMSHOSTLIB_MAX_LEN_OF_APPLICATION_DATA				(  24 )  //the spec defines it as 23, we round it to a byte resolution
#define SMSHOSTLIB_MAX_DATE_N_TIME_SIZE						(   6 )	//the date and time maximum length
#define SMSHOSTLIB_MAX_LOG_STRING_SIZE						( 300 )	//including ASCIZ
#define SMSHOSTLIB_MAX_MODEM_VERSION_STRING_SIZE            ( 250 )	//including ASCIZ
#define SMSHOSTLIB_IP_PLAT_NAME_MAX_LEN						(  64 )
#define SMSHOSTLIB_IP_PLAT_LANGUAGE_MAX_VARIANT				(  10 )
#define SMSHOSTLIB_CURRENT_TABLE_ID							( 0xFFFFFFFF )

#define SMSHOSTLIB_MIN_VHF_FREQ								( 174000000 )
#define SMSHOSTLIB_MAX_VHF_FREQ								( 240000000 )
#define SMSHOSTLIB_MIN_UHF_FREQ								( 474000000 )
#define SMSHOSTLIB_MAX_UHF_FREQ								( 858000000 )
#define SMSHOSTLIB_MIN_L1_FREQ								( 1452000000 )
#define SMSHOSTLIB_MAX_L1_FREQ								( 1492000000 )
#define SMSHOSTLIB_MIN_L2_FREQ								( 1672500000 )
#define SMSHOSTLIB_MAX_L2_FREQ								( 1672500000 )

#define SMS_HOST_MAX_DEVICE_NAME_STRING_LEN					(256)
#define SMS_HOST_MAX_DEVICE_NICK_NAME_LEN					(64)
#define SMS_HOST_DVBT_SERVICE_DATA_HANDLE					(1)
#define SMS_INVALID_DEVICE_INSTANCE_HANDLE					( (SmsHostApiDeviceHandle)0xFFFFFFFF )

#define SMSHOSTLIB_CMMB_MAX_NETWORK_NAME_LEN				(16)
/*************************************************************************
*			 Enums
*************************************************************************/

/// Return Codes
typedef enum
{
	SMSHOSTLIB_ERR_OK							= 0x00000000,
	SMSHOSTLIB_ERR_PENDING						= 0x00000001,
	SMSHOSTLIB_ERR_UNDEFINED_ERR				= 0x80000000,
	SMSHOSTLIB_ERR_NOT_IMPLEMENTED				= 0x80000002,
	SMSHOSTLIB_ERR_NOT_SUPPORTED				= 0x80000003,

	SMSHOSTLIB_ERR_LIB_NOT_INITIATED			= 0x80000004,
	SMSHOSTLIB_ERR_LIB_ALREADY_INITIATED		= 0x80000005,
	SMSHOSTLIB_ERR_DEVICE_NOT_INITIATED			= 0x80000006,
	SMSHOSTLIB_ERR_COMM_NOT_VALID				= 0x80000007,
	SMSHOSTLIB_ERR_TASK_CREATION_FAILED			= 0x80000008,

	SMSHOSTLIB_ERR_INVALID_ARG					= 0x80000009,
	SMSHOSTLIB_ERR_LIST_FULL					= 0x8000000A,
	SMSHOSTLIB_ERR_DAB_LIB_INIT_FAILED			= 0x8000000B,
	SMSHOSTLIB_ERR_MEM_ALLOC_FAILED				= 0x8000000C,
	SMSHOSTLIB_ERR_TIMEOUT						= 0x8000000D,
	SMSHOSTLIB_ERR_FS_ERROR						= 0x8000000E,
	SMSHOSTLIB_ERR_HANDINIT_ERROR				= 0x8000000F,
	SMSHOSTLIB_ERR_ALREADY_EXIST				= 0x80000010,
	SMSHOSTLIB_ERR_SRV_IS_PLAYING				= 0x80000011,
	SMSHOSTLIB_ERR_ONLY_ONE_SRV_ALLOWED			= 0x80000012,
	SMSHOSTLIB_ERR_ONLY_DMB_SRV_ALLOWED			= 0x80000013,	
	SMSHOSTLIB_ERR_RESPONSE_OUT_OF_SYNC			= 0x80000014,	

	SMSHOSTLIB_ERR_ENSEMBLE_NOT_EXIST			= 0x80000020,
	SMSHOSTLIB_ERR_COMPONENT_NOT_EXIST			= 0x80000021,
	SMSHOSTLIB_ERR_ALREADY_PLAYED				= 0x80000022,
	SMSHOSTLIB_ERR_PLAYING_ANOTHER_ENSEMBLE		= 0x80000023,
	SMSHOSTLIB_ERR_EXCEED_MAX_SERVICES			= 0x80000024,
	SMSHOSTLIB_ERR_NOT_SUPPORTED_TRANSMIT_MODE	= 0x80000025,
	SMSHOSTLIB_ERR_HANDLE_NOT_AVAIL				= 0x80000026,
	SMSHOSTLIB_ERR_MODEM_START_FAILED			= 0x80000027,
	
	SMSHOSTLIB_ERR_EXCEED_MAX_HANDLES			= 0x80000030,
	SMSHOSTLIB_ERR_HANDLE_INVALID				= 0x80000031,
	SMSHOSTLIB_ERR_MODEM_STOP_FAILED			= 0x80000032,
	SMSHOSTLIB_ERR_CA_STOP_SERVICE_FAILED		= 0x80000033,
	SMSHOSTLIB_ERR_CS_STOP_SERVICE_FAILED		= 0x80000034,
	
	SMSHOSTLIB_ERR_ALREADY_ACTIVE				= 0x80000040,
	SMSHOSTLIB_ERR_NOT_ACTIVE					= 0x80000041,
	SMSHOSTLIB_ERR_SRVM_RESPONSE_TIMEOUT		= 0x80000042,
	SMSHOSTLIB_TUNE_RESPONSE_TIMEOUT			= 0x80000043,
	SMSHOSTLIB_ERR_MODEM_NOT_LOCKED				= 0x80000044,
	SMSHOSTLIB_ERR_NO_SI_ACQ					= 0x80000045,
	SMSHOSTLIB_ERR_SRVM_UNDEFINED_ERR			= 0x80000046,
	SMSHOSTLIB_ERR_ENSEMBLE_IDX_EXCEED			= 0x80000047,
	SMSHOSTLIB_ERR_SERVICE_ACTIVE				= 0x80000048,
	SMSHOSTLIB_ERR_COMM_ERR						= 0x80000049,
	SMSHOSTLIB_ERR_SCAN_ACTIVE					= 0x8000004A,
	SMSHOSTLIB_ERR_SCAN_NOT_ACTIVE				= 0x8000004B,
	SMSHOSTLIB_ERR_SERVICE_MAPPING_ACTIVE		= 0x8000004C,
	SMSHOSTLIB_ERR_SERVICE_MAPPING_ERROR		= 0x8000004D,
	SMSHOSTLIB_ERR_IP_SERVICE_IS_NOT_MAPPED		= 0x8000004E,
	SMSHOSTLIB_ERR_IP_SERVICE_DOES_NOT_EXIST	= 0x8000004F,
	SMSHOSTLIB_ERR_PID_FILTER_DOES_NOT_EXIST	= 0x80000050,
	SMSHOSTLIB_ERR_MODEM_NO_TIMESLICING_IN_TPS	= 0x80000051,
	SMSHOSTLIB_ERR_SCAN_CELL_ID_FILTER_ERROR	= 0x80000052,
	SMSHOSTLIB_ERR_SCAN_NETWORK_ID_FILTER_ERROR	= 0x80000053,
	SMSHOSTLIB_ERR_SCAN_STOPPED_BY_HOST			= 0x80000054,

	SMSHOSTLIB_ERR_DEVICE_DISCONNECTED			= 0x80000060,
	SMSHOSTLIB_ERR_DEVICE_DOES_NOT_EXIST		= 0x80000061,

	SMSHOSTLIB_ERR_CA_FAILURE					= 0x80000062,
    SMSHOSTLIB_ERR_DEVICE_NEED_REINIT   		= 0x80000063,

	SMSHOSTLIB_ERR_NETWORK_DOES_NOT_EXIST		= 0x80000070,
	SMSHOSTLIB_ERR_NETWORK_ALREADY_EXISTS		= 0x80000071,
	SMSHOSTLIB_ERR_EXCEEDS_MAX_NETWORKS_NUM		= 0x80000072,
	SMSHOSTLIB_ERR_TIMESLOT_NOT_ACTIVE			= 0x80000073,
	SMSHOSTLIB_ERR_CHANNEL_DOES_NOT_EXIST		= 0x80000074,
	SMSHOSTLIB_ERR_EXCEED_MAX_CHANNELS			= 0x80000075,
	SMSHOSTLIB_ERR_SERVICE_DOES_NOT_EXIST		= 0x80000076,
	SMSHOSTLIB_ERR_FREQ_POINT_DOES_NOT_EXIST	= 0x80000077,
	SMSHOSTLIB_ERR_PLAYING_ANOTHER_NETWORK		= 0x80000078,
} SMSHOSTLIB_ERR_CODES_E;

/// Communication interface
typedef enum
{
	SMSHOSTLIB_COMM_UART = 0,
	SMSHOSTLIB_COMM_USB,
	SMSHOSTLIB_COMM_SDIO,
	SMSHOSTLIB_COMM_SPI,
	SMSHOSTLIB_COMM_HIF,
	SMSHOSTLIB_COMM_UNDEFINED = 0xFFFFFFFF

} SMSHOSTLIB_COMM_TYPES_E;

/// Communication state
typedef enum SMSHOSTLIB_COMM_STATE_E
{
	SMSHOSTLIB_COMM_DISCONNECTED = 0,
	SMSHOSTLIB_COMM_CONNECTED,
	SMSHOSTLIB_COMM_STATE_UNDEFINED = 0xFFFFFFFF

} SMSHOSTLIB_COMM_STATE_ET;

/// SMS1000 working mode
typedef enum
{
	SMSHOSTLIB_DEVMD_DVBT,
	SMSHOSTLIB_DEVMD_DVBH,
	SMSHOSTLIB_DEVMD_DAB_TDMB,
	SMSHOSTLIB_DEVMD_DAB_TDMB_DABIP,
	SMSHOSTLIB_DEVMD_DVBT_BDA,
	SMSHOSTLIB_DEVMD_ISDBT,
	SMSHOSTLIB_DEVMD_ISDBT_BDA,
	SMSHOSTLIB_DEVMD_CMMB,
	SMSHOSTLIB_DEVMD_RAW_TUNER,
	SMSHOSTLIB_DEVMD_NONE = 0xFFFFFFFF

} SMSHOSTLIB_DEVICE_MODES_E;

/// Message type responses to APIs requests. The enum is analog to the APIs names
typedef enum
{
// Initialization and Termination
	SMSHOSTLIB_MSG_INIT_DEVICE_RES = 0,			//!< Response to #SmsHostApiDeviceInit_Req
	SMSHOSTLIB_MSG_DEVICE_TERMINATE_RES,		//!< Response to #SmsHostApiDeviceTerminate_Req

// Information and Statistics		
	SMSHOSTLIB_MSG_GET_VERSION_RES = 1000,		//!< Response to #SmsHostApiGetVersion_Req
	SMSHOSTLIB_MSG_GET_STATISTICS_RES,			//!< Response to #SmsHostApiGetStatistics_Req
	SMSHOSTLIB_MSG_GET_RECEPTION_RES,			//!< Response to #SmsHostApiGetReceptionQuality_Req
	SMSHOSTLIB_MSG_GET_STATISTICS_EX_RES,		//!< Response to #SmsHostApiGetStatisticsEx_Req

// Tune and Scan operations	
	SMSHOSTLIB_MSG_TUNE_RES        = 2000,		//!< Response to #SmsHostApiTune_Req
	SMSHOSTLIB_MSG_SCAN_START_RES,				//!< Response to #SmsHostApiScanStart_Req
	SMSHOSTLIB_MSG_SCAN_STOP_RES,				//!< Response to #SmsHostApiScanStop_Req
	SMSHOSTLIB_MSG_ISDBT_TUNE_RES,				//!< Response to #SmsHostApiIsdbtTune_Req

// DVB-T
	SMSHOSTLIB_MSG_ADD_PID_FILTER_RES = 3000,	//!< Response to #SmsHostApiAddPidFilter_Req
	SMSHOSTLIB_MSG_REMOVE_PID_FILTER_RES,		//!< Response to #SmsHostApiRemovePidFilter_Req
	SMSHOSTLIB_MSG_RETRIEVE_PID_FILTER_LIST_RES,//!< Response to #SmsHostApiRetrievePidFilterList_Req
	SMSHOSTLIB_MSG_GET_PID_FILTER_LIST_RES,		//!< Obsolete Response to SmsHostGetPidFilterList_Req
	SMSHOSTLIN_MSG_GET_SI_TBL_RES,				//!< Response to #SmsHostApiDvbGetSITable_Req

// DVB-H
	SMSHOSTLIB_MSG_ADD_IP_FILTER_RES = 4000,	//!< Response to #SmsHostApiAddIpFilter_Req
	SMSHOSTLIB_MSG_REMOVE_IP_FILTER_RES,		//!< Response to #SmsHostApiRemoveIpFilter_Req
	SMSHOSTLIB_MSG_GET_IP_FILTER_LIST_RES,		//!< Response to #SmsHostApiGetIpFilterList_Req
	SMSHOSTLIB_MSG_SET_MPE_TBL_PARAMS_RES,		//!< Internal for debugging purposes
	SMSHOSTLIB_MSG_GET_AVAIL_IP_PLATFORMS_RES,	//!< Response to #SmsHostApiGetAvailableIpPlatforms_Req
	SMSHOSTLIB_MSG_UTC_TIME_RES,				//!< Response to #SmsHostApiDvbTDT_Req
	SMSHOSTLIB_MSG_TIME_OFFSET_RES,				//!< Response to #SmsHostApiDvbTOT_Req
	SMSHOSTLIB_MSG_SET_OOC_TIMER_RES,			//!< Response to #SmsHostApiSetOOCTimer_Req
	

// DAB, T-DMB and eDAB
	SMSHOSTLIB_MSG_START_SERVICE_RES = 5000,	//!< Response to #SmsHostApiDabStartService_Req
	SMSHOSTLIB_MSG_STOP_SERVICE_RES,			//!< Response to #SmsHostApiDabStopService_Req
	SMSHOSTLIB_MSG_GET_ENSEMBLE_INFO_RES,		//!< Response to #SmsHostApiDabGetEnsembleInfo_Req
	SMSHOSTLIB_MSG_GET_SERVICES_RES,			//!< Response to #SmsHostApiDabGetServices_Req
	SMSHOSTLIB_MSG_GET_COMPONENTS_INFO_RES,		//!< Response to #SmsHostApiDabGetComponentsInfo_Req
	SMSHOSTLIB_GET_CURR_ENSEMBLE_INDEX_RES,		//!< Response to #SmsHostApiDabGetCurrentEnsembleIndex_Req
	SMSHOSTLIB_MSG_POST_VITERBI_BER_STATE_SET_RES,//!< Obsolete 
	SMSHOSTLIB_MSG_GET_COMBINED_COMPONENTS_INFO_RES,//!< Response to #SmsHostApiDabGetCombinedComponentsInfo_Req
	SMSHOSTLIB_MSG_GET_SUBCHANNEL_INFO_RES,		//!< Placeholder for future API

// GPIO
	SMSHOSTLIB_MSG_GPIO_CONFIGURE_RES = 6000,	//!< Response to #SmsHostApiGPIOConfigure_Req
	SMSHOSTLIB_MSG_GPIO_SET_LEVEL_RES,			//!< Response to #SmsHostApiGPIOSetLevel_Req
	SMSHOSTLIB_MSG_GPIO_GET_LEVEL_RES,			//!< Response to #SmsHostApiGPIOGetLevel_Req
	SMSHOSTLIB_MSG_POWER_DOWN_RES,				//!< Response to #SmsHostApiPowerDown_Req

// Raw tuner mode
	SMSHOSTLIB_MSG_RAW_CAPTURE_RES = 7000,		//!< Response to #SmsHostApiRawCapture_Req 
	SMSHOSTLIB_MSG_RAW_CAPTURE_ABORT_RES,		//!< Response to #SmsHostApiRawCaptureAbort_Req 
	SMSHOSTLIB_MSG_RAW_GET_COUNTER_RES,			//!< Response to #SmsHostApiRawGetCounter_Req
	SMSHOSTLIB_MSG_RAW_GET_AGC_RES,				//!< Response to #SmsHostApiRawGetAGC_Req

// Debug and testing
	SMSHOSTLIB_MSG_SMS_LOOPBACK_RES	= 7100,		//!< Response for loopback test event
	SMSHOSTLIB_MSG_SMS_DATA_PUMP_RES,			//!< Response for data pump request
	SMSHOSTLIB_MSG_SMS_EXEC_TEST_1_RES,		//!< Response for USB IN EP set request

// ISDB-T
	SMSHOSTLIB_MSG_ISDBT_SET_LAYER_RES = 8000,	//!< Response to #SmsHostApiIsdbtSelectLayer_Req

// Indications (Unsolicited events)

// Scan
	SMSHOSTLIB_SCAN_PROGRESS_IND	= 10000,	//!< Indicates that a frequency had been scanned	
	SMSHOSTLIB_SCAN_COMPLETE_IND,				//!< Indicates that the scanned has been completed

// Service Information
	SMSHOSTLIB_DVB_SI_CHANGE_IND	= 11000,	//!< Indicates that current SI has been changed
	SMSHOSTLIB_DAB_RECONFIG_IN_PROG_IND = 11100,//!< Indicates that DAB re-configuration is taking place	
	SMSHOSTLIB_DAB_ENSEMBLE_CHANGED_IND,		//!< Indicates that DAB current ensemble has been changed	
	SMSHOSTLIB_MSG_FAST_INFORMATION_BLOCK_IND,	//!< Indication contains raw FIC data

// Handover
	SMSHOSTLIB_HANDOVER_IND	= 12000,			//!< Indicates that DVB-H Handover has been performed
	SMSHOSTLIB_RECEPTION_STATUS_IND,			//!< Indicates the current reception status
	SMSHOSTLIB_INCONSISTENT_DATABASE_IND,		//!< Indicates inconsistency between the internal database and the received psi/si

// Log	
    SMSHOSTLIB_MSG_LOG_ITEM_IND = 13000,		//!< Forwarding of an asynchronous Log message from SMS1000 
	SMSHOSTLIB_MSG_LOG_STRING_IND,				//!< Forwarding of a log string from Siano Host Control Library. SMSHOSTLIB_MSG_LOG_STRING_IND data is a null terminated string. The PayloadLen value is up to SMSHOSTLIB_MAX_LOG_STRING_SIZE 
// Other
	SMSHOSTLIB_MSG_UART_FOR_INFRARED_IND	= 14000,	//!< Notification to host with data received from UART
	SMSHOSTLIB_MSG_COMM_STATE_CHANGED_IND,		//!< Notification to host that the current communication state has changed
	SMSHOSTLIB_MSG_DEVICE_UNPLUGGED_IND,		//!< Notification that a device has been unplugged (PNP)
	SMSHOSTLIB_MSG_DEVICE_NEW_DEVICE_DETECTED_IND,	//!< Notification that a new device was detected (PNP)
	SMSHOSTLIB_MSG_RAW_CAPTURE_COMPLETE_IND,	//!< Notification that a raw capture operation has been completed
// CAS
	SMSHOSTLIB_DAB_CAS_CHANNEL_EVENT_IND,		//!< Notification of CAS channel event
	SMSHOSTLIB_DAB_CAS_APPLICATION_EVENT_IND,	//!< Notification of CAS application event
	SMSHOSTLIB_DAB_CAS_DESCRAMBLER_ERROR_EVENT_IND,	//!< Notification of CAS Descrambler error

// Debug and testing
	SMSHOSTLIB_MSG_SMS_DATA_PUMP_IND			//!< Notification of data pump event
} SMSHOSTLIB_MSG_TYPE_RES_E;

typedef enum SMSHOSTLIB_FREQ_BANDWIDTH_E
{ 
	BW_8_MHZ		= 0,
	BW_7_MHZ		= 1,	
	BW_6_MHZ		= 2,
	BW_5_MHZ		= 3,
	BW_ISDBT_1SEG	= 4,
	BW_ISDBT_3SEG	= 5,
	BW_2_MHZ		= 6,
	BW_1_5_MHZ		= 15,
	BW_UNKNOWN		= 0xFFFF

} SMSHOSTLIB_FREQ_BANDWIDTH_ET;

typedef enum SMSHOSTLIB_DVB_MODEM_STATE_E
{ 
	MS_WAKE_UP				= 1,
	MS_SET_INITIAL_LNA		= 2,
	MS_ACQUIRE_LOCK			= 3,
	MS_SET_LNA				= 4,
	MS_PRE_SLICE			= 5,
	MS_IN_SLICE				= 6,
	MS_POST_SLICE			= 7,
	MS_IQ_AMBIG				= 8,
	MS_SYR_FIRST_SYNC		= 9,
	MS_CHECK_COARSE_FREQ	= 10,
	MS_HANDLE_IFFT			= 11,
	MS_PREPARE_TO_HANDLE_IFFT=12,
	MS_HANDLE_IFFT_IN_SLICE	= 13,
	MS_HO_SCAN_PHASE		= 14,
	MS_DEBUG_STATE			= 15,
	MS_ALLOW_AGC_SETTLE		= 16
	
} SMSHOSTLIB_DVB_MODEM_STATE_ET;

typedef enum SMSHOSTLIB_CODE_RATE_E
{ 
	CR_1_2	= 0,
	CR_2_3	= 1,
	CR_3_4	= 2,
	CR_5_6	= 3,
	CR_7_8	= 4

} SMSHOSTLIB_CODE_RATE_ET;

typedef enum SMSHOSTLIB_HIERARCHY_E
{ 
	HR_NONE		= 0,
	HR_ALPHA_1	= 1,
	HR_ALPHA_2	= 2,
	HR_ALPHA_4	= 3

} SMSHOSTLIB_HIERARCHY_ET;

typedef enum SMSHOSTLIB_CONSTELLATION_E
{ 
	CN_QPSK		= 0,
	CN_16_QAM	= 1,
	CN_64_QAM	= 2,
	CN_DQPSK	= 3

} SMSHOSTLIB_CONSTELLATION_ET;

//! IP classification
typedef enum
{
	IP_V_4 = 0,
	IP_V_6,
	IP_V_UNKNOWN = 0xFFFFFFFF

} SMSHOSTLIB_IP_TYPE_ET;

//! Scan filter types
typedef enum 
{
	SCAN_FILTER_CELL_ID_TYPE,						//!< Filter the scanned channels using cell id filters
	SCAN_FILTER_NETWORK_ID_TYPE,					//!< Filter the scanned channels using network id filters
	SCAN_FILTER_PLATFORM_ID_TYPE					//!< Filter the scanned channels using network id filters
} SMSHOSTLIB_SCAN_FILTER_TYPE_ET;

//! Scan analyze types
typedef enum 
{
	ANALYSE_SRV_INFO,								//!< Service information processing is performed on each frequency found
	CLEAN_PREV_DB_AT_SCAN_START,					//!< Clean the previous Scan DB at the current scan start command
	DO_NOT_CLEAN_PREV_DB_AT_SCAN_START,				//!< Clean the previous Scan DB only if the current scan succeeds
	ENERGY_SCAN_ONLY								//!< Energy level scan; prints to debug each Freq. and its In-Band-Power
} SMSHOSTLIB_SCAN_ANALYSE_TYPE_ET;

//! Indicator for Scan operation
typedef enum 
{
	SCAN_PARAM_TABLE,								//!< The scan range is given by the structure #SMSHOSTLIB_SCAN_TABLE_ST
	SCAN_PARAM_RANGE								//!< The scan range is given by the structure #SMSHOSTLIB_SCAN_RANGE_ST							 	
} SMSHOSTLIB_SCAN_PARAM_STD_ET;

//! Reception Status Indication
typedef enum 
{
	RECEPTION_STATUS_OK,							//!< Good system reception indication
	RECEPTION_STATUS_FREQUENCY_FAIL,				//!< System reception failure on the current frequency
	RECEPTION_STATUS_ALL_BAND_FAIL_VALID_CHANNELS,	//!< System reception failure on all frequencies in the current band - valid channels exist
	RECEPTION_STATUS_ALL_BAND_FAIL_OOC				//!< System reception failure on all frequencies in the current band - no additional valid channels
} SMSHOSTLIB_RECEPTION_STATUS_ET;

typedef enum
/*! Reception Quality result ( Scale range: 5 (best) to 0 (no reception at all) ) */
{
	RECEPTION_QUALITY_UNKNOWN,
	RECEPTION_QUALITY_FAIL,	
	RECEPTION_QUALITY_POOR,	
	RECEPTION_QUALITY_AVERAGE,
	RECEPTION_QUALITY_GOOD,	
	RECEPTION_QUALITY_EXCELLENT

} SMSHOSTLIB_RECEPTION_QUALITY_ET;

//! TMID definition
typedef enum
{
	DAB_TMID_MSC_STREAM_AUDIO = 0,
	DAB_TMID_MSC_STREAM_DATA  = 1,
	DAB_TMID_FIDC			  = 2,
	DAB_TMID_MSC_PACKET_DATA  = 3
} SMSHOSTLIB_TMID_TYPES_ET;

//! ISDB-T hierarchy selection for the @SmsHostApiIsdbtSelectLayer_Req API
typedef enum
{
	ISDBT_LAYER_A	 = 0,
	ISDBT_LAYER_B	 = 1,
	ISDBT_LAYER_A_B  = 2,
	ISDBT_LAYER_C    = 3
} SMSHOSTLIB_ISDBT_LAYER_ET;

/*************************************************************************
*			 Typedefs
*************************************************************************/

/// Single instance control lib client callback function prototype 
//  Kept for backward compatibility
typedef void (*SmsCtrlCbFunc)( SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		//!< Response type	
							  SMSHOSTLIB_ERR_CODES_E		ErrCode,		//!< Response success code
							  void* 						pPayload,		//!< Response payload
							  UINT32						PayloadLen);	//!< Response payload length


/// HostApi control lib client callback function prototype
typedef void (*SmsHostApiCtrlCbFunc)( 
									 void*						ClientContext,	//!< Context of client
									 SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		//!< Response type	
									 SMSHOSTLIB_ERR_CODES_E		ErrCode,		//!< Response success code
									 void* 						pPayload,		//!< Response payload
									 UINT32						PayloadLen);	//!< Response payload length

// Callback for service data read from an active service, passed to OpenService_req
typedef void ( *SmsHostApiDataCbFunc)(
									  void*	ClientPtr, 
									  UINT32	ServiceHandle, 
									  UINT8*	pBuffer, 
									  UINT32	BufferSize );

/// A device instance handle
typedef void* SmsHostApiDeviceHandle ;

typedef void* HANDLE;

/*************************************************************************
*			 Structures
*************************************************************************/

/// Initialization - init lib params for single instance compatibility
typedef struct SMSHOSTLIB_INITLIB_PARAMS_S
{
	UINT32						Size;				//!< Put sizeof(#SMSHOSTLIB_INITLIB_PARAMS_ST)into this field
	SMSHOSTLIB_COMM_TYPES_E		CommType;			//!< Communication type (according to 
													//!<		#SMSHOSTLIB_COMM_TYPES_E enumerator)
	void*						pCommParam;			//!< Additional Communication Params. This is platform 
													//!<		dependent and usually should be set to NULL.
	UINT32						padding;
	const char*					WorkingDirectory;	//!< File system directory for saving service information
} SMSHOSTLIB_INITLIB_PARAMS_ST;

/// Initialization - multiple instances init lib params
typedef struct SMSHOSTLIB_API_INITLIB_PARAMS_S
{
	UINT32						Size;				//!< Put sizeof(#SMSHOSTLIB_API_INITLIB_PARAMS_ST)into this field
	SmsHostApiCtrlCbFunc		pCtrlCallback ;		//!< Callback for control responses
	SmsHostApiDataCbFunc		pDataCallback ;		//!< Callback for asynchronous data reading
													//!<  Leave NULL for synchronous read
	SMSHOSTLIB_COMM_TYPES_E		CommType;			//!< Communication type (according to 
													//!<		#SMSHOSTLIB_COMM_TYPES_E enumerator)
	void*						pCommParam;			//!< Additional Communication Params. This is platform 
													//!<		dependent and usually should be set to NULL.
	const char*					WorkingDirectory;	//!< File system directory for saving service information
	BOOL						IsSupportMultInstances ; //!< Is multiple instances support needed
} SMSHOSTLIB_API_INITLIB_PARAMS_ST ;


/// Logging
//format of SMSHOSTLIB_MSG_LOG_ITEM. 
//Actual size is sizeof(SMSHOSTLIB_LOG_HEADER_ST) + ( number_of_parameters * sizeof(UINT32) )

typedef struct SMSHOSTLIB_LOG_HEADER_S
{
    UINT32 n_filtering_classification;
    UINT32 e_format_string_tag;
    UINT32 n_timestamp_microseconds;
} SMSHOSTLIB_LOG_HEADER_ST;

typedef struct SMSHOSTLIB_LOG_ITEM_S
{
    SMSHOSTLIB_LOG_HEADER_ST x_header;
    UINT32                   a_n_parameters[SMSHOSTLIB_MAX_NUM_LOG_PARAMETERS];
} SMSHOSTLIB_LOG_ITEM_ST;

/// Versioning
typedef struct SMSHOSTLIB_VERSIONING_S
{
	UINT8			Major;
	UINT8			Minor;
	UINT8			Patch;
	UINT8			FieldPatch;
} SMSHOSTLIB_VERSIONING_ST;

/// Version
typedef struct SMSHOSTLIB_VERSION_S
{
	UINT16						ChipModel;				//!< e.g. 0x1102 for SMS-1102 "Nova"
	UINT8						Step;					//!< 0 - Step A
	UINT8						MetalFix;				//!< 0 - Metal 0
	UINT8						FirmwareId;				//!< 0xFF - ROM or see @SMSHOSTLIB_DEVICE_MODES_E
	UINT8						SupportedProtocols;		/*!< Bitwise OR combination of supported 
																protocols, see @SMSHOSTLIB_DEVICE_MODES_E */
	SMSHOSTLIB_VERSIONING_ST	FwVer;					//!< Firmware version
	SMSHOSTLIB_VERSIONING_ST	RomVer;					//!< ROM version
	UINT8						TextLabel[34];			//!< Text label
	SMSHOSTLIB_VERSIONING_ST	RFVer;					//!< RF tuner version
}SMSHOSTLIB_VERSION_ST;

typedef struct SMSHOSTLIB_COMM_STATE_CHG_IND_S
{
	SMSHOSTLIB_COMM_STATE_ET NewState;	

} SMSHOSTLIB_COMM_STATE_CHG_IND_ST;

/// Statistics information returned as response for SmsHostApiGetStatistics_Req
typedef struct SMSHOSTLIB_STATISTICS_S
{
	UINT32 Reserved;				//!< Reserved

	// Common parameters
	UINT32 IsRfLocked;				//!< 0 - not locked, 1 - locked
	UINT32 IsDemodLocked;			//!< 0 - not locked, 1 - locked
	UINT32 IsExternalLNAOn;			//!< 0 - external LNA off, 1 - external LNA on

	// Reception quality
	INT32  SNR;						//!< dB
	UINT32 BER;						//!< Post Viterbi BER [1E-5]
	UINT32 FIB_CRC;					//!< CRC errors percentage, valid only for DAB
	UINT32 TS_PER;					//!< Transport stream PER, 0xFFFFFFFF indicate N/A, valid only for DVB-T/H
	UINT32 MFER;					//!< DVB-H frame error rate in percentage, 0xFFFFFFFF indicate N/A, valid only for DVB-H
	INT32  RSSI;					//!< dBm
	INT32  InBandPwr;				//!< In band power in dBM
	INT32  CarrierOffset;			//!< Carrier Offset in bin/1024

	// Transmission parameters
	UINT32 Frequency;				//!< Frequency in Hz
	UINT32 Bandwidth;				//!< Bandwidth in MHz, valid only for DVB-T/H
	UINT32 TransmissionMode;		//!< Transmission Mode, for DAB modes 1-4, for DVB-T/H FFT mode carriers in Kilos
	UINT32 ModemState;				//!< from SMSHOSTLIB_DVB_MODEM_STATE_ET , valid only for DVB-T/H
	UINT32 GuardInterval;			//!< Guard Interval, 1 divided by value , valid only for DVB-T/H
	UINT32 CodeRate;				//!< Code Rate from SMSHOSTLIB_CODE_RATE_ET, valid only for DVB-T/H
	UINT32 LPCodeRate;				//!< Low Priority Code Rate from SMSHOSTLIB_CODE_RATE_ET, valid only for DVB-T/H
	UINT32 Hierarchy;				//!< Hierarchy from SMSHOSTLIB_HIERARCHY_ET, valid only for DVB-T/H
	UINT32 Constellation;			//!< Constellation from SMSHOSTLIB_CONSTELLATION_ET, valid only for DVB-T/H

	// Burst parameters, valid only for DVB-H
	UINT32 BurstSize;				//!< Current burst size in bytes, valid only for DVB-H
	UINT32 BurstDuration;			//!< Current burst duration in mSec, valid only for DVB-H
	UINT32 BurstCycleTime;			//!< Current burst cycle time in mSec, valid only for DVB-H
	UINT32 CalculatedBurstCycleTime;//!< Current burst cycle time in mSec, as calculated by demodulator, valid only for DVB-H
	UINT32 NumOfRows;				//!< Number of rows in MPE table, valid only for DVB-H
	UINT32 NumOfPaddCols;			//!< Number of padding columns in MPE table, valid only for DVB-H
	UINT32 NumOfPunctCols;			//!< Number of puncturing columns in MPE table, valid only for DVB-H
	UINT32 ErrorTSPackets;			//!< Number of erroneous transport-stream packets
	UINT32 TotalTSPackets;			//!< Total number of transport-stream packets
	UINT32 NumOfValidMpeTlbs;		//!< Number of MPE tables which do not include errors after MPE RS decoding
	UINT32 NumOfInvalidMpeTlbs;		//!< Number of MPE tables which include errors after MPE RS decoding
	UINT32 NumOfCorrectedMpeTlbs;	//!< Number of MPE tables which were corrected by MPE RS decoding
 	// Common params
	UINT32 BERErrorCount;			//!< Number of errornous SYNC bits.
	UINT32 BERBitCount;				//!< Total number of SYNC bits.

	// Interface information
	UINT32 SmsToHostTxErrors;		//!< Total number of transmission errors.

	// DAB/T-DMB  
	UINT32 PreBER; 					//!< DAB/T-DMB only: Pre Viterbi BER [1E-5]

	// DVB-H TPS parameters
	UINT32 CellId;					//!< TPS Cell ID in bits 15..0, bits 31..16 zero; if set to 0xFFFFFFFF cell_id not yet recovered
	UINT32 DvbhSrvIndHP;			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	UINT32 DvbhSrvIndLP;			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator

	UINT32 NumMPEReceived;			//!< DVB-H, Num MPE section received

	UINT32 ReservedFields[10];		//!< Reserved 

} SMSHOSTLIB_STATISTICS_ST;


// Statistics information returned as response for SmsHostApiGetStatisticsEx_Req for DVB applications, SMS1100 and up
typedef struct SMSHOSTLIB_STATISTICS_DVB_S
{
	UINT32 StatisticsType;			//!< Enumerator identifying the type of the structure.  Values are the same as SMSHOSTLIB_DEVICE_MODES_E
      								//!< This fiels MUST always first in any statistics structure

	UINT32 FullSize;				//!< Total size of the structure returned by the modem.  If the size requested by 
									//!< the host is smaller than FullSize, the struct will be truncated
	// Common parameters
	UINT32 IsRfLocked;				//!< 0 - not locked, 1 - locked
	UINT32 IsDemodLocked;			//!< 0 - not locked, 1 - locked
	UINT32 IsExternalLNAOn;			//!< 0 - external LNA off, 1 - external LNA on

	// Reception quality
	INT32  SNR;						//!< dB
	UINT32 BER;						//!< Post Viterbi BER [1E-5]
	UINT32 BERErrorCount;			//!< Number of errornous SYNC bits.
	UINT32 BERBitCount;				//!< Total number of SYNC bits.
	UINT32 TS_PER;					//!< Transport stream PER, 0xFFFFFFFF indicate N/A
	UINT32 MFER;					//!< DVB-H frame error rate in percentage, 0xFFFFFFFF indicate N/A, valid only for DVB-H
	INT32  RSSI;					//!< dBm
	INT32  InBandPwr;				//!< In band power in dBM
	INT32  CarrierOffset;			//!< Carrier Offset in bin/1024

	// Transmission parameters
	UINT32 Frequency;				//!< Frequency in Hz
	UINT32 Bandwidth;				//!< Bandwidth in MHz
	UINT32 ModemState;				//!< from SMSHOSTLIB_DVB_MODEM_STATE_ET
	UINT32 TransmissionMode;		//!< FFT mode carriers in Kilos
	UINT32 GuardInterval;			//!< Guard Interval, 1 divided by value
	UINT32 CodeRate;				//!< Code Rate from SMSHOSTLIB_CODE_RATE_ET
	UINT32 LPCodeRate;				//!< Low Priority Code Rate from SMSHOSTLIB_CODE_RATE_ET
	UINT32 Hierarchy;				//!< Hierarchy from SMSHOSTLIB_HIERARCHY_ET
	UINT32 Constellation;			//!< Constellation from SMSHOSTLIB_CONSTELLATION_ET

	// Burst parameters, valid only for DVB-H
	UINT32 BurstSize;				//!< Current burst size in bytes, valid only for DVB-H
	UINT32 BurstDuration;			//!< Current burst duration in mSec, valid only for DVB-H
	UINT32 BurstCycleTime;			//!< Current burst cycle time in mSec, valid only for DVB-H
	UINT32 CalculatedBurstCycleTime;//!< Current burst cycle time in mSec, as calculated by demodulator, valid only for DVB-H
	UINT32 NumOfRows;				//!< Number of rows in MPE table, valid only for DVB-H
	UINT32 NumOfPaddCols;			//!< Number of padding columns in MPE table, valid only for DVB-H
	UINT32 NumOfPunctCols;			//!< Number of puncturing columns in MPE table, valid only for DVB-H
	UINT32 ErrorTSPackets;			//!< Number of erroneous transport-stream packets
	UINT32 TotalTSPackets;			//!< Total number of transport-stream packets
	UINT32 NumOfValidMpeTlbs;		//!< Number of MPE tables which do not include errors after MPE RS decoding, valid only for DVB-H
	UINT32 NumOfInvalidMpeTlbs;		//!< Number of MPE tables which include errors after MPE RS decoding, valid only for DVB-H
	UINT32 NumOfCorrectedMpeTlbs;	//!< Number of MPE tables which were corrected by MPE RS decoding, valid only for DVB-H
	UINT32 NumMPEReceived;			//!< DVB-H, Num MPE section received

	// DVB-H TPS parameters
	UINT32 CellId;					//!< TPS Cell ID in bits 15..0, bits 31..16 zero; if set to 0xFFFFFFFF cell_id not yet recovered
	UINT32 DvbhSrvIndHP;			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	UINT32 DvbhSrvIndLP;			//!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator

	// Interface information
	UINT32 SmsToHostTxErrors;		//!< Total number of transmission errors.

} SMSHOSTLIB_STATISTICS_DVB_ST;

// Statistics information returned as response for SmsHostApiGetStatisticsEx_Req for DAB/DMB applications, SMS1100 and up
typedef struct SMSHOSTLIB_STATISTICS_DAB_S
{
	UINT32 StatisticsType;			//!< Enumerator identifying the type of the structure.  Values are the same as SMSHOSTLIB_DEVICE_MODES_E
									//!< This fiels MUST always first in any statistics structure

	UINT32 FullSize;				//!< Total size of the structure returned by the modem.  If the size requested by 
									//!< the host is smaller than FullSize, the struct will be truncated

	// Common parameters
	UINT32 IsRfLocked;				//!< 0 - not locked, 1 - locked
	UINT32 IsDemodLocked;			//!< 0 - not locked, 1 - locked
	UINT32 IsExternalLNAOn;			//!< 0 - external LNA off, 1 - external LNA on

	// Reception quality
	INT32  SNR;						//!< dB
	UINT32 Signal;					//!< dB
	UINT32 Noise;					//!< dB
	UINT32 BER;						//!< Post Viterbi BER [1E-5], calculated in T-DMB only
	UINT32 BERErrorCount;			//!< Number of errornous SYNC bits.
	UINT32 BERBitCount;				//!< Total number of SYNC bits.
	UINT32 PreBER; 					//!< Pre Viterbi BER [1E-5]
	UINT32 FIB_CRC;					//!< FIB CRC errors percentage
	UINT32 TS_PER;					//!< Transport stream PER, 0xFFFFFFFF indicate N/A, valid only for T-DMB
	UINT32 ErrorTSPackets;			//!< Number of erroneous transport-stream packets
	UINT32 TotalTSPackets;			//!< Total number of transport-stream packets
	INT32  RSSI;					//!< dBm
	INT32  InBandPwr;				//!< In band power in dBM

	// Transmission parameters
	UINT32 Frequency;				//!< Frequency in Hz
	UINT32 TransmissionMode;		//!< DAB Transmission Mode 1-4

	// Interface information
	UINT32 SmsToHostTxErrors;		//!< Total number of transmission errors.

} SMSHOSTLIB_STATISTICS_DAB_ST;

// Helper struct for ISDB-T statistics
typedef struct SMSHOSTLIB_ISDBT_LAYER_STAT_S
{
	// Per-layer information
	UINT32 CodeRate;			//!< Code Rate from SMSHOSTLIB_CODE_RATE_ET, 255 means layer does not exist
	UINT32 Constellation;		//!< Constellation from SMSHOSTLIB_CONSTELLATION_ET, 255 means layer does not exist
	UINT32 BER;					//!< Post Viterbi BER [1E-5], 0xFFFFFFFF indicate N/A
	UINT32 BERErrorCount;		//!< Post Viterbi Error Bits Count
	UINT32 BERBitCount;			//!< Post Viterbi Total Bits Count
	UINT32 PreBER; 				//!< Pre Viterbi BER [1E-5], 0xFFFFFFFF indicate N/A
	UINT32 TS_PER;				//!< Transport stream PER [%], 0xFFFFFFFF indicate N/A
	UINT32 ErrorTSPackets;		//!< Number of erroneous transport-stream packets
	UINT32 TotalTSPackets;		//!< Total number of transport-stream packets
	UINT32 TILdepthI;			//!< Time interleaver depth I parameter, 255 means layer does not exist
	UINT32 NumberOfSegments;	//!< Number of segments in layer A, 255 means layer does not exist
	UINT32 TMCCErrors;			//!< TMCC errors
} SMSHOSTLIB_ISDBT_LAYER_STAT_ST;

// Statistics information returned as response for SmsHostApiGetStatisticsEx_Req for ISDB-T applications, SMS1100 and up
typedef struct SMSHOSTLIB_STATISTICS_ISDBT_S
{
	UINT32 StatisticsType;			//!< Enumerator identifying the type of the structure.  Values are the same as SMSHOSTLIB_DEVICE_MODES_E
									//!< This fiels MUST always first in any statistics structure

	UINT32 FullSize;				//!< Total size of the structure returned by the modem.  If the size requested by 
									//!< the host is smaller than FullSize, the struct will be truncated

	// Common parameters
	UINT32 IsRfLocked;				//!< 0 - not locked, 1 - locked
	UINT32 IsDemodLocked;			//!< 0 - not locked, 1 - locked
	UINT32 IsExternalLNAOn;			//!< 0 - external LNA off, 1 - external LNA on

	// Reception quality
	INT32  SNR;						//!< dB
	INT32  RSSI;					//!< dBm
	INT32  InBandPwr;				//!< In band power in dBM
	INT32  CarrierOffset;			//!< Carrier Offset in Hz

	// Transmission parameters
	UINT32 Frequency;				//!< Frequency in Hz
	UINT32 Bandwidth;				//!< Bandwidth in MHz
	UINT32 TransmissionMode;		//!< ISDB-T transmission mode
	UINT32 ModemState;				//!< 0 - Acquisition, 1 - Locked
	UINT32 GuardInterval;			//!< Guard Interval, 1 divided by value
	UINT32 SystemType;				//!< ISDB-T system type (ISDB-T / ISDB-Tsb)
	UINT32 PartialReception;		//!< TRUE - partial reception, FALSE otherwise
	UINT32 NumOfLayers;				//!< Number of ISDB-T layers in the network

	// Per-layer information
	// Layers A, B and C
	SMSHOSTLIB_ISDBT_LAYER_STAT_ST	LayerInfo[3];	//!< Per-layer statistics, see SMSHOSTLIB_ISDBT_LAYER_STAT_ST	

	// Interface information
	UINT32 SmsToHostTxErrors;		//!< Total number of transmission errors.

} SMSHOSTLIB_STATISTICS_ISDBT_ST;





/// Scan by given range structure
typedef struct SMSHOSTLIB_SCAN_RANGE_S
{
	UINT32		BottomFreq;		//!< The bottom center frequency of the range (Hz)
	UINT32		Gap;			//!< The gap in Hz between consequent center frequencies 
	UINT32		UpFreq;			//!< The upper center frequency of the range (Hz)
} SMSHOSTLIB_SCAN_RANGE_ST; 

/// Scan by given table structure
typedef struct SMSHOSTLIB_SCAN_TABLE_S
{
	UINT32		NumOfFreq;		//!< Number of frequencies 
	UINT32*		pFreqBuff;		//!< Pointer to a buffer contains frequencies (Hz) 
} SMSHOSTLIB_SCAN_TABLE_ST; 

/// DAB/DMB Ensemble information structure
typedef struct
{
	UINT32	EID;			//!< Ensemble ID
	UINT32	Frequency;		//!< Ensemble Frequency 
	UINT32	NumOfService;	//!< Number of services in ensemble 
	UINT8	pDateAndTime[SMSHOSTLIB_MAX_DATE_N_TIME_SIZE]; //!< Date and time 
	UINT8	pLabel[SMSHOSTLIB_MAX_LABEL_SIZE+1];	//!< Ensemble label; Add one byte for End Of String 

} SMSHOSTLIB_ENSEMBLE_INFO_ST;

/// DAB/DMB Service information structure
typedef struct
{
	UINT32	SID;				  	//!< Service ID
	UINT32	Frequency;				//!< Frequency that service is served
	UINT32	Pty;				 	//!< Program type 
	UINT32	CAID;				 	//!< Conditional access ID 
	UINT32	NumOfServiceComponent;	//!< Number of service component for current service
	UINT8	pLabel[SMSHOSTLIB_MAX_LABEL_SIZE+1];	//!< Service label; Add one byte for End Of String

} SMSHOSTLIB_SERVICE_INFO_ST;

typedef struct
{
    UINT32 UserApplicationType;
	UINT32 UserApplicationDataLength;
    UINT8  Data[SMSHOSTLIB_MAX_LEN_OF_APPLICATION_DATA];

} SMSHOSTLIB_COMPONENT_USER_APPL_DATA_ST;

/// DAB/DMB component information structure
typedef struct
{
	UINT32	TMID;			//!< TMID type (according to #SMSHOSTLIB_TMID_TYPES_ET enumerator)
	UINT32	ASCTy;			//!< Audio service type 
	UINT32	DSCTy;			//!< Data service type. see. EN300 401 6.3.1 (0x18:DMB Video service) 
	UINT32	SCIdS;			//!< Service component Index within service
	UINT32	SCId;			//!< Service component (if packet mode)
	UINT32	SubChID;		//!< Sub-channel ID 
	UINT32	FIDCID;			//!< FIDC channel ID (if FIDC channel) 
	UINT32	PacketAddress;	//!< Packet address (if packet mode) 
	UINT32	Primary;		//!< Primary or not primary 	
	UINT32	CAFlag;			//!< Conditional access flag 
	UINT32  SCCA;			//!< Service Component Conditional Access
	UINT32  DG_flag;		//!< Data Groups are used or not to transport the Service Component
	UINT32	Language;		//!< Component Language 
	UINT32  NumOfUserApplications;//!< Component User Applications information
	SMSHOSTLIB_COMPONENT_USER_APPL_DATA_ST UserAppData[SMSHOSTLIB_MAX_USER_APPLICATIONS];
	UINT8	pLabel[SMSHOSTLIB_MAX_LABEL_SIZE+1];	//!< Service component label; Add one byte for End Of String

} SMSHOSTLIB_COMPONENT_INFO_ST;

/// DAB/DMB component information structure
typedef struct
{
	UINT32							EnsembleIdx;	// !< The ensemble index
	SMSHOSTLIB_ENSEMBLE_INFO_ST		EnsembleInfo;	// !< Ensemble information
	SMSHOSTLIB_SERVICE_INFO_ST		ServiceInfo;	// !< Service information
	SMSHOSTLIB_COMPONENT_INFO_ST	ComponentInfo;	// !< Component information

} SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST;

/// Obsolete PID list structure
typedef struct SMSHOSTLIB_PID_FILTERS_S
{
	UINT32	PID_List_Len;
	UINT32	PID_Filters[SMSHOSTLIB_MAX_FILTERS_LIST_LEN];

} SMSHOSTLIB_PID_FILTERS_ST;

//! IPv4 basic structure
typedef	struct SMSHOSTLIB_IPv4_S
{
	UINT8				a_IPv4_addr[4];			//!< IPv4 address

} SMSHOSTLIB_IPV4_ST;

//! IPv6 basic structure
typedef	struct SMSHOSTLIB_IPv6_S
{
	UINT8				a_IPv6_addr[16];			//!< IPv6 address

} SMSHOSTLIB_IPV6_ST;

union SMSHOSTLIB_IP_UN
{	//! Either IPv4 or IPv6 union
	SMSHOSTLIB_IPV4_ST	IPv4;
	SMSHOSTLIB_IPV6_ST	IPv6;
};

//! Platform and provider names structure											
typedef struct SMSHOSTLIB_IP_PL_NAME_S
{
	UINT8 Language[3];										//!< Language code, 3 character
	UINT8 PlatformNameLen;									//!< Platform name length
	UINT8 pPlatformName[SMSHOSTLIB_IP_PLAT_NAME_MAX_LEN];	//!< Platform name
	UINT8 ProviderNameLen;									//!< Provider name length
	UINT8 pProviderName[SMSHOSTLIB_IP_PLAT_NAME_MAX_LEN];	//!< Provider name
} SMSHOSTLIB_IP_PL_NAME_ST;
											
//! Platform IDs basic structure
typedef struct SMSHOSTLIB_IP_PLAT_S
{
	UINT32							PlatformID; 
	SMSHOSTLIB_IP_PL_NAME_ST		Name[SMSHOSTLIB_IP_PLAT_LANGUAGE_MAX_VARIANT]; 
} SMSHOSTLIB_IP_PLAT_ST; 

/// IP Service (DVB-H) structure
typedef struct SMSHOSTLIB_IP_DVBH_SRV_S
{
	UINT32					IpPlatformID;
	SMSHOSTLIB_IP_TYPE_ET	IpType;
	union SMSHOSTLIB_IP_UN	pIpSrcAddr;
	union SMSHOSTLIB_IP_UN	pIpDstAddr;
} SMSHOSTLIB_IP_DVBH_SRV_ST;

/// GPIO Configuration structure
typedef struct SMSHOSTLIB_GPIO_CONFIG_S
{
	UINT8	Direction;				//!< GPIO direction: Input - 0, Output - 1
	UINT8	PullUpDown;				//!< PullUp/PullDown: None - 0, PullDown - 1, PullUp - 2, Keeper - 3
	UINT8	InputCharacteristics;	//!< Input Characteristics: Normal - 0, Schmitt trigger - 1
	UINT8	OutputSlewRate;			//!< SMS1002: Output Slew Rate:	Fast slew rate - 0, Slow slew rate - 1
									//!< SMS1102: Output Slew Rate:	0.45V/ns - 0, 0.9V/ns - 1, 1.7V/ns - 2, 3.3V/ns - 3,
	UINT8	OutputDriving;			//!< SMS1002: Output driving capability:	4mA - 0, 8mA - 1, 12mA - 2, 16mA - 3
									//!< SMS1102: Output driving capability:	1.5mA - 0, 2.8mA - 1, 4mA - 2, 7mA - 3
									//!<										10mA - 4, 11mA - 5, 14mA - 6, 16mA - 7
} SMSHOSTLIB_GPIO_CONFIG_ST;

/// UTC Time structure
typedef struct UTC_TIME_S
{
	UINT16	UTC_Date;				//!< Date, MJD (by annex C)
	UINT8	UTC_Time[3];			//!< BCD Time (Hour, minute and seconds)
}UTC_TIME_ST;

/// Local time offset descriptor structure
typedef struct TIME_OFFSET_S
{
	UINT8		country_code[3];			//!< Country code, ISO 3166
	UINT8		country_region_id;			//!< In-Country region code, 6 bit, Least bit alignment
	UINT8		local_time_offset_polarity;	//!< Time offset polarity to the Greenwich, 1 bit, Least bit alignment
	UINT16		local_time_offset;			//!< Time offset 
	UTC_TIME_ST	time_of_change;				//!< See #UTC_TIME_ST
	UINT16		next_time_offset;			//!< BCD Time (Hour, minute)
}TIME_OFFSET_ST;

/// A struct that describes an SMS device
/// Returned by GetDevicesList, and used by the client for opening a device

typedef struct SmsHostApiDeviceData_S 
{
	// A string identifying this device
	char						DeviceName[SMS_HOST_MAX_DEVICE_NAME_STRING_LEN] ;
	// A logical name exposed to the user
	char						NickName[SMS_HOST_MAX_DEVICE_NICK_NAME_LEN];
	// Is device initialized (by the application)
	BOOL						IsDeviceInit ;
	// Is device connected to the host
	BOOL						IsDeviceConnected ;
	// Device handle - for non-initialized device, the value is SMS_INVALID_DEVICE_INSTANCE_HANDLE
	SmsHostApiDeviceHandle		DevHdl;													
	// Device mode - for non-initialized device, the value is SMSHOSTLIB_DEVMD_NONE
	SMSHOSTLIB_DEVICE_MODES_E	DeviceMode;												
} SmsHostApiDeviceData_ST ;

/*************************************************************************
*			 CMMB definitions 
*************************************************************************/
enum
{
	SMSHOSTLIB_MSG_CMMB_START_SERVICE_RES = 8000,	//!< Response to #SmsHostApiCmmbStartService_Req
	SMSHOSTLIB_MSG_CMMB_STOP_SERVICE_RES,			//!< Response to #SmsHostApiCmmbStopService_Req
	SMSHOSTLIB_MSG_CMMB_START_CONTROL_INFO_RES,		//!< Response to #SmsHostApiCmmbStartControlInfo_Req
	SMSHOSTLIB_MSG_CMMB_STOP_CONTROL_INFO_RES,		//!< Response to #SmsHostApiCmmbStopControlInfo_Req
	SMSHOSTLIB_MSG_CMMB_GET_AVAIL_NETWORKS_RES		//!< Response to #SmsHostApiCmmbGetAvailableNetworks_Req
};

//! CMMB Network basic structure
typedef struct SMSHOSTLIB_CMMB_NETWORK_INFO_S
{
	UINT32							NetworkLevel; 
	UINT32							NetworkNumber; 
	UINT8							Name[SMSHOSTLIB_CMMB_MAX_NETWORK_NAME_LEN]; 
} SMSHOSTLIB_CMMB_NETWORK_INFO_ST;						//!< CMMB Network information struct


// Default APIs set is the single instance API (backward compatible)
#if !defined(SMSHOSTLIB_SINGLE_DEVICE_API) && !defined (SMSHOSTLIB_MULTI_DEVICE_API)
	#define SMSHOSTLIB_SINGLE_DEVICE_API
#endif

#ifdef SMSHOSTLIB_SINGLE_DEVICE_API
/*************************************************************************
*			 Library and and receiver initialization, termination,
*            and power management API
**************************************************************************/

/** @defgroup group1 General and Power Management APIs
 *  @{
 */

/*!
	Initiates the SMS host library.
	
	\param[in]	pCtrlCallback	Control callback function, should be of type #SmsCtrlCbFunc
	\param[in]	pInitLibParams	Library initialization information, according to #SMSHOSTLIB_INITLIB_PARAMS_ST structure

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.
	
	\remark		This command execution is synchronous.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostLibInitEx(	SmsCtrlCbFunc					pCtrlCallback,
						SMSHOSTLIB_INITLIB_PARAMS_ST*	pInitLibParams);

/*************************************************************************/

/*!
	Terminates the SMS host library.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is synchronous.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostLibDeinit( void );

/*************************************************************************/

/*!
	Host requests the device to initialize. 
	Device performs initialization operations but does not output any data yet.
	
	\param[in] DeviceMode	Work mode, according to #SMSHOSTLIB_DEVICE_MODES_E enumerator
	\param[in] pSwImage		Pointer to the SW image
	\param[in] SwImageSize	Size of the SW image
	\param[in] Crystal		Supplied clock's frequency

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.
				
	\see		Response type is SMSHOSTLIB_MSG_INIT_DEVICE_RES. \n
				There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 	
	SmsHostInitDevice_Req(	SMSHOSTLIB_DEVICE_MODES_E	DeviceMode, 
							UINT8*						pSwImage,	
							UINT32						SwImageSize,
							UINT32 						Crystal );

/*************************************************************************/


/** 
 *  @}
 */
 
/** @defgroup group2 Information and Statistics Retrieval APIs
 *  @{
 */

/*************************************************************************
*			 Information and Statistics Retrieval APIs
*************************************************************************/

/*!
	Requests version information from the device and the library.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.
				
	\see		Response type is SMSHOSTLIB_MSG_GET_VERSION_RES. \n
				The response includes a version string. 
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostGetVersion_Req( void );

/*************************************************************************/

/*!
	Requests statistics indications.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GET_STATISTICS_RES \n
				The response includes the supplementary structure SMSHOSTLIB_STATISTICS_ST. 
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostGetStatistics_Req( void );

/*************************************************************************/

/*!
	Requests reception quality.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GET_RECEPTION_RES \n
				The response includes the supplementary integer (UINT32) 
				according to #SMSHOSTLIB_RECEPTION_QUALITY_ET
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostGetReceptionQuality_Req( void );

/*************************************************************************/

/** 
 *  @}
 */

/** @defgroup group3 Tune and Scan APIs
 *  @{
 */
	
/*************************************************************************
*			 Tune and Scan APIs
*************************************************************************/

/*!
	Requests the SMS1000 to tune to a specific frequency
	
	\param[in] Frequency	Frequency Frequency (Hz)
	\param[in] Bandwidth	Bandwidth, according to #SMSHOSTLIB_FREQ_BANDWIDTH_ET 
							enumerator (applicable only for DVB-T/H, ignored for DAB/DMB)

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_TUNE_RES \n
				There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostTune_Req(	UINT32						Frequency,	 /* frequency in Hz */
						SMSHOSTLIB_FREQ_BANDWIDTH_ET	Bandwidth);	 /* bandwidth */

/*************************************************************************/

/*!
	Requests the SMS1000 to perform a scan operation

	\param[in] ScanType				Type of scan, according to #SMSHOSTLIB_SCAN_ANALYSE_TYPE_ET enumerator
	\param[in] FreqBandwidth		Bandwidth, according to #SMSHOSTLIB_FREQ_BANDWIDTH_ET
	\param[in] ParamsType			Defines if we scan according to range of frequencies, or by given table. See #SMSHOSTLIB_SCAN_PARAM_STD_ET.
 	\param[in] pFreqBuff			Buffer contains either #SMSHOSTLIB_SCAN_TABLE_ST structure or 
									#SMSHOSTLIB_SCAN_RANGE_ST structure (according to ParamsType)

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_SCAN_START_RES \n
				The response includes the supplementary boolean representing the start operation status

	\note		
				-# This API is supported only for DVB-H and DAB/DMB. It is not supported in DVB-T.
				-# In DAB/DMB there is a limit of maximum DAB_MAX_MULTIPLEXES supported active ensembles in parallel.
				In case scan encounters more than DAB_MAX_MULTIPLEXES active ensembles it indicates failure for the rest of the frequencies. \n
				-# If N frequencies are scanned, the function returns N-1 times SMSHOSTLIB_SCAN_PROGRESS_IND,
				and than one time SMSHOSTLIB_SCAN_COMPLETE_IND.

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsHostScanStart_Req( SMSHOSTLIB_SCAN_ANALYSE_TYPE_ET	ScanType,
															SMSHOSTLIB_FREQ_BANDWIDTH_ET	FreqBandwidth,
															SMSHOSTLIB_SCAN_PARAM_STD_ET	ParamsType,
															void*							pFreqBuff );

/*************************************************************************/

/*!
	Requests the SMS1000 to stop a scan operation

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_SCAN_STOP_RES \n
				
	\note		SMSHOSTLIB_SCAN_PROGRESS_IND and SMSHOSTLIB_SCAN_COMPLETE_IND may be received  
				between the stop request and the stop asynchronous response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsHostScanStop_Req( void );

/*************************************************************************/

/** 
 *  @}
 */

/** @defgroup group4 DVB APIs
 *  @{
 */

/************************************************************************
*			 DVB APIs
*************************************************************************/

/*!
	Requests to add PID to PID filters list.
	
	\param[in]	PID Program ID.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_ADD_PID_FILTER_RES \n
				The response inculdes the supplementary integer (UINT32) that represents
				the Stream Device handle number.
*/				
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostAddPidFilter_Req( UINT32 PID );

/*************************************************************************/

/*!
	Requests to remove PID from PID filters list.
	
	\param[in]	PID PID.

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_REMOVE_PID_FILTER_RES \n
				There are no supplementary parameters in the response.

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostRemovePidFilter_Req( UINT32 PID );

/*************************************************************************/

/*!
	Requests PID filters list.
	
	\param[in]	MaxPidNum	The maximum number of PIDs may reported (Maximum buffer length).
	\param[out]	pActNum		The actual number of reported PIDs
	\param[out]	pPidList	Buffer of reported filtered PIDs
	
	
	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_RETRIEVE_PID_FILTER_LIST_RES

	\note		The maximum length of the PID filters list is 
				defined as #SMSHOSTLIB_MAX_FILTERS_LIST_LEN. 				
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostRetrievePidFilterList_Req(	UINT32		MaxPidNum,
										UINT32*		pActNum,		
								 		UINT32*		pPidList );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group5 DVB-H APIs
*  @{
*/

/************************************************************************
*			 DVB-H APIs
*************************************************************************/

/*!
	Requests to add IP to IP filters list.
	
	\param[in]	pIpFilter		DVB-H service information, according to #SMSHOSTLIB_IP_DVBH_SRV_ST structure

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_ADD_IP_FILTER_RES \n
				There are no supplementary parameters in the response.
*/
	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostAddIpFilter_Req(	SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
	Requests to remove IP from IP filters list
	
	\param[in]	pIpFilter		Structure contains DVB-H service information

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available

	\see		Response type is SMSHOSTLIB_MSG_REMOVE_IP_FILTER_RES \n
				There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostRemoveIpFilter_Req(	SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
	Requests IP filters list.
	
	\param[in]	MaxNumFilters	The maximum number of IP filters may reported (Maximum buffer length).
	\param[out]	pActNum		The actual number of reported IP filters
	\param[out]	pIpFilter	Buffer of reported filtered IPs

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GET_IP_FILTER_LIST_RES \n
				There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostGetIpFilterList_Req( UINT32						MaxNumFilters,
								UINT32*						pActNum,
								SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
	Request a list of available IP platforms

	\param[in]	MaxNumIpPlatforms	The maximum number of IP platforms may reported (Maximum buffer length).
	\param[out]	pActNum				The actual number of reported IP platforms
	\param[out]	pIpPlatforms		Buffer of reported IP platforms

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GET_AVAIL_IP_PLATFORMS_RES \n
				The response will provide a pointer to a structure of type AVAIL_IPO_PLAT_ST
	*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostGetAvailableIpPlatforms_Req (	UINT32					MaxNumIpPlatforms,  
											UINT32*					pActNum,
											SMSHOSTLIB_IP_PLAT_ST*	pIpPlatforms ); 


/*************************************************************************/

/*!
	Request a DVB SI table

	\param[in]	TablePID			The PID of the required table
	\param[in]	TableID				The ID of the required table
	\param[in]	MaxTableBuffSize	The maximum length (byte) available in the application buffer
	\param[out]	pActSize			The actual size of the table
	\param[out]	pTableBuff			Pointer to the application buffer that will contain the table
	
	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIN_MSG_GET_SI_TBL_RES \n
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostDvbGetSITable_Req(	UINT32	TablePID,
								UINT32	TableID,
								UINT32	MaxTableBuffSize,
								UINT32*	pActSize,
								UINT8*	pTableBuff );

/*************************************************************************/
/*!
	Retrieves TDT's UTC time field .

	\param[out]	pUtcTime		Current time and date

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.
				
	\see		Response type is SMSHOSTLIB_MSG_UTC_TIME_RES. \n
				There are no supplementary parameters in the response.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostDvbTDT_Req( UTC_TIME_ST* pUtcTime );

/*************************************************************************/

/*!
	Retrieves TOT's data

	\param[out]	pUtcTime			Current time and date
	\param[in]	MaxNumElemnt		Maximum number of elements
	\param[out]	pActElemntNum		Actual number of reported time offset elements
	\param[out]	pTimeElementsArray	Buffer for time offset elements

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.
				
	\see		Response type is SMSHOSTLIB_MSG_TIME_OFFSET_RES. \n
				There are no supplementary parameters in the response.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostDvbTOT_Req(	UTC_TIME_ST*	pUtcTime,
						UINT32			MaxNumElemnt,
						UINT32*			pActElemntNum,
						TIME_OFFSET_ST*	pTimeElementsArray );

/** 
*  @}
*/

/** @defgroup group6 DAB/DMB APIs
 *  @{
 */

/*************************************************************************
*			 DAB/DMB API
*************************************************************************/

/*************************************************************************/

/*!
	Get current Ensemble information
	
	\param[in]	MaxNumOfEnsembles	The maximum number of ensembles may reported (Maximum buffer length).
	\param[out]	pActNum				The actual number of reported ensembles
	\param[out]	pEnsembleInfo		Buffer of reported ensembles

	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

	\remark		This command execution is asynchronous. The buffer is\n
				filled when the response SMSHOSTLIB_MSG_GET_ENSEMBLE_INFO_RES is generated.\n

				pActNum can return zero elements with OK error code.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabGetEnsembleInfo_Req(	UINT32 							MaxNumOfEnsembles,
									UINT32*							pActNum,
									SMSHOSTLIB_ENSEMBLE_INFO_ST*	pEnsembleInfo );

/*************************************************************************/

/*!
	Request to retrieve information about available services in a specific ensemble

	\param[in]	EIdx				The ensemble index
	\param[in]	MaxNumOfServices	The maximum number of services may reported (Maximum buffer length).
	\param[out]	pActNum				The actual number of reported services
	\param[out] pServiceInfos		Pointer to array of service information structures

	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

	\remark		This command execution is asynchronous. 
				Response type is SMSHOSTLIB_MSG_GET_SERVICES_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabGetServices_Req (	UINT32 						EIdx,
								UINT32 						MaxNumOfServices,
								UINT32*						pActNum,
								SMSHOSTLIB_SERVICE_INFO_ST* pServiceInfos );

/*************************************************************************/

/*!
	Request to retrieve information about available service components for a given service
	and a given ensemble

	\param[in]	EIdx				The ensemble index
	\param[in]	ServiceID			The service ID
	\param[in]	MaxNumOfComponents	The maximum number of service components may reported (Maximum buffer length).
	\param[out]	pActNum				The actual number of reported service components
	\param[out] pComponentInfos		Pointer to array of service components information structures

	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

	\remark		This command execution is asynchronous. 
				Response type is SMSHOSTLIB_MSG_GET_COMPONENTS_INFO_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabGetComponentsInfo_Req(UINT32							EIdx,
									UINT32							ServiceID, 
								 	UINT32							MaxNumOfComponents,
								 	UINT32*							pActNum,
									SMSHOSTLIB_COMPONENT_INFO_ST*	pComponentInfos );

/*************************************************************************/

/*!
	Request to retrieve all information about available service components

	\param[in]	MaxNumOfCombinedComponents	The maximum number of service components may reported (Maximum buffer length).
	\param[out]	pActNum						The actual number of reported service components
	\param[out] pCombinedComponentInfos		Pointer to array of combined service components information structures

	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

	\remark		This command execution is asynchronous. 
				Response type is SMSHOSTLIB_MSG_GET_COMBINED_COMPONENTS_INFO_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabGetCombinedComponentsInfo_Req(
				UINT32									MaxNumOfCombinedComponents,
				UINT32*									pActNum,
				SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST*	pCombinedComponentInfos );

/*************************************************************************/

/*!
	Requests to begin reception of a given service

	\param[in]  EIdx			The ensemble index
	\param[in]  ServiceID		Service Identifier
	\param[in]  SCIdS			Service Component Identifier within the Service
	\param[out] pServiceHandle	Returned service handle, valid only when the response is OK

	\return		Response type is a handle that represents the data stream being played.

	\remark		This command execution is asynchronous\n
				Response is SMSHOSTLIB_MSG_START_SERVICE_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabStartService_Req(	UINT32 EIdx,
								UINT32 ServiceID,
								UINT32 SCIdS,
								UINT32 *pServiceHandle );

/*************************************************************************/

/*!
	Requests to stop reception of a given service

	\param[in]	ServiceHandle	Handle of given service, obtained from SmsHostDabStartService_Req()

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous \n
				Response is SMSHOSTLIB_MSG_STOP_SERVICE_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostDabStopService_Req (UINT32 ServiceHandle);

/*************************************************************************/

/*!
	Request to get the index of the current ensemble

	\param[out]	pEIdx	Pointer to Ensemble ID

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous\n
				Response is SMSHOSTLIB_GET_CURR_ENSEMBLE_INDEX_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostDabGetCurrentEnsembleIndex_Req ( UINT32* pEIdx );


/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group7 GPIO Control APIs
 *  @{
 */

/*!
	Requests the SMS1000 to configure a PIN to be used as GPIO

	\param[in]	PinNum				Pin number
	\param[in]	pGpioConfig			GPIO configuration information, according to #SMSHOSTLIB_GPIO_CONFIG_ST

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GPIO_CONFIGURE_RES \n
				There are no supplementary parameters in the response.

	\note		This function can be called once per allocated pin. After a pin is
				configured as GPIO, it may not re-configured back to its origin state.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostGPIOConfigure_Req (UINT8 PinNum, SMSHOSTLIB_GPIO_CONFIG_ST *pGpioConfig);

/*************************************************************************/

/*!
	Requests the SMS1000 to set GPIO level.

	\param[in]	PinNum				PIN number
	\param[in]	NewLevel			The new level of the GPIO. Low - 0, High - 1

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GPIO_SET_LEVEL_RES \n
				There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostGPIOSetLevel_Req (UINT8 PinNum, UINT8 NewLevel );

/*************************************************************************/

/*!
	Requests the SMS1000 to get GPIO level.

	\param[in]	PinNum			PIN number
	
	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GPIO_GET_LEVEL_RES \n
				The response includes the current level as UINT32 value
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostGPIOGetLevel_Req (UINT8 PinNum);

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group8 Obsolete APIs
 *  @{
 */

/*************************************************************************
*			 OBSOLETE API
*************************************************************************/

/*************************************************************************/

/*!
	Initiates the SMS host library.
	
	\param[in]	pCtrlCallback	Control callback function. 

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.
	
	\remark		This command execution is synchronous.

	\note		This function is obsolete. Please use #SmsHostLibInitEx instead.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostLibInit(	SmsCtrlCbFunc pCtrlCallback );
	
/*************************************************************************/

/*!
	Host initializes the communication type

	\param[in] commType		Communication type (according to #SMSHOSTLIB_COMM_TYPES_E enumerator)
	\param[in] portName		Port name (only if communication type is UART)
	\param[in] bitRate		Bit rate (only if communication type is UART)

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is synchronous

	\note		This function is obsolete. Please use #SmsHostLibInitEx instead.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostSetCommType(	SMSHOSTLIB_COMM_TYPES_E		commType,
						const char*					portName,
						UINT32						bitRate);

/*************************************************************************/

/*!
	Requests to begin reception of a given service
	
	\param[in] ServiceId	Service Identifier
	\param[in] SCIdS		Service Component Identifier within the Service
	
	\return		Response type is a handle that represents the data stream being played.
	
	\remark		This command execution is synchronous.  No callback is given.

	\note		This function is obsolete. Please use #SmsHostDabStartService_Req instead.

*/
HANDLE SMSHOSTLIB_API  
	SmsHostStartService (UINT32 ServiceId, UINT32 SCIdS);

/*************************************************************************/

/*!
	Requests to stop reception of a given service
	
	\param[in]	Service	Handle of given service, obtained from SmsHostStartService
	
	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.
	
	\remark		This command execution is synchronous.  No callback is given.

	\note		This function is obsolete. Please use #SmsHostDabStopService_Req instead.

*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostStopService (HANDLE Service);

/*************************************************************************/

/*!
	Get current Ensemble information
	
	\param[out] pEnsembleInfo	Pointer to buffer for returned data
	
	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator
	
	\remark		This command execution is synchronous.  No callback is given.

	\note		This function is obsolete. Please use #SmsHostDabGetEnsembleInfo_Req instead.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostGetEnsembleInfo (SMSHOSTLIB_ENSEMBLE_INFO_ST* pEnsembleInfo);

/*************************************************************************/

/*!
	Request to retrieve information about available services in the ensemble
	
	\param[out] pServiceInfos	Pointer to array of service information structures.  The array must be large enough to hold #SMSHOSTLIB_DAB_MAX_SERVICES entries
	\param[out] pServiceCount	Pointer to integer for returned number of services. The result should be equal to the value of SMSHOSTLIB_ENSEMBLE_INFO::NumOfService
	
	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator
	
	\remark		This command execution is synchronous.  No callback is given.

	\note		This function is obsolete. Please use #SmsHostDabGetServices_Req instead.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostGetServices (SMSHOSTLIB_SERVICE_INFO_ST* pServiceInfos, UINT32* pServiceCount);

/*************************************************************************/

/*!
	Request to retrieve information about available service components for a given service
	
	\param[in]  serviceId		Service Identifier of given service
	\param[out] pComponentInfos	Pointer to array of service component information structures.  The array must be large enough to hold #SMSHOSTLIB_DAB_MAX_SERVICE_COMPONENTS_IN_SERVICE entries
	\param[out] pComponentCount	Pointer to integer for returned number of service components. Should be equal to SMSHOSTLIB_SERVICE_INFO::NumOfServiceComponent
	
	\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator
	
	\remark		This command execution is synchronous.  No callback is given.

	\note		This function is obsolete. Please use #SmsHostDabGetComponentsInfo_Req instead.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostGetComponentsInfo (UINT32 serviceId, SMSHOSTLIB_COMPONENT_INFO_ST* pComponentInfos, UINT32* pComponentCount);

/*************************************************************************/

/*!
	Requests PID filters list.
	
	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostLibInit() is executed when a response is available.

	\see		Response type is SMSHOSTLIB_MSG_GET_PID_FILTER_LIST_RES \n
								
				The response includes the supplementary structure SMSHOSTLIB_PID_FILTERS_ST
				contains the filters-in PIDs.

	\note		
				-# The maximum length of the PID filters list is 
				defined as #SMSHOSTLIB_MAX_FILTERS_LIST_LEN.\n
				-# This function is obsolete. Please use #SmsHostRetrievePidFilterList_Req instead.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostGetPidFilterList_Req( void );

/** 
*  @}
*/

#endif 	// SMSHOSTLIB_SINGLE_DEVICE_API


#ifdef SMSHOSTLIB_MULTI_DEVICE_API
/*************************************************************************/

/*************************************************************************/
/// Multiple Instances API
/*************************************************************************/

/*************************************************************************
*			 Library and and receiver initialization, termination,
*            and power management API
**************************************************************************/

/** @defgroup group10 General and Power Management APIs
*  @{
*/

/*!
Initiates the SMS host library.

\param[in]	pInitLibParams	Library initialization information, according to #SMSHOSTLIB_API_INITLIB_PARAMS_ST structure

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is synchronous.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiLibInit( SMSHOSTLIB_API_INITLIB_PARAMS_ST*	pInitLibParams );

/*************************************************************************/

/*!
Terminates the SMS host library.

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is synchronous.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiLibTerminate( void );

/*************************************************************************/

/*!
Returns a list of existing SMS devices 

\param[in]  MaxDevNum		Max number of devices to be reported in the buffer
\param[out] pActNum			Actual number of reported devices
\param[out] DeviceDataArr	An array of Devices reported, allocated by the caller,
							The array size is MaxDevNum

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is synchronous. The list is complete
			when the function returns.

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 	
	SmsHostApiGetDevicesList(UINT32						MaxDevNum,
							UINT32*						pActNum,
							SmsHostApiDeviceData_ST*	DeviceDataArr );


/*************************************************************************/

/*!
Host requests the device to initialize. 
Device performs initialization operations but does not output any data yet.

\param[in]	pDeviceName		The name of device to open. 
							In single instance mode - leave this parameter NULL.
							(Single instance mode is set in #SMSHOSTLIB_API_INITLIB_PARAMS_ST)
							In multiple instance mode, the name is received from an earlier 
							call to #SmsHostApiGetDevicesList
\param[in]	DeviceMode		Work mode, according to #SMSHOSTLIB_DEVICE_MODES_E enumerator
\param[in]	Crystal			Supplied clock's frequency
\param[in]	ClientContext	An opaque pointer that will be passed as is to the client
							callback function when the callback is called in the context
							of this device
\param[out] pDevHdl			A pointer to a handle of the opened device 
							(or NULL if the function failed)


\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_INIT_DEVICE_RES. \n
There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 	
SmsHostApiDeviceInit_Req(
						 const char*					pDeviceName,
						 SMSHOSTLIB_DEVICE_MODES_E		DeviceMode, 
						 UINT32 						Crystal,
						 void*							ClientContext,
						 SmsHostApiDeviceHandle*		pDevHdl
						 );

/*************************************************************************/


/** 
*  @}
*/

/** @defgroup group12 Information and Statistics Retrieval APIs
*  @{
*/

/*************************************************************************
*			 Information and Statistics Retrieval APIs
*************************************************************************/

/*!
Requests version information from the device and the library.

\param[in]	DevHdl	A handle of the device, got from a previous
					call to SmsHostApiDeviceInit_Req

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GET_VERSION_RES. \n
			The response includes a version string. 
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiGetVersion_Req( SmsHostApiDeviceHandle DevHdl );

/*************************************************************************/

/*!
Requests statistics indications.

\param[in]	DevHdl	A handle of the device, got from a previous
					call to SmsHostApiDeviceInit_Req

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.  This API
			is obsolete in SMS1100 and up, and SmsHostApiGetStatisticsEx_Req
			should be used instead


\see		Response type is SMSHOSTLIB_MSG_GET_STATISTICS_RES \n
The response includes the supplementary structure SMSHOSTLIB_STATISTICS_ST. 
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiGetStatistics_Req( SmsHostApiDeviceHandle DevHdl );

/*************************************************************************/

/*!
Requests statistics indications (SMS1100 and up).

\param[in]	DevHdl	A handle of the device, got from a previous
					call to SmsHostApiDeviceInit_Req

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GET_STATISTICS_EX_RES \n
The response includes the supplementary structure which may be one of the following:
	SMSHOSTLIB_STATISTICS_DVB_ST		For DVB-T/H applications
	SMSHOSTLIB_STATISTICS_DAB_ST		For DAB/T-DMB applications
	SMSHOSTLIB_STATISTICS_ISDBT_ST		For ISDB-T applications

The first UINT32 of the statistics structure hold the type of the structure using SMSHOSTLIB_DEVICE_MODES_E enumerator values

*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiGetStatisticsEx_Req( SmsHostApiDeviceHandle DevHdl );

/*************************************************************************/

/*!
Requests reception quality.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GET_RECEPTION_RES \n
			The response includes the supplementary integer (UINT32) 
			according to #SMSHOSTLIB_RECEPTION_QUALITY_ET
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiGetReceptionQuality_Req( SmsHostApiDeviceHandle DevHdl );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group13 Tune and Scan APIs
*  @{
*/

/*************************************************************************
*			 Tune and Scan APIs
*************************************************************************/

/*!
Requests the SMS1100 to tune ISDB-T to a specific frequency

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in] Frequency	Frequency Frequency (Hz)
\param[in] Bandwidth	Bandwidth, according to #SMSHOSTLIB_FREQ_BANDWIDTH_ET 
\c enumerator (applicable only for DVB-T/H, ignored for DAB/DMB)
\param[in] SegmentNumber	Segment number in the range 0-13, needed for ISDB-T SB

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_ISDBT_TUNE_RES \n
There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
SmsHostApiIsdbtTune_Req(SmsHostApiDeviceHandle			DevHdl,
						UINT32							Frequency,		/* frequency in Hz		*/
						SMSHOSTLIB_FREQ_BANDWIDTH_ET	Bandwidth,		/* bandwidth			*/
						UINT32							SegmentNumber);	/* segment number 0-13	*/

/*************************************************************************/

/*!
Requests the SMS1000 to tune to a specific frequency

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in] Frequency	Frequency Frequency (Hz)
\param[in] Bandwidth	Bandwidth, according to #SMSHOSTLIB_FREQ_BANDWIDTH_ET 
enumerator (applicable only for DVB-T/H, ignored for DAB/DMB)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_TUNE_RES \n
There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
SmsHostApiTune_Req(SmsHostApiDeviceHandle		DevHdl,
				   UINT32						Frequency,	 /* frequency in Hz */
				   SMSHOSTLIB_FREQ_BANDWIDTH_ET	Bandwidth);	 /* bandwidth */

/*************************************************************************/

/*!
Configures a list of filters to be used in a subsequent SCAN operation

\param[in] DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in] FilterType			Type of filters in the list, according to #SMSHOSTLIB_SCAN_FILTER_TYPE_ET enumerator
\param[in] NrFilters			Number of entries in the list
\param[in] pFilterList			Buffer contains a list of values to be used as channel scan filters

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is synchronous, there is no associated callback

\note
-# This API is supported only for DVB-H
-# The host sw can configure a <cellId> filter list, a <networkId> filter list, or both in two subsequent calls
-# A second fiter list of the same type invalidates a precedent one
-# Each scan request needs its own filter configuration, the fiters are not kept after the scan requests

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiScanFilterListAdd_Req(SmsHostApiDeviceHandle			    DevHdl,
										SMSHOSTLIB_SCAN_FILTER_TYPE_ET	FilterType,
										UINT32							NrFilters,
										UINT32*							pFilterList);

/*************************************************************************/

/*!
Requests the SMS1000 to perform a scan operation

\param[in]	DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in] ScanType				Type of scan, according to #SMSHOSTLIB_SCAN_PARAM_STD_ET enumerator
\param[in] FreqBandwidth		Bandwidth, according to #SMSHOSTLIB_FREQ_BANDWIDTH_ET
\param[in] ParamsType			Defines if we scan according to range of frequencies, or by given table.
\param[in] pFreqBuff			Buffer contains either #SMSHOSTLIB_SCAN_TABLE_ST structure or 
#SMSHOSTLIB_SCAN_RANGE_ST structure (according to ParamsType)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_SCAN_START_RES \n
The response includes the supplementary boolean representing the start operation status

\note		
-# This API is supported only for DVB-H and DAB/DMB. It is not supported in DVB-T.
-# In DAB/DMB there is a limit of maximum DAB_MAX_MULTIPLEXES supported active ensembles in parallel.
In case scan encounters more than DAB_MAX_MULTIPLEXES active ensembles it indicates failure for the rest of the frequencies. \n
-# If N frequencies are scanned, the function returns N-1 times SMSHOSTLIB_SCAN_PROGRESS_IND,
and than one time SMSHOSTLIB_SCAN_COMPLETE_IND.

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiScanStart_Req(SmsHostApiDeviceHandle DevHdl,
						SMSHOSTLIB_SCAN_ANALYSE_TYPE_ET	ScanType,
						SMSHOSTLIB_FREQ_BANDWIDTH_ET	FreqBandwidth,
						SMSHOSTLIB_SCAN_PARAM_STD_ET	ParamsType,
						void*							pFreqBuff );

/*************************************************************************/

/*!
Requests the SMS1000 to stop a scan operation

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_SCAN_STOP_RES \n

\note		SMSHOSTLIB_SCAN_PROGRESS_IND and SMSHOSTLIB_SCAN_COMPLETE_IND may be received  
between the stop request and the stop asynchronous response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiScanStop_Req( SmsHostApiDeviceHandle DevHdl );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group14 DVB APIs
*  @{
*/

/************************************************************************
*			 DVB APIs
*************************************************************************/

/*!
Requests to add PID to PID filters list.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	PID Program ID.

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_ADD_PID_FILTER_RES \n
The response inculdes the supplementary integer (UINT32) that represents
the Stream Device handle number.
*/				
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiAddPidFilter_Req( SmsHostApiDeviceHandle DevHdl,
						   UINT32 PID );

/*************************************************************************/

/*!
Requests to remove PID from PID filters list.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	PID PID.

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_REMOVE_PID_FILTER_RES \n
There are no supplementary parameters in the response.

*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiRemovePidFilter_Req( SmsHostApiDeviceHandle DevHdl,
							  UINT32 PID );

/*************************************************************************/

/*!
Requests PID filters list.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	MaxPidNum	The maximum number of PIDs may reported (Maximum buffer length).
\param[out]	pActNum		The actual number of reported PIDs
\param[out]	pPidList	Buffer of reported filtered PIDs


\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_RETRIEVE_PID_FILTER_LIST_RES

\note		The maximum length of the PID filters list is 
defined as #SMSHOSTLIB_MAX_FILTERS_LIST_LEN. 				
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
SmsHostApiRetrievePidFilterList_Req(	SmsHostApiDeviceHandle	DevHdl,
										UINT32					MaxPidNum,
										UINT32*					pActNum,		
										UINT32*					pPidList );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group15 DVB-H APIs
*  @{
*/

/************************************************************************
*			 DVB-H APIs
*************************************************************************/

/*!
Requests to add IP to IP filters list.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	pIpFilter		DVB-H service information, according to #SMSHOSTLIB_IP_DVBH_SRV_ST structure

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_ADD_IP_FILTER_RES \n
There are no supplementary parameters in the response.
*/

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiAddIpFilter_Req(	SmsHostApiDeviceHandle DevHdl,
						  SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
Requests to remove IP from IP filters list

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	pIpFilter		Structure contains DVB-H service information

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available

\see		Response type is SMSHOSTLIB_MSG_REMOVE_IP_FILTER_RES \n
There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiRemoveIpFilter_Req(	SmsHostApiDeviceHandle DevHdl, 
									SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
Requests IP filters list.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	MaxNumFilters	The maximum number of IP filters may reported (Maximum buffer length).
\param[out]	pActNum		The actual number of reported IP filters
\param[out]	pIpFilter	Buffer of reported filtered IPs

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GET_IP_FILTER_LIST_RES \n
There are no supplementary parameters in the response.
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiGetIpFilterList_Req(	SmsHostApiDeviceHandle		DevHdl,
									UINT32						MaxNumFilters,
									UINT32*						pActNum,
									SMSHOSTLIB_IP_DVBH_SRV_ST*	pIpFilter );

/*************************************************************************/

/*!
Request a list of available IP platforms

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	MaxNumIpPlatforms	The maximum number of IP platforms may reported (Maximum buffer length).
\param[out]	pActNum				The actual number of reported IP platforms
\param[out]	pIpPlatforms		Buffer of reported IP platforms

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GET_AVAIL_IP_PLATFORMS_RES \n
The response will provide a pointer to a structure of type AVAIL_IPO_PLAT_ST
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
SmsHostApiGetAvailableIpPlatforms_Req (	SmsHostApiDeviceHandle	DevHdl,
										UINT32					MaxNumIpPlatforms,  
										UINT32*					pActNum,
										SMSHOSTLIB_IP_PLAT_ST*	pIpPlatforms ); 


/*************************************************************************/

/*!
Request a DVB SI table

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	TablePID			The PID of the required table
\param[in]	TableID				The ID of the required table
\param[in]	MaxTableBuffSize	The maximum length (byte) available in the application buffer
\param[out]	pActSize			The actual size of the table
\param[out]	pTableBuff			Pointer to the application buffer that will contain the table

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIN_MSG_GET_SI_TBL_RES \n
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiDvbGetSITable_Req(SmsHostApiDeviceHandle	DevHdl,
								UINT32					TablePID,
								UINT32					TableID,
								UINT32					MaxTableBuffSize,
								UINT32*					pActSize,
								UINT8*					pTableBuff );

/*************************************************************************/
/*!
Retrieves TDT's UTC time field .

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[out]	pUtcTime		Current time and date

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_UTC_TIME_RES. \n
There are no supplementary parameters in the response.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiDvbTDT_Req(	SmsHostApiDeviceHandle	DevHdl,
							UTC_TIME_ST*			pUtcTime );

/*************************************************************************/

/*!
Retrieves TOT's data

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[out]	pUtcTime			Current time and date
\param[in]	MaxNumElemnt		Maximum number of elements
\param[out]	pActElemntNum		Actual number of reported time offset elements
\param[out]	pTimeElementsArray	Buffer for time offset elements

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_TIME_OFFSET_RES. \n
There are no supplementary parameters in the response.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiDvbTOT_Req(	SmsHostApiDeviceHandle	DevHdl,
							UTC_TIME_ST*			pUtcTime,
							UINT32					MaxNumElemnt,
							UINT32*					pActElemntNum,
							TIME_OFFSET_ST*			pTimeElementsArray );


/*************************************************************************/

/*!
Set the DVB-H out of coverage area timers. During this period the system will
try periodically to synchronize on the last used frequency and on the neighbor
cells.

\param[in]	DevHdl		A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	Timer1Value	The OOC T1 value to be configured in milisecond units
During this period the system scans only the Last Used Frequency

\param[in]	Timer2Value	The OOC T2 value to be configured in milisecond units
During this period the system scans the LUF and the neighbor cells

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_SET_OOC_TIMER_RES. \n
There are no supplementary parameters in the response.
*/

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiSetOOCTimer_Req(	SmsHostApiDeviceHandle	DevHdl,
								UINT32					Timer1Value,
								UINT32					Timer2Value);

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group16 DAB/DMB APIs
*  @{
*/

/*************************************************************************
*			 DAB/DMB API
*************************************************************************/

/*************************************************************************/

/*!
Get current Ensemble information

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	MaxNumOfEnsembles	The maximum number of ensembles may reported (Maximum buffer length).
\param[out]	pActNum				The actual number of reported ensembles
\param[out]	pEnsembleInfo		Buffer of reported ensembles

\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

\remark		This command execution is asynchronous. The buffer is\n
filled when the response SMSHOSTLIB_MSG_GET_ENSEMBLE_INFO_RES is generated.\n

pActNum can return zero elements with OK error code.
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabGetEnsembleInfo_Req(SmsHostApiDeviceHandle			DevHdl,
									UINT32 							MaxNumOfEnsembles,
									UINT32*							pActNum,
									SMSHOSTLIB_ENSEMBLE_INFO_ST*	pEnsembleInfo );

/*************************************************************************/

/*!
Request to retrieve information about available services in a specific ensemble

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	EIdx				The ensemble index
\param[in]	MaxNumOfServices	The maximum number of services may reported (Maximum buffer length).
\param[out]	pActNum				The actual number of reported services
\param[out] pServiceInfos		Pointer to array of service information structures

\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

\remark		This command execution is asynchronous. 
Response type is SMSHOSTLIB_MSG_GET_SERVICES_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabGetServices_Req (	SmsHostApiDeviceHandle		DevHdl,
									UINT32 						EIdx,
									UINT32 						MaxNumOfServices,
									UINT32*						pActNum,
									SMSHOSTLIB_SERVICE_INFO_ST* pServiceInfos );

/*************************************************************************/

/*!
Request to retrieve information about available service components for a given service
and a given ensemble

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	EIdx				The ensemble index
\param[in]	ServiceID			The service ID
\param[in]	MaxNumOfComponents	The maximum number of service components may reported (Maximum buffer length).
\param[out]	pActNum				The actual number of reported service components
\param[out] pComponentInfos		Pointer to array of service components information structures

\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

\remark		This command execution is asynchronous. 
Response type is SMSHOSTLIB_MSG_GET_COMPONENTS_INFO_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabGetComponentsInfo_Req(	SmsHostApiDeviceHandle			DevHdl,
										UINT32							EIdx,
										UINT32							ServiceID, 
										UINT32							MaxNumOfComponents,
										UINT32*							pActNum,
										SMSHOSTLIB_COMPONENT_INFO_ST*	pComponentInfos );

/*************************************************************************/

/*!
Request to retrieve all information about available service components

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	MaxNumOfCombinedComponents	The maximum number of service components may reported (Maximum buffer length).
\param[out]	pActNum						The actual number of reported service components
\param[out] pCombinedComponentInfos		Pointer to array of combined service components information structures

\return		Error code by #SMSHOSTLIB_ERR_CODES_E enumerator

\remark		This command execution is asynchronous. 
Response type is SMSHOSTLIB_MSG_GET_COMBINED_COMPONENTS_INFO_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabGetCombinedComponentsInfo_Req(	
		SmsHostApiDeviceHandle					DevHdl,
		UINT32									MaxNumOfCombinedComponents,
		UINT32*									pActNum,
		SMSHOSTLIB_COMBINED_COMPONENT_INFO_ST*	pCombinedComponentInfos );

/*************************************************************************/

/*!
Requests to begin reception of a given service

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]  EIdx			The ensemble index
\param[in]  ServiceID		Service Identifier
\param[in]  SCIdS			Service Component Identifier within the Service
\param[out] pServiceHandle	Returned service handle, valid only when the response is OK

\return		Response type is a handle that represents the data stream being played.

\remark		This command execution is asynchronous\n
Response is SMSHOSTLIB_MSG_START_SERVICE_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabStartService_Req(	SmsHostApiDeviceHandle	DevHdl,
									UINT32 					EIdx,
									UINT32 					ServiceID,
									UINT32 					SCIdS,
									UINT32 					*pServiceHandle );

/*************************************************************************/

/*!
Requests to stop reception of a given service

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	ServiceHandle	Handle of given service, obtained from SmsHostApiDabStartService_Req()

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous \n
Response is SMSHOSTLIB_MSG_STOP_SERVICE_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiDabStopService_Req (	SmsHostApiDeviceHandle	DevHdl, 
									UINT32					ServiceHandle);

/*************************************************************************/

/*!
Request to get the index of the current ensemble

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[out]	pEIdx	Pointer to Ensemble ID

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous\n
Response is SMSHOSTLIB_GET_CURR_ENSEMBLE_INDEX_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiDabGetCurrentEnsembleIndex_Req (	SmsHostApiDeviceHandle	DevHdl,
												UINT32*					pEIdx );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group17 ISDB-T APIs
*  @{
*/
/*************************************************************************/

/*!
Select which ISDB-T layer(s) are processed by the demodulator

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	Layer	ISDB-T layer

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous\n
			Response is SMSHOSTLIB_ISDBT_SET_LAYER_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiIsdbtSelectLayer_Req (	SmsHostApiDeviceHandle		DevHdl,
										SMSHOSTLIB_ISDBT_LAYER_ET	Layer );

/*************************************************************************/

/** 
*  @}
*/

/** @defgroup group18 GPIO Control APIs
*  @{
*/

/*!
Requests the SMS1000 to configure a PIN to be used as GPIO

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	PinNum				Pin number
\param[in]	pGpioConfig			GPIO configuration information, according to #SMSHOSTLIB_GPIO_CONFIG_ST

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GPIO_CONFIGURE_RES \n
There are no supplementary parameters in the response.

\note		This function can be called once per allocated pin. After a pin is
configured as GPIO, it may not re-configured back to its origin state.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
SmsHostApiGPIOConfigure_Req (	SmsHostApiDeviceHandle		DevHdl,
								UINT8						PinNum, 
								SMSHOSTLIB_GPIO_CONFIG_ST*	pGpioConfig);

/*************************************************************************/

/*!
Requests the SMS1000 to set GPIO level.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	PinNum				PIN number
\param[in]	NewLevel			The new level of the GPIO. Low - 0, High - 1

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GPIO_SET_LEVEL_RES \n
There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiGPIOSetLevel_Req (	SmsHostApiDeviceHandle	DevHdl,
									UINT8					PinNum, 
									UINT8					NewLevel );

/*************************************************************************/

/*!
Requests the SMS1000 to be ready to external Power Down.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_POWER_DOWN_RES \n
There are no supplementary parameters in the response.
*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
SmsHostApiPowerDown_Req( SmsHostApiDeviceHandle DevHdl );



/*************************************************************************/

/*!
Requests the SMS1000 to get GPIO level.

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	PinNum			PIN number

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_GPIO_GET_LEVEL_RES \n
The response includes the current level as UINT32 value
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiGPIOGetLevel_Req (SmsHostApiDeviceHandle	DevHdl,
								UINT8					PinNum);

/*************************************************************************/

/*************************************************************************/

/*!
Terminate (close) a device

\param[in]	DevHdl	A handle of the device (given at #SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
\c SmsHostApiLibInit() is executed when a response is available.

*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiDeviceTerminate_Req( SmsHostApiDeviceHandle	DevHdl );

/*************************************************************************/

/*************************************************************************/

/*!
Read data from an open service

\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	ServiceHandle	A data service handle got from a StartService request
\param[in]	pBuffer	A pointer to a buffer to which the data will be written
\param[in]	MaxBufferSize	The size of the buffer - the maximal number of bytes to write into it 
\param[in]	pNumBytesRead	The actual number of bytes read 

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is synchronous

*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiServiceReadBuf(	SmsHostApiDeviceHandle	DevHdl,
								UINT32					ServiceHandle,
								UINT8*					pBuffer, 
								UINT32					MaxBufferSize,
								UINT32*					pNumBytesRead );


/*************************************************************************/


/** 
*  @}
*/



/** @defgroup group19 Raw Tuner Mode APIs
*  @{
*/

/*!
Start a raw I/Q sample capture operation

\param[in]	DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	Frequency			RF frequency, in Hz
\param[in]	Bandwidth			Bandwidth
\param[in]	SamplesNum			Number of samples to capture.  Each sample consists of 8 bits I and 8 bits Q.  


\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_RAW_CAPTURE_RES \n
			There are no supplementary parameters in the response.

\note		
			This API is supported only for the SMS1100 or higher chipsets.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiRawCapture_Req(SmsHostApiDeviceHandle			DevHdl,
							UINT32							Frequency,	 /* frequency in Hz */
							SMSHOSTLIB_FREQ_BANDWIDTH_ET	Bandwidth,	 /* bandwidth */
							UINT32							SamplesNum);

/*!
Abort the raw I/Q sample capture operation

\param[in]	DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_RAW_CAPTURE_ABORT_RES \n
			There are no supplementary parameters in the response.

\note		
			This API is supported only for the SMS1100 or higher chipsets.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiRawCaptureAbort_Req(SmsHostApiDeviceHandle DevHdl);


/*!
Retrieves a snapshot of the current raw samples counter

\param[in]	DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_RAW_GET_COUNTER_RES \n
			The response includes the current sample counter as a UINT32 value

\note		
			This API is supported only for the SMS1100 or higher chipsets.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiRawGetCounter_Req(SmsHostApiDeviceHandle DevHdl);


/*!
Retrieve raw AGC parameters

\param[in]	DevHdl				A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is SMSHOSTLIB_MSG_RAW_GET_AGC_RES \n
			The response includes the supplementary structure SMSHOSTLIB_AGC_ST. 

\note		
			This API is supported only for the SMS1100 or higher chipsets.

*/	

SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiRawGetAGC_Req(SmsHostApiDeviceHandle DevHdl);

/** 
*  @}
*/


//*********************************** CMMB ***********************************//

/** @defgroup group20 CMMB APIs
*  @{
*/
/*************************************************************************/


/*!
Request a list of available CMMB Networks

	\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
	\param[in]	MaxNumNetworks		The maximum number of IP platforms may reported (Maximum buffer length).
	\param[out]	pActNum				The actual number of reported Networks
	\param[out]	pNetworks			Buffer of reported Networks

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous, the callback given at
				\c SmsHostApiLibInit() is executed when a response is available.

	\see		Response type is #SMSHOSTLIB_MSG_CMMB_GET_AVAIL_NETWORKS_RES \n
				The response will provide a pointer to a structure of type #SMSHOSTLIB_CMMB_NETWORK_INFO_ST
*/	
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiCmmbGetAvailableNetworks_Req (	SmsHostApiDeviceHandle				DevHdl,
												UINT32								MaxNumNetworks,  
												UINT32*								pActNum,
												SMSHOSTLIB_CMMB_NETWORK_INFO_ST*	pNetworks ); 


/*************************************************************************/

/*!
Requests to begin reception of a given service

	\param[in]	DevHdl			A handle of the device (given at SmsHostApiDeviceInit_Req)
	\param[in]  NetworkLevel	Network number and NetworkLevel identify a unique network
	\param[in]  NetworkNumber	Network number and NetworkLevel identify a unique network
	\param[in]  ServiceID		Service Identifier
	\param[out] pServiceHandle	Returned service handle, valid only when the response is OK

	\return		Response type is a handle that represents the data stream being played.

	\remark		This command execution is asynchronous\n
				Response is #SMSHOSTLIB_MSG_CMMB_START_SERVICE_RES
*/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API  
	SmsHostApiCmmbStartService_Req(	SmsHostApiDeviceHandle DevHdl,
									UINT32 NetworkLevel,
									UINT32 NetworkNumber,
									UINT32 ServiceID,
									UINT32* pServiceHandle );


/*************************************************************************/

/*!
Requests to stop reception of a given service

	\param[in]	DevHdl	A handle of the device (given at SmsHostApiDeviceInit_Req)
	\param[in]	ServiceHandle	Handle of given service, obtained from SmsHostApiCmmbStartService_Req()

	\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

	\remark		This command execution is asynchronous \n
				Response is #SMSHOSTLIB_MSG_CMMB_STOP_SERVICE_RES
*/

/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API
	SmsHostApiCmmbStopService_Req (	SmsHostApiDeviceHandle	DevHdl, 
									UINT32					ServiceHandle);

/*************************************************************************/

/*!
Requests to add the Control Information data of a network.
After this request is processed, the control information data 
will be received the returned service handle

\param[in]	DevHdl			A handle of the device (given at SmsHostApiDeviceInit_Req)
\param[in]	NetworkLevel	Network number and NetworkLevel together identify a unique network
\param[in]	NetworkNumber	Network number and NetworkLevel together identify a unique network

\param[out] pServiceHandle	Returned service handle, valid only when the response is OK

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is #SMSHOSTLIB_MSG_CMMB_START_CONTROL_INFO_RES \n
			The response includes the supplementary integer (UINT32) that represents \n
			the data service handle number.
*/				
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiCmmbStartControlInfo_Req(	SmsHostApiDeviceHandle	DevHdl,
										UINT32 NetworkLevel,
										UINT32 NetworkNumber,
										UINT32* pServiceHandle );

/*************************************************************************/

/*!
Requests to stop the Control Information data of a network, previously 
started by a call to SmsHostApiCmmbStartControlInfo_Req.

\param[in]	DevHdl			A handle of the device (given at SmsHostApiDeviceInit_Req)

\return		Error code by  #SMSHOSTLIB_ERR_CODES_E enumerator.

\remark		This command execution is asynchronous, the callback given at
			\c SmsHostApiLibInit() is executed when a response is available.

\see		Response type is #SMSHOSTLIB_MSG_CMMB_STOP_CONTROL_INFO_RES \n
*/				
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API 
	SmsHostApiCmmbStopControlInfo_Req(	SmsHostApiDeviceHandle	DevHdl );

/** 
*  @}
*/

#endif //SMSHOSTLIB_MULTI_DEVICE_API

#if defined (__cplusplus)
}
#endif

#endif
