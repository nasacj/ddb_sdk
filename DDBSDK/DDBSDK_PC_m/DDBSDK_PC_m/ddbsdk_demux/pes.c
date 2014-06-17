#include <string.h>

#include "demux_config.h"
#include "demux_if.h"
#include "SyncLayerPacket.h"

#define PROGRAM_STREAM_MAP  0xBC
#define PRIVATE_STREAM_1    0xBD
#define PADDING_STREAM      0xBE
#define PRIVATE_STREAM_2    0xBF
#define MPEG_AUDIO          0xC0
#define MPEG_VIDEO          0xE0
#define ECM_STREAM          0xF0
#define EMM_STREAM          0xF1
#define DSM_CC_STREAM       0xF2
#define ISO_13522_STREAM    0xF3

#define SEC14496_SL_PACKED_STREAM   0xFA
#define PROGRAM_STREAM_DIRECTORY    0xFF

typedef struct dvbpes_packet_s
{
	uint16_t pes_packet_length;
	
	int8_t   pes_scrambling_control;
	int8_t   pes_priority;
	int8_t   data_alignment_indicator;
	int8_t   copyright;
	int8_t   original_or_copy;
	int8_t   pts_dts_flag;
	int8_t   escr_flag;
	int8_t   es_rate_flag;
	int8_t   dsm_trick_mode_flag;
	int8_t   additional_copy_info_flag;
	int8_t   pes_crc_flag;
	int8_t   pes_extention_flag;
	uint8_t  pes_header_data_length;

	int8_t   f_pes_first_pack;
	int8_t   pts_high;
	int8_t   dts_high;
	uint32_t pts_low;
	uint32_t dts_low;

} dvbpes_packet_t;

