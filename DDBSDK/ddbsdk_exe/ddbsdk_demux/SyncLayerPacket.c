#include <stdlib.h>
#include <string.h>

#include "demux_config.h"
#include "demux_if.h"
#include "SyncLayerPacket.h"

typedef struct _BIT_STREAM_
{
	uint8_t * stream;
	int32_t   pos;
} BIT_TS;

uint32_t   get_bits_ts(BIT_TS * bt, int32_t n)
{
	 int32_t   p = n + bt->pos;
	uint32_t   val;

	if(p < 8) {

		val = ((((* bt->stream) << bt->pos) & 0xFF) >> (8 - n));
		bt->pos += n;
		return val;

	} else if(p < 16) {

		uint32_t   buf;
		buf = ((* bt->stream) << 8) + (* (bt->stream + 1));
		val = (((buf << bt->pos) & 0xFFFF) >> (16 - n));
		bt->stream ++;
		bt->pos = p - 8;
		return val;

	} else if(p < 24) {

		uint32_t   buf;
		buf = ((* bt->stream) << 16) + ((* (bt->stream + 1)) << 8) + (* (bt->stream + 2));
		val = (((buf << bt->pos) & 0xFFFFFF) >> (24 - n));
		bt->stream += 2;
		bt->pos = p - 16;
		return val;

	} else if(p < 32) {

		uint32_t   buf;
		buf = ((* bt->stream) << 24) + ((* (bt->stream + 1)) << 16)
			+ ((* (bt->stream + 2)) << 8) + (* (bt->stream + 3));
		val = ((buf << bt->pos) >> (32 - n));
		bt->stream += 3;
		bt->pos = p - 24;
		return val;

	} else {  /* n <= 32 */

		uint32_t   buf;
		buf = ((* bt->stream) << 24) + ((* (bt->stream + 1)) << 16)
			+ ((* (bt->stream + 2)) << 8) + (* (bt->stream + 3));
		val = (buf << bt->pos) >> (32 - n);
		val = val | (bt->stream[4] >> (40 - p));
		bt->stream += 4;
		bt->pos = p - 32;
		return val;
	}

}

