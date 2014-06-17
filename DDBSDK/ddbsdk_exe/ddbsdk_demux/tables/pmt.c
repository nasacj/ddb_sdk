


#include <stdio.h>
#include <string.h>

#include "../demux_config.h"
#include "../demux_if.h"
#include "pmt.h"
#include "../descriptors/dr_29.h"
#include "../descriptors/dr_30.h"
#include "../descriptors/dr_ca.h"

uint32_t pushPacketPMT(dvb_program_property_t * param1, uint8_t * buf)
{
	int32_t   section_length, program_info_length;
	int32_t   left_length, ES_info_length;
	int32_t   stream_num = 0;
	int32_t   program_num;
	uint8_t * temp_ptr;

	section_length = ((buf[6] & 0xF) << 8) + buf[7]; 
	program_num = (buf[8] << 8) + buf[9];
	param1->g_pcr_pid = ((buf[13] & 0x1F) << 8) + buf[14];
	program_info_length = ((buf[15] & 0xF) << 8) + buf[16];

	if(program_info_length > 0) {
		if (buf[17] == 0x09)
		{
			ICA_descriptor(&param1, &buf[17]);
		}
		else
		{
			IOD_descriptor(&param1->iod_dr, &buf[17]);  /* Descriptor 29 */
		}

	}

	left_length = section_length - 9 - program_info_length - 4/* CRC32 */;
	temp_ptr = buf + 17 + program_info_length;

	do {
		param1->sub_pid[stream_num].stream_type = (uint16_t)temp_ptr[0];
		param1->sub_pid[stream_num].elementary_pid = ((temp_ptr[1] & 0x1F) << 8) + temp_ptr[2];
		ES_info_length = ((temp_ptr[3] & 0xF) << 8) + temp_ptr[4];

		if(ES_info_length) {
			SL_descriptor(&param1->sub_pid[stream_num], &temp_ptr[5]);
		}

		temp_ptr += ES_info_length + 5;
		left_length -= ES_info_length + 5;
		stream_num ++;

	} while( left_length );

	return UNPACK_OK;
}

