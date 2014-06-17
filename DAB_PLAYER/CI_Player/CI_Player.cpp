#include "stdafx.h"
#include "CI_Player.h"


// ciRESULT GetStreamData(void * wParam, void * lParam, void * bHandled, void* pvContext)
// {
// 	ciRESULT ret = 0;
// 	int length;
// 	long*  pRetVal = (long*)wParam;
// 	unsigned char*  pECMBuff = (unsigned char*)lParam;
// 	ret = length;
// 	*((long*)bHandled) = length;
// 	Sleep(100);
// 	return ret;
// }

CI_Player::CI_Player()
{
	m_PlaybackSDK = NULL;
	memset (&m_CallBacks, 0, sizeof(m_CallBacks));
	//memset (m_szPlayFilename, 0, 256);
	//memset (m_szPlayFileExt, 0, 256);
	//memset (&m_ClipInfo, 0, sizeof(m_ClipInfo));
	m_nCurPlaybackPos = 0;
	m_nVolume = 48;
	m_VRHandle = 0;
}

CI_Player::~CI_Player()
{

}

ciRESULT CI_Player::InitialPlayer(PFNCI_PLAYER_CALLBACK getStreamFun)
{
	ciRESULT ret = CI_Player_Initialize(&m_PlaybackSDK);
	m_CallBacks.GetStreamData = getStreamFun;
	return ret;
}

ciRESULT CI_Player::PlayDMBFlow(ci32s leftside, ci32s topside, ci32s rightside, ci32s bottomside, void* flowType)
{
	ciRESULT  ret = 0;
	int video_enable = 0;

	CI_Player_GetPlaybackStatus(m_PlaybackSDK, (ci32u*)&m_iPlayStatus);
	CI_Player_SetConfig(m_PlaybackSDK,CI_CFG_VIDEO_RENDER,&video_enable);
	if (m_PlaybackSDK != NULL && m_iPlayStatus == CI_RUNNING)
	{
		ret = CI_Player_Pause(m_PlaybackSDK);
		if (ret)
		{
			return ret;
		}
	}
	CI_Player_Close(m_PlaybackSDK);

	CI_Player_RegisterPlayerCallback(m_PlaybackSDK, &m_CallBacks);

	ciRESULT ciRet = CI_Player_Open_TimeOut(m_PlaybackSDK, flowType,5000); 
	//ciRESULT ciRet = CI_Player_Open(m_PlaybackSDK, TEXT("dmb")); 
	if (ciRet != CI_ERR_NONE)
	{
		return ciRet;
	}

	// enable video render
	video_enable = 1;
	CI_Player_SetConfig(m_PlaybackSDK,CI_CFG_VIDEO_RENDER,&video_enable);

	/*CI_Player_GetClipInfo(m_PlaybackSDK,NULL,&m_ClipInfo);*/

	//set display area
	CI_DISPLAY_PARAM  DisplayPara;
	memset((void*)&DisplayPara,0, sizeof(DisplayPara));
	CI_RECT  rc_Clip = {0, 0, 320, 240};
	rc_Clip.left = leftside;
	rc_Clip.top = topside;
	rc_Clip.right = rightside;
	rc_Clip.bottom = bottomside;
	DisplayPara.rectClip = rc_Clip;
	CI_Player_SetConfig(m_PlaybackSDK, CI_CFG_DISPLAY_PARAM, (ciVoid *)&DisplayPara);

	//设置音量：默认为 48 (0-100)
	CI_Player_SetConfig(m_PlaybackSDK, CI_CFG_AUDIO_VOLUME2, (ciVoid *)&m_nVolume);

	//set play status
	CI_Player_GetPlaybackStatus(m_PlaybackSDK, (ci32u*)&m_iPlayStatus);
	if (m_iPlayStatus == CI_STOPPED || m_iPlayStatus == CI_PAUSED)
	{
		ret = CI_Player_Play(m_PlaybackSDK);
		if (ret)
		{
			return ret;
		}
		m_iPlayStatus = CI_RUNNING;
	}
	return ret;
}

int CI_Player::GetVolumeValue()
{
	CI_Player_GetConfig(m_PlaybackSDK, CI_CFG_AUDIO_VOLUME2, (ciVoid *)&m_nVolume);
	return m_nVolume;
}

void CI_Player::SetVolumeValue(int volumeValue)
{
	m_nVolume = volumeValue;
	if (m_nVolume > 100)
	{
		m_nVolume = 100;
	}
	if(m_nVolume < 0)
	{
		m_nVolume = 0;
	}
	CI_Player_SetConfig(m_PlaybackSDK, CI_CFG_AUDIO_VOLUME2, (ciVoid *)&m_nVolume);
}

void CI_Player::DeInitialPlayer()
{
	if(m_PlaybackSDK)
	{
		CI_Player_Stop(m_PlaybackSDK);
		CI_Player_Close(m_PlaybackSDK);
		CI_Player_Destroy(&m_PlaybackSDK);
	}

}

void CI_Player::ResetPlayer()
{
	TRACE(L"重置播放器\n");
	ciRESULT  ret = 0;
	int video_enable = 0;

	CI_Player_GetPlaybackStatus(m_PlaybackSDK, (ci32u*)&m_iPlayStatus);
	CI_Player_SetConfig(m_PlaybackSDK,CI_CFG_VIDEO_RENDER,&video_enable);
	if (m_PlaybackSDK != NULL && m_iPlayStatus == CI_RUNNING)
	{
		ret = CI_Player_Pause(m_PlaybackSDK);
		if (ret)
		{
			return;
		}
	}
	//CI_Player_Close(m_PlaybackSDK);
}

void CI_Player::SetScreen(ci32s $leftside, ci32s $topside, ci32s $rightside, ci32s $bottomside)
{
	CI_DISPLAY_PARAM  DisplayPara;
	CI_RECT  rc_Clip = {0};
	memset((void*)&DisplayPara,0, sizeof(DisplayPara));
	rc_Clip.left = $leftside;
	rc_Clip.top = $topside;
	rc_Clip.right = $rightside;
	rc_Clip.bottom = $bottomside;
	DisplayPara.rectClip = rc_Clip;
	CI_Player_SetConfig(m_PlaybackSDK, CI_CFG_DISPLAY_PARAM, (ciVoid *)&DisplayPara);
	//TRACE(L"Full Screen!\n");
}