void push_14496SL_Pack(
	SLConfig_DR * sl_config_dr,
	  SL_Packet * sl_pack,
	    uint8_t * sl_buf
	)
{
	BIT_TS   bt;

	/* initiate bt */
	bt.pos = 0;
	bt.stream = sl_buf;

	sl_pack->paddingFlag = 0;
	sl_pack->paddingBits = 0;
	if (sl_config_dr->useAccessUnitStartFlag)
		sl_pack->accessUnitStartFlag = get_bits_ts(&bt, 1);
	if (sl_config_dr->useAccessUnitEndFlag)
		sl_pack->accessUnitEndFlag = get_bits_ts(&bt, 1);
	if (sl_config_dr->OCRLength>0)
		sl_pack->OCRflag = get_bits_ts(&bt, 1);
	if (sl_config_dr->useIdleFlag)
		sl_pack->idleFlag = get_bits_ts(&bt, 1);
	if (sl_config_dr->usePaddingFlag)
		sl_pack->paddingFlag = get_bits_ts(&bt, 1);
	if (sl_pack->paddingFlag)
		sl_pack->paddingBits = get_bits_ts(&bt, 3);

	if (!sl_pack->idleFlag && (!sl_pack->paddingFlag || sl_pack->paddingBits!=0)) {
		if (sl_config_dr->packetSeqNumLength > 0) 
			sl_pack->packetSequenceNumber = get_bits_ts(&bt, sl_config_dr->packetSeqNumLength);
		if (sl_config_dr->degradationPriorityLength > 0)
			sl_pack->DegPrioflag = get_bits_ts(&bt, 1);
		if (sl_pack->DegPrioflag && sl_config_dr->degradationPriorityLength)
			sl_pack->degradationPriority = get_bits_ts(&bt, sl_config_dr->degradationPriorityLength);
		if (sl_pack->OCRflag) {
			if(sl_config_dr->OCRLength == 33) {
				get_bits_ts(&bt, 1);
				sl_pack->objectClockReference = get_bits_ts(&bt, 32);
			} else {
				sl_pack->objectClockReference = get_bits_ts(&bt, sl_config_dr->OCRLength);
			}
#if 0
printf("OCR: 0x%x\n", sl_pack->objectClockReference);
#endif
		}
		if (sl_pack->accessUnitStartFlag) {
			if (sl_config_dr->useRandomAccessPointFlag)
				sl_pack->randomAccessPointFlag = get_bits_ts(&bt, 1);;
			if (sl_config_dr->AU_seqNumLength >0)
				sl_pack->AU_sequenceNumber = get_bits_ts(&bt, sl_config_dr->AU_seqNumLength);
			if (sl_config_dr->useTimeStampsFlag) {
				sl_pack->decodingTimeStampFlag = get_bits_ts(&bt, 1);
				sl_pack->compositionTimeStampFlag = get_bits_ts(&bt, 1);
			}
			if (sl_config_dr->instantBitrateLength>0)
				sl_pack->instantBitrateFlag = get_bits_ts(&bt, 1);
			if (sl_pack->decodingTimeStampFlag) {
				if(sl_config_dr->timeStampLength == 33) {
					get_bits_ts(&bt, 1);
					sl_pack->decodingTimeStamp = get_bits_ts(&bt, 32);
				} else {
					sl_pack->decodingTimeStamp = get_bits_ts(&bt, 32);
				}
			}
			if (sl_pack->compositionTimeStampFlag){
				if(sl_config_dr->timeStampLength == 33) {
					get_bits_ts(&bt, 1);
					sl_pack->compositionTimeStamp = get_bits_ts(&bt, 32);
				} else {
					sl_pack->compositionTimeStamp = get_bits_ts(&bt, sl_config_dr->timeStampLength);
				}
			}
			if (sl_config_dr->AU_Length > 0)
				sl_pack->accessUnitLength = get_bits_ts(&bt, sl_config_dr->AU_Length);
			if (sl_pack->instantBitrateFlag)
				sl_pack->instantBitrate = get_bits_ts(&bt, sl_config_dr->instantBitrateLength);
		}
	}
	sl_pack->payload = bt.stream + (bt.pos > 0);
}

uint32_t pushDecoderConfigDescriptor(DecoderConfig_DR * decoder_dr, uint8_t * buf)
{
//	BIT_TS   bt;

	/* initiate bt */
//	bt.pos = 0;
//	bt.stream = buf;

	decoder_dr->DecoderConfig_DRTag = buf[0];  //get_bits_ts(&bt, 8);
	if(decoder_dr->DecoderConfig_DRTag != 0x4) return ERR_SLConfig_Dr;

	decoder_dr->DRsize = buf[1];  //get_bits_ts(&bt, 8);
	decoder_dr->objectTypeIndication = buf[2];  //get_bits_ts(&bt, 8);
	decoder_dr->streamType = (buf[3] & 0xFC) >> 2;  //get_bits_ts(&bt, 6);
	decoder_dr->upStream = (buf[3] & 2) >> 1;  //get_bits_ts(&bt, 1);
	decoder_dr->reserved = buf[3] & 1;  //get_bits_ts(&bt, 1);
	decoder_dr->bufferSizeDB = (buf[4] << 16) + (buf[5] << 8) + buf[6];  //get_bits_ts(&bt, 24);
	decoder_dr->maxBitrate = (buf[7] << 24) + (buf[8] << 16) + (buf[9] << 8) + buf[10];  //get_bits_ts(&bt, 32);
	decoder_dr->avgBitrate = (buf[11] << 24) + (buf[12] << 16) + (buf[13] << 8) + buf[14];  //get_bits_ts(&bt, 32);

	if(decoder_dr->DRsize > 13 && buf[15] == 5){
		decoder_dr->decSpecInfo = &buf[15];
	} else {
		decoder_dr->decSpecInfo = NULL;
	}
	return UNPACK_OK;
}

