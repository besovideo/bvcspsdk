// BVCSPTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h> 
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "BVCSP.h"
#include "DecConfig.h"
#include "BVAuth.h"

#ifdef _MSC_VER
    #ifdef _WIN64
        #ifdef _DEBUG
            #pragma comment(lib, "..\\..\\lib\\BVCSP_x64.lib")
        #else
            #pragma comment(lib, "..\\..\\lib\\BVCSP_x64.lib")
        #endif
    #elif _WIN32
        #ifdef _DEBUG
            #pragma comment(lib, "..\\..\\lib\\BVCSP.lib")
        #else
            #pragma comment(lib, "..\\..\\lib\\BVCSP.lib")
        #endif
    #endif // _WIN32
#endif // _MSC_VER


/*
char szServerAddr[] = "61.191.27.18";
char szUserName[] = "root";
char szPassword[] = "besovideo88";
char szDlgPUID[] = "PU_55AA0000";*/

char szServerAddr[] = "127.0.0.1";
char szUserName[] = "admin";
char szPassword[] = "123456";
char szDlgPUID[] = "PU_55AA0008";
int  iServerCUPort = 9701;
int  iServerPUPort = 9702;
int  iCUCmdProtoType = BVCU_PROTOTYPE_TCP;
int  iPUCmdProtoType = BVCU_PROTOTYPE_UDP;
int  iDlgChannelIndex = 0;

BVCSP_HSession g_hSession = NULL;
BVCSP_HDialog g_hDialog = NULL;

