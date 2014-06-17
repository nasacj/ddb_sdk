#ifndef _DEMUX_IF_H_
#define _DEMUX_IF_H_

#include "syncLayerPacket.h"
/*
 ******************************************************
 *			   macro define interface
 ******************************************************
*/
//#ifndef TS_COUNT
//#define TS_COUNT 204
//#endif

//#ifndef TS_PACK_SIZE
//#define TS_PACK_SIZE
//#endif

#ifdef __cplusplus
extern "C" {
#endif

#define   TS_VALID_COUNT   188                           //the BYTE count of one TS PACKET

#define   CREAT_PAT_TABLE    0
#define   CREAT_PMT_TABLE    1

#define   INIT_PAT_TABLE    0
#define   INIT_PMT_TABLE    1

#define  ERR_Object_Dr          0x00010000
#define  ERR_InitialObject_Dr   0x00020000
#define  ERR_ES_Dr              0x00000300
#define  ERR_DecoderConfig_Dr   0x00000004
#define  ERR_DecSpecInfo_Dr     0x00000005
#define  ERR_SLConfig_Dr        0x00000006

#define  UNPACK_OK              0x00000000
#define  PACK_ERR               0x01000000
#define  NO_SIGNAL              0x02000000

#define   DEMUX_PAT                0x01000000
#define   DEMUX_PMT                0x02000000
#define   DEMUX_PES_VIDEO          0x03000000
#define   DEMUX_PES_AUDIO          0x04000000
#define   DEMUX_14496_SECTION      0x05000000

/*
 ******************************************************
 *			   struct and typedef interface
 ******************************************************
*/

typedef struct sub_pid_property_s
{
	unsigned short  stream_type;
	unsigned short  elementary_pid;

	unsigned short  ES_ID;
}sub_pid_property_t;

typedef struct dvb_program_property_s
{
	unsigned short   g_program_num;
	unsigned short   g_pmt_pid;
	unsigned short   g_pcr_pid;
	unsigned short   g_video_pid;
	unsigned short   g_audio_pid;
	unsigned short   od_es_num_in_iod;
	unsigned int     cur_ves_length;
	unsigned int     cur_aes_length;
			
	         Object_DR * object_dr_video;
	         Object_DR * object_dr_audio;
	avcDecoderSpecInfo   AVC_info;
	aacDecoderSpecInfo   AAC_info;

		Initial_CA_DR	 ca_des[2];

	  InitialObject_DR   iod_dr;
	         Object_DR   object_dr[2];
	sub_pid_property_t   sub_pid[4];
}dvb_program_property_t;

typedef struct dvb_demux_interface_s
{
	/* buffers for demux: ts, audio, video buffer ptr */
	unsigned char * dvb_ts_ptr;		    //store ts stream for demux
	unsigned char * dvb_ts_ptr_s;	    //ts stream start pointer
	unsigned char * dvb_ts_ptr_e;	    //ts stream end pointer
	unsigned char * dvb_ts_ptr_m;	    //ts stream middle pointer

	unsigned char * dvb_video_ptr;      //store demuxed video stream
	unsigned char * dvb_video_ptr_s;      //store demuxed video stream
	unsigned char * dvb_video_ptr_e;      //store demuxed video stream


	unsigned char * dvb_audio_ptr;      //store demuxed audio stream
	unsigned char * dvb_audio_ptr_s;      //store demuxed audio stream
	unsigned char * dvb_audio_ptr_e;      //store demuxed audio stream

	unsigned char * dvb_ptsv_ptr;       //store video's pts
	unsigned char * dvb_ptsa_ptr;       //store audio's pts

	/* previous pes frame start address */
	unsigned char * dvb_prev_pesv_ptr;
	unsigned char * dvb_prev_pesa_ptr;

	/* message queue */
//	 unsigned int * dvb_qv_msg_ptr;
//	 unsigned int * dvb_qa_msg_ptr;
	 unsigned int   pts_video;
	 unsigned int   pts_audio;

	/* handler */	
	         void * h_dvbpsi_pat;
	         void * h_dvbpsi_pmt;

	/* system timer */
	unsigned int    sys_pcr;            //demuxed system reference time
	unsigned int    sys_local;          //local coretimer's ticks

	/* pid */
	dvb_program_property_t  program_inf[5];
	   unsigned int  cur_prog_num;

	/* others */
	 int   pack_count;
	 int   err_count;

	/* flag */
	char   f_have_pcr;
	char   f_pat_finished;
	char   f_pmt_finished;
	char   v_conti_counter;
	char   a_conti_counter;
	char   v_count_start;
	char   a_count_start;

	         int    ts_pack_offset;
	unsigned int    local_time;

	/* error control parameter */

} demux_if_t;



#define CIR_BUF_INC1(PTR, BUF_START_PTR, BUF_SIZE) \
{ \
	((unsigned char *)PTR) ++; \
	if((PTR) >= (unsigned char *)(BUF_START_PTR + BUF_SIZE)) \
		(PTR) = (unsigned char *)BUF_START_PTR; \
}

#define CIR_BUF_INC4(PTR, BUF_START_PTR, BUF_SIZE)  \
{ \
	((unsigned int *)PTR) ++; \
	if((PTR) >= (unsigned int *)(BUF_START_PTR + BUF_SIZE)) \
		(PTR) = (unsigned int *)BUF_START_PTR; \
}


#define CIR_BUF_INCn(PTR, BYTES, BUF_START_PTR, BUF_SIZE)  \
{ \
	((unsigned char *)PTR) += (BYTES); \
	if((PTR) >= (unsigned char *)(BUF_START_PTR + BUF_SIZE)) \
		(PTR) = (unsigned char *)((PTR) - BUF_SIZE); \
}

#define CIR_BUF_DECn(PTR, BYTES, BUF_START_PTR, BUF_SIZE)  \
{ \
	((unsigned char *)PTR) -= (BYTES); \
	if((PTR) < (unsigned char *)BUF_START_PTR) \
		(PTR) = (unsigned char *)((PTR) + BUF_SIZE); \
}


/*
 ******************************************************
 *			   demux interface function prototype
 ******************************************************
*/
uint32_t paul_Demux(int32_t   handle, uint32_t   opt, void * param1, void * param2);
void demuxStream(char *inFile);
#if defined (__cplusplus)
}
#endif
#endif