uint32_t pushSLConfigDescriptor(SLConfig_DR * sl_config_dr, uint8_t * buf)
{
	uint8_t * temp_ptr;
//	BIT_TS   bt;

	/* initiate bt */
//	bt.pos = 0;
//	bt.stream = buf;

	sl_config_dr->SLConfig_DRTag = buf[0];  //get_bits_ts(&bt, 8);
	if(sl_config_dr->SLConfig_DRTag != 0x6) return ERR_SLConfig_Dr;

	sl_config_dr->DRsize = buf[1];  //get_bits_ts(&bt, 8);
	sl_config_dr->Predefined = buf[2];  //get_bits_ts(&bt, 8);
	temp_ptr = &buf[3];
	if (sl_config_dr->Predefined == 0) {
		sl_config_dr->useAccessUnitStartFlag = (buf[3] >> 7) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->useAccessUnitEndFlag = (buf[3] >> 6) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->useRandomAccessPointFlag = (buf[3] >> 5) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->hasRandomAccessUnitsOnlyFlag = (buf[3] >> 4) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->usePaddingFlag = (buf[3] >> 3) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->useTimeStampsFlag = (buf[3] >> 2) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->useIdleFlag = (buf[3] >> 1) & 1;  //get_bits_ts(&bt, 1);
		sl_config_dr->durationFlag = buf[3] & 1;  //get_bits_ts(&bt, 1);

		sl_config_dr->timeStampResolution = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];  //get_bits_ts(&bt, 32);
		sl_config_dr->OCRResolution = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];  //get_bits_ts(&bt, 32);

		sl_config_dr->timeStampLength = buf[12];  //get_bits_ts(&bt, 8); // must be ¡Ü 64
		sl_config_dr->OCRLength = buf[13];  //get_bits_ts(&bt, 8); // must be ¡Ü 64
		sl_config_dr->AU_Length = buf[14];  //get_bits_ts(&bt, 8); // must be ¡Ü 32
		sl_config_dr->instantBitrateLength = buf[15];  //get_bits_ts(&bt, 8);
		sl_config_dr->degradationPriorityLength = (buf[16] >> 4) & 0xF;  //get_bits_ts(&bt, 4);
		sl_config_dr->AU_seqNumLength = ((buf[16] & 0xF) << 1) + ((buf[17] >> 7) & 1);  //get_bits_ts(&bt, 5); // must be ¡Ü 16
		sl_config_dr->packetSeqNumLength = (buf[17] >> 2) & 0x1F;  //get_bits_ts(&bt, 5); // must be ¡Ü 16
		sl_config_dr->reserved = buf[17] & 3;  //get_bits_ts(&bt, 2);
		temp_ptr = &buf[18];
	}
	if (sl_config_dr->durationFlag) {
		sl_config_dr->timeScale = (temp_ptr[0] << 24) | (temp_ptr[1] << 16) | (temp_ptr[2] << 8) | temp_ptr[3];  //get_bits_ts(&bt, 32);
		sl_config_dr->accessUnitDuration = (temp_ptr[4] << 8) | temp_ptr[5];  //get_bits_ts(&bt, 16);
		sl_config_dr->compositionUnitDuration = (temp_ptr[6] << 8) | temp_ptr[7];  //get_bits_ts(&bt, 16);
		temp_ptr = &temp_ptr[8];
	}
	if (!sl_config_dr->useTimeStampsFlag) {
//		sl_config_dr->startDecodingTimeStamp = get_bits_ts(&bt, sl_config_dr->timeStampLength);
//		sl_config_dr->startCompositionTimeStamp = get_bits_ts(&bt, sl_config_dr->timeStampLength);
	}
	return UNPACK_OK;
}