// 解码器 
BVCU_DECCFG_DeviceInfo g_decInfo;
BVCU_DECCFG_Options g_decOptions;
BVCU_DECCFG_DecoderParam g_decDecoder;
BVCU_DECCFG_DisplayParam g_decDisplay;
BVCU_DECCFG_DataChanInfo g_decChannel[2];
void InitDecInfo()
{
    memset(&g_decInfo, 0, sizeof(g_decInfo));
    strcpy(g_decInfo.szManufacturer, "fhj test decoder");
    strcpy(g_decInfo.szProductName, "fhj test decoder");
    strcpy(g_decInfo.szSoftwareVersion, "fhj test decoder");
    strcpy(g_decInfo.szHardwareVersion, "fhj test decoder");
    strcpy(g_decInfo.szName, "fhj test decoder");
    g_decInfo.iLanguage[0] = BVCU_LANGUAGE_ENGLISH;
    g_decInfo.iLanguage[1] = BVCU_LANGUAGE_CHINESE_SIMPLIFIED;
    g_decInfo.iLanguage[2] = BVCU_LANGUAGE_CHINESE_TRADITIONAL;
    g_decInfo.iPUType = BVCU_PUTYPE_DECODER;
    g_decInfo.iDecChanCount = 4;
    g_decInfo.iDispChanCount = 1;
    g_decInfo.iOnlineThrough = BVCU_PU_ONLINE_THROUGH_ETHERNET;
    g_decInfo.iBootDuration = 0;

    g_decInfo.stVgaSupportResolution[0].iHeight = 1080;
    g_decInfo.stVgaSupportResolution[0].iWidth = 1920;
    g_decInfo.stVgaSupportResolution[1].iHeight = 900;
    g_decInfo.stVgaSupportResolution[1].iWidth = 1600;
    g_decInfo.stVgaSupportResolution[2].iHeight = 720;
    g_decInfo.stVgaSupportResolution[2].iWidth = 1280;

    g_decInfo.stHdmiSupportResolution[0].iHeight = 1080;
    g_decInfo.stHdmiSupportResolution[0].iWidth = 1920;
    g_decInfo.stHdmiSupportResolution[1].iHeight = 900;
    g_decInfo.stHdmiSupportResolution[1].iWidth = 1600;
    g_decInfo.stHdmiSupportResolution[2].iHeight = 720;
    g_decInfo.stHdmiSupportResolution[2].iWidth = 1280;

    g_decInfo.stDviSupportResolution[0].iHeight = 1080;
    g_decInfo.stDviSupportResolution[0].iWidth = 1920;
    g_decInfo.stDviSupportResolution[1].iHeight = 900;
    g_decInfo.stDviSupportResolution[1].iWidth = 1600;
    g_decInfo.stDviSupportResolution[2].iHeight = 720;
    g_decInfo.stDviSupportResolution[2].iWidth = 1280;

    g_decInfo.stYpbprSupportResolution[0].iHeight = 1080;
    g_decInfo.stYpbprSupportResolution[0].iWidth = 1920;
    g_decInfo.stYpbprSupportResolution[1].iHeight = 900;
    g_decInfo.stYpbprSupportResolution[1].iWidth = 1600;
    g_decInfo.stYpbprSupportResolution[2].iHeight = 720;
    g_decInfo.stYpbprSupportResolution[2].iWidth = 1280;

    memset(&g_decOptions, 0x00, sizeof(g_decOptions));
    g_decOptions.iTimeOut = 30*1000; // 会话命令等超时时间，单位毫秒。
    g_decOptions.iSMSRetryCount = 0; // 服务器断线重连次数： -1：不断线重连。 0：断线一直尝试重连。或 >0（尝试次数）
    g_decOptions.iChanRetryCount = 0;// 数据通道断线重连次数： 值意义同 iSMSRetryCount
    g_decOptions.iRetryDelay = 60* 1000;    // 断线重连时间间隔。 单位:毫秒 参考值 60*1000
    g_decOptions.iCaptureVolume = 100; // 音频采集音量，正常范围0～100
    g_decOptions.bOSD = 1;      // 是否在显示中叠加信息。0-否 1-是
    g_decOptions.iDelayMax = 5000; // A/V数据从接收到输出，最大允许的延迟，不要超过10 000。单位：毫秒（参考值：5000）
    g_decOptions.iDelayMin = 1000; // A/V数据从接收到输出，最小允许的延迟，不要超过10 000。单位：毫秒（参考值：1000）
    g_decOptions.iDelayVsSmooth = 3; // 播放延迟与平滑选择。取值范围1～10，越大则播放越平滑，但延迟变大（参考值：3）

    memset(&g_decDecoder, 0x00, sizeof(g_decDecoder));
    g_decDecoder.iDecChanIndex = 0; // 解码通道号 从0开始，BVCU_DECCFG_DeviceInfo.iDecNums-1
    g_decDecoder.bDecodeEnable = 1; // 解码起停标志，0-停止，1-主动解码，2-远程回放（暂不实现），可写
    g_decDecoder.iPoolTime = 10*1000;     // 轮询间隔 单位：毫秒  可写
    g_decDecoder.iDataChanMaxCount = 2;   // 当前解码通道支持的数据通道数 最大值为BVCU_MAX_TOUR_IPCHANNEL_COUNT
    g_decDecoder.iDataChanCount = 1;      // 添加到列表的数据通道数量；即pDataChanInfo数组大小。  最大值为iDataChanMaxCount，可写
    g_decDecoder.pDataChanInfo = g_decChannel; //配置的数据通道列表，数组列表顺序为多路轮询的顺序。可写
    g_decDecoder.iDecodeStatus = 1;   //当前状态:0:未启动，1-正在主动解码，2-正在远程回放（暂不实现）
    memset(g_decChannel, 0x00, sizeof(g_decChannel));
    g_decChannel[0].bEnable = 1;  // 是否使能， 0-不使能， 1-使能。 
    g_decChannel[0].iDataChanType = BVCU_IPC_NET_PROTO_TYPE_SMARTEYE; // 通道协议类型， 见 BVCU_IPC_NET_PROTO_TYPE_*， 目前只支持BVCU_IPC_NET_PROTO_TYPE_SMARTEYE
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szAddr, "61.191.27.18");     //服务器IP或域名
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iPort = 9701;           //服务器端口
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iProto = BVCU_PROTOTYPE_TCP;         //使用的协议类型，TCP/UDP
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iKeepAliveTimeout = 60*1000; // 设备心跳超时时间，单位秒。超过该时间设备收不到心跳包回复，认为服务器下线。范围： 45-200
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szUserName,"besovideo"); // 登录用户名
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szPassword, "besovideo"); // 登录密码
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.szPUID, "PU_55AA0008");
    g_decChannel[0].stStatus.bOnline = 1; // 通道当前状态
    g_decChannel[0].stStatus.iCodeIDA = SAVCODEC_ID_G726;  // 音频编码格式  见 SAVCODEC_ID_*(SAVCodec.h)
    g_decChannel[0].stStatus.iFpsDecA = 25*10000;  // 音频解码帧率  单位1/10000帧每秒
    g_decChannel[0].stStatus.iDecodedA = 100000; // 已经解码的音频帧数
    g_decChannel[0].stStatus.iLostRateA = 5*10000;// 音频丢包(或帧)率，单位1/10000
    g_decChannel[0].stStatus.iKbpsA = 32;    // 音频数据码率，单位 Kbits/second
    g_decChannel[0].stStatus.iCodeIDV = SAVCODEC_ID_H264;  // 视频编码格式  见 SAVCODEC_ID_*(SAVCodec.h)
    g_decChannel[0].stStatus.iFpsDecV = 25*10000;  // 视频解码帧率  单位1/10000帧每秒
    g_decChannel[0].stStatus.iDecodedV = 2500000; // 已经解码的视频帧数
    g_decChannel[0].stStatus.iLostRateV = 5*10000;// 视频丢包(或帧)率，单位1/10000
    g_decChannel[0].stStatus.iKbpsV = 1*1024;    // 视频数据码率，单位 Kbits/second
    g_decChannel[0].stStatus.iImgW = 1280; // 解码通道当前的图像大小,宽
    g_decChannel[0].stStatus.iImgH = 720; // 高

    memset(&g_decDisplay, 0x00, sizeof(g_decDisplay));
    g_decDisplay.iDispChanIndex = 0; // 显示通道号 从0开始，BVCU_DECCFG_DeviceInfo.iDispNums-1
    g_decDisplay.iAudioWindowIdx = 0;// 音频开启子窗口,对应 iDecChanIndex下标, -1: 关闭音频。 可写
    g_decDisplay.iPlaybackVolume = 90;//播放音量，正常范围0～100
    g_decDisplay.iFullWindowIdx = 0;// 全屏某个子窗口,对应 iDecChanIndex下标, -1: 不全屏子窗口。 可写
    g_decDisplay.iDispFormatSup = BVCU_DISPLAY_FORMAT_VGA; // 支持的输出模式, 见 BVCU_DISPLAY_FORMAT_*
    g_decDisplay.stResolution.iHeight = 720; // 分辨率，根据iDispFormatSup从BVCU_DECCFG_DeviceInfo能力集获取。 可写
    g_decDisplay.stResolution.iWidth = 1280;
    g_decDisplay.iVideoFormat = BVCU_VIDEOFORMAT_NTSC;      // 见 BVCU_VIDEOFORMAT_*
    g_decDisplay.iWindowModeSup[0] = 1;// 支持的画面模式。1,2,4,6,8,9,10,13,14,16,17,19,22,25，请参考各厂家客户端窗口布局。0: 无效值。
    g_decDisplay.iWindowModeSup[1] = 2;
    g_decDisplay.iWindowModeSup[2] = 4;
    g_decDisplay.iWindowModeSup[3] = 6;
    g_decDisplay.iWindowModeSup[4] = 8;
    g_decDisplay.iWindowModeSup[5] = 9;
    g_decDisplay.iWindowModeSup[6] = 10;
    g_decDisplay.iWindowModeSup[7] = 13;
    g_decDisplay.iWindowModeSup[8] = 14;
    g_decDisplay.iWindowMode = 4;       // 当前使用的画面模式。值意义同iWindewModeSup。从iWindewModeSup中取。可写
    for (int i = 0; i < 4; ++i)
        g_decDisplay.iDecChanIndex[i] = i;
    for (int i = 4; i < BVCU_DEC_MAX_WINDOWS; ++ i)
        g_decDisplay.iDecChanIndex[i] = -1; // 各个子窗口关联的解码通道号。-1：无效值，没有关联解码通道的子窗口填-1。可写
    g_decDisplay.stStatus.iDispStatus = 1;     // 显示状态：0：未显示，1：启动显示
    g_decDisplay.stStatus.iDispFormat = BVCU_DISPLAY_FORMAT_VGA;     // 显示接口类型： 见 BVCU_DISPLAY_FORMAT_*
    g_decDisplay.stStatus.iVideoFormat = BVCU_VIDEOFORMAT_NTSC;    // 视频制式: 见 BVCU_VIDEOFORMAT_*
    g_decDisplay.stStatus.iWindowMode = 4;     // 画面模式: 参考BVCU_DECCFG_DisplayParam.iWindowModeSup
    for (int i = 0; i < 4; ++ i)
        g_decDisplay.stStatus.iFpsDisp[i] = 15*10000;  // 每个子画面的显示帧率 单位1/10000帧每秒
    g_decDisplay.stStatus.bFullScreenMode = 0; // 屏幕模式  0-普通 1-全屏模式
}

