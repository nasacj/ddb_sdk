
#ifndef _PES_H_
#define _PES_H_

uint32_t pushPacketVideoPES(demux_if_t * param1, uint8_t * buf);
uint32_t pushPacketAudioPES(demux_if_t * param1, uint8_t * buf);
#endif