uint32_t pushESDescriptor(ES_DR * es_dr, uint8_t * buf)
{
//	BIT_TS   bt;

	/* initiate bt */
//	bt.pos = 0;
//	bt.stream = buf;

	uint8_t * buf_temp;
	uint32_t   err_val;

	es_dr->ES_DRTag = buf[0];  //get_bits_ts(&bt, 8);
	if(es_dr->ES_DRTag != 0x3) return ERR_ES_Dr; 

	es_dr->DRsize = buf[1];  //get_bits_ts(&bt, 8);
	es_dr->ES_ID = (buf[2] << 8) + buf[3];  //get_bits_ts(&bt, 16);
	es_dr->streamDependencyFlag = (buf[4] >> 7) & 1;  //get_bits_ts(&bt, 1);
	es_dr->URL_Flag = (buf[4] >> 6) & 1;  //get_bits_ts(&bt, 1);
	es_dr->OCRstreamFlag = (buf[4] >> 5) & 1;  //get_bits_ts(&bt, 1);
	es_dr->streamPriority = buf[4] & 0x1F;  //get_bits_ts(&bt, 5);

	buf_temp = &buf[5];
	if(es_dr->OCRstreamFlag) {
		es_dr->OCR_ES_ID = (buf_temp[0] << 8) + buf_temp[1];  //get_bits_ts(&bt, 16);
		buf_temp += 2;
	}
	err_val = pushDecoderConfigDescriptor(&es_dr->decoder_dr, &buf_temp[0]);
	err_val += pushSLConfigDescriptor(&es_dr->sl_config_dr, &buf_temp[es_dr->decoder_dr.DRsize + 2]);

	return err_val == UNPACK_OK ? UNPACK_OK : err_val | ERR_ES_Dr;
}

uint32_t InitialObjectDescriptor(InitialObject_DR * iod_dr, uint8_t * buf)
{
//	BIT_TS   bt;

	/* initiate bt */
//	bt.pos = 0;
//	bt.stream = buf;

	uint32_t   err_val;

	iod_dr->InitialObject_DRTag = buf[0];  //get_bits_ts(&bt, 8);
	if(iod_dr->InitialObject_DRTag != 0x2) return ERR_InitialObject_Dr;

	iod_dr->DRsize = buf[1];  //get_bits_ts(&bt, 8);

	iod_dr->OD_ID =  (buf[2] << 2) + (buf[3] >> 6);  //get_bits_ts(&bt, 10);
	iod_dr->URL_Flag =  (buf[3] >> 5) & 1;  //get_bits_ts(&bt, 1);
	iod_dr->includeInlineProfileLevelFlag =  (buf[3] >> 4) & 1;  //get_bits_ts(&bt, 1);
	iod_dr->Reserved =  buf[3] & 0xF;  //get_bits_ts(&bt, 4);
	if (! iod_dr->URL_Flag) {

		iod_dr->ODProfileLevelIndication =  buf[4];  //get_bits_ts(&bt, 8);
		iod_dr->sceneProfileLevelIndication =  buf[5];  //get_bits_ts(&bt, 8);
		iod_dr->audioProfileLevelIndication =  buf[6];  //get_bits_ts(&bt, 8);
		iod_dr->visualProfileLevelIndication =  buf[7];  //get_bits_ts(&bt, 8);
		iod_dr->graphicsProfileLevelIndication =  buf[8];  //get_bits_ts(&bt, 8);

		/* ES Descriptor of OD,  ES_ID: 0x0002 */
		err_val = pushESDescriptor(&iod_dr->es_dr[0], &buf[9]);

		/*ES Descriptor of BIFS*/
		err_val += pushESDescriptor(&iod_dr->es_dr[1], &buf[9 + iod_dr->es_dr[0].DRsize + 2]);
	}

	return err_val == UNPACK_OK ? UNPACK_OK : err_val | ERR_InitialObject_Dr;
}

