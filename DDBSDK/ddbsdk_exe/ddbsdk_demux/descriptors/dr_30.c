
#include "../demux_config.h"
#include "../demux_if.h"
#include "dr_30.h"

void SL_descriptor (sub_pid_property_t * sub_prop, uint8_t * buf) 
{
	  int8_t   dr_tag;
	uint32_t   length;

	dr_tag = buf[0];
	length = buf[1];

	sub_prop->ES_ID = (buf[2] << 8) + buf[3];
}
