

#include <windows.h>
#include "../src/ddbsdk.h"
#include "../ddbsdk_ca/CA_STB_decode.h"
#include "ddbsdk_data.h"

extern ddbsdk_t   ddbsdk;
static BOOL haveReadTab = 0;
//static PBYTE g_ddbsdk_servFileBuffer;
ddbsdk_servFileTab_t  g_servTab = { 0 };

extern "C"
{
	bool g_bSerFileHaveCW;
}


#define  SERVICEFILEBUFSIZE  4096

static unsigned int  crc32_table[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static void CRC_crc32Calculate(unsigned char *buffer, unsigned int size, unsigned int *crc32)
{
	unsigned long crc32_reg = 0xFFFFFFFF;
	unsigned long cntByte,temp1,temp2,temp3;

	for (cntByte = 0; cntByte < size; cntByte++)
	{
		temp1=(crc32_reg << 8 );
		temp2=crc32_table[((crc32_reg >> 24) ^ *buffer) & 0xFF];
		temp3=temp1^temp2;
		crc32_reg = (crc32_reg << 8 ) ^ crc32_table[((crc32_reg >> 24) ^ *buffer++) & 0xFF];
	}
	*crc32 = crc32_reg;
}

typedef struct stat_file_format_s
{
	ddbsdk_servFile_syntax_t  synt;
	u32   block_num_down;
    u32   down_stat_bytes;
}stat_file_format_t;

__inline void S2WS(char * str, WCHAR * wstr) 
{
	char * ptr = str;
	WCHAR * wptr = wstr;

	while(* ptr != '\0') {
		* wptr = (WCHAR)(* ptr);
		ptr ++;
		wptr ++;
	}
	* wptr = 0;
}

static unsigned long
saveServiceFileData(void * param)
{
	unsigned char * pOutBuf = (unsigned char *)param;
// 	ddbsdk_servFileTab_t * g_servTab =NULL;
// 	memcpy(&g_servTab, pOutBuf, sizeof(ddbsdk_servFileTab_t *));
// 	WriteLog("saveServiceFileData::g_servTab:%x", g_servTab);

	BOOL * isRunning = 0;
	memcpy(&isRunning, pOutBuf, sizeof(BOOL *));


	BOOL * isSuppended = 0;
	memcpy(&isSuppended, &pOutBuf[sizeof(BOOL *)], sizeof(BOOL *));
	printf("initServiceFileData::isSuppended:%x", isSuppended);

	HANDLE downloadFileHandle = NULL;
	memcpy(&downloadFileHandle, &pOutBuf[sizeof(BOOL *) + sizeof(BOOL *)], sizeof(HANDLE));
	printf("initServiceFileData::downloadFileHandle:%x", downloadFileHandle);
	u16 pid = 0;
	memcpy(&pid, &pOutBuf[sizeof(BOOL *) + sizeof(BOOL *) + sizeof(HANDLE)], sizeof(u16));
	printf("initServiceFileData::pid:%x", pid);

	int   nOutBufSize = SERVICEFILEBUFSIZE;
	unsigned long   BytesReturned = 0;
	ddbsdk_servFileData_t * pServiceFile;
	ddbsdk_servFile_t * pServFileDowning;
	ddbsdk_servFile_syntax_t  synt;
	char   filePathMain[256] = { 0 };
	char   filePathSave[256] = { 0 };
	char   filePath_STATUS[256] = { 0 };
	WCHAR  wfilePath_STATUS[256] = { 0 };
	FILE * fp;
	u32    num;
	u32    total_num;
	u32    sizeofStatusBuffer;
	char * cfileTypePath;
	FILE * foc;

	char   pathType[9][16] = 
	{
		PREFIX_PATH_SYSFILE, PREFIX_PATH_MEDIA, PREFIX_PATH_AD, PREFIX_PATH_MUSIC,
		PREFIX_PATH_NET, PREFIX_PATH_POIFOOD, PREFIX_PATH_PROGLIST, PREFIX_PATH_LIST, 
		PREFIX_PATH_OTHER
	};

	/* create directory for  */
	sprintf(filePathMain, "%s%s", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME);
	WriteLog("%s", filePathMain);
	SERVER_TAB_LOCK;
	for(num=0; num<9; num++) {
		sprintf(filePath_STATUS, "%s%s\\check.log", filePathMain, pathType[num]);
		foc = fopen(filePath_STATUS, "wb");
		if( foc == NULL) {
			memset(filePath_STATUS, 0, sizeof(filePath_STATUS));
			memset(wfilePath_STATUS, 0, sizeof(wfilePath_STATUS));
			sprintf(filePath_STATUS, "%s%s", filePathMain, pathType[num]);
			S2WS(filePath_STATUS, wfilePath_STATUS);
			CreateDirectory(wfilePath_STATUS, NULL);
		}
		else{
			fclose(foc);
		}
	}
	SERVER_TAB_UNLOCK;
	memset(filePath_STATUS, 0, sizeof(filePath_STATUS));
	sprintf(filePath_STATUS, "%s\\filedown_status.log", filePathMain);

	while(*isRunning) {

		if (*isSuppended)
		{
			printf("suspend in while!\n");
			SuspendThread(GetCurrentThread());
			printf("i am resumed now!\n");
		}
		printf("");
		/* 
		sleep some time to get data, if a file not finished, 
		sleep 20ms,	else sleep 1000ms
		*/
NEXT_PACKAGE:
		if(BytesReturned) {
			Sleep(10);
		} else {
			Sleep(1000);
		}

		/* get data */
		((ddbsdk_serviceFile *)downloadFileHandle)->getPlainPkgData(pOutBuf, nOutBufSize, &BytesReturned);

		if(BytesReturned > 0) {
			unsigned int   crcResult, crcRight;
			/* CRC verify */
			crcRight = (pOutBuf[BytesReturned - 4] << 24) |
				(pOutBuf[BytesReturned - 3] << 16) |
				(pOutBuf[BytesReturned - 2] << 8) |
				pOutBuf[BytesReturned - 1];
			CRC_crc32Calculate(pOutBuf, BytesReturned - 4, &crcResult);
			if(crcRight == crcResult) {

				/* parse data，save one block of data */
				pServiceFile = (ddbsdk_servFileData_t *)pOutBuf;
				synt.FILE_ID = (pServiceFile->file_id1 << 24) | 
					(pServiceFile->file_id2 << 16) | (pServiceFile->file_id3 << 8) | pServiceFile->file_id4;
				synt.FILE_TYPE = pServiceFile->file_type;
				synt.Version = pServiceFile->version;
				synt.FILE_LENGTH = (pServiceFile->file_length1 << 24) | 
					(pServiceFile->file_length2 << 16) | (pServiceFile->file_length3 << 8) | pServiceFile->file_length4;
				synt.FILE_VALID_ENDDATE = (pServiceFile->file_valid_enddate1 << 8) |
					pServiceFile->file_valid_enddate2;
				synt.FILE_VALID_ENDTIME = (pServiceFile->file_valid_endtime1 << 8) |
					pServiceFile->file_valid_endtime2;
				synt.Block_length = pServiceFile->block_length_type == 0 ? 256 :
					pServiceFile->block_length_type == 1 ? 512 : 1024;
				synt.Block_num = (pServiceFile->block_total_num1 << 8) | pServiceFile->block_total_num2;
				synt.Cur_block_num = (pServiceFile->cur_block_num1 << 8) | pServiceFile->cur_block_num2;
				if(synt.FILE_TYPE <= POIFOODFILE) {
					cfileTypePath = pathType[synt.FILE_TYPE];
				} else if (synt.FILE_TYPE == PROGRAMFILE) {
					cfileTypePath = pathType[6];
				} else if (synt.FILE_TYPE == LISTFILE) {
					cfileTypePath = pathType[7];
				} else {
					cfileTypePath = pathType[8];
				}
				sprintf(filePathSave, "%s%s\\%x.tmp", filePathMain, cfileTypePath, synt.FILE_ID);

				if(g_servTab.num_down + g_servTab.num_complete >= MAXNUMDOWN) 
					goto NEXT_PACKAGE;

				SERVER_TAB_LOCK;

				/* check data table downloading */
				for(num=0; num<g_servTab.num_down + g_servTab.num_complete; num++) {
					if(g_servTab.ddbsdk_downFiles[num].synt.FILE_ID == synt.FILE_ID)
					{
						/*  if new version, down restart */
						if(g_servTab.ddbsdk_downFiles[num].synt.Version != synt.Version) {
							
							/* allocate memory for a file download */
							memcpy(&(g_servTab.ddbsdk_downFiles[num].synt), &synt, sizeof(synt));
							g_servTab.ddbsdk_downFiles[num].numDownloaded = 0;
							/* set the PID!!!!!!!!!!!!!!!!!*/
							g_servTab.ddbsdk_downFiles[num].pid = pid;

							sizeofStatusBuffer = sizeof(ddbsdk_servFileDownStatus_1024_t);
							g_servTab.ddbsdk_downFiles[num].pStatus = (void *)calloc(sizeofStatusBuffer, 1);
							if(g_servTab.ddbsdk_downFiles[num].pStatus == NULL) {
								g_servTab.ddbsdk_downFiles[num].synt.FILE_ID = 0;
								printf("memory calloc error!\n");
								WriteLog("memory calloc error!");

								SERVER_TAB_UNLOCK;
								continue;
							}
							pServFileDowning = &(g_servTab.ddbsdk_downFiles[num]);
							//g_servTab.num_down ++;

							/* create a file, all fill 0 */
							{
								unsigned char * zero1024 = (unsigned char *)calloc(1024, 1);
								u32   fileNum, statFilePos;
								int   leftLength;
								stat_file_format_t  stat_format;

								fp = fopen(filePathSave, "wb");
								if(fp == NULL) {
									printf("file open error!\n");
									WriteLog("file open error!");
									free(zero1024);

									SERVER_TAB_UNLOCK;
									continue;
								}
								for(num=0; num<synt.Block_num-1; num++) {
									fwrite(zero1024, 1, pServFileDowning->synt.Block_length, fp);
								}
								///////////获取剩余文件长度的方法很巧妙！////////////
								leftLength = pServFileDowning->synt.FILE_LENGTH & (pServFileDowning->synt.Block_length - 1);
								fwrite(zero1024, 1, leftLength, fp);
								fclose(fp);

#if 0
								/* update file downing status */
								fp = fopen(filePath_STATUS, "a+b");
								fseek(fp, 0, SEEK_END);
								statFilePos = ftell(fp);
								pServFileDowning->statFilePos = statFilePos;//记录接下来要写的位置

								memcpy(&stat_format, &synt, sizeof(ddbsdk_servFile_syntax_t));
								stat_format.block_num_down = 0;
								stat_format.down_stat_bytes = (synt.Block_num + 7) / 8;//这里什么意思？
								fwrite(&stat_format, 1, sizeof(stat_format), fp);
								fwrite(zero1024, 1, stat_format.down_stat_bytes, fp);//为什么要把空缓存中的0写到文件？fwrite(&zero1024, 1, stat_format.down_stat_bytes, fp);
								fclose(fp);
#endif
								free(zero1024);
								goto DONOT_CREATE_TAB;
							}
						} else {
							if(g_servTab.ddbsdk_downFiles[num].numDownloaded >= g_servTab.ddbsdk_downFiles[num].synt.Block_num) {
								SERVER_TAB_UNLOCK;
								goto NEXT_PACKAGE;
							} else {
								pServFileDowning = &(g_servTab.ddbsdk_downFiles[num]);
								goto DONOT_CREATE_TAB;
							}
						}
					}
				}

				/* allocate memory for a file download */
				total_num = g_servTab.num_down + g_servTab.num_complete;
				memcpy(&(g_servTab.ddbsdk_downFiles[total_num].synt), &synt, sizeof(synt));
				g_servTab.ddbsdk_downFiles[total_num].numDownloaded = 0;
				/* set the PID!!!!!!!!!!!!!!!!!!!!!!!!*/
				g_servTab.ddbsdk_downFiles[total_num].pid = pid;

				sizeofStatusBuffer = sizeof(ddbsdk_servFileDownStatus_1024_t);
				g_servTab.ddbsdk_downFiles[total_num].pStatus = (void *)calloc(sizeofStatusBuffer, 1);
				if(g_servTab.ddbsdk_downFiles[total_num].pStatus == NULL) {
					g_servTab.ddbsdk_downFiles[total_num].synt.FILE_ID = 0;
					printf("memory calloc error!\n");
					WriteLog("memory calloc error!");

					SERVER_TAB_UNLOCK;
					continue;
				}
				pServFileDowning = &(g_servTab.ddbsdk_downFiles[total_num]);
				g_servTab.num_down ++;

				/* create a file, all fill 0 */
				{
					unsigned char * zero1024 = (unsigned char *)calloc(1024, 1);
					u32   fileNum, statFilePos;
					int   leftLength;
					stat_file_format_t  stat_format;


					fp = fopen(filePathSave, "wb");
					if(fp == NULL) {
						printf("file open error!\n");
						WriteLog("file open error!");
						free(zero1024);

						SERVER_TAB_UNLOCK;
						continue;
					}
					for(num=0; num<synt.Block_num-1; num++) {
						fwrite(zero1024, 1, pServFileDowning->synt.Block_length, fp);
					}
					///////////获取剩余文件长度的方法很奇怪////////////
					leftLength = pServFileDowning->synt.FILE_LENGTH & (pServFileDowning->synt.Block_length - 1);
					fwrite(zero1024, 1, leftLength, fp);
					fclose(fp);

					/* update file downing status */
					fp = fopen(filePath_STATUS, "a+b");
					fseek(fp, 0, SEEK_END);
					statFilePos = ftell(fp);
					pServFileDowning->statFilePos = statFilePos;//记录接下来要写的位置

					memcpy(&stat_format, &synt, sizeof(ddbsdk_servFile_syntax_t));
					stat_format.block_num_down = 0;
					stat_format.down_stat_bytes = (synt.Block_num + 7) / 8;//这里什么意思？用一位来表示块
					fwrite(&stat_format, 1, sizeof(stat_format), fp);
					fwrite(zero1024, 1, stat_format.down_stat_bytes, fp);//为什么要把空缓存中的0写到文件？fwrite(&zero1024, 1, stat_format.down_stat_bytes, fp);
					////////////////////////////////////////////////////////////////////////////
					fwrite(&pServFileDowning->pid, 1, sizeof(u16), fp);
					/////////////////////////////////////////////////////////////////////////////
					fclose(fp);

					/////总日志文件的第一个int型记录文件数量，这里+1
					fp = fopen(filePath_STATUS, "r+b");
					if(fp != NULL) {
						fseek(fp, 0, SEEK_SET);
						fread(&fileNum, 1, 4, fp);
						fileNum += 1;
						fseek(fp, 0, SEEK_SET);
						fwrite(&fileNum, 1, 4, fp);
						fclose(fp);
					} else {
						printf("file open error!\n");
						WriteLog("file open error!");
						free(zero1024);

						SERVER_TAB_UNLOCK;
						continue;
					}
					free(zero1024);
				}
DONOT_CREATE_TAB:
				int   headerLen = 21;
				int leftNum;
				ddbsdk_servFileDownStatus_1024_t * statPtr = (ddbsdk_servFileDownStatus_1024_t *)pServFileDowning->pStatus;
				/* memcpy to save data block */
				statPtr->data_pos[statPtr->numDown] = synt.Cur_block_num;
				memcpy(&statPtr->data[statPtr->numDown * synt.Block_length], &pOutBuf[headerLen], synt.Block_length);

				/* update download status */
				statPtr->numDown ++;
				leftNum = pServFileDowning->synt.Block_num - pServFileDowning->numDownloaded;

				/* if have 10 package, save it */
				if(statPtr->numDown * synt.Block_length >= BLOCKDOWNBUFSIZE || statPtr->numDown >= leftNum) { 
					int   blk_num_pos, stat_byte_pos, stat_bit_pos;//不知道作用？
					/* check if file exist */

					fp = fopen(filePathSave, "r+b");
					if(fp == NULL) {
						printf("file open error!\n");
						WriteLog("file open error!\n");

						SERVER_TAB_UNLOCK;
						continue;
					} else {
						for(num=0; num<statPtr->numDown; num++) {
							blk_num_pos = statPtr->data_pos[num];
							stat_bit_pos = blk_num_pos & 7;
							stat_byte_pos = blk_num_pos / 8;
							/* check one bit */
							if(! (statPtr->blockStatus[stat_byte_pos] & (1 << stat_bit_pos))) { /* if have saved the block, donnot write */
								if(statPtr->data_pos[num] == (pServFileDowning->synt.Block_num - 1)) { /* if it's the last block */
									fseek(fp, statPtr->data_pos[num] * synt.Block_length, SEEK_SET);
									fwrite(&statPtr->data[num * synt.Block_length], 1, (pServFileDowning->synt.FILE_LENGTH & (synt.Block_length - 1)), fp);
								} else {
									fseek(fp, statPtr->data_pos[num] * synt.Block_length, SEEK_SET);
									fwrite(&statPtr->data[num * synt.Block_length], 1, synt.Block_length, fp);
								}
								/* set one bit */
								statPtr->blockStatus[stat_byte_pos] |= (1 << stat_bit_pos);
								pServFileDowning->numDownloaded ++;
							}
						}
						fclose(fp);

						/* update file downing status */
						u32  statusLength;
						fp = fopen(filePath_STATUS, "r+b");
						fseek(fp, pServFileDowning->statFilePos + sizeof(ddbsdk_servFile_syntax_t), SEEK_SET);
						statusLength = (pServFileDowning->synt.Block_num + 7) / 8;
						fwrite(&pServFileDowning->numDownloaded, 1, 4, fp);
						fseek(fp, pServFileDowning->statFilePos + sizeof(ddbsdk_servFile_syntax_t) + 8, SEEK_SET);
						fwrite(statPtr->blockStatus, 1, statusLength, fp);
						fclose(fp);
					}

					statPtr->numDown = 0;
				}
				/* check the data if all package finished, if complete, 
				    change file name(.tmp -> .dat), free the memory */
				if(pServFileDowning->numDownloaded >= pServFileDowning->synt.Block_num) {
					/* free memory */
					free(pServFileDowning->pStatus);
					pServFileDowning->pStatus = NULL;
					g_servTab.num_complete ++;
					g_servTab.num_down --;
				}

				SERVER_TAB_UNLOCK;

			} else {
				WriteLog("service file download crc error!\n");
			}
		}
	}

	//free(pOutBuf);
	return 0;
}


/*
  FILE NUM -- 4 BYTES

  HEADER   -- 20 BYTES
  BLOCK NUM DOWN  -- 4 BYTES
  DOWN STATUS BYTES  -- 4 BYTES
  STATUS  -- bytes
*/

unsigned long
ddbsdk_serviceFile::initServiceFileData(void)
{
	DWORD dwThreadID;
	FILE  * fLog;
	FILE  * fLogTemp;
	char   filePathMain[256] = { 0 };
	char   filePath_STATUS[256] = { 0 };
	char   filePath_Del[256] = { 0 };
	WCHAR  wfilePath_Del[256] = { 0 };
	WCHAR  wfilePath_STATUS[256] = { 0 };
	u32    fileNum, num;
	u32    lengthForRead, readLen, filePos, sizeofStatusBuffer;
	u32    zero = 0;
	ddbsdk_servFileDownStatus_1024_t * statPtr;
	ddbsdk_servFile_syntax_t * synt;
	char * cfileTypePath;
	char   haveDeleteFile = 0;
	stat_file_format_t  stat_format;

	char   pathType[9][16] = 
	{
		PREFIX_PATH_SYSFILE, PREFIX_PATH_MEDIA, PREFIX_PATH_AD, PREFIX_PATH_MUSIC,
		PREFIX_PATH_NET, PREFIX_PATH_POIFOOD, PREFIX_PATH_PROGLIST, PREFIX_PATH_LIST, 
		PREFIX_PATH_OTHER
	};

	sprintf(filePathMain, "%s%s", ddbsdk.ddbsdk_path_main, DDBSDK_MAINPATH_NAME);
	sprintf(filePath_STATUS, "%s\\filedown_status.log", filePathMain);

	/* init the ECM/EMM/program pid 
	((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->ecm_pid = 4117;
	((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->emm_pid = 4097;
	((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->prog_pid = 264;//270
	((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_bDDBHaveCW = &g_bSerFileHaveCW;
	((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->dataType = DownloadFile;
	CA_initEcmEmmPid(((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->ecm_pid,
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->emm_pid);

	printf("ddbsdk.hServiceFile->g_bDDBHaveCW = %x",((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->g_bDDBHaveCW);
*/

	//对象在此同步：从检查是否是第一次创建log文件开始
	SERVER_TAB_LOCK;

	/* check file donwloading status files */
	/* 1. check file: filedown_status.log */
	fLog = fopen(filePath_STATUS, "r+b");
	if(fLog == NULL) {
		u32   zero = 0;
		fLog = fopen(filePath_STATUS, "wb");
		fwrite(&zero, 1, sizeof(zero), fLog);
		fclose(fLog);
		SERVER_TAB_UNLOCK;
		goto CREATE_FILEDOWN_THREAD;
	}


	if (!haveReadTab)
	{
		/* 2. analyze finished / downing / outdate Files, remove outdate files */
		fread(&fileNum, 1, 4, fLog);
		num = 0;
		while(num < fileNum) {

			lengthForRead = sizeof(ddbsdk_servFile_syntax_t);
			filePos = ftell(fLog);
			synt = &g_servTab.ddbsdk_downFiles[num].synt;
			if((readLen = fread(synt, 1, sizeof(ddbsdk_servFile_syntax_t), fLog)) == lengthForRead) {

				/* if the file out of date, delete it */
				u32   hour = ((ddbsdk.tdtTime & 0xFF0000) >> 16);
				u32   minu = ((ddbsdk.tdtTime & 0xFF00) >> 8);
				hour = ((hour & 0xF0) >> 4) * 10 + (hour & 0x0F);
				minu = ((minu & 0xF0) >> 4) * 10 + (minu & 0x0F);
				u32   minutTimeCur = hour * 60 + minu;
				if(synt->FILE_ID == 0) continue;
				if((synt->FILE_VALID_ENDDATE <= ddbsdk.mjdDate) && 
					(synt->FILE_VALID_ENDDATE != ddbsdk.mjdDate || synt->FILE_VALID_ENDTIME <= minutTimeCur)) {

						u32   delFilePos;
						if(synt->FILE_TYPE <= POIFOODFILE) {
							cfileTypePath = pathType[synt->FILE_TYPE];
						} else if (synt->FILE_TYPE == PROGRAMFILE) {
							cfileTypePath = pathType[6];
						} else if (synt->FILE_TYPE == LISTFILE) {
							cfileTypePath = pathType[7];
						} else {
							cfileTypePath = pathType[8];
						}
						sprintf(filePath_Del, "%s%s\\%x.tmp", filePathMain, cfileTypePath, synt->FILE_ID);
						/* delete file */
						S2WS(filePath_Del, wfilePath_Del);
						DeleteFile(wfilePath_Del);
						haveDeleteFile = 1;
						fread(&g_servTab.ddbsdk_downFiles[num].numDownloaded, 1, sizeof(g_servTab.ddbsdk_downFiles[num].numDownloaded), fLog);
						fread(&lengthForRead, 1, 4, fLog);
						delFilePos = ftell(fLog) + lengthForRead;

						fseek(fLog, filePos, SEEK_SET);
						fwrite(&zero, 1, 4, fLog);   /* write to set FILE_ID = 0 */
						fseek(fLog, delFilePos, SEEK_SET);
						continue;
				}

				/* read the file download status */
				g_servTab.ddbsdk_downFiles[num].statFilePos = filePos;
				fread(&g_servTab.ddbsdk_downFiles[num].numDownloaded, 1, sizeof(g_servTab.ddbsdk_downFiles[num].numDownloaded), fLog);
				if(g_servTab.ddbsdk_downFiles[num].numDownloaded < g_servTab.ddbsdk_downFiles[num].synt.Block_num) {
					/* if have not finished */
					g_servTab.num_down ++;
					fread(&lengthForRead, 1, 4, fLog);
					sizeofStatusBuffer = sizeof(ddbsdk_servFileDownStatus_1024_t);
					g_servTab.ddbsdk_downFiles[num].pStatus = calloc(sizeofStatusBuffer, 1);
					statPtr = (ddbsdk_servFileDownStatus_1024_t *)g_servTab.ddbsdk_downFiles[num].pStatus;

					statPtr->numDown = 0;
					fread(statPtr->blockStatus, 1, lengthForRead, fLog);
					////////////////////////////////////////////////////////////////////////////////
					fread(&g_servTab.ddbsdk_downFiles[num].pid, 1, sizeof(u16), fLog);
					/////////////////////////////////////////////////////////////////////////////////
					memset(statPtr->data_pos, 0, sizeof(statPtr->data_pos));
				} else {
					u32  comFilePos;
					g_servTab.num_complete ++;
					fread(&lengthForRead, 1, 4, fLog);
					comFilePos = ftell(fLog) + lengthForRead;
					fseek(fLog, comFilePos, SEEK_SET);
					g_servTab.ddbsdk_downFiles[num].pStatus = NULL;
				}
				num ++;
			} else {
				break;
			}
		}

		fclose(fLog);

		if(haveDeleteFile) {
			/* 3. update filedown_status.log */
			u32   totalFileNum = g_servTab.num_complete + g_servTab.num_down;
			u32   filePos = 0, flengthTemp;
			u32   FILE_ID;
			u8    statusBuf[4096];

			sprintf(filePath_STATUS, "%s\\filedown_status.tmp", filePathMain);
			sprintf(filePath_Del, "%s\\filedown_status.log", filePathMain);

			fLogTemp = fopen(filePath_STATUS, "wb");
			fLog = fopen(filePath_Del, "rb");
			fseek(fLog, 0, SEEK_END);
			flengthTemp = ftell(fLog);

			fseek(fLogTemp, 0, SEEK_SET);
			fwrite(&totalFileNum, 1, 4, fLogTemp);
			fseek(fLog, 4, SEEK_SET);
			for(num=0; num<totalFileNum; num++) {
				if(filePos > flengthTemp) break;
				filePos = ftell(fLog);
				fread(&FILE_ID, 1, 4, fLog);
				if(FILE_ID == 0) {  /* if FILE_ID == 0, the file have been deleted */
					fseek(fLog, filePos, SEEK_SET);
					fread(&stat_format, 1, sizeof(stat_format), fLog);
					filePos += 4 + sizeof(stat_format) + stat_format.down_stat_bytes;
					fseek(fLog, filePos, SEEK_SET);
				} else {
					fseek(fLog, filePos, SEEK_SET);
					fread(&stat_format, 1, sizeof(stat_format), fLog);
					fwrite(&stat_format, 1, sizeof(stat_format), fLogTemp);
					fread(statusBuf, 1, stat_format.down_stat_bytes, fLog);
					//////////////////////////////////////////////////////////////////////////
					u16 pid = 0;
					fread(&pid, 1, sizeof(u16), fLog);
					//////////////////////////////////////////////////////
					fseek(fLogTemp, filePos + sizeof(stat_format), SEEK_SET);
					fwrite(statusBuf, 1, stat_format.down_stat_bytes, fLogTemp);
					/////////////////////////////////////////////////////////////////////////////
					fwrite(&pid, 1, sizeof(u16), fLogTemp);
					////////////////////////////////////////////////////////////////////////////
				}
			}
			fclose(fLogTemp);
			fclose(fLog);

			/* delete filedown_status.log */
			S2WS(filePath_Del, wfilePath_Del);
			DeleteFile(wfilePath_Del);

			/* rename filedown_status.tmp to filedown_status.log(MoveFile) */
			S2WS(filePath_STATUS, wfilePath_STATUS);
			MoveFile(wfilePath_STATUS, wfilePath_Del);
		}

		haveReadTab = TRUE;
	} 
	else
	{
		fclose(fLog);
	}
	

	//SERVER_TAB_UNLOCK;//移至线程中，为了让线程完整创建文件夹
	SERVER_TAB_UNLOCK;

CREATE_FILEDOWN_THREAD:
	//TODO 注意这里的buffer是全局变量
	this->g_ddbsdk_servFileBuffer = (PBYTE) malloc(SERVICEFILEBUFSIZE);
//	unsigned int sturctSize = sizeof(ddbsdk_servFileTab_t *);
// 	ddbsdk_servFileTab_t * servTab_p = &(this->g_servTab);
// 	memcpy(g_ddbsdk_servFileBuffer, &servTab_p, sturctSize);
// 	WriteLog("initServiceFileData::servTab_p:%x", servTab_p);

	this->isThreadRunning = TRUE;
	BOOL * isRunning = &(this->isThreadRunning);
	memcpy(this->g_ddbsdk_servFileBuffer, &isRunning, sizeof(BOOL *));
//	WriteLog("initServiceFileData::isThreadRunning:%x", isRunning);
	printf("initServiceFileData::isThreadRunning:%x", isRunning);

	this->isThreadSuppended = FALSE;
	BOOL * isSuppended = &(this->isThreadSuppended);
	memcpy(&(this->g_ddbsdk_servFileBuffer[sizeof(BOOL *)]), &isSuppended, sizeof(BOOL *));
	printf("initServiceFileData::isSuspended:%x", isSuppended);

	memcpy(&(this->g_ddbsdk_servFileBuffer[sizeof(BOOL *) + sizeof(BOOL *)/*sturctSize*/]), &(this->downHandle), sizeof(HANDLE));
	printf("initServiceFileData::downHandle:%x", downHandle);

	u16 pid = this->prog_pid;
	memcpy(&(this->g_ddbsdk_servFileBuffer[sizeof(BOOL *) + sizeof(BOOL *) + sizeof(HANDLE)]), &pid, sizeof(u16));
	printf("initServiceFileData::pid:%x", pid);

	//::g_servTab_p = &(this->g_servTab);
	//::downloadFileHandle = this->downHandle;
	/* create a thread process file data */
	this->hCheckServiceFile = CreateThread(NULL, 0, saveServiceFileData, this->g_ddbsdk_servFileBuffer, 0, &dwThreadID);

	//SetThreadPriority( this->hCheckServiceFile, THREAD_PRIORITY_BELOW_NORMAL);

	return 0;
}



unsigned long
ddbsdk_serviceFile::deinitServiceFileData(void)
{
	s32   num;

	//停止线程
	this->isThreadRunning = FALSE;
	WaitForSingleObject(this->hCheckServiceFile, INFINITE);

	/* free all status buffer */
	if (g_servTab.num_down != 0 || g_servTab.num_complete != 0)
	{
		for(num=0; num<g_servTab.num_complete+g_servTab.num_down; num++) {
			if(g_servTab.ddbsdk_downFiles[num].pStatus != NULL && this->prog_pid == g_servTab.ddbsdk_downFiles[num].pid) {
				free(g_servTab.ddbsdk_downFiles[num].pStatus);
				g_servTab.ddbsdk_downFiles[num].pStatus = NULL;
			}
		}
		//memset(&g_servTab, 0, sizeof(ddbsdk_servFileTab_t));
	}
	
	free(this->g_ddbsdk_servFileBuffer);
	this->g_ddbsdk_servFileBuffer = NULL;
	CloseHandle(this->hCheckServiceFile);
	this->hCheckServiceFile = NULL;

	return 0;
}