void SceneDescriptor()
{

}

uint32_t ObjectDescriptor(Object_DR * od_dr, uint8_t * buf)
{
//	BIT_TS   bt;

	/* initiate bt */
//	bt.pos = 0;
//	bt.stream = buf;
	uint32_t   err_val;

	od_dr->object_DRTag = buf[0];  //get_bits_ts(&bt, 8);
	if(od_dr->object_DRTag != 0x1) return ERR_Object_Dr;

	od_dr->DRsize = buf[1];  //get_bits_ts(&bt, 8);
	od_dr->object_DRID = (buf[2] << 2) + (buf[3] >> 6);  //get_bits_ts(&bt, 10);
	od_dr->URL_flag =  (buf[3] >> 5) & 1;  //get_bits_ts(&bt, 1);
	od_dr->reserved =  buf[3] & 0x1F;  //get_bits_ts(&bt, 5);

	if(! od_dr->URL_flag) {
	   err_val = pushESDescriptor(&od_dr->es_dr, &buf[4]);
	}
	return err_val == UNPACK_OK ? UNPACK_OK : err_val | ERR_Object_Dr;
}

uint32_t push_avcDecoderSpecInfo(avcDecoderSpecInfo * avc_info, uint8_t * buf)
{
	int32_t   length = buf[1], i;
	uint8_t * temp_ptr;

	if(buf[0] != 0x5) return ERR_DecSpecInfo_Dr;
	avc_info->configureationVersion = buf[2]; /* 8, =1(const) */
	avc_info->AVCprofileIndication = buf[3];  /* 8 */
	avc_info->profile_compatibility = buf[4]; /* 8 */
	avc_info->AVClevelIndication = buf[5];    /* 8 */
	avc_info->reserved1 = buf[6] >> 2;             /* 6 */
	avc_info->lengthSizeMinusOne = buf[6] & 3;    /* 2 */
	avc_info->reserved2 = buf[7] >> 5;             /* 3 */
	avc_info->numOfSequenceParameterSets = buf[7] & 0x1F;  /* 5 */

	temp_ptr = &buf[8];
	for (i=0; i< avc_info->numOfSequenceParameterSets; i++) {
		avc_info->sequenceParameterLength = (temp_ptr[0] << 8) | temp_ptr[1];    /* 16 */
		avc_info->sps_ptr = &temp_ptr[2];
		temp_ptr += avc_info->sequenceParameterLength + 2;
	}
	avc_info->numOfPictureParamterSets = temp_ptr[0];   /* 8 */
	temp_ptr += 1;
	for (i=0; i< avc_info->numOfPictureParamterSets; i++) {
		avc_info->pictureParameterSetLength = (temp_ptr[0] << 8) | temp_ptr[1];    /* 16 */
		avc_info->pps_ptr = &temp_ptr[2];
		temp_ptr += avc_info->numOfPictureParamterSets + 2;
	}
	return UNPACK_OK;
}

