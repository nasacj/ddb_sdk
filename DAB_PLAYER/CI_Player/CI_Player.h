#ifndef _CI_PLAYER_H_
#define _CI_PLAYER_H_

#include "CI_SDK_Playback.h"

class CI_Player
{
private:
	ciHandle			m_PlaybackSDK;
	PLAY_CALLBACK_ARRAY	m_CallBacks;
	CI_STATUS			m_iPlayStatus;
	//TCHAR				m_szPlayFilename[256];
	//TCHAR				m_szPlayFileExt[256];
	//CI_CLIP_INFO		m_ClipInfo;
	int					m_nCurPlaybackPos;
	int					m_nVolume;
	HWND				m_VRHandle;

public:
	CI_Player();
	virtual ~CI_Player();

	ciRESULT InitialPlayer(PFNCI_PLAYER_CALLBACK);
	void ResetPlayer();
	void DeInitialPlayer();
	ciRESULT PlayDMBFlow(ci32s leftside, ci32s topside, ci32s rightside, ci32s bottomside, void* flowType);
	int GetVolumeValue();
	void SetVolumeValue(int);
	void SetScreen(ci32s $leftside, ci32s $topside, ci32s $rightside, ci32s $bottomside);

};

#endif