void CU_Dialog_OnEvent(BVCSP_HDialog hDialog, int iEventCode, BVCSP_Event_DialogCmd* pParam)
{
    int iResult = (int)(pParam);
    printf("CU Dialog event hDialog:%p iEventCode:%d result:%d \n", hDialog, iEventCode, iResult);
    if (iEventCode == BVCSP_EVENT_DIALOG_CLOSE || BVCU_Result_FAILED(iResult))
        g_hDialog = NULL;
}

#define H264OUTFILE 0
#if H264OUTFILE
static FILE* pOutH264 = NULL;
#endif
BVCU_Result CU_Dialog_afterRecv(BVCSP_HDialog hDialog, BVCSP_Packet* pPacket)
{
    static int iLastTime = 0;
    static int ifps = 0;
    if (pPacket->iDataType == BVCSP_DATA_TYPE_VIDEO)
    {
        int iNowTime = GetTickCount();
        if (iLastTime == 0 || iNowTime - iLastTime >= 1000)
        {
            printf(" video fps: %d \r\n", ifps);
            ifps = 0;
            iLastTime = iNowTime;
        }
        else
            ++ifps;
    }
    else if (pPacket->iDataType == BVCSP_DATA_TYPE_GPS)
    {
        BVCU_PUCFG_GPSData* pGPSData = (BVCU_PUCFG_GPSData*)(pPacket->pData);
        printf("GPS: lat: %d lng: %d time:%04d-%02d-%02d %02d-%02d-%02d bOK:%d \r\n", pGPSData->iLatitude, pGPSData->iLongitude,
            pGPSData->stTime.iYear, pGPSData->stTime.iMonth, pGPSData->stTime.iDay, pGPSData->stTime.iHour, pGPSData->stTime.iMinute, pGPSData->stTime.iMinute,
            pGPSData->bOrientationState);
    }
    else if (pPacket->iDataType == BVCSP_DATA_TYPE_TSP)
    {
        printf("TSP: len: %d data: %s\r\n", pPacket->iDataSize, pPacket->pData);
    }
#if H264OUTFILE
    if ((pPacket->iDataType == BVCSP_DATA_TYPE_VIDEO) && pPacket->bKeyFrame && !pOutH264)
            pOutH264 = fopen("./BVCSP.264", "wb");
    if ((pPacket->iDataType == BVCSP_DATA_TYPE_VIDEO) && pOutH264)
    {
        fwrite(pPacket->pData, 1, pPacket->iDataSize, pOutH264);
        printf("video frame : size-%d  bKey-%d \r\n", pPacket->iDataSize, pPacket->bKeyFrame);
    }
#else
    if ((pPacket->bKeyFrame && pPacket->iDataType == BVCSP_DATA_TYPE_VIDEO) || pPacket->bLostFrame)
    {
        printf("CU Dialog %p Recv Data: \n", hDialog);
        printf("    Type:%d  seq:%d ipts:%lld len:%d bKey:%d bLost:%d\n",
            pPacket->iDataType, pPacket->iSeq, pPacket->iPTS, pPacket->iDataSize, pPacket->bKeyFrame, pPacket->bLostFrame);
    }
#endif
    return BVCU_RESULT_S_OK;
}

