#ifdef _WIN32
#include <Windows.h>
#endif
#include <BVCSP.h>
#include <DecConfig.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#ifdef _MSC_VER
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\..\\libSAV\\lib\\debug\\libSAV_x64.lib")
#pragma comment(lib, "..\\..\\lib\\debug\\BVCSP_x64.lib")
#else
#pragma comment(lib, "..\\..\\..\\libSAV\\lib\\release\\libSAV_x64.lib")
#pragma comment(lib, "..\\..\\lib\\release\\BVCSP_x64.lib")
#endif
#elif _WIN32
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\..\\libSAV\\lib\\debug\\libSAV.lib")
#pragma comment(lib, "..\\..\\lib\\debug\\BVCSP.lib")
#else
#pragma comment(lib, "..\\..\\..\\libSAV\\lib\\release\\libSAV.lib")
#pragma comment(lib, "..\\..\\lib\\release\\BVCSP.lib")
#endif
#endif // _WIN32
#endif // _MSC_VER

char szServerAddr[] = "192.168.6.76";
char szDlgPUID[] = "PU_55AA0008";
int  iServerPUPort = 9702;
int  iDlgChannelIndex = 0;

BVCSP_HSession g_hSession = NULL;
BVCU_DECCFG_DeviceInfo g_decInfo;
BVCU_DECCFG_Options g_decOptions;
BVCU_DECCFG_DecoderParam g_decDecoder;
BVCU_DECCFG_DisplayParam g_decDisplay;
BVCU_DECCFG_DataChanInfo g_decChannel[2];

int g_eCodecID = -1;
int g_eSampleFormat = -1;
int g_iBitRate = -1;
int g_iChannelCount = -1;
int g_iSampleRate = -1;
char g_pExtraData[2] = { 0 };
int g_iExtraDataSize = 0;

