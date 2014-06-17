
#include "../demux_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../demux_if.h"
#include "../SyncLayerPacket.h"
#include "dr_ca.h"

void ICA_descriptor (dvb_program_property_t * param1, uint8_t * buf)
{
	uint8_t   dr_tag;
	short num = 0;
	int readcount = 0;

	while(1)
	{
		dr_tag = buf[readcount+0];
		if(dr_tag != 0x9)
		{
			return;
		}
		param1->ca_des[num].ca_des_tag = dr_tag;
		param1->ca_des[num].des_length = buf[readcount+1];
		param1->ca_des[num].ca_system_id = (buf[readcount+2] << 8) | buf[readcount+3];
		param1->ca_des[num].ca_pid = ((buf[readcount+4] & 0x1F) << 8) | buf[readcount+5];
		num++;
		readcount += 6;
	}
/*
	dr_tag = buf[0];
	if(dr_tag != 0x9){
		return;
	}
	for (num = 0; num < program_info_length / 6; num++)
	{
		param1->ca_des[num].ca_des_tag = dr_tag;
		param1->ca_des[num].des_length = buf[1];
		param1->ca_des[num].ca_system_id = (buf[2] << 8) | buf[3];
		param1->ca_des[num].ca_pid = ((buf[4] & 0x1F) << 8) | buf[5];
	}
*/
}