void CU_Cmd_OnEvent(BVCSP_HSession hSession, BVCSP_Command* pCommand, BVCSP_Event_SessionCmd* pParam)
{
    printf("Cmd callback. method-%d  submethod-%d \n", pCommand->iMethod, pCommand->iSubMethod);
    if (pParam)
    {
        printf("    cmd result-%d percent-%d \n", pParam->iResult, pParam->iPercent);
    }
}
void OnSessionEvent(BVCSP_HSession hSession, int iEventCode, void* pParam)
{
    int iResult = (int)(pParam);
    printf("session event hSession:%p iEventCode:%d result:%d \n", hSession, iEventCode, iResult);
    printf("server ip:%s port: %d %d user/passwd: %s/%s \n", szServerAddr, iServerCUPort, iServerPUPort, szUserName, szPassword);
    if (BVCSP_EVENT_SESSION_OPEN == iEventCode && iResult == BVCU_RESULT_S_OK)
    {  // 登录成功，注册解码器信息。
        BVCSP_SessionInfo sesInfo;
        BVCSP_GetSessionInfo(hSession, &sesInfo);

        if (sesInfo.stParam.iClientType == BVCSP_CLIENT_TYPE_DEC)
        {
            strcpy(g_decInfo.szDeviceID, sesInfo.stParam.szClientID);
            BVCSP_Command cmd;
            memset(&cmd, 0, sizeof(cmd));
            cmd.iSize = sizeof(cmd);
            cmd.iMethod = BVCU_METHOD_CONTROL;
            cmd.iSubMethod = BVCU_SUBMETHOD_DEC_DEVICEINFO;
            strcpy(cmd.szTargetID, sesInfo.stParam.szClientID);
            cmd.stMsgContent.iDataCount = 1;
            cmd.stMsgContent.pData = &g_decInfo;
            cmd.OnEvent = CU_Cmd_OnEvent;
            BVCSP_SendCmd(hSession, &cmd);
        }
    }
    else
        g_hSession = NULL;
}
BVCU_Result CU_OnNotify(BVCSP_HSession hSession, BVCSP_NotifyMsgContent* pData)
{
    printf("On Notify callback. submethod-%d \n", pData->iSubMethod);
    if (pData->iSubMethod == BVCU_SUBMETHOD_PU_CHANNELINFO)
    {
        BVCU_PUCFG_PUChannelInfo* pPU = (BVCU_PUCFG_PUChannelInfo*)(pData->stMsgContent.pData);
        printf("设备上下线通知： PU(%s) status-%d \r\n", pPU->szPUID, pPU->iOnlineStatus);
    }
    return BVCU_RESULT_S_OK;
}
BVCU_Result CU_OnCommand(BVCSP_HSession hSession, BVCSP_Command* pCommand)
{
    BVCSP_Event_SessionCmd szResult;
    memset(&szResult, 0x00, sizeof(szResult));
    szResult.iPercent = 100;
    szResult.iResult = BVCU_RESULT_S_OK;
    if (pCommand->iMethod == BVCU_METHOD_QUERY)
    {
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DEVICEINFO)
        {
            szResult.stContent.iDataCount = 1;
            szResult.stContent.pData = &g_decInfo;
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_OPTIONS)
        {
            szResult.stContent.iDataCount = 1;
            szResult.stContent.pData = &g_decOptions;
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DECODER)
        {
            szResult.stContent.iDataCount = 1;
            szResult.stContent.pData = &g_decDecoder;
            g_decDecoder.iDecChanIndex = pCommand->iTargetIndex;
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DISPLAY)
        {
            szResult.stContent.iDataCount = 1;
            szResult.stContent.pData = &g_decDisplay;
            g_decDisplay.iDispChanIndex = pCommand->iTargetIndex;
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
    }
    else if (pCommand->iMethod == BVCU_METHOD_CONTROL)
    {
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DEVICEINFO)
        {
            if (pCommand->stMsgContent.iDataCount == 1 && pCommand->stMsgContent.pData)
            {
                g_decInfo.iLanguageIndex = ((BVCU_DECCFG_DeviceInfo*)pCommand->stMsgContent.pData)->iLanguageIndex;
                strcpy(g_decInfo.szName, ((BVCU_DECCFG_DeviceInfo*)pCommand->stMsgContent.pData)->szName);
            }
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_OPTIONS)
        {
            if (pCommand->stMsgContent.iDataCount == 1 && pCommand->stMsgContent.pData)
                g_decOptions = *((BVCU_DECCFG_Options*)pCommand->stMsgContent.pData);
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DECODER)
        {
            if (pCommand->stMsgContent.iDataCount == 1 && pCommand->stMsgContent.pData)
            {
                BVCU_DECCFG_DecoderParam* pDecoderParam = (BVCU_DECCFG_DecoderParam*)(pCommand->stMsgContent.pData);
                g_decDecoder = *pDecoderParam;
                g_decDecoder.iDataChanCount = pDecoderParam->iDataChanCount > 2 ? 2 : pDecoderParam->iDataChanCount;
                g_decDecoder.pDataChanInfo = g_decChannel;
                for (int i = 0; i < 2 && i < pDecoderParam->iDataChanCount; ++ i)
                {
                    g_decChannel[i] = pDecoderParam->pDataChanInfo[i];
                }
            }
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
        if (pCommand->iSubMethod == BVCU_SUBMETHOD_DEC_DISPLAY)
        {
            if (pCommand->stMsgContent.iDataCount == 1 && pCommand->stMsgContent.pData)
                g_decDisplay = *((BVCU_DECCFG_DisplayParam*)pCommand->stMsgContent.pData);
            pCommand->OnEvent(hSession, pCommand, &szResult);
            return BVCU_RESULT_S_OK;
        }
    }
    return BVCU_RESULT_E_FAILED;// 不接受这个命令处理
}