std::vector<BVCSP_Packet> g_packetVec;



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
    g_decOptions.iTimeOut = 30 * 1000; // �Ự����ȳ�ʱʱ�䣬��λ���롣
    g_decOptions.iSMSRetryCount = 0; // �������������������� -1�������������� 0������һֱ������������ >0�����Դ�����
    g_decOptions.iChanRetryCount = 0;// ����ͨ���������������� ֵ����ͬ iSMSRetryCount
    g_decOptions.iRetryDelay = 60 * 1000;    // ��������ʱ������ ��λ:���� �ο�ֵ 60*1000
    g_decOptions.iCaptureVolume = 100; // ��Ƶ�ɼ�������������Χ0��100
    g_decOptions.bOSD = 1;      // �Ƿ�����ʾ�е�����Ϣ��0-�� 1-��
    g_decOptions.iDelayMax = 5000; // A/V���ݴӽ��յ���������������ӳ٣���Ҫ����10 000����λ�����루�ο�ֵ��5000��
    g_decOptions.iDelayMin = 1000; // A/V���ݴӽ��յ��������С������ӳ٣���Ҫ����10 000����λ�����루�ο�ֵ��1000��
    g_decOptions.iDelayVsSmooth = 3; // �����ӳ���ƽ��ѡ��ȡֵ��Χ1��10��Խ���򲥷�Խƽ�������ӳٱ�󣨲ο�ֵ��3��

    memset(&g_decDecoder, 0x00, sizeof(g_decDecoder));
    g_decDecoder.iDecChanIndex = 0; // ����ͨ���� ��0��ʼ��BVCU_DECCFG_DeviceInfo.iDecNums-1
    g_decDecoder.bDecodeEnable = 1; // ������ͣ��־��0-ֹͣ��1-�������룬2-Զ�̻طţ��ݲ�ʵ�֣�����д
    g_decDecoder.iPoolTime = 10 * 1000;     // ��ѯ��� ��λ������  ��д
    g_decDecoder.iDataChanMaxCount = 2;   // ��ǰ����ͨ��֧�ֵ�����ͨ���� ���ֵΪBVCU_MAX_TOUR_IPCHANNEL_COUNT
    g_decDecoder.iDataChanCount = 1;      // ��ӵ��б������ͨ����������pDataChanInfo�����С��  ���ֵΪiDataChanMaxCount����д
    g_decDecoder.pDataChanInfo = g_decChannel; //���õ�����ͨ���б������б�˳��Ϊ��·��ѯ��˳�򡣿�д
    g_decDecoder.iDecodeStatus = 1;   //��ǰ״̬:0:δ������1-�����������룬2-����Զ�̻طţ��ݲ�ʵ�֣�
    memset(g_decChannel, 0x00, sizeof(g_decChannel));
    g_decChannel[0].bEnable = 1;  // �Ƿ�ʹ�ܣ� 0-��ʹ�ܣ� 1-ʹ�ܡ� 
    g_decChannel[0].iDataChanType = BVCU_IPC_NET_PROTO_TYPE_SMARTEYE; // ͨ��Э�����ͣ� �� BVCU_IPC_NET_PROTO_TYPE_*�� Ŀǰֻ֧��BVCU_IPC_NET_PROTO_TYPE_SMARTEYE
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szAddr, "61.191.27.18");     //������IP������
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iPort = 9701;           //�������˿�
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iProto = BVCU_PROTOTYPE_TCP;         //ʹ�õ�Э�����ͣ�TCP/UDP
    g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.iKeepAliveTimeout = 60 * 1000; // �豸������ʱʱ�䣬��λ�롣������ʱ���豸�ղ����������ظ�����Ϊ���������ߡ���Χ�� 45-200
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szUserName, "besovideo"); // ��¼�û���
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.stBVServer.szPassword, "besovideo"); // ��¼����
    strcpy(g_decChannel[0].stChannelParam.stBVChannelInfo.szPUID, "PU_55AA0008");
    g_decChannel[0].stStatus.bOnline = 1; // ͨ����ǰ״̬
    g_decChannel[0].stStatus.iCodeIDA = SAVCODEC_ID_G726;  // ��Ƶ�����ʽ  �� SAVCODEC_ID_*(SAVCodec.h)
    g_decChannel[0].stStatus.iFpsDecA = 25 * 10000;  // ��Ƶ����֡��  ��λ1/10000֡ÿ��
    g_decChannel[0].stStatus.iDecodedA = 100000; // �Ѿ��������Ƶ֡��
    g_decChannel[0].stStatus.iLostRateA = 5 * 10000;// ��Ƶ����(��֡)�ʣ���λ1/10000
    g_decChannel[0].stStatus.iKbpsA = 32;    // ��Ƶ�������ʣ���λ Kbits/second
    g_decChannel[0].stStatus.iCodeIDV = SAVCODEC_ID_H264;  // ��Ƶ�����ʽ  �� SAVCODEC_ID_*(SAVCodec.h)
    g_decChannel[0].stStatus.iFpsDecV = 25 * 10000;  // ��Ƶ����֡��  ��λ1/10000֡ÿ��
    g_decChannel[0].stStatus.iDecodedV = 2500000; // �Ѿ��������Ƶ֡��
    g_decChannel[0].stStatus.iLostRateV = 5 * 10000;// ��Ƶ����(��֡)�ʣ���λ1/10000
    g_decChannel[0].stStatus.iKbpsV = 1 * 1024;    // ��Ƶ�������ʣ���λ Kbits/second
    g_decChannel[0].stStatus.iImgW = 1280; // ����ͨ����ǰ��ͼ���С,��
    g_decChannel[0].stStatus.iImgH = 720; // ��

    memset(&g_decDisplay, 0x00, sizeof(g_decDisplay));
    g_decDisplay.iDispChanIndex = 0; // ��ʾͨ���� ��0��ʼ��BVCU_DECCFG_DeviceInfo.iDispNums-1
    g_decDisplay.iAudioWindowIdx = 0;// ��Ƶ�����Ӵ���,��Ӧ iDecChanIndex�±�, -1: �ر���Ƶ�� ��д
    g_decDisplay.iPlaybackVolume = 90;//����������������Χ0��100
    g_decDisplay.iFullWindowIdx = 0;// ȫ��ĳ���Ӵ���,��Ӧ iDecChanIndex�±�, -1: ��ȫ���Ӵ��ڡ� ��д
    g_decDisplay.iDispFormatSup = BVCU_DISPLAY_FORMAT_VGA; // ֧�ֵ����ģʽ, �� BVCU_DISPLAY_FORMAT_*
    g_decDisplay.stResolution.iHeight = 720; // �ֱ��ʣ�����iDispFormatSup��BVCU_DECCFG_DeviceInfo��������ȡ�� ��д
    g_decDisplay.stResolution.iWidth = 1280;
    g_decDisplay.iVideoFormat = BVCU_VIDEOFORMAT_NTSC;      // �� BVCU_VIDEOFORMAT_*
    g_decDisplay.iWindowModeSup[0] = 1;// ֧�ֵĻ���ģʽ��1,2,4,6,8,9,10,13,14,16,17,19,22,25����ο������ҿͻ��˴��ڲ��֡�0: ��Чֵ��
    g_decDisplay.iWindowModeSup[1] = 2;
    g_decDisplay.iWindowModeSup[2] = 4;
    g_decDisplay.iWindowModeSup[3] = 6;
    g_decDisplay.iWindowModeSup[4] = 8;
    g_decDisplay.iWindowModeSup[5] = 9;
    g_decDisplay.iWindowModeSup[6] = 10;
    g_decDisplay.iWindowModeSup[7] = 13;
    g_decDisplay.iWindowModeSup[8] = 14;
    g_decDisplay.iWindowMode = 4;       // ��ǰʹ�õĻ���ģʽ��ֵ����ͬiWindewModeSup����iWindewModeSup��ȡ����д
    for (int i = 0; i < 4; ++i)
        g_decDisplay.iDecChanIndex[i] = i;
    for (int i = 4; i < BVCU_DEC_MAX_WINDOWS; ++i)
        g_decDisplay.iDecChanIndex[i] = -1; // �����Ӵ��ڹ����Ľ���ͨ���š�-1����Чֵ��û�й�������ͨ�����Ӵ�����-1����д
    g_decDisplay.stStatus.iDispStatus = 1;     // ��ʾ״̬��0��δ��ʾ��1��������ʾ
    g_decDisplay.stStatus.iDispFormat = BVCU_DISPLAY_FORMAT_VGA;     // ��ʾ�ӿ����ͣ� �� BVCU_DISPLAY_FORMAT_*
    g_decDisplay.stStatus.iVideoFormat = BVCU_VIDEOFORMAT_NTSC;    // ��Ƶ��ʽ: �� BVCU_VIDEOFORMAT_*
    g_decDisplay.stStatus.iWindowMode = 4;     // ����ģʽ: �ο�BVCU_DECCFG_DisplayParam.iWindowModeSup
    for (int i = 0; i < 4; ++i)
        g_decDisplay.stStatus.iFpsDisp[i] = 15 * 10000;  // ÿ���ӻ������ʾ֡�� ��λ1/10000֡ÿ��
    g_decDisplay.stStatus.bFullScreenMode = 0; // ��Ļģʽ  0-��ͨ 1-ȫ��ģʽ
}

