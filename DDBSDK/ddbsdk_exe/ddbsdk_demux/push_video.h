#ifndef _PUSH_VIDEO_H_
#define _PUSH_VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "demux_config.h"
#include "demux_if.h"
#include "SyncLayerPacket.h"

uint32_t pushPacketVideoPES2(SLConfig_DR * sl_config_dr, uint8_t * in_Buf, int * headerLength, unsigned long * pkgLenght);
uint32_t pushPacketAudioPES2(demux_if_t * param1, uint8_t * buf);

#ifdef __cplusplus
}
#endif
#endif