/***************************************************************************
 *   File Name: CI_SDK_Playback.h
 *   Create Date: 2007-01-01 (YYYY-MM-DD)
 *   Vesion: 01.00.00 (xx.xx.xx)
 *   Copyright (C) 2007 by Cidana(ShangHai)                                      
 *   Email: dicky.qi@cidana.com
 *          wyvern.zhao@cidana.com                                                     
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *                          
 *   The above copyright notice and this permission notice shall be 
 *   included in all copies or substantial portions of the Software.
 *                                                                         
 *   You should have received a copy of the Cidana License     
 *   along with this program; if not, write to the                         
 *   ShangHai Cidana, Inc.                                       
 ***************************************************************************/



#ifndef _CI_SDK_PLAYBACK_H_
#define _CI_SDK_PLAYBACK_H_

#include "CI_SDK_Typedef.h"

/* **************************************************************************************************
 ****************************************************************************************************
 *			Player interface		  
 * Description:
 *	This interface defines the player control and other 
 *	configuration methods of player for the 
 *	application layer.
 ****************************************************************************************************
 ****************************************************************************************************/

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Initialize
 * Description: Initialize the handle of player object.
 * Parameters: 
 *	ppPlayer 	[in/out]:
 *		The handle of player object. Input a pointer of handle, the returned handle value 
 *		will be the initialized player object handle.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 * Notes:
 *	The initial interface must be called once ahead of other player interfaces, and the returned 
 *	player object handle should pass to all other implemented player interface.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Initialize(ciHandle* ppPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Destroy
 * Description:
 *	Destroy the handle of player object.
 * Parameters:
 *	ppPlayer 	[in]:
 *		Player object handle created by CI_Player_Initialize interface
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 * Notes:
 *	The destroy interface must be called while exiting player program.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Destroy(ciHandle* ppPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Open
 * Description:
 *	Open the clip via a specified URL.
 * Parameters:
 * 	pPlayer 	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * 	pFileName	[in]:
 * 		The pFileName is the specified file including whole path. In the future, it will support
 *		standard URL format.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Open(ciHandle pPlayer, ciVoid* pFileName);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Open_TimeOut
 * Description:
 *	Open the clip via a specified URL with Time out limit.
 * Parameters:
 * 	pPlayer 	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * 	pFileName	[in]:
 * 		The pFileName is the specified file including whole path. In the future, it will support
 *		standard URL format.
 *  nTimeOutMs  [in]:
 *		nTimeOutMs is the timeout parameter, if the open procedure use too much time, it will return
 *		fail. this parameter is in millisecond
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Open_TimeOut(ciHandle pPlayer, ciVoid* pFileName, ci32u nTimeOutMs);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Close
 * Description:
 *	Close the clip resource, the state will switch to initial status.
 * Parameters:
 * 	pPlayer 	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 * Notes:
 *	Call this function to close the media stream and release related resources.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Close(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_GetClipInfo
 * Description:
 *	Get the general information of media clip.
 * Parameters:
 * 	pPlayer 	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * 	szURL 		[in]:
 *		The clip URL , NULL to get info of current opened clip.
 *	lpClipInfo 	[out]:
 *		The general info of specified media clip.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_GetClipInfo(ciHandle pPlayer, ciVoid* szURL, LPCI_CLIP_INFO lpClipInfo);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_GetDeviceInfo
 * Description:
 *	Get the general information of devide.
 * Parameters:
 *	pPlayer		 	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	lpDeviceInfo 	[out]:
 *		The general info of current device.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code
 *************************************************************************************************/
CI_API ciRESULT CI_Player_GetDeviceInfo(ciHandle pPlayer, LPCI_DEVICE_INFO lpDeviceInfo);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_RegisterPlayerCallback
 * Description:
 *	Register player callback function Return value is the error code.
 * Parameters:
 *	pPlayer		   	[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	pCallbackList	[in]:
 *		The Callback function pointer list.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_RegisterPlayerCallback(ciHandle pPlayer, void* pCallbackList);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Play
 * Description:
 *	Start to playback the opened clip or paused clip.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Play(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_FastForward
 * Description:
 *	Turn current playback into fast forward mode.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_FastForward(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Pause
 * Description:
 *	Try to pause the playing clip.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Pause(ciHandle pPlayer);

/**************************************************************************************************
* Function declare:
*	CI_Player_Shifting
* Description:
*	This interface is supposed to be used when lcing playback a stream. During time shifting,
*   the playback is paused, but we still receive stream data and store them in some temporal
*   files. Once we resume the playing, we can carry on the program we paused it.
* Parameters:
*	pPlayer		[in]:
*		Player object handle created by CI_Player_Initialize interface.
* Return value:
*	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
*************************************************************************************************/
CI_API ciRESULT CI_Player_Shifting(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Pause
 * Description:
 *	Try to pause the playing clip.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Resume(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_Stop
 * Description:
 *	Try to stop the playing clip.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_Stop(ciHandle pPlayer);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_SetPosition
 * Description:
 *	Seek the clip to the specified position.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	dwPosition	[in]:
 *		The position in ms which wants to seek to.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_SetPosition(ciHandle pPlayer, ci32u dwPosition);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_GetPosition
 * Description:
 *	Get the current position of clip.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	dwPosition	[out]:
 *		The current playback position.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_GetPosition(ciHandle pPlayer, ci32u* dwPosition);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_GetPlaybackStatus
 * Description:
 *	Get the current status of player.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	dwStatus	[out]:
 *		The current playback status. Such as PLAYING, PAUSE, STOP
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_GetPlaybackStatus(ciHandle pPlayer, ci32u* dwStatus);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_SetConfig
 * Description:
 *	Set the specified configuration to player.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	dwCfgType	[in]:
 *		The configuration type.
 *	pValue		[in]:
 *		The value, and it is correlative to specified configuration type.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code.
 *************************************************************************************************/
CI_API ciRESULT CI_Player_SetConfig(ciHandle pPlayer, PLAYER_CFG_TYPE dwCfgType , ciVoid* pValue);

/**************************************************************************************************
 * Function declare:
 *	CI_Player_GetConfig
 * Description:
 *	Get the specified configuration of player.
 * Parameters:
 *	pPlayer		[in]:
 *		Player object handle created by CI_Player_Initialize interface.
 *	dwCfgType	[in]:
 *		The configuration type.
 *	pValue		[out]:
 *		The value, and it is correlative to specified configuration type.
 * Return value:
 *	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code. 
 *************************************************************************************************/
CI_API ciRESULT CI_Player_GetConfig (ciHandle pPlayer, PLAYER_CFG_TYPE dwCfgType , ciVoid* pValue);

/*************************************************************************************************
* Function declare:
*	CI_Get_ID3DataFromFile
* Description:
*	Get the specified ID3 tag information of file.
* Parameters:
*	filename	[in]:
*		input file path and name.
*	ciIdTag3	[out]:
*		ID3 tag information of input file.
* Return value:
*		CI_ID3_ERROR if tag v1 and v2 both not exist,
*		CI_ID3V1_EXIST if only tag v1 exist,	
*       CI_ID3V2_EXIST if only tag v2 exist,
*		CI_ID3V1V2_EXIST if both v1 and v2 exist
*************************************************************************************************/
CI_API CI_ID3_STATUS CI_Get_ID3DataFromFile(ciVoid *filename, LPCI_IDTag3_Data ciIdTag3);

/*************************************************************************************************
* Function declare:
*	CI_ID3_Close
* Description:
*	Release the resource used by ID3.
* Parameters:
*		None.
*************************************************************************************************/
CI_API ciRESULT CI_ID3_Close(void);

/*************************************************************************************************
* Function declare:
*	CI_Get_MediaFilePreview
* Description:
*	Get preview of the specified file.
* Parameters:
*	LPPreviewOption	    [in]:
*		input preview option
*	pBuf				[out]:
*		If pBuf is NULL, memory size will be returned through cbSize.Else it should be the buffer 
*		address.
*	cbSize			    [in/out]:
*		Buffer size
* Return value:
*	CI_ERR_NONE if success. !CI_ERR_NONE if any error occurs. Return value is the error code. 
*************************************************************************************************/
CI_API ciRESULT CI_Get_MediaFilePreview(LPPreviewOption ciPreviewOpt, ci8u* pBuf, ci32u *cbSize);

#ifdef __APPLE__
/*************************************************************************************************
 YUV to RGB Funcs
 *************************************************************************************************/
ciRESULT yuv2rgb_c_init(int isRgb, int bpp, int fullRange, int brightness, int contrast, int saturation);
ciRESULT yuv2rgb_c_uninit();
ciRESULT yuv2rgb_c_32(unsigned char* src[], int srcStride[], int srcSliceY, int srcSliceH, unsigned char* dst, int dstStride, int dst_w);
ciRESULT yuv2rgb_c_24_rgb(unsigned char* src[], int srcStride[], int srcSliceY, int srcSliceH, unsigned char* dst[], int dstStride[], int dst_w);
ciRESULT yuv2rgb_c_24_bgr(unsigned char* src[], int srcStride[], int srcSliceY, int srcSliceH, unsigned char* dst[], int dstStride[], int dst_w);
ciRESULT yuv2rgb_c_16(unsigned char* src[], int srcStride[], int srcSliceY, int srcSliceH, unsigned char* dst, int dstStride, int dst_w);
#endif

#endif  // ~_CI_SDK_PLAYBACK_H_