void OnEvent(BVCSP_HSession hSession, int iEventCode, void* pParam)
{
    int iResult = (int)(pParam);
    printf("session event hSession:%p iEventCode:%d result:%d \n", hSession, iEventCode, iResult);
    if (BVCSP_EVENT_SESSION_OPEN == iEventCode && iResult == BVCU_RESULT_S_OK)
    {  // ��¼�ɹ���ע���������Ϣ��
    }
    else
        g_hSession = NULL;
}

BVCU_Result OnCommand(BVCSP_HSession hSession, BVCSP_Command* pCommand)
{
    return BVCU_RESULT_E_FAILED;
}

BVCU_Result OnNotify(BVCSP_HSession hSession, BVCSP_NotifyMsgContent* pData)
{
    return BVCU_RESULT_E_FAILED;
}

BVCSP_HDialog g_hGPS = NULL;
BVCSP_HDialog g_hAV = NULL;

#ifdef _WIN32
unsigned __stdcall
#else
void *
#endif
gpsThread(void * pParam)
{
    while (g_hGPS)
    {
        BVCU_PUCFG_GPSData gpsdata;
        memset(&gpsdata, 0, sizeof(gpsdata));
        gpsdata.iAngle = 0;
        gpsdata.bAntennaState = 1;
        gpsdata.iHeight = 0;
        gpsdata.iLatitude = 318524623;
        gpsdata.iLongitude = 1171980847;
        gpsdata.bOrientationState = 1;
        gpsdata.iSpeed = 0;
        gpsdata.iStarCount = -1;
        time_t now = time(NULL);
        struct tm t;
        gmtime_s(&t, &now);
        gpsdata.stTime.iYear = t.tm_year + 1900;
        gpsdata.stTime.iMonth = t.tm_mon + 1;
        gpsdata.stTime.iDay = t.tm_mday;
        gpsdata.stTime.iHour = t.tm_hour + 1;
        gpsdata.stTime.iMinute = t.tm_min + 1;
        gpsdata.stTime.iSecond = t.tm_sec + 1;
        
        BVCSP_Packet ppacket;
        memset(&ppacket, 0x00, sizeof(ppacket));
        ppacket.iDataType = BVCSP_DATA_TYPE_GPS;
        ppacket.iDataSize = sizeof(gpsdata);
        ppacket.pData = &gpsdata;
        BVCSP_Dialog_Write(g_hGPS, &ppacket);
#ifdef _WIN32
        Sleep(1000);
#else
        usleep(1000000);
#endif // _WIN32
    }
    return 0;
}