uint32_t push_aacDecoderSpecInfo(aacDecoderSpecInfo * aac_info, uint8_t * buf)
{
	int32_t   length = buf[1];
	uint8_t * temp_ptr;
	int32_t   sample_freq[15] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 
		22050, 16000, 12000, 11025, 8000, 7350, 0, 0};

	if(buf[0] != 0x5) return ERR_DecSpecInfo_Dr;
	aac_info->audioObjectType = buf[2] >> 3;  /* 5 */
	aac_info->samplingFrequencyIndex = ((buf[2] & 7) << 1) | (buf[3] >> 7);  /* 4 */
	temp_ptr = &buf[3];

	if(aac_info->samplingFrequencyIndex == 0xf) {
		aac_info->samplingFrequency = ((temp_ptr[0] & 0x7F) << 17)  | (temp_ptr[1] << 9) | (temp_ptr[2] << 1) | (temp_ptr[3] >> 7);
		temp_ptr += 3;
	} else {
		aac_info->samplingFrequency = sample_freq[aac_info->samplingFrequencyIndex];
	}
	aac_info->channelConfiguration = (temp_ptr[0] >> 3) & 0xF;
	aac_info->sbrPresentFlag = -1;
	if ( aac_info->audioObjectType == 5 ) {
		aac_info->extensionAudioObjectType = aac_info->audioObjectType;
		aac_info->sbrPresentFlag = 1;
		aac_info->extensionSamplingFrequencyIndex = ((temp_ptr[0] & 7) << 1) | (temp_ptr[1] >> 7);
		if ( aac_info->extensionSamplingFrequencyIndex == 0xf ) {
			aac_info->extensionSamplingFrequency = 0;
		} else {
			aac_info->extensionSamplingFrequency = sample_freq[aac_info->extensionSamplingFrequency];
		}
		aac_info->audioObjectType = (temp_ptr[1] >> 2) & 0x1F;
	} else {
		aac_info->extensionAudioObjectType = 0;
	}
	return UNPACK_OK;
}

#define  SCENE_DESCRIPTOR   4
#define  OBJECT_DESCRIPTOR  5
uint32_t pushISO_IEC_14496_section(dvb_program_property_t * param1, uint8_t * buf)
{
	 int8_t   table_id;
	int32_t   section_length;
	SL_Packet sl_pack_od;

	InitialObject_DR * iod_dr = &param1->iod_dr;
	       Object_DR * object_dr0 = &param1->object_dr[0];
	       Object_DR * object_dr1 = &param1->object_dr[1];

	uint8_t   ObjectDescriptorUpdateTag;
	uint8_t   ObjectDescriptorUpdateSize;
	uint8_t * temp_ptr;

	uint32_t   err_val;

	table_id = buf[5];
	section_length = ((buf[6] & 0x0F) << 8) + buf[7];

	if(table_id == SCENE_DESCRIPTOR) {

		SceneDescriptor();

	} else if(table_id == OBJECT_DESCRIPTOR) {
		/* 1. push SL packet header */
		memset(&sl_pack_od, 0, sizeof(SL_Packet));
		push_14496SL_Pack(&iod_dr->es_dr[param1->od_es_num_in_iod].sl_config_dr, &sl_pack_od, &buf[13]);

		temp_ptr = sl_pack_od.payload;
		ObjectDescriptorUpdateTag = temp_ptr[0];  //get_bits_ts(&bt, 8);
		ObjectDescriptorUpdateSize = temp_ptr[1];  //get_bits_ts(&bt, 8);

		/* 2. push OD (video) */
		if((err_val = ObjectDescriptor(object_dr0, &temp_ptr[2])) != UNPACK_OK) return err_val;

		/* 3. push OD (audio) */
		if((err_val = ObjectDescriptor(object_dr1, &temp_ptr[2 + object_dr0->DRsize + 2])) != UNPACK_OK) return err_val;

		/* 4. set the video and audio object descriptor pointer */
		if( object_dr0->es_dr.decoder_dr.streamType == 0x4) {  //visual stream
			param1->object_dr_video = object_dr0;
		} else if( object_dr0->es_dr.decoder_dr.streamType == 0x5) {
			param1->object_dr_audio = object_dr0;
		}
		if( object_dr1->es_dr.decoder_dr.streamType == 0x4) {  //visual stream
			param1->object_dr_video = object_dr1;
		} else if( object_dr1->es_dr.decoder_dr.streamType == 0x5) {
			param1->object_dr_audio = object_dr1;
		}

		/* 5. parse the video and audio decoder spec info descriptor */
		push_avcDecoderSpecInfo(&param1->AVC_info, param1->object_dr_video->es_dr.decoder_dr.decSpecInfo);
		push_aacDecoderSpecInfo(&param1->AAC_info, param1->object_dr_audio->es_dr.decoder_dr.decSpecInfo);
	}

	return UNPACK_OK;
}