uint32_t pushPacketVideoPES(demux_if_t * param1, uint8_t * buf)
{
	SL_Packet   sl_pack;
	dvbpes_packet_t   pes_pack;

	uint8_t *  temp_ptr = buf;
	uint8_t *  pes_end_ptr = param1->dvb_video_ptr_e;
	uint8_t *  pes_start_ptr = param1->dvb_video_ptr_s;
	uint8_t ** pes_buf = &param1->dvb_video_ptr;
	uint8_t    stream_id;
	int32_t    pack_copy_len, pack_copy_len1;
	int32_t    adap_field_length = 0;
	int32_t    pes_buf_size;

	if(buf[0] != 0x47) return PACK_ERR;
	memset(&pes_pack, 0, sizeof(dvbpes_packet_t));
	memset(&sl_pack, 0, sizeof(SL_Packet));

	//parse TS packet header
	if((uint8_t)(buf[1] & 0x40) == 0x40) {
		pes_pack.f_pes_first_pack = 1;
	} else{
		pes_pack.f_pes_first_pack = 0;
	}

	temp_ptr = &buf[4];
	//parse adaptation_field 
	if((buf[3] & 0x30) == 0x30) {

		adap_field_length = buf[4];
		temp_ptr += adap_field_length + 1;
	} else if((buf[3] & 0x30) == 0x20){

		return UNPACK_OK;
	}

	if (pes_pack.f_pes_first_pack == 1) {  //for PES start PACK

		//packet start code prefix
		if(((temp_ptr[0] << 16) | (temp_ptr[1] << 8) | temp_ptr[2]) != 0x1) return PACK_ERR; 
		//stream_id
		stream_id = temp_ptr[3];
		//pes packet length
		pes_pack.pes_packet_length = (temp_ptr[4] << 8) | temp_ptr[5];

		switch (stream_id) {
		case PROGRAM_STREAM_MAP:
		case PRIVATE_STREAM_2:
		case ECM_STREAM:
		case EMM_STREAM:
		case PROGRAM_STREAM_DIRECTORY:
			break;
		case PADDING_STREAM:
		
			break;
		default:
			if((stream_id >= 0xF4 && stream_id <= 0xFE) || stream_id < 0xBC) {

				/* new_support for 14496-1 */
				if(stream_id == SEC14496_SL_PACKED_STREAM) {

					pes_pack.pes_scrambling_control = (temp_ptr[6] & 0x30) >> 4;
					pes_pack.pes_priority = (temp_ptr[6] & 0x08) >> 3;
					pes_pack.data_alignment_indicator = (temp_ptr[6] & 0x04) >> 2;
					pes_pack.copyright = (temp_ptr[6] & 0x02) >> 1;
					pes_pack.original_or_copy = temp_ptr[6] & 0x01;
					pes_pack.pts_dts_flag = (temp_ptr[7] & 0xC0) >> 6;
					pes_pack.escr_flag = (temp_ptr[7] & 0x20) >> 5;
					pes_pack.es_rate_flag = (temp_ptr[7] & 0x10) >> 4;
					pes_pack.dsm_trick_mode_flag = (temp_ptr[7] & 0x08) >> 3;
					pes_pack.additional_copy_info_flag = (temp_ptr[7] & 0x04) >> 2;
					pes_pack.pes_crc_flag = (temp_ptr[7] & 0x02) >> 1;
					pes_pack.pes_extention_flag = temp_ptr[7] & 0x01;
					pes_pack.pes_header_data_length = temp_ptr[8];
					temp_ptr += 8 + pes_pack.pes_header_data_length + 1;

					push_14496SL_Pack(&param1->program_inf[param1->cur_prog_num].object_dr_video->es_dr.sl_config_dr,
						&sl_pack, temp_ptr);
					temp_ptr = sl_pack.payload;
					param1->program_inf[param1->cur_prog_num].cur_ves_length = 
						(temp_ptr[0] << 24) | (temp_ptr[1] << 16) | (temp_ptr[2] << 8) | temp_ptr[3];

					param1->pts_video = sl_pack.compositionTimeStamp;
					temp_ptr += 4;
					pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);

					pes_buf_size = pes_end_ptr - pes_start_ptr;
					if((* pes_buf) + pack_copy_len <= pes_end_ptr) {
#if 1
(* pes_buf)[0] = 0;
(* pes_buf)[1] = 0;
(* pes_buf)[2] = 0;
(* pes_buf)[3] = 1;
(* pes_buf) += 4;
#endif
						memcpy((* pes_buf), temp_ptr, pack_copy_len);
						(* pes_buf) += pack_copy_len;
					} else {
						pack_copy_len1 = pes_end_ptr - (* pes_buf);

						memcpy((* pes_buf), temp_ptr, pack_copy_len1);
						memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
						(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
					}
				}
			} else {

				pes_pack.pes_scrambling_control = (temp_ptr[6] & 0x30) >> 4;
				pes_pack.pes_priority = (temp_ptr[6] & 0x08) >> 3;
				pes_pack.data_alignment_indicator = (temp_ptr[6] & 0x04) >> 2;
				pes_pack.copyright = (temp_ptr[6] & 0x02) >> 1;
				pes_pack.original_or_copy = temp_ptr[6] & 0x01;
				pes_pack.pts_dts_flag = (temp_ptr[7] & 0xC0) >> 6;
				pes_pack.escr_flag = (temp_ptr[7] & 0x20) >> 5;
				pes_pack.es_rate_flag = (temp_ptr[7] & 0x10) >> 4;
				pes_pack.dsm_trick_mode_flag = (temp_ptr[7] & 0x08) >> 3;
				pes_pack.additional_copy_info_flag = (temp_ptr[7] & 0x04) >> 2;
				pes_pack.pes_crc_flag = (temp_ptr[7] & 0x02) >> 1;
				pes_pack.pes_extention_flag = temp_ptr[7] & 0x01;
				pes_pack.pes_header_data_length = temp_ptr[8];

				if(pes_pack.pts_dts_flag == 2) {

					pes_pack.pts_high = temp_ptr[9] & 0x08;
					pes_pack.pts_low  = ((temp_ptr[9] >> 1) << 30) | (temp_ptr[10] << 22) |
						((temp_ptr[11] >> 1) << 15) | (temp_ptr[12] << 7) | (temp_ptr[13] >> 1);
					pes_pack.dts_high = (int8_t)0xFF;
					pes_pack.dts_low = 0xFFFFFFFF;

				} else if (pes_pack.pts_dts_flag == 3){

					pes_pack.pts_high = temp_ptr[9] & 0x08;
					pes_pack.pts_low  = ((temp_ptr[9] >> 1) << 30) | (temp_ptr[10] << 22) |
						((temp_ptr[11] >> 1) << 15) | (temp_ptr[12] << 7) | (temp_ptr[13] >> 1);
					pes_pack.dts_high = temp_ptr[14] & 0x08;
					pes_pack.dts_low  = ((temp_ptr[15] >> 1) << 30) | (temp_ptr[16] << 22) |
						((temp_ptr[17] >> 1) << 15) | (temp_ptr[18] << 7) | (temp_ptr[19] >> 1);

				} else if (pes_pack.pts_dts_flag == 0) {

					pes_pack.pts_high = (int8_t)0xFF;
					pes_pack.pts_low = 0xFFFFFFFF;
					pes_pack.dts_high = (int8_t)0xFF;
					pes_pack.dts_low = 0xFFFFFFFF;
				}

				param1->pts_video = pes_pack.pts_low;
				if(pes_pack.escr_flag == 1) return PACK_ERR;
				if(pes_pack.es_rate_flag == 1) return PACK_ERR;
				if(pes_pack.dsm_trick_mode_flag == 1) return PACK_ERR;
				if(pes_pack.additional_copy_info_flag == 1) return PACK_ERR;
				if(pes_pack.pes_crc_flag == 1) return PACK_ERR;
				if(pes_pack.pes_extention_flag == 1) return PACK_ERR;

				temp_ptr = temp_ptr + 9 + pes_pack.pes_header_data_length;
				pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);

				//add PTS and DTS at the begin of Video or Audio stream
				pes_buf_size = pes_end_ptr - pes_start_ptr;

				if((* pes_buf) + pack_copy_len <= pes_end_ptr) {

					memcpy((* pes_buf), temp_ptr, pack_copy_len);
					(* pes_buf) += pack_copy_len;
				} else {
					pack_copy_len1 = pes_end_ptr - (* pes_buf);

					memcpy(pes_buf, temp_ptr, pack_copy_len1);
					memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
					(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
				}
			}
		}

	} else {

		pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);
		if((uint32_t)((* pes_buf) + pack_copy_len) <= (uint32_t)pes_end_ptr) {

			memcpy((* pes_buf), temp_ptr, pack_copy_len);
			(* pes_buf) += pack_copy_len;

		} else {

			pack_copy_len1 = pes_end_ptr - (* pes_buf);

			memcpy((* pes_buf), temp_ptr, pack_copy_len1);
			memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
			(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
		}
	}

	return UNPACK_OK;
}

