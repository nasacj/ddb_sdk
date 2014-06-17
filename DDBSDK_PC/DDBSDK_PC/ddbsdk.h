#ifndef DDBSDK_PC_H_
#define DDBSDK_PC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DDBSDK_VERSION   0x00000501

#define DDBSDK_API	__declspec(dllexport)


#define DDBSDK_ERRNO_BASE			0xF1000000
#define DDBSDK_ERRNO_OK				0

#define DDBSDK_ERRNO_POINTER_INVALID		(DDBSDK_ERRNO_BASE + 1)		//输入的指针无效
#define DDBSDK_ERRNO_DEVICE_OPEN_FAIL		(DDBSDK_ERRNO_BASE + 2)		//设备打开失败


typedef unsigned char	U8;
typedef unsigned short	U16;
typedef	unsigned int	U32;

typedef U8		*PByte;
typedef U16		*PWord;
typedef	U32		*PUInt;

typedef void	*Handle;
typedef U32		ErrorNo;


//启动节目后用户获取数据的回调函数
typedef void ( *DDDBSDK_ApiDataCbFunc)(U8*	pBuffer, 
									   U32	BufferSize );

//获取CA信息的回调函数
typedef void (__stdcall * __stdcall ShowCAPromptMessage)(U8* message, 
														 U32 messageLength); 

enum DeviceType{TS_DMB, TS_FILE, TS_SOCKET};

typedef struct _TS_Device_
{
	Handle		deviceHandle;
	DeviceType	deviceType;
	PWord		pram;
	U32			timeout;

}TS_Device;

typedef struct _TS_Service_
{
	Handle		serviceHandle;
	U8			serviceLable[32];
	U8			EnsembleName[32];
	U8			SubChannleId;
	U16			ServiceType;
	U16			BitRateKhz;
	U8			SrvCharSet;

}TS_Service;

typedef struct _TS_Program_
{
	U16		pid;
	U8		type;
	DDDBSDK_ApiDataCbFunc	pDataCallback;
	ShowCAPromptMessage		pCAMessageCallback;

}TS_Program;





DDBSDK_API ErrorNo DDB_StartDevice(TS_Device*	device, 
								   TS_Service*	serivces, 
								   int*			serivcesRetNum);


#ifdef __cplusplus
}
#endif

#endif