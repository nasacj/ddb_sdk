
#ifndef _SYNCLAYERPACKET_
#define _SYNCLAYERPACKET_

typedef struct _CA_DES_SPEC_INFO_FORPMT_
{
	uint8_t		ca_des_tag;
	uint8_t		des_length;
	uint16_t	ca_system_id;
	uint16_t	ca_pid;
}Initial_CA_DR;


typedef struct _AVC_DEC_SPEC_INFO_
{
	uint8_t   configureationVersion; /* 8, =1(const) */
	uint8_t   AVCprofileIndication;  /* 8 */
	uint8_t   profile_compatibility; /* 8 */
	uint8_t   AVClevelIndication;    /* 8 */
	 int8_t   reserved1;             /* 6 */
	 int8_t   lengthSizeMinusOne;    /* 2 */
	 int8_t   reserved2;             /* 3 */

	  int8_t   numOfSequenceParameterSets;  /* 5 */
	uint16_t   sequenceParameterLength;    /* 16 */
	 uint8_t * sps_ptr;

	 uint8_t   numOfPictureParamterSets;   /* 8 */
	uint16_t   pictureParameterSetLength;    /* 16 */
	 uint8_t * pps_ptr;
}avcDecoderSpecInfo;

typedef struct _AAC_DEC_SPEC_INFO_
{
	  int8_t   audioObjectType;  /* 5 */
	  int8_t   samplingFrequencyIndex;  /* 4 */
	 int32_t   samplingFrequency;  /* 24 */
	  int8_t   channelConfiguration; /* 4 */
	  int8_t   sbrPresentFlag;
	  int8_t   extensionAudioObjectType;
	  int8_t   extensionSamplingFrequencyIndex; /* 4  */
	 int32_t   extensionSamplingFrequency;  /* 24 */
}aacDecoderSpecInfo;

typedef struct _DECODE_CONFIG_DR_
{
	 uint8_t   DecoderConfig_DRTag;   /* 8 */
	 uint8_t   DRsize;   /* 8 */
	 uint8_t   objectTypeIndication;   /* 8 */
	 uint8_t   streamType;   /* 6 */
	  int8_t   upStream;     /* 1 */
	  int8_t   reserved;   /* 1 */
	 int32_t   bufferSizeDB;   /* 24 */
	uint32_t   maxBitrate;   /* 32 */
	uint32_t   avgBitrate;   /* 32 */

	 uint8_t * decSpecInfo;
}DecoderConfig_DR;

typedef struct _SL_CONFIG_DR_
{
	 uint8_t   SLConfig_DRTag;   /* 8 */
	 uint8_t   DRsize;   /* 8 */
	 uint8_t   Predefined;   /* 8 */
	  int8_t   useAccessUnitStartFlag;   /* 1 */
	  int8_t   useAccessUnitEndFlag;   /* 1 */
	  int8_t   useRandomAccessPointFlag;   /* 1 */
	  int8_t   hasRandomAccessUnitsOnlyFlag;   /* 1 */
	  int8_t   usePaddingFlag;   /* 1 */
	  int8_t   useTimeStampsFlag;   /* 1 */
	  int8_t   useIdleFlag;   /* 1 */
	  int8_t   durationFlag;   /* 1 */
	uint32_t   timeStampResolution;   /* 32 */
	uint32_t   OCRResolution;   /* 32 */
	 uint8_t   timeStampLength;   /* 8 */
	 uint8_t   OCRLength;   /* 8 */
	 uint8_t   AU_Length;   /* 8 */
	 uint8_t   instantBitrateLength;   /* 8 */
	  int8_t   degradationPriorityLength;   /* 4 */
	  int8_t   AU_seqNumLength;   /* 5 */
	  int8_t   packetSeqNumLength;   /* 5 */
	  int8_t   reserved;   /* 2 */

	uint32_t   timeScale;   /* 32 */
	uint16_t   accessUnitDuration;   /* 16 */
	uint16_t   compositionUnitDuration;   /* 16 */

	uint32_t   startDecodingTimeStamp;  /* timeStampLength */
	uint32_t   startCompositionTimeStamp;  /* timeStampLength */

}SLConfig_DR;