#ifdef _WIN32
unsigned __stdcall
#else
void *
#endif
avThread(void * pParam)
{
    std::vector<BVCSP_Packet>::iterator it = g_packetVec.begin();
    while (g_hAV && g_packetVec.size() > 0)
    {
        if (it == g_packetVec.end())
            break;
        BVCSP_Packet ppacket;
        memset(&ppacket, 0x00, sizeof(ppacket));
        ppacket.iDataType = BVCSP_DATA_TYPE_AUDIO;
        ppacket.iDataSize = it->iDataSize;
        ppacket.pData = it->pData;
        ppacket.iPTS = it->iPTS;
        BVCSP_Dialog_Write(g_hAV, &ppacket);
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif // _WIN32
        ++it;
    }
    return 0;
}
void Dialog_OnEvent(BVCSP_HDialog hDialog, int iEventCode, BVCSP_Event_DialogCmd* pParam)
{
    if (hDialog == g_hGPS)
    {
        if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
        {
            printf("GPS open result = %d\n", pParam->iResult);
            if (BVCU_Result_SUCCEEDED(pParam->iResult))
            {
#ifdef _WIN32
                unsigned threadID;
                _beginthreadex(NULL, 0, gpsThread, NULL, 0, &threadID);
#else
                pthread_t tidp;
                pthread_create(&tidp, NULL, gpsThread, NULL);
#endif
            }

            
        }
        else if (iEventCode == BVCSP_EVENT_DIALOG_CLOSE)
        {
            printf("GPS close result = %d\n", pParam->iResult);
            g_hGPS = NULL;
        }
    }
    else if (hDialog == g_hAV)
    {
        if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
        {
            printf("AV open result = %d\n", pParam->iResult);
            if (BVCU_Result_SUCCEEDED(pParam->iResult))
            {
#ifdef _WIN32
                unsigned threadID;
                _beginthreadex(NULL, 0, avThread, NULL, 0, &threadID);
#else
                pthread_t tidp;
                pthread_create(&tidp, NULL, avThread, NULL);
#endif
            }
        }
        else if (iEventCode == BVCSP_EVENT_DIALOG_CLOSE)
        {
            printf("AV close result = %d\n", pParam->iResult);
            g_hAV = NULL;
        }
    }
}

BVCU_Result OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam)
{
    if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
    {
        if (pParam->iAVStreamDir & BVCU_MEDIADIR_DATASEND)
        {
            if (pParam->stTarget.iIndexMajor >= BVCU_SUBDEV_INDEXMAJOR_MIN_GPS && pParam->stTarget.iIndexMajor <= BVCU_SUBDEV_INDEXMAJOR_MAX_GPS)
            {
                pParam->OnEvent = Dialog_OnEvent;
                g_hGPS = hDialog;
                //����������첽���򷵻�BVCU_RESULT_S_PENDING,�첽����pParam->OnEvent
                return BVCU_RESULT_S_OK;
            }
        }
        else if (pParam->iAVStreamDir & BVCU_MEDIADIR_AUDIOSEND)
        {
            if (pParam->stTarget.iIndexMajor >= BVCU_SUBDEV_INDEXMAJOR_MIN_CHANNEL && pParam->stTarget.iIndexMajor <= BVCU_SUBDEV_INDEXMAJOR_MAX_CHANNEL)
            {
                pParam->OnEvent = Dialog_OnEvent;
                g_hAV = hDialog;
                //����������첽���򷵻�BVCU_RESULT_S_PENDING,�첽����pParam->OnEvent
                pParam->iAVStreamDir = BVCU_MEDIADIR_AUDIOSEND;
                pParam->szMyselfAudio.codec = (SAVCodec_ID)g_eCodecID;
                pParam->szMyselfAudio.eSampleFormat = (SAV_SampleFormat)g_eSampleFormat;
                pParam->szMyselfAudio.iBitrate = g_iBitRate;
                pParam->szMyselfAudio.iChannelCount = g_iChannelCount;
                pParam->szMyselfAudio.iSampleRate = g_iSampleRate;
                pParam->szMyselfAudio.pExtraData = (char*)g_pExtraData;
                pParam->szMyselfAudio.iExtraDataSize = g_iExtraDataSize;
                return BVCU_RESULT_S_OK;
            }
        }
    }
    return BVCU_RESULT_E_FAILED;
}

