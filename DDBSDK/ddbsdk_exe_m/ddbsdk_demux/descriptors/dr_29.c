
#include "../demux_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../SyncLayerPacket.h"
#include "dr_29.h"

void IOD_descriptor (InitialObject_DR * iod_dr, uint8_t * buf)
{
	  int8_t   dr_tag;
	  int8_t   Scope_of_IOD_label, IOD_label;
	uint32_t   length;

	dr_tag = buf[0];
	length = buf[1];
	Scope_of_IOD_label = buf[2];
	IOD_label = buf[3];
	InitialObjectDescriptor(iod_dr, &buf[4]);
}