#if ENABLE_AUTH

void tpCmdCallBack(struct BVRCommond* pThis, struct BVRCommondRes* pResponse)
{
    printf("%s method: %d, result: %d\n ", __FUNCTION__, pResponse->iMethod, pResponse->iResult);
}

void PU_OnAuthEvent(BVRAuthParam* param, BVRAuthResult result, BVRPermission* permission, int pemsCount)
{
    printf("\n--------------- \n\
     result: %d                   \n\
     rand code: %d              \n\
     auth code: %d              \n\
     SerialNumber: %s             \n\
     issuer: %s                   \n\
     type: %s                     \n\
     id: %s                       \n\
     user_data: %d                \n\
     result: %s                   \n\
    ---------------\n",
        result,
        param->tagInfo.RandCode,
        param->tagInfo.AuthCode,
        param->SerialNumber,
        param->certInfo.IssueUser,
        param->termInfo.Type,
        param->termInfo.ID,
        (int)param->user_data,
        param->innerInfo.errstr);

    if (AUTH_RResult_Register_Wait == result) {
        int rc = BVCSP_Trial(param->tagInfo.AuthCode);
        printf("### trial %d\n\n", rc);
    }

    if (AUTH_Result_OK == result)
    {
         printf("AUTH_Result_OK \n");
    }
}
#endif