void loginout()
{
    if (g_hSession == NULL)
    {
        //��¼
        InitDecInfo();
        BVCSP_SessionParam sesParam;
        memset(&sesParam, 0x00, sizeof(sesParam));
        sesParam.iSize = sizeof(sesParam);
        sesParam.iClientType = BVCSP_CLIENT_TYPE_PU;
        sesParam.iTimeOut = 8000;
        strncpy_s(sesParam.szClientID, szDlgPUID, _TRUNCATE);
        strncpy_s(sesParam.szServerAddr, sizeof(sesParam.szServerAddr), szServerAddr, _TRUNCATE);
        strncpy_s(sesParam.szUserAgent, "bvcsp_test", _TRUNCATE);
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
            strncpy_s(g_deviceInfo.szName, "fhj_PU", _TRUNCATE);

            //video
            g_channelInfo[0].iChannelIndex = BVCU_SUBDEV_INDEXMAJOR_MIN_CHANNEL;
            g_channelInfo[0].iMediaDir = BVCU_MEDIADIR_VIDEOSEND;
            strcpy(g_channelInfo[0].szName, "video");
            //gps
            g_channelInfo[1].iChannelIndex = BVCU_SUBDEV_INDEXMAJOR_MIN_GPS;
            g_channelInfo[1].iMediaDir = BVCU_MEDIADIR_DATASEND;
            strcpy(g_channelInfo[1].szName, "gps");

            sesParam.stEntityInfo.pPUInfo = &g_deviceInfo;
            sesParam.stEntityInfo.pChannelList = g_channelInfo;
            sesParam.stEntityInfo.iChannelCount = 2;
        }

        BVCU_Result bResult = BVCSP_Login(&g_hSession, &sesParam);
    }
    else
    {
        //�ǳ�
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

void Log_Callback(int level, const char* log)
{
    printf("%s\n", log);
}

void make_dsi(unsigned int sampling_frequency_index, unsigned int channel_configuration, unsigned char* dsi)
{
    unsigned int object_type = 2; // AAC LC by default
    dsi[0] = (object_type << 3) | (sampling_frequency_index >> 1);
    dsi[1] = ((sampling_frequency_index & 1) << 7) | (channel_configuration << 3);
}
int get_sr_index(unsigned int sampling_frequency)
{
    switch (sampling_frequency) {
    case 96000: return 0;
    case 88200: return 1;
    case 64000: return 2;
    case 48000: return 3;
    case 44100: return 4;
    case 32000: return 5;
    case 24000: return 6;
    case 22050: return 7;
    case 16000: return 8;
    case 12000: return 9;
    case 11025: return 10;
    case 8000: return 11;
    case 7350: return 12;
    default: return 0;
    }
}

//void GenMyAAC()
//{
//    SAVContainer_Context context;
//    std::vector<SAV_Packet> packetVec;
//    memset(&context, 0, sizeof(context));
//    context.iSize = sizeof(context);
//    context.bMux = SAV_BOOL_FALSE;
//    context.sFileName = "test.aac";
//    SAV_Result result = SAVContainer_Open(&context);
//    if (SAV_Result_FAILED(result)) {
//        printf("open failed ,result:%d\n", result);
//        return;
//    }
//
//    char dsi[2];
//    make_dsi((unsigned int)get_sr_index((unsigned int)context.ppStreams[0]->stAudioParam.iSampleRate),
//        (unsigned int)context.ppStreams[0]->stAudioParam.iChannelCount, (unsigned char*)dsi);
//    context.ppStreams[0]->pExtraData = (uint8_t*)dsi;
//    context.ppStreams[0]->iExtraDataSize = 2;
//    SAV_Packet packet;
//    memset(&packet, 0, sizeof(packet));
//    packet.iSize = sizeof(packet);
//
//    //�����Զ���aac
//    FILE *fd = NULL;
//    fopen_s(&fd, "test.myaac", "wb");
//    if (fd == NULL)
//        return;
//
//    int eCodecID = context.ppStreams[0]->eCodecID;
//    int eSampleFormat = context.ppStreams[0]->stAudioParam.eSampleFormat;
//    int iBitRate = context.ppStreams[0]->iBitRate;
//    int iChannelCount = context.ppStreams[0]->stAudioParam.iChannelCount;
//    int iSampleRate = context.ppStreams[0]->stAudioParam.iSampleRate;
//    char* pExtraData = (char*)context.ppStreams[0]->pExtraData;
//    int iExtraDataSize = context.ppStreams[0]->iExtraDataSize;
//    fwrite(&eCodecID, 1, 4, fd);
//    fwrite(&eSampleFormat, 1, 4, fd);
//    fwrite(&iBitRate, 1, 4, fd);
//    fwrite(&iChannelCount, 1, 4, fd);
//    fwrite(&iSampleRate, 1, 4, fd);
//    fwrite(&iExtraDataSize, 1, 4, fd);
//    fwrite(pExtraData, 1, iExtraDataSize, fd);
//    while (SAV_Result_SUCCEEDED(result = SAVContainer_Process(&context, &packet))) {
//        SAV_Packet temp = packet;
//        temp.pData = (SAV_TYPE_UINT8*)new char[packet.iDataSize];
//        memset(temp.pData, 0, packet.iDataSize);
//        memcpy_s(temp.pData, temp.iDataSize, packet.pData, packet.iDataSize);
//        packetVec.push_back(temp);
//    }
//
//    int count = packetVec.size();
//    fwrite(&count, 1, 4, fd);
//    std::vector<SAV_Packet>::iterator it = packetVec.begin();
//    while (it != packetVec.end())
//    {
//        fwrite(&it->iPTS, 1, 8, fd);
//        fwrite(&it->iDataSize, 1, 4, fd);
//        fwrite(it->pData, 1, it->iDataSize, fd);
//        delete[]it->pData;
//        ++it;
//    }
//    fclose(fd);
//}

bool ReadMyAAC()
{
    FILE *fd = fopen("test.myaac", "rb");
    if (fd == NULL)
        return false;
    fread(&g_eCodecID, 1, 4, fd);
    fread(&g_eSampleFormat, 1, 4, fd);
    fread(&g_iBitRate, 1, 4, fd);
    fread(&g_iChannelCount, 1, 4, fd);
    fread(&g_iSampleRate, 1, 4, fd);
    fread(&g_iExtraDataSize, 1, 4, fd);
    fread(&g_pExtraData, 1, g_iExtraDataSize, fd);

    int count = 0;
    fread(&count, 1, 4, fd);
    for (int i = 0; i < count; ++i)
    {
        BVCSP_Packet packet;
        fread(&packet.iPTS, 1, 8, fd);
        fread(&packet.iDataSize, 1, 4, fd);
        packet.pData = new SAV_TYPE_UINT8[packet.iDataSize];
        if (packet.pData == NULL)
        {
            fclose(fd);
            return false;
        }
        fread(packet.pData, 1, packet.iDataSize, fd);
        g_packetVec.push_back(packet);
    }
    fclose(fd);
    return true;
}


int main()
{
    //GenMyAAC();
    if (!ReadMyAAC())
    {
        printf("��ȡMyAAC�ļ�ʧ�ܣ�����\n");
        system("pause");
        return -1;
    }
    BVCSP_SetLogCallback(Log_Callback, BVCU_LOG_LEVEL_INFO);
    BVCSP_Initialize(1, 0);
    unsigned threadID;

    while (1)
    {
        int iType;
        printf("0:PU test 5:exit\r\n");
        scanf("%d", &iType);
        if (iType == 5)
            break;
        if (0 == iType)
            loginout();
    }
    BVCSP_Finish();
    return 0;
}
