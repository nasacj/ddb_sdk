int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	DWORD BytesReturned = 0;
	DWORD size;
	PBYTE buffer;
	bool debug_exit = false;
	int getCount = 0;
	bool deviceOpen = false;
	int  file_test_mode = 0;

	char   strbuffer[256] = { 0 };
	char * sPtr = (char *)strbuffer;

	TEXT2ASC(lpCmdLine, sPtr);
	if(strlen(sPtr) < 2) usagePrint(0x00000302);

	DDB_Init(NULL, 0);

	DDB_Open(0, 0, 0);

	/* allocate memory for data transfer */
	size = 100 * 1024;
	buffer = (PBYTE)malloc(size);
	memset(buffer, 0, size);

	while(1) {
		if(sPtr == '\0') break;
		getComLineChar(&sPtr);
		switch(* sPtr) {
		case 'f':
			{
			ddbsdk_reconfigure_t param;
			param.frequency = getComLineVal(&sPtr); /* 14563840000(LAB) / 1470080000 */
			param.testInSMGLabFlag = 0;
			strcpy(param.ddbsdk_path_main, "\\MMC_Storage");
			DDB_IOControl(NULL, IOCTL_DDBSDK_RECONFIGURE, (unsigned char *)&param, sizeof(ddbsdk_reconfigure_t), NULL, 0, NULL);
			printf("set freq: %d \n", param.frequency);
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
			deviceOpen = true;
			printf("----------open device!--------------\n");
			}
			break;
		case 'd':
			DWORD dwThreadID_1;
			DWORD nParameter_1;
			HANDLE  hThread_1;
			if(deviceOpen == false) {
				if(file_test_mode == 0) {
					DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
				} else {
					hThread_1 = CreateThread(NULL, 0, thread_sendStreamData, &nParameter_1, 0, &dwThreadID_1);
				}
				deviceOpen = true;
			}

			switch(getComLineVal(&sPtr)) {
			case 0:  /* traffic */
				DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);
				{
				FILE  * f_data = fopen("\\Temp\\traffic.dat", "wb");
				DWORD testCount = 0;
				while(1) {
					if(debug_exit == true) break;
					DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_TRAFFIC, NULL, 0, buffer, size, &BytesReturned);
					getCount = BytesReturned;
					testCount ++;
					if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
					if((testCount & 0x7) == 0) {
						printf("count: %d, bytes: %d\n", testCount, getCount);
					}
				}
				fclose(f_data);
				}
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);
				break;
			case 1:  /* weather */
				DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_WEATHER, NULL, 0, NULL, 0, NULL);
				{
				FILE  * f_data = fopen("\\Temp\\weather.dat", "wb");
				DWORD testCount = 0;
				while(1) {
					if(debug_exit == true) break;
					DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_WEATHER, NULL, 0, buffer, size, &BytesReturned);
					getCount = BytesReturned;
					testCount ++;
					if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
					if((testCount & 0x7) == 0) {
						printf("count: %d, bytes: %d\n", testCount, getCount);
					}
				}
				fclose(f_data);
				}
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_WEATHER, NULL, 0, NULL, 0, NULL);
				break;
			case 2:  /* finance */
				DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_FINANCE, NULL, 0, NULL, 0, NULL);
				{
				FILE  * f_data = fopen("\\Temp\\finance.dat", "wb");
				DWORD testCount = 0;
				while(1) {
//					if(debug_exit == true) break;
					DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_FINANCE, NULL, 0, buffer, size, &BytesReturned);
					getCount = BytesReturned;
					testCount ++;
//					if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
					if((testCount & 0x7) == 0) {
						printf("count: %d, bytes: %d\n", testCount, getCount);
					}
				}
				fclose(f_data);
				}
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_FINANCE, NULL, 0, NULL, 0, NULL);
				break;
			case 3:  /* PIO */
				DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_PIOINFO, NULL, 0, NULL, 0, NULL);
				{
				FILE  * f_data = fopen("\\Temp\\pioinfo.dat", "wb");
				DWORD testCount = 0;
				while(1) {
					if(debug_exit == true) break;
					DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_PIOINFO, NULL, 0, buffer, size, &BytesReturned);
					getCount = BytesReturned;
					testCount ++;
					if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
					if((testCount & 0x7) == 0) {
						printf("count: %d, bytes: %d\n", testCount, getCount);
					}
				}
				fclose(f_data);
				}
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_PIOINFO, NULL, 0, NULL, 0, NULL);
				break;
			case 4:  /* File */
				{
				ddbsdk_servFileStatus_t   servStatus = { 0 };
				ddbsdk_servFileInfo_t fileInfo = { 0 };
				int cnt = 0;
				DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
				servStatus.selectedFILE_TYPE = POIFOODFILE;
				while(1) {
					Sleep(2000);
					DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_DATA_DOWNLOADINGFILESTATUS, 
						(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), 
						(unsigned char *)&servStatus, sizeof(ddbsdk_servFileStatus_t), NULL);
					if(servStatus.numDownloading == 0) {
						printf("no file downloading!\n");
					} else {
						printf("downing: %d!\n", servStatus.numDownloading);
					}
					while(servStatus.fileID_downing[cnt] != 0 && cnt < 8) {
						printf("downing: %x\n", servStatus.fileID_downing[cnt]);
						cnt ++;
					}
					if(servStatus.numFinished == 0) {
						printf("no file finished!\n");
					} else {
						printf("finished: %d!\n", servStatus.numFinished);
					}
					cnt = 0;
					while(servStatus.fileID_finished[cnt] != 0 && cnt < 8) {
						printf("finished: %x\n", servStatus.fileID_finished[cnt]);
						cnt ++;
					}

					/* get one file that have finished */
					if(servStatus.numFinished) {
						u8 *  temp_fileBuf, * temp_filePtr;
						u32   n;
						FILE * df;
						unsigned long    bytesRet;

						fileInfo.selectedFILE_ID = servStatus.fileID_finished[0];
						DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_ONEFILEINFO, (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), 
							(unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL);

						temp_filePtr = temp_fileBuf = (u8 *)malloc(fileInfo.Block_num * fileInfo.Block_length);
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_OPEN,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);
						if(fileInfo.file_get != NULL) {
							for(n=0; n<fileInfo.Block_num; n++) {
								DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), temp_filePtr, fileInfo.Block_length, &bytesRet);
								temp_filePtr += fileInfo.Block_length;
							}
							df = fopen("\\MMC_Storage\\file.dat", "wb");
							fwrite(temp_fileBuf, 1, fileInfo.FILE_LENGTH, df);
							fclose(df);
						}
						free(temp_fileBuf);
						DDB_IOControl(NULL, IOCTL_DDBSDK_GETONEDATABLOCK_FINISHEDFILE_CLOSE,  (unsigned char *)&fileInfo, sizeof(ddbsdk_servFileInfo_t), NULL, 0, NULL);

						break;
					}
				}
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_DOWNLOADFILE, NULL, 0, NULL, 0, NULL);
				}
				break;
			}
