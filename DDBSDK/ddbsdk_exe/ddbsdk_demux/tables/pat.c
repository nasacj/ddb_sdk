


#include <stdio.h>

#include "../demux_config.h"
#include "../demux_if.h"
#include "pat.h"


uint32_t pushPacketPAT(demux_if_t * param1, uint8_t * buf)
{
	int32_t   length;
	int32_t   p_num;

	length = ((buf[6] & 0xF) << 8) + buf[7];

	length -= (5 + 4);  /* header(5) + crc32(4) */

	for(p_num=0; p_num<(length/4); p_num++) {
		param1->program_inf[p_num].g_program_num = (buf[13 + p_num * 4] << 8) + buf[14 + p_num * 4];
		param1->program_inf[p_num].g_pmt_pid = ((buf[15 + p_num * 4] & 0x1f) << 8) + buf[16 + p_num * 4];
	}
	return UNPACK_OK;
}