uint32_t pushPacketAudioPES(demux_if_t * param1, uint8_t * buf)
{
	SL_Packet   sl_pack;
	dvbpes_packet_t   pes_pack;

	uint8_t * temp_ptr = buf;
	uint8_t * pes_end_ptr = param1->dvb_audio_ptr_e;
	uint8_t * pes_start_ptr = param1->dvb_audio_ptr_s;
	uint8_t **pes_buf = &param1->dvb_audio_ptr;
	uint8_t   stream_id;
	int32_t   pack_copy_len, pack_copy_len1;
	int32_t   adap_field_length = 0;
	int32_t   pes_buf_size;

	if(buf[0] != 0x47) return PACK_ERR;
	memset(&pes_pack, 0, sizeof(dvbpes_packet_t));
	memset(&sl_pack, 0, sizeof(SL_Packet));

	//parse TS packet header
	if((uint8_t)(buf[1] & 0x40) == 0x40) {
		pes_pack.f_pes_first_pack = 1;
	} else{
		pes_pack.f_pes_first_pack = 0;
	}

	temp_ptr = &buf[4];
	//parse adaptation_field 
	if((buf[3] & 0x30) == 0x30) {
#if 0
{
if(buf[4] > 0 && ((buf[5] & 0x10) > 0)) {
unsigned int G_PCR_Base = (buf[6] << 25) | (buf[7] << 17) | 
		(buf[8] << 9) | (buf[9] << 1) | (buf[10] >> 7);
printf("PCR: 0x%x\n", G_PCR_Base);
}
}
#endif
		adap_field_length = buf[4];
		temp_ptr += adap_field_length + 1;
	} else if((buf[3] & 0x30) == 0x20){

		return UNPACK_OK;
	}

	if (pes_pack.f_pes_first_pack == 1) {  //for PES start PACK

		//packet start code prefix
		if(((temp_ptr[0] << 16) | (temp_ptr[1] << 8) | temp_ptr[2]) != 0x1) return PACK_ERR; 
		//stream_id
		stream_id = temp_ptr[3];
		//pes packet length
		pes_pack.pes_packet_length = (temp_ptr[4] << 8) | temp_ptr[5];

		switch (stream_id) {
		case PROGRAM_STREAM_MAP:
		case PRIVATE_STREAM_2:
		case ECM_STREAM:
		case EMM_STREAM:
		case PROGRAM_STREAM_DIRECTORY:
			break;
		case PADDING_STREAM:
		
			break;
		default:
			if((stream_id >= 0xF4 && stream_id <= 0xFE) || stream_id < 0xBC) {

				/* new_support for 14496-1 */
				if(stream_id == SEC14496_SL_PACKED_STREAM) {

					pes_pack.pes_scrambling_control = (temp_ptr[6] & 0x30) >> 4;
					pes_pack.pes_priority = (temp_ptr[6] & 0x08) >> 3;
					pes_pack.data_alignment_indicator = (temp_ptr[6] & 0x04) >> 2;
					pes_pack.copyright = (temp_ptr[6] & 0x02) >> 1;
					pes_pack.original_or_copy = temp_ptr[6] & 0x01;
					pes_pack.pts_dts_flag = (temp_ptr[7] & 0xC0) >> 6;
					pes_pack.escr_flag = (temp_ptr[7] & 0x20) >> 5;
					pes_pack.es_rate_flag = (temp_ptr[7] & 0x10) >> 4;
					pes_pack.dsm_trick_mode_flag = (temp_ptr[7] & 0x08) >> 3;
					pes_pack.additional_copy_info_flag = (temp_ptr[7] & 0x04) >> 2;
					pes_pack.pes_crc_flag = (temp_ptr[7] & 0x02) >> 1;
					pes_pack.pes_extention_flag = temp_ptr[7] & 0x01;
					pes_pack.pes_header_data_length = temp_ptr[8];
					temp_ptr += 8 + pes_pack.pes_header_data_length + 1;

					push_14496SL_Pack(&param1->program_inf[param1->cur_prog_num].object_dr_audio->es_dr.sl_config_dr,
						&sl_pack, temp_ptr);
					temp_ptr = sl_pack.payload;
					pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);

					pes_buf_size = pes_end_ptr - pes_start_ptr;
					if((* pes_buf) + pack_copy_len <= pes_end_ptr) {
#if 1
/* add ADTS header, AAC PLUS */
(* pes_buf)[0] = 0xFF;
(* pes_buf)[1] = 0xF9;
(* pes_buf)[2] = 0x58;
(* pes_buf)[3] = 0x80;
(* pes_buf)[4] = 0x13;
(* pes_buf)[5] = 0x1F;
(* pes_buf)[6] = 0xFC;
(* pes_buf) += 7;
#endif
#if 0
/* add ADTS header, AAC BSAC */
(* pes_buf)[0] = 0xFF;
(* pes_buf)[1] = 0xF1;
(* pes_buf)[2] = 0x58;
(* pes_buf)[3] = 0x80;
(* pes_buf)[4] = 0x13;
(* pes_buf)[5] = 0x1F;
(* pes_buf)[6] = 0xFC;
(* pes_buf) += 7;
#endif
						memcpy((* pes_buf), temp_ptr, pack_copy_len);
						(* pes_buf) += pack_copy_len;
					} else {
						pack_copy_len1 = pes_end_ptr - (* pes_buf);

						memcpy((* pes_buf), temp_ptr, pack_copy_len1);
						memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
						(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
					}
				}
			} else {

				pes_pack.pes_scrambling_control = (temp_ptr[6] & 0x30) >> 4;
				pes_pack.pes_priority = (temp_ptr[6] & 0x08) >> 3;
				pes_pack.data_alignment_indicator = (temp_ptr[6] & 0x04) >> 2;
				pes_pack.copyright = (temp_ptr[6] & 0x02) >> 1;
				pes_pack.original_or_copy = temp_ptr[6] & 0x01;
				pes_pack.pts_dts_flag = (temp_ptr[7] & 0xC0) >> 6;
				pes_pack.escr_flag = (temp_ptr[7] & 0x20) >> 5;
				pes_pack.es_rate_flag = (temp_ptr[7] & 0x10) >> 4;
				pes_pack.dsm_trick_mode_flag = (temp_ptr[7] & 0x08) >> 3;
				pes_pack.additional_copy_info_flag = (temp_ptr[7] & 0x04) >> 2;
				pes_pack.pes_crc_flag = (temp_ptr[7] & 0x02) >> 1;
				pes_pack.pes_extention_flag = temp_ptr[7] & 0x01;
				pes_pack.pes_header_data_length = temp_ptr[8];

				if(pes_pack.pts_dts_flag == 2) {

					pes_pack.pts_high = temp_ptr[9] & 0x08;
					pes_pack.pts_low  = ((temp_ptr[9] >> 1) << 30) | (temp_ptr[10] << 22) |
						((temp_ptr[11] >> 1) << 15) | (temp_ptr[12] << 7) | (temp_ptr[13] >> 1);
					pes_pack.dts_high = (int8_t)0xFF;
					pes_pack.dts_low = 0xFFFFFFFF;

				} else if (pes_pack.pts_dts_flag == 3){

					pes_pack.pts_high = temp_ptr[9] & 0x08;
					pes_pack.pts_low  = ((temp_ptr[9] >> 1) << 30) | (temp_ptr[10] << 22) |
						((temp_ptr[11] >> 1) << 15) | (temp_ptr[12] << 7) | (temp_ptr[13] >> 1);
					pes_pack.dts_high = temp_ptr[14] & 0x08;
					pes_pack.dts_low  = ((temp_ptr[15] >> 1) << 30) | (temp_ptr[16] << 22) |
						((temp_ptr[17] >> 1) << 15) | (temp_ptr[18] << 7) | (temp_ptr[19] >> 1);

				} else if (pes_pack.pts_dts_flag == 0) {

					pes_pack.pts_high = (int8_t)0xFF;
					pes_pack.pts_low = 0xFFFFFFFF;
					pes_pack.dts_high = (int8_t)0xFF;
					pes_pack.dts_low = 0xFFFFFFFF;
				}

				param1->pts_audio = pes_pack.pts_low;
				if(pes_pack.escr_flag == 1) return PACK_ERR;
				if(pes_pack.es_rate_flag == 1) return PACK_ERR;
				if(pes_pack.dsm_trick_mode_flag == 1) return PACK_ERR;
				if(pes_pack.additional_copy_info_flag == 1) return PACK_ERR;
				if(pes_pack.pes_crc_flag == 1) return PACK_ERR;
				if(pes_pack.pes_extention_flag == 1) return PACK_ERR;

				temp_ptr = temp_ptr + 9 + pes_pack.pes_header_data_length;
				pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);

				//add PTS and DTS at the begin of Video or Audio stream
				pes_buf_size = pes_end_ptr - pes_start_ptr;

				if((* pes_buf) + pack_copy_len <= pes_end_ptr) {

					memcpy((* pes_buf), temp_ptr, pack_copy_len);
					(* pes_buf) += pack_copy_len;
				} else {
					pack_copy_len1 = pes_end_ptr - (* pes_buf);

					memcpy((* pes_buf), temp_ptr, pack_copy_len1);
					memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
					(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
				}
			}
		}

	} else {

		pack_copy_len = TS_VALID_COUNT - (temp_ptr - buf);
		if((uint32_t)((* pes_buf) + pack_copy_len) <= (uint32_t)pes_end_ptr) {

			memcpy((* pes_buf), temp_ptr, pack_copy_len);
			(* pes_buf) += pack_copy_len;

		} else {

			pack_copy_len1 = pes_end_ptr - (* pes_buf);

			memcpy((* pes_buf), temp_ptr, pack_copy_len1);
			memcpy(pes_start_ptr, temp_ptr + pack_copy_len1, pack_copy_len - pack_copy_len1);
			(* pes_buf) = pes_start_ptr + pack_copy_len - pack_copy_len1;
		}
	}

	return UNPACK_OK;
}