#if 0
			if(deviceOpen == true) {
				DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DMBDEVICE, NULL, 0, NULL, 0, NULL);
				deviceOpen = false;
			}
#endif
			break;
		case 'l':
			{
			DWORD dwThreadID_1;
			DWORD nParameter_1;
			HANDLE  hThread_1;
			ddbsdk_dmbdataInfo_t    dataInfo = { 0 };

			if(deviceOpen == false) {
				if(file_test_mode == 0) {
					DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
				} else {
					hThread_1 = CreateThread(NULL, 0, thread_sendStreamData, &nParameter_1, 0, &dwThreadID_1);
				}
				deviceOpen = true;
			}
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_DATAPROGRAM, NULL, 0, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), NULL);

			dataInfo.selectServiceNo = getComLineVal(&sPtr);
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_XXXX, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), NULL, 0, NULL);
			{
			FILE  * f_data = fopen("\\Temp\\data.dat", "wb");
			DWORD testCount = 0;
			while(1) {
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_XXXX, (unsigned char *)&dataInfo, sizeof(ddbsdk_dmbdataInfo_t), buffer, size, &BytesReturned);
				getCount = BytesReturned;
				testCount ++;
				if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
				if((testCount & 0x7) == 0) {
					printf("count: %d, bytes: %d\n", testCount, getCount);
				}
			}
			fclose(f_data);
			}
			DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_XXXX, NULL, 0, NULL, 0, NULL);
			}

			break;
		case 'v':
			{
			DWORD dwThreadID_1, dwThreadID_2, dwThreadID_3;
			DWORD nParameter_1, nParameter_2, nParameter_3;
			HANDLE  hThread_1, hThread_2, hThread_3;
			ddbsdk_dmbvideoInfo_t    videoInfo = { 0 };
			if(deviceOpen == false) {
				if(file_test_mode == 0) {
					DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
				} else {
					hThread_1 = CreateThread(NULL, 0, thread_sendStreamVideo, &nParameter_1, 0, &dwThreadID_1);
				}
				deviceOpen = true;
			}
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_VIDEOPROGRAM, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL);

			videoInfo.selectServiceNo = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_VIDEO, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), NULL, 0, NULL);

			/* create thread to get video */
			hThread_2 = CreateThread(NULL, 0, thread_getStreamVideo, &nParameter_2, 0, &dwThreadID_2);

			/* create thread to get audio */
			hThread_3 = CreateThread(NULL, 0, thread_getStreamAudio, &nParameter_3, 0, &dwThreadID_3);

			while(1) { 
				Sleep(1000);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_VIDEO_PCR, NULL, 0, (unsigned char *)&videoInfo, sizeof(ddbsdk_dmbvideoInfo_t), &BytesReturned);
				printf("pcr: 0x%x%x\n", (u32)(videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base >> 32),
					(u32)videoInfo.videoInfo[videoInfo.selectServiceNo].pcr_base);
			}
			if(file_test_mode == 1) {
				CloseHandle(hThread_1);
			}
			CloseHandle(hThread_2);
			CloseHandle(hThread_3);

			DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_VIDEO, NULL, 0, NULL, 0, NULL);
			}
			break;
		case 'm':
			{
			DWORD dwThreadID_1;
			DWORD nParameter_1;
			HANDLE  hThread_1;
			ddbsdk_dmbmusicamInfo_t    musicamInfo = { 0 };
			if(deviceOpen == false) {
				if(file_test_mode == 0) {
					DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
				} else {
					hThread_1 = CreateThread(NULL, 0, thread_sendStreamMusicam, &nParameter_1, 0, &dwThreadID_1);
				}
				deviceOpen = true;
			}
			DDB_IOControl(NULL, IOCTL_DDBSDK_LIST_MUSICAMPROGRAM, NULL, 0, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL);

			musicamInfo.selectServiceNo = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_MUSICAM, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL, 0, NULL);
			{
			FILE  * f_data = fopen("\\Temp\\musicam.dat", "wb");
NEXT_MUSICAM_INDEX:
			DWORD testCount = 0;
			while(1) {
				Sleep(10);
				DDB_IOControl(NULL, IOCTL_DDBSDK_GET_MUSICAM_DATA, NULL, 0, buffer, size, &BytesReturned);
				getCount = BytesReturned;
				testCount ++;
				if(getCount > 0) fwrite(buffer, 1, getCount, f_data);
				if((testCount & 0x7) == 0) {
					printf("count: %d, bytes: %d\n", testCount, getCount);
				}

				if(testCount > 100000) {
					break;
				}
			}
			musicamInfo.selectServiceNo ++;
			if(musicamInfo.selectServiceNo >= musicamInfo.ServiceNum)
				musicamInfo.selectServiceNo = 0;
			DDB_IOControl(NULL, IOCTL_DDBSDK_CHANGE_MUSICAM_CHAL, (unsigned char *)&musicamInfo, sizeof(ddbsdk_dmbmusicamInfo_t), NULL, 0, NULL);
			printf("change to channel %d\n", musicamInfo.selectServiceNo);
			goto NEXT_MUSICAM_INDEX;

			fclose(f_data);
			}
			DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_MUSICAM, NULL, 0, NULL, 0, NULL);
			}
			break;
		case 'r':
			break;
		case 'h':
			{
			ddbsdk_versionInfo_t  verInfo;
			DDB_IOControl(NULL, IOCTL_DDBSDK_VERSIONINFO, NULL, 0, (unsigned char *)&verInfo, sizeof(verInfo), NULL);
			usagePrint(verInfo.ddbsdk_version);
			}
			break;
		case 's':
			{
			ddbsdk_versionInfo_t  verInfo;
			DDB_IOControl(NULL, IOCTL_DDBSDK_VERSIONINFO, NULL, 0, (unsigned char *)&verInfo, sizeof(verInfo), NULL);
			printf("ddbsdk version: %x.%x.%x\n", (verInfo.ddbsdk_version >> 16) & 0xFF, 
				(verInfo.ddbsdk_version >> 8) & 0xFF, verInfo.ddbsdk_version & 0xFF);
			printf("CA version: %x.%x.%x.%x\n", (verInfo.ca_version >> 24) & 0xFF, 
				(verInfo.ca_version >> 16) & 0xFF, (verInfo.ca_version >> 8) & 0xFF, verInfo.ca_version & 0xFF);
			printf("CA Info: cardNo-%d, manuName-%s, tvsName-%s\n", verInfo.ca_card_no, verInfo.ca_ManuName, verInfo.ca_TvsName);
			printf("  entitleTime-%d年第%d天, startTime-%d年第%d天, endTime-%d年第%d天\n", 
				get_time_t_year(verInfo.ca_entitleTime), get_time_t_day(verInfo.ca_entitleTime),
				get_time_t_year(verInfo.ca_startTime), get_time_t_day(verInfo.ca_startTime),
				get_time_t_year(verInfo.ca_endTime), get_time_t_day(verInfo.ca_endTime));
			}
			break;
		case 'p':
			break;
		case 't':
			{
			ddbsdk_reconfigure_t param;
			param.frequency = 1456384000;
			param.testInSMGLabFlag = 1;
			DDB_IOControl(NULL, IOCTL_DDBSDK_RECONFIGURE, (unsigned char *)&param, sizeof(ddbsdk_reconfigure_t), NULL, 0, NULL);
			printf("set freq: %d \n", param.frequency);
			DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DMBDEVICE, NULL, 0, NULL, 0, NULL);
			deviceOpen = true;
			printf("----------open device!--------------\n");
			}
			break;
		case 'x':
			file_test_mode = 1;
			DDB_IOControl(NULL, IOCTL_DDBSDK_SET_FILEINPUT_TEST_MODE, NULL, 0, NULL, 0, NULL);
			break;
		default:
			usagePrint(0x00000302);
			goto APP_EXIT;
		}
	}
	DDB_Close(NULL);
	DDB_Deinit(0);
