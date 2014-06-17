
#include <stdio.h>
#include <string.h>
#include "demux_config.h"
#include "demux_if.h"
#include "SyncLayerPacket.h"
#include "tables/pat.h"
#include "tables/pmt.h"
#include "pes.h"
#include "push_video.h"

uint32_t paul_Demux(
	   int32_t   handle,
	  uint32_t   opt,
	      void * param1,
	      void * param2
	)
{
	if(handle) {
		switch(opt)
		{
		case DEMUX_PAT:
			return pushPacketPAT((demux_if_t *)param1, (uint8_t *)param2);

		case DEMUX_PMT:
			return pushPacketPMT((dvb_program_property_t *)param1, (uint8_t *)param2);

		case DEMUX_14496_SECTION:
			return pushISO_IEC_14496_section((dvb_program_property_t *)param1, (uint8_t *)param2);

		case DEMUX_PES_VIDEO:
			return pushPacketVideoPES((demux_if_t *)param1, (uint8_t *)param2);

		case DEMUX_PES_AUDIO:
			return pushPacketAudioPES((demux_if_t *)param1, (uint8_t *)param2);
		}
	}
	return UNPACK_OK;
}

#if 0
#define   VES_BUFFER_SIZE   1024*1024
#define   AES_BUFFER_SIZE   1024*512
uint8_t   video_es[VES_BUFFER_SIZE];
uint8_t   audio_es[AES_BUFFER_SIZE];

void printfBuf(uint8_t *buf)
{
	int i;
	int c=0;
	for(i=0; i<188; i++)
	{
		c++;
		printf("%.2x\t",buf[i]);
		if(c%8 == 0)
			printf("\n");
	}
	printf("\n");
}