void test_auth()
{
#if ENABLE_AUTH
    BVRAuthParam param;
    memset(&param, 0, sizeof(BVRAuthParam));
    {
        param.iSize = sizeof(BVRAuthParam);
        param.OnAuthEvent = PU_OnAuthEvent;
        // 测试用的序列号，根据您认证情况填写。
        strcpy(param.SerialNumber, "term_25eb27712f9cc9d3b18caf6ea1531acc");
        {  // 设备硬件信息。
            strcpy(param.termInfo.Type, "PU");
            strcpy(param.termInfo.ID, "PU_12456");
            strcpy(param.termInfo.ModelNumber, "ModelNumber_123");
            strcpy(param.termInfo.MAC, "MAC_14156");
            strcpy(param.termInfo.IMEI, "IMEI_235328965jkljfkl");
            strcpy(param.termInfo.HardwareProvider, "HardwareProvider_sdjgkagjnsjakdfj");
            strcpy(param.termInfo.HardwareSN, "HardwareSN_GHK6BC290E7E4AF4B59DB24B576A8ABJ");
            strcpy(param.termInfo.HardwareVersion, "HardwareVersion_akgjk");
            strcpy(param.termInfo.SoftwareProvider, "SoftwareProvider_sadkgjaskl");
            strcpy(param.termInfo.SoftwareVersion, "SoftwareVersion_sdkgj");
            strcpy(param.termInfo.OSType, "OSType_214ui");
            strcpy(param.termInfo.OSVersion, "OSVersion_sajkdg");
            strcpy(param.termInfo.OSID, "OSID_sdajgk");
            strcpy(param.termInfo.CPU, "CPU_234i");
            strcpy(param.termInfo.Desc, "Desc_ad");
        }
    }
    {
        /**  开发者app key 密钥，需要向我司销售申请，这里这个key用于测试。
        type: PU
        app_id: app_7c5f1f420ae4431b
        rsa_n(hex): ad6645c5dcd9bb398716ea193411c5d7
        rsa_e(hex): 93487bd8b721095f
        */
        char* app_id = "app_xxxxxxxxxxxxxxxx";
        char* n = "94ab94e08d971e72a36f48841d66a151";
        char* e = "e942cfb03769cbd3";
        char data[256];
        int data_len = 0;
        char base64[512];
        int base64_len;

        char encrypt[512];
        int encrypt_len = 0;

        data_len = sizeof(data);
        memset(data, 0, sizeof(data));

        encrypt_len = sizeof(encrypt);
        memset(encrypt, 0, sizeof(encrypt));

        base64_len = sizeof(base64);
        memset(base64, 0, sizeof(base64));

        // 获取需要字符串
        BVCSP_GetEncryptedData(data, &data_len);
        // Base64(RSA(data))
        BVCU_Result bvresult = BVCSP_EncryptData(n, e, data, data_len, encrypt, &encrypt_len);
        if (BVCU_Result_SUCCEEDED(bvresult))
        {
            base64_len = base64_encode((const unsigned char*)encrypt, encrypt_len, base64);

            strcpy(param.appInfo.appId, app_id);
            param.appInfo.ciph_data = base64;
            param.appInfo.ciph_len = strlen(base64);

#if 0
            char outdata[512];
            int outdata_len = sizeof(outdata);
            BVCSP_EncryptData(n, e, data, data_len, outdata, &outdata_len);
            param.appInfo.ciph_data = outdata;
            param.appInfo.ciph_len = outdata_len;
#endif
        }
    }
    BVCSP_Auth(&param);
#endif
}