APP_EXIT:

#if 0

#if DDBSDK_SERVICEFILE_DEBUG_FILE
	/* open ts file */
	FILE * fp;
	int   flen, pos;
	unsigned char * tempTsBuffer;
	fp = fopen("\\MMC_Storage\\test2.ts", "rb");

	/* initiate service */
	/* initiate to start serviceFIle download service */
	ddbsdk.hServiceFile = new ddbsdk_serviceFile;
	if(ddbsdk.hServiceFile) {
		((ddbsdk_serviceFile *)ddbsdk.hServiceFile)->initServiceFileData();
	}
	ddbsdk.serviceFile_open = 1;
	tempTsBuffer = (unsigned char *)malloc(TEMPBUFSIZE);

	while(1) {
		Sleep(30);
		/* read ts file to test service file download */
		flen = fread(tempTsBuffer, 1, TEMPBUFSIZE, fp);

		/* call datacallback to transfer data */
		DataCallback(NULL, 0, tempTsBuffer, flen);

		pos = ftell(fp);
		if(feof(fp)) break;
		if(pos > 400000) {
			Sleep(10);
		}
	}
	free(tempTsBuffer);
	delete ddbsdk.hServiceFile;
	ddbsdk.hServiceFile = 0;
	fclose(fp);
#endif


