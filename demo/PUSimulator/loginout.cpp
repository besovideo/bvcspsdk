#include "loginout.h"
#include <BVCSP.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "GPSDialog.h"
#include "AVDialog.h"
#include "FileTransfer.h"
#include "Command.h"

BVCSP_HSession g_hSession = NULL;
char szDlgPUID[] = "PU_1";

void OnEvent(BVCSP_HSession hSession, int iEventCode, void* pParam)
{
    int iResult = (int)(pParam);
    printf("session event hSession:%p iEventCode:%d result:%d \n", hSession, iEventCode, iResult);
    if (BVCSP_EVENT_SESSION_OPEN == iEventCode && iResult == BVCU_RESULT_S_OK)
    {  // 登录成功，注册解码器信息。
    }
    else
        g_hSession = NULL;
}

BVCU_Result OnCommand(BVCSP_HSession hSession, BVCSP_Command* pCommand)
{
    if (g_hSession == hSession)
    {
        BV_OnCommand(hSession, pCommand);
    }
    return BVCU_RESULT_E_FAILED;
}

BVCU_Result OnNotify(BVCSP_HSession hSession, BVCSP_NotifyMsgContent* pData)
{
    return BVCU_RESULT_E_FAILED;
}

BVCU_Result OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam)
{
    printf("OnDialogCmd  iEventCode:%d \n",iEventCode);
    if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
    {
        if (pParam->iAVStreamDir & BVCU_MEDIADIR_DATASEND)
        {
            if (pParam->stTarget.iIndexMajor >= BVCU_SUBDEV_INDEXMAJOR_MIN_GPS && pParam->stTarget.iIndexMajor <= BVCU_SUBDEV_INDEXMAJOR_MAX_GPS)
            {
                return GPS_OnDialogCmd(hDialog, iEventCode, pParam);
            }
            else if (pParam->stTarget.iIndexMajor == BVCU_SUBDEV_INDEXMAJOR_DOWNLOAD)
            {
                return FileTransfer_OnDialogCmd(hDialog, iEventCode, pParam);
            }
        }
        else if (pParam->iAVStreamDir & BVCU_MEDIADIR_AUDIOSEND)
        {
            if (pParam->stTarget.iIndexMajor >= BVCU_SUBDEV_INDEXMAJOR_MIN_CHANNEL && pParam->stTarget.iIndexMajor <= BVCU_SUBDEV_INDEXMAJOR_MAX_CHANNEL)
            {
                return AV_OnDialogCmd(hDialog, iEventCode, pParam);
            }
        }
    }
    return BVCU_RESULT_E_FAILED;
}


void loginout(const char* szServerAddr, int iServerPUPort)
{
    if (g_hSession == NULL)
    {
        //登录
        //InitDecInfo();
        BVCSP_SessionParam sesParam;
        memset(&sesParam, 0x00, sizeof(sesParam));
        sesParam.iSize = sizeof(sesParam);
        sesParam.iClientType = BVCSP_CLIENT_TYPE_PU;
        sesParam.iTimeOut = 8000;
        strcpy(sesParam.szClientID, szDlgPUID);
        strcpy(sesParam.szServerAddr, szServerAddr);
        strcpy(sesParam.szUserAgent, "bvcsp_test");
        sesParam.iServerPort = iServerPUPort;

        sesParam.OnEvent = OnEvent;
        sesParam.OnCommand = OnCommand;
        sesParam.OnNotify = OnNotify;
        sesParam.OnDialogCmd = OnDialogCmd;

        {  // PU
            static BVCU_PUCFG_DeviceInfo g_deviceInfo;
            static BVCU_PUCFG_ChannelInfo g_channelInfo[4];
            memset(&g_deviceInfo, 0x00, sizeof(g_deviceInfo));
            memset(g_channelInfo, 0x00, sizeof(g_channelInfo));
            strcpy(g_deviceInfo.szName, "fhj_PU");
            g_deviceInfo.iAudioInCount = 1;
            g_deviceInfo.iAudioOutCount = 1;
            g_deviceInfo.iChannelCount = 3;
            g_deviceInfo.iGPSCount = 1;

            //audio
            g_channelInfo[0].iChannelIndex = BVCU_SUBDEV_INDEXMAJOR_MIN_CHANNEL;
            g_channelInfo[0].iMediaDir = BVCU_MEDIADIR_AUDIOSEND | BVCU_MEDIADIR_VIDEOSEND | BVCU_MEDIADIR_AUDIORECV;
            strcpy(g_channelInfo[0].szName, "audio");

            /*g_channelInfo[1].iChannelIndex = BVCU_SUBDEV_INDEXMAJOR_MIN_CHANNEL + 1;
            g_channelInfo[1].iMediaDir = BVCU_MEDIADIR_AUDIORECV;
            strcpy(g_channelInfo[1].szName, "audiorecv");*/
            //gps
            /*g_channelInfo[1].iChannelIndex = BVCU_SUBDEV_INDEXMAJOR_MIN_GPS;
            g_channelInfo[1].iMediaDir = BVCU_MEDIADIR_DATASEND;
            strcpy(g_channelInfo[1].szName, "gps");*/

            sesParam.stEntityInfo.pPUInfo = &g_deviceInfo;
            sesParam.stEntityInfo.pChannelList = g_channelInfo;
            sesParam.stEntityInfo.iChannelCount = 1;
        }

        BVCU_Result bResult = BVCSP_Login(&g_hSession, &sesParam);
        printf("BVCSP_Login: %d\n", bResult);
    }
    else
    {
        //登出
        if (g_hSession)
        {
            BVCSP_SessionInfo tSessionInfo;
            BVCSP_GetSessionInfo(g_hSession, &tSessionInfo);
            printf("hSession: %p  type: %d \n", g_hSession, tSessionInfo.stParam.iClientType);
            printf("Server: name-\"%s\" version-\"%s\" \n", tSessionInfo.szServerName, tSessionInfo.szServerVersion);
        }
        BVCSP_Logout(g_hSession);

        g_hSession = NULL;
    }
}