typedef struct _Element_STREAM_DR_
{
	 uint8_t   ES_DRTag;   /* 8 */
	 uint8_t   DRsize;     /* 8 */
	uint16_t   ES_ID;      /* 16 */
	  int8_t   streamDependencyFlag;   /* 1 */
	  int8_t   URL_Flag;   /* 1 */
	  int8_t   OCRstreamFlag;    /* 1 */
	  int8_t   streamPriority;   /* 5 */
	uint16_t   OCR_ES_ID;  /* 16 */

	DecoderConfig_DR   decoder_dr;
	     SLConfig_DR   sl_config_dr;
}ES_DR;

typedef struct _INITIAL_OBJECT_DR_
{
	 uint8_t   InitialObject_DRTag;   /* 8 */
	uint16_t   DRsize;   /* 16 */
	 int16_t   OD_ID;   /* 10 */
	  int8_t   URL_Flag;   /* 1 */
	  int8_t   includeInlineProfileLevelFlag;   /* 1 */
	  int8_t   Reserved;   /* 4 */
	 uint8_t   ODProfileLevelIndication;   /* 8 */
	 uint8_t   sceneProfileLevelIndication;   /* 8 */
	 uint8_t   audioProfileLevelIndication;   /* 8 */
	 uint8_t   visualProfileLevelIndication;   /* 8 */
	 uint8_t   graphicsProfileLevelIndication;   /* 8 */

	   ES_DR   es_dr[2];
}InitialObject_DR;


typedef struct _OBJECT_DR_
{
	 uint8_t   object_DRTag;
	uint16_t   DRsize;
	 int16_t   object_DRID;
	  int8_t   URL_flag;
	  int8_t   reserved;

	   ES_DR   es_dr;
}Object_DR;

typedef struct _SL_PACKET_
{
	int8_t   accessUnitStartFlag;  /* bit(1) */
	int8_t   accessUnitEndFlag;  /* bit(1) */
	int8_t   OCRflag;  /* bit(1) */
	int8_t   idleFlag;  /* bit(1) */
	int8_t   paddingFlag;  /* bit(1) */
	int8_t   paddingBits;  /* bit(3) */

	int32_t  packetSequenceNumber;  /* bit(SL.packetSeqNumLength) */
	int8_t   DegPrioflag;  /* bit(1) */
	int8_t   degradationPriority;  /* bit(SL.degradationPriorityLength) */
	int8_t   objectClockReference_HIGH;
	uint32_t objectClockReference;  /* bit(SL.OCRLength) */

	int8_t   randomAccessPointFlag;  /* bit(1) */
	int32_t  AU_sequenceNumber;  /* bit(SL.AU_seqNumLength) */
	int8_t   decodingTimeStampFlag;  /* bit(1) */
	int8_t   compositionTimeStampFlag;  /* bit(1) */

	int8_t   instantBitrateFlag;  /* bit(1) */
	int8_t   decodingTimeStamp_HIGH;
	uint32_t decodingTimeStamp;  /* bit(SL.timeStampLength) */
	int8_t   compositionTimeStamp_HIGH;
	uint32_t compositionTimeStamp;  /* bit(SL.timeStampLength) */
	int32_t  accessUnitLength;  /* bit(SL.AU_Length) */
	int32_t  instantBitrate;  /* bit(SL.instantBitrateLength) */

	uint8_t * payload;
}SL_Packet;

uint32_t InitialObjectDescriptor(void * iod_dr, uint8_t * buf);
uint32_t pushISO_IEC_14496_section(void * param1, uint8_t * buf);
void push_14496SL_Pack(SLConfig_DR * sl_config_dr, SL_Packet * sl_pack, uint8_t * sl_buf);
#endif