#if ! DDBSDK_SERVICEFILE_DEBUG_FILE
	DDB_Init(NULL, 0);

	DDB_Open(0, 0, 0);

	/* allocate memory for data transfer */
	size = 100 * 1024;
	buffer = (PBYTE)malloc(size);
	memset(buffer, 0, size);
#endif

#if 0
	ddbsdk_reconfigure_t param;
	param.frequency = 1458096000;
	DDB_IOControl(NULL, IOCTL_DDBSDK_RECONFIGURE, NULL, 0, NULL, 0, NULL);
#endif

#if 0  /* finance program */
	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_FINANCE, NULL, 0, NULL, 0, NULL);

	{
	FILE  * f_data = fopen("\\Temp\\finance.dat", "wb");
	DWORD testCount = 0;
	while(1) {
		if(debug_exit == true) break;
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_FINANCE, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount >0 ) fwrite(buffer, 1, getCount, f_data);
		if((testCount & 0x7) == 0) {
			printf("count: %d, bytes: %d\n", testCount, getCount);
		}
	}
	fclose(f_data);
	}

	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_FINANCE, NULL, 0, NULL, 0, NULL);
#endif

#if 0  /* traffic program */
	DDB_IOControl(NULL, IOCTL_DDBSDK_OPEN_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);
	{
	FILE  * f_data = fopen("\\Temp\\traffic.dat", "wb");
	DWORD testCount = 0;
	while(1) {
		if(debug_exit == true) break;
		DDB_IOControl(NULL, IOCTL_DDBSDK_GET_DATA_TRAFFIC, NULL, 0, buffer, size, &BytesReturned);
		getCount = BytesReturned;
		testCount ++;
		if(getCount >0 ) fwrite(buffer, 1, getCount, f_data);
		if((testCount & 0x7) == 0) {
			printf("count: %d, bytes: %d\n", testCount, getCount);
		}
	}
	fclose(f_data);
	}
	DDB_IOControl(NULL, IOCTL_DDBSDK_CLOSE_DATA_TRAFFIC, NULL, 0, NULL, 0, NULL);
#endif
#if ! DDBSDK_SERVICEFILE_DEBUG_FILE
	DDB_Close(NULL);

	DDB_Deinit(0);
#endif

#endif
	return 0;
}