void loginout(int iType/* 0:pu 1:CU*/)
{
    // TODO: Add your control notification handler code here
    if (g_hSession == NULL)
    {
        InitDecInfo();
        BVCSP_SessionParam sesParam;
        memset(&sesParam, 0x00, sizeof(sesParam));
        sesParam.iSize = sizeof(sesParam);
        sesParam.iTimeOut = 8000;
        strncpy_s(sesParam.szClientID, "123456", _TRUNCATE);
        strncpy_s(sesParam.szServerAddr, sizeof(sesParam.szServerAddr), szServerAddr, _TRUNCATE);
        strncpy_s(sesParam.szUserName, sizeof(sesParam.szUserName), szUserName, _TRUNCATE);
        strncpy_s(sesParam.szPassword, sizeof(sesParam.szPassword), szPassword, _TRUNCATE);
        strncpy_s(sesParam.szUserAgent, "bvcsp_test", _TRUNCATE);
        if (iType)
        {
            sesParam.iClientType = BVCSP_CLIENT_TYPE_DEC; // DEC
            sesParam.iServerPort = iServerCUPort;
            sesParam.iCmdProtoType = iCUCmdProtoType;
        }
        else
        {
            sesParam.iClientType = BVCSP_CLIENT_TYPE_PU;
            sesParam.iServerPort = iServerPUPort;
            sesParam.iCmdProtoType = iPUCmdProtoType;
        }
        sesParam.OnCommand = CU_OnCommand;
        sesParam.OnDialogCmd;
        sesParam.OnEvent = OnSessionEvent;
        sesParam.OnNotify = CU_OnNotify;

        {  // PU
            static BVCU_PUCFG_DeviceInfo g_deviceInfo;
            static BVCU_PUCFG_ChannelInfo g_channelInfo[4];
            memset(&g_deviceInfo, 0x00, sizeof(g_deviceInfo));
            memset(g_channelInfo, 0x00, sizeof(g_channelInfo));
            strncpy_s(g_deviceInfo.szName, "fhj_PU", _TRUNCATE);
            for (int i = 0; i < 4; ++i){
                g_channelInfo[i].iChannelIndex = i;
                g_channelInfo[i].iMediaDir = 10;
            }
            sesParam.stEntityInfo.pPUInfo = &g_deviceInfo;
            sesParam.stEntityInfo.pChannelList = g_channelInfo;
            sesParam.stEntityInfo.iChannelCount = 4;
        }

        BVCU_Result bResult = BVCSP_Login(&g_hSession, &sesParam);
        printf("BVCSP_Login: %d\n", bResult);
    }
    else
    {
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
void command(int iType)
{
    if (g_hSession)
    {
        BVCSP_Command bvCmd;
        memset(&bvCmd, 0x00, sizeof(bvCmd));
        bvCmd.iSize = sizeof(bvCmd);
        bvCmd.iMethod = BVCU_METHOD_QUERY;
        bvCmd.iSubMethod = BVCU_SUBMETHOD_DEC_LIST;
        bvCmd.OnEvent = CU_Cmd_OnEvent;
        BVCSP_SendCmd(g_hSession, &bvCmd);
    }
}
void dialog(int iType)
{
    if (g_hSession)
    {
        if (!g_hDialog)
        {
            BVCSP_DialogParam dlgParam;
            memset(&dlgParam, 0x00, sizeof(dlgParam));
            dlgParam.iSize = sizeof(dlgParam);
            dlgParam.hSession = g_hSession;
            strncpy_s(dlgParam.stTarget.szID,sizeof(dlgParam.stTarget.szID), szDlgPUID, _TRUNCATE);
            dlgParam.stTarget.iIndexMajor = iDlgChannelIndex;
            dlgParam.iAVStreamDir = BVCU_MEDIADIR_VIDEORECV;// | BVCU_MEDIADIR_AUDIORECV;
            /*dlgParam.stTarget.iIndexMajor = BVCU_SUBDEV_INDEXMAJOR_MIN_TSP;
            dlgParam.iAVStreamDir = BVCU_MEDIADIR_DATASEND | BVCU_MEDIADIR_DATARECV;*/
            /*dlgParam.stTarget.iIndexMajor = BVCU_SUBDEV_INDEXMAJOR_MIN_GPS;
            dlgParam.iAVStreamDir = BVCU_MEDIADIR_DATARECV;*/
            dlgParam.OnEvent = CU_Dialog_OnEvent;
            dlgParam.afterRecv = CU_Dialog_afterRecv;
            BVCSP_DialogControlParam dlgControl;
            memset(&dlgControl, 0x00, sizeof(dlgControl));
            dlgControl.iTimeOut = 30 * 1000;
            dlgControl.iDelayMax = 5000;
            dlgControl.iDelayMin = 800;
            dlgControl.iDelayVsSmooth = 3;

            BVCSP_Dialog_Open(&g_hDialog, &dlgParam, &dlgControl);
        }
        else
        {
            BVCSP_DialogInfo dlgInfo;
            BVCSP_GetDialogInfo(g_hDialog, &dlgInfo);
            printf("hDialog: %p puID: %s channel: %d mediaDir: %d \n", g_hDialog, dlgInfo.stParam.stTarget.szID,
                dlgInfo.stParam.stTarget.iIndexMajor, dlgInfo.stParam.iAVStreamDir);
            if (dlgInfo.stParam.szTargetVideo.iHeight)
                printf("video: height: %d width: %d \n", dlgInfo.stParam.szTargetVideo.iHeight, dlgInfo.stParam.szTargetVideo.iWidth);
            BVCSP_Dialog_Close(g_hDialog);
            g_hDialog = NULL;
        }
    }
}

BOOL g_bRun = TRUE;
unsigned __stdcall workThread(void * pParam)
{
    while (g_bRun)
    {
        BVCSP_HandleEvent();
        Sleep(5);
    }
    return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
    BVCSP_Initialize(1, 0);
    unsigned threadID;
    HANDLE m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, workThread, NULL, 0, &threadID);

    while (1)
    {
        int iType;
        printf("0:PU test   1:CU test  2:cmd  3:notify  4:dialog  5:exit  6:auth\r\n");
        scanf("%d", &iType);
        if (iType == 5)
            break;
        if (0 == iType || iType == 1)
            loginout(iType);
        else if (iType == 2)
            command(iType);
        else if (iType == 4)
            dialog(iType);
        else if (6 == iType) {
            test_auth();
        }
        else
        {
            BVCSP_Packet ppacket;
            memset(&ppacket, 0x00, sizeof(ppacket));
            ppacket.iDataType = BVCSP_DATA_TYPE_TSP;
            ppacket.iSeq = 1;
            ppacket.bKeyFrame = 1;
            ppacket.iPTS = GetTickCount() * 1000;
            ppacket.iDataSize = 6;
            ppacket.pData = "hello";
            if (g_hDialog)
                BVCSP_Dialog_Write(g_hDialog, &ppacket);
        }
    }
    g_bRun = FALSE;
    BVCSP_Finish();
	return 0;
}