void demuxStream(char *inFile)
{
	int count = 0;
	int c = 0;
	 int32_t   video_id, audio_id;
	 int32_t   video_pid_num, audio_pid_num, i;
	 int32_t   OD_ES_ID, od_pid;
	uint32_t   err_val, length;
	 int32_t   pid;
	 uint8_t   ts_buf[188];
	 uint8_t * buf_ptr = ts_buf;
	    FILE * ts_file, * fa;

	demux_if_t   demux_param;
	dvb_program_property_t * cur_prog_info;

	memset(&demux_param, 0, sizeof(demux_if_t));

	//ts_file = fopen("E:\\dump4.ts", "rb");
	ts_file = fopen(inFile, "rb");

	demux_param.cur_prog_num = 0;
	/*1. parse the PAT*/
	length = fread(ts_buf, 188, 1, ts_file);

	while(length > 0) {
		pid = ((ts_buf[1] & 0xF) << 8) + ts_buf[2];
		count++;
		//printf("%d___pid=%x\n",count,pid);
		if(pid == 0) {

			err_val = paul_Demux(1, DEMUX_PAT, &demux_param, buf_ptr);
			break;
		}
		
		//printf("%d___pid=%x\n",count,pid);
		length = fread(ts_buf, 188, 1, ts_file);
	}
	fseek(ts_file, 0, SEEK_SET);

	cur_prog_info = &demux_param.program_inf[demux_param.cur_prog_num];
	//cur_prog_info = &demux_param.program_inf[1];
	//printf("demux_param.cur_prog_num=%x\n",demux_param.cur_prog_num);
	//printf("g_pmt_pid=%x\n",cur_prog_info->g_pmt_pid);

	/*2. parse the PMT( InitialObjectDescriptor )*/
	length = fread(ts_buf, 188, 1, ts_file);
	while(length > 0) {
		pid = ((ts_buf[1] & 0xF) << 8) + ts_buf[2];
		if(pid == cur_prog_info->g_pmt_pid) {
			//printfBuf(&ts_buf);
			err_val = paul_Demux(1, DEMUX_PMT, cur_prog_info, buf_ptr);
			break;
		}
		length = fread(ts_buf, 188, 1, ts_file);
	}
	
	if(cur_prog_info->iod_dr.es_dr[0].decoder_dr.streamType == 1) 
		cur_prog_info->od_es_num_in_iod = 0;
	if(cur_prog_info->iod_dr.es_dr[1].decoder_dr.streamType == 1)
		cur_prog_info->od_es_num_in_iod = 1;
	
	OD_ES_ID = cur_prog_info->iod_dr.es_dr[cur_prog_info->od_es_num_in_iod].ES_ID;
	if(cur_prog_info->sub_pid[0].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[0].elementary_pid;
	if(cur_prog_info->sub_pid[1].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[1].elementary_pid;
	if(cur_prog_info->sub_pid[2].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[2].elementary_pid;
	if(cur_prog_info->sub_pid[3].ES_ID == OD_ES_ID) 
		od_pid = cur_prog_info->sub_pid[3].elementary_pid;

	/*3. parse the 14496 section( ObjectDescriptor )*/
	fseek(ts_file, 0, SEEK_SET);
	length = fread(ts_buf, 188, 1, ts_file);
	while(length > 0) {
		pid = ((ts_buf[1] & 0xF) << 8) + ts_buf[2];
		if(pid == od_pid) {
			if(cur_prog_info->sub_pid[cur_prog_info->od_es_num_in_iod].stream_type == 0x13) {
				
				//printfBuf(&ts_buf);
				
				err_val = paul_Demux(1, DEMUX_14496_SECTION, cur_prog_info, buf_ptr);
				break;
			}
		}
		length = fread(ts_buf, 188, 1, ts_file);
	}

	/*4.parse PES packet*/
	video_id = cur_prog_info->object_dr_video->es_dr.ES_ID;
	audio_id = cur_prog_info->object_dr_audio->es_dr.ES_ID;
	for(i=0; i<4; i++) {
		if(video_id == cur_prog_info->sub_pid[i].ES_ID){
			video_pid_num = i;
			break;
		}
	}
	for(i=0; i<4; i++) {
		if(audio_id == cur_prog_info->sub_pid[i].ES_ID){
			audio_pid_num = i;
			break;
		}
	}
	cur_prog_info->g_video_pid = cur_prog_info->sub_pid[video_pid_num].elementary_pid;
	cur_prog_info->g_audio_pid = cur_prog_info->sub_pid[audio_pid_num].elementary_pid;
	demux_param.dvb_video_ptr = video_es;
	demux_param.dvb_video_ptr_s = video_es;
	demux_param.dvb_video_ptr_e = &video_es[VES_BUFFER_SIZE - 1];
	demux_param.dvb_audio_ptr = audio_es;
	demux_param.dvb_audio_ptr_s = audio_es;
	demux_param.dvb_audio_ptr_e = &audio_es[AES_BUFFER_SIZE - 1];

	fseek(ts_file, 0, SEEK_SET);

	count = 0;
	length = fread(ts_buf, 188, 1, ts_file);
	while(length > 0) {
		pid = ((ts_buf[1] & 0xF) << 8) + ts_buf[2];

		
#if 1
		if(pid == cur_prog_info->g_video_pid) {
			//printf("PID = %x的数据：\n",pid);
			//printfBuf(ts_buf);

			
			err_val = paul_Demux(1, DEMUX_PES_VIDEO, &demux_param, buf_ptr);
			

			
			/******************************************************
			if((uint8_t)(buf_ptr[1] & 0x40) == 0x40)
				count++;
			if (count == 2)
				break;
			pushPacketVideoPES2(&demux_param, buf_ptr);
			*******************************************************/

			if(demux_param.dvb_video_ptr > demux_param.dvb_video_ptr_s + VES_BUFFER_SIZE / 2) break;
		}
#endif
		if(pid == cur_prog_info->g_audio_pid) {
			//printf("PID = %x的数据：\n",pid);
			//printfBuf(ts_buf);

			err_val = paul_Demux(1, DEMUX_PES_AUDIO, &demux_param, buf_ptr);
			if(demux_param.dvb_audio_ptr > demux_param.dvb_audio_ptr_s + AES_BUFFER_SIZE / 2) break;
		}
		length = fread(ts_buf, 188, 1, ts_file);
	}
	{
	fa = fopen("\\temp\\test_h264.264", "wb");
	fwrite(demux_param.dvb_video_ptr_s, VES_BUFFER_SIZE / 2, 1, fa);
	fclose(fa);
	}
	fa = fopen("\\temp\\test_aacplus.aac", "wb");
	fwrite(demux_param.dvb_audio_ptr_s, AES_BUFFER_SIZE / 2, 1, fa);
	fclose(fa);

	fclose(ts_file);




	c=0;

	for (count = 0; count < 4; count++)
	{
		if(demux_param.program_inf[count].g_pmt_pid == 0) break;
		c++;
		printf("\nProgram:%d - PID:%x\n",c,demux_param.program_inf[count].g_pmt_pid);
		printf("---program_num:%x\n",demux_param.program_inf[count].g_program_num);
		printf("---PCR_PID:%x\n",demux_param.program_inf[count].g_pcr_pid);
		printf("---Audio_PID:%x\n",demux_param.program_inf[count].g_audio_pid);
		printf("---Video_PID:%x\n",demux_param.program_inf[count].g_video_pid);
	}
	printf("\nThere are %d programs!\n",c);

}
#endif
