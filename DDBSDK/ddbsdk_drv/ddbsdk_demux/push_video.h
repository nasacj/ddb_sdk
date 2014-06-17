#ifndef _PUSH_VIDEO_H_
#define _PUSH_VIDEO_H_

uint32_t pushPacketVideoPES2(demux_if_t * param1, uint8_t * buf);
uint32_t pushPacketAudioPES2(demux_if_t * param1, uint8_t * buf);
#endif