#ifdef _WIN32
#include <Windows.h>
#endif
#include "AVDialog.h"
#include <stdio.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <stdlib.h>
#include <SAVContainer.h>



static BVCSP_HDialog g_hAV = NULL;
static std::vector<BVCSP_Packet> g_packetVec;

static int g_eCodecID = -1;
static int g_eSampleFormat = -1;
static int g_iBitRate = -1;
static int g_iChannelCount = -1;
static int g_iSampleRate = -1;
static char g_pExtraData[2] = { 0 };
static int g_iExtraDataSize = 0;

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

void GenMyAAC()
{
    SAVContainer_Context context;
    std::vector<SAV_Packet> packetVec;
    memset(&context, 0, sizeof(context));
    context.iSize = sizeof(context);
    context.bMux = SAV_BOOL_FALSE;
    context.sFileName = "test.aac";
    SAV_Result result = SAVContainer_Open(&context);
    if (SAV_Result_FAILED(result)) {
        printf("open failed ,result:%d\n", result);
        return;
    }

    char dsi[2];
    make_dsi((unsigned int)get_sr_index((unsigned int)context.ppStreams[0]->stAudioParam.iSampleRate),
        (unsigned int)context.ppStreams[0]->stAudioParam.iChannelCount, (unsigned char*)dsi);
    context.ppStreams[0]->pExtraData = (uint8_t*)dsi;
    context.ppStreams[0]->iExtraDataSize = 2;
    SAV_Packet packet;
    memset(&packet, 0, sizeof(packet));
    packet.iSize = sizeof(packet);

    //生成自定义aac
    FILE *fd = NULL;
    fopen_s(&fd, "test.myaac", "wb");
    if (fd == NULL)
        return;

    int eCodecID = context.ppStreams[0]->eCodecID;
    int eSampleFormat = context.ppStreams[0]->stAudioParam.eSampleFormat;
    int iBitRate = context.ppStreams[0]->iBitRate;
    int iChannelCount = context.ppStreams[0]->stAudioParam.iChannelCount;
    int iSampleRate = context.ppStreams[0]->stAudioParam.iSampleRate;
    char* pExtraData = (char*)context.ppStreams[0]->pExtraData;
    int iExtraDataSize = context.ppStreams[0]->iExtraDataSize;
    fwrite(&eCodecID, 1, 4, fd);
    fwrite(&eSampleFormat, 1, 4, fd);
    fwrite(&iBitRate, 1, 4, fd);
    fwrite(&iChannelCount, 1, 4, fd);
    fwrite(&iSampleRate, 1, 4, fd);
    fwrite(&iExtraDataSize, 1, 4, fd);
    fwrite(pExtraData, 1, iExtraDataSize, fd);
    while (SAV_Result_SUCCEEDED(result = SAVContainer_Process(&context, &packet))) {
        SAV_Packet temp = packet;
        temp.pData = (SAV_TYPE_UINT8*)new char[packet.iDataSize];
        memset(temp.pData, 0, packet.iDataSize);
        memcpy_s(temp.pData, temp.iDataSize, packet.pData, packet.iDataSize);
        packetVec.push_back(temp);
    }

    int count = packetVec.size();
    fwrite(&count, 1, 4, fd);
    std::vector<SAV_Packet>::iterator it = packetVec.begin();
    while (it != packetVec.end())
    {
        fwrite(&it->iPTS, 1, 8, fd);
        fwrite(&it->iDataSize, 1, 4, fd);
        fwrite(it->pData, 1, it->iDataSize, fd);
        delete[]it->pData;
        ++it;
    }
    fclose(fd);
}

#ifdef _WIN32
static unsigned __stdcall
#else
static void *
#endif
avThread(void * pParam)
{
    int64_t diffPts = 200000;
    int times = 0;
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
        ppacket.iPTS = 1539570383000000 + diffPts * times++;
        //printf("ppacket.iPTS=%lld\n", ppacket.iPTS);
        BVCSP_Dialog_Write(g_hAV, &ppacket);
#ifdef _WIN32
        Sleep(20);
#else
        usleep(100000);
#endif // _WIN32
        ++it;
    }
    return 0;
}
void Dialog_OnEvent(BVCSP_HDialog hDialog, int iEventCode, BVCSP_Event_DialogCmd* pParam)
{
   if (hDialog == g_hAV)
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
        else if (iEventCode == BVCSP_EVENT_DIALOG_UPDATE)
        {
            printf("AV update result = %d\n", pParam->iResult);
        }
    }
}
BVCU_Result Dialog_afterRecv(BVCSP_HDialog hDialog, BVCSP_Packet* pPacket)
{
    BVCSP_DialogInfo stDialogInfo;
    memset(&stDialogInfo, 0, sizeof(stDialogInfo));
    BVCSP_GetDialogInfo(g_hAV, &stDialogInfo);
    printf("pPacket.datesize =%d,codec =0x%x,iBitrate =%d,iSampleRate =%d\n", pPacket->iDataSize, stDialogInfo.stParam.szTargetAudio.codec, stDialogInfo.stParam.szTargetAudio.iBitrate, stDialogInfo.stParam.szTargetAudio.iSampleRate);
    return BVCU_RESULT_S_OK;
}

BVCU_Result AV_OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam)
{
    if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
    {
        printf("AV_OnDialogCmd BVCSP_EVENT_DIALOG_OPEN\n");
        pParam->OnEvent = Dialog_OnEvent;
        g_hAV = hDialog;
        //如果这里是异步，则返回BVCU_RESULT_S_PENDING,异步调用pParam->OnEvent
        if (pParam->iAVStreamDir & BVCU_MEDIADIR_AUDIOSEND)
        {
            pParam->szMyselfAudio.codec = (SAVCodec_ID)g_eCodecID;
            pParam->szMyselfAudio.eSampleFormat = (SAV_SampleFormat)g_eSampleFormat;
            pParam->szMyselfAudio.iBitrate = g_iBitRate;
            pParam->szMyselfAudio.iChannelCount = g_iChannelCount;
            pParam->szMyselfAudio.iSampleRate = g_iSampleRate;
            pParam->szMyselfAudio.pExtraData = (char*)g_pExtraData;
            pParam->szMyselfAudio.iExtraDataSize = g_iExtraDataSize;
        }
        if (pParam->iAVStreamDir & BVCU_MEDIADIR_AUDIORECV)
        {
            pParam->szTargetAudio.codec = SAVCODEC_ID_G726;
            pParam->szTargetAudio.eSampleFormat = SAV_SAMPLE_FMT_FLTP;
            pParam->szTargetAudio.iBitrate = 32000;
            pParam->szTargetAudio.iChannelCount = 1;
            pParam->szTargetAudio.iSampleRate = 8000;
            pParam->afterRecv = Dialog_afterRecv;
        }
        //pParam->iAVStreamDir = BVCU_MEDIADIR_AUDIOSEND;
        
        return BVCU_RESULT_S_OK;
    }
    else if (iEventCode == BVCSP_EVENT_DIALOG_UPDATE)
    {
        printf("AV_OnDialogCmd BVCSP_EVENT_DIALOG_UPDATE\n");
        return BVCU_RESULT_S_OK;
    }
    return BVCU_RESULT_E_UNSUPPORTED;
    
}

BVCU_Result GetDialogInfo()
{
    return BVCU_RESULT_S_OK;
}