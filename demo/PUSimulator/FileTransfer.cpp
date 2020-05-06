#ifdef _WIN32
#include <Windows.h>
#endif
#include "FileTransfer.h"
#include <stdio.h>
#include <memory.h>
#include <time.h>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <stdlib.h>

static FILE* g_file = NULL;
static BVCSP_HDialog g_hFileTransfer = NULL;
BVCSP_FileTarget g_stFileTarget;

#ifdef _WIN32
static unsigned __stdcall
#else
static void *
#endif
transferThread(void * pParam)
{
    if (g_file == NULL)
        return 0;
    int iTransferBytes = 0;
    bool bSuccess = false;
    time_t iCompleTime = 0;
    while (g_hFileTransfer)
    {
        if (!bSuccess)
        {
            char sendBuf[800];
            int iSendLen = fread(sendBuf, 1, 800, g_file);
            if (iSendLen > 0)
            {
                BVCSP_Packet bvcspPacket;
                memset(&bvcspPacket, 0, sizeof(bvcspPacket));
                bvcspPacket.bKeyFrame = 1;
                bvcspPacket.iDataType = BVCSP_DATA_TYPE_TSP;
                bvcspPacket.iDataSize = iSendLen;
                bvcspPacket.pData = sendBuf;
                if (BVCU_Result_FAILED(BVCSP_Dialog_Write(g_hFileTransfer, &bvcspPacket)))
                {
                    fseek(g_file, 0 - iSendLen, SEEK_CUR);
                    //break;
                }
                else
                    iTransferBytes += iSendLen;
            }
            else {
                //上传成功, 延时关闭dialog，大约60s，收到close消息可提前关闭
                bSuccess = true;
                iCompleTime = time(NULL);
                //break;
            }
        }
        if (iCompleTime > 0)
        {
            if (difftime(time(NULL), iCompleTime) > 60)
            {
                fclose(g_file);
                g_file = NULL;
            }
        }

#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif // _WIN32
    }

    if (g_file)
    {
        fclose(g_file);
        g_file = NULL;
    }
    return 0;
}

static void Dialog_OnEvent(BVCSP_HDialog hDialog, int iEventCode, BVCSP_Event_DialogCmd* pParam)
{
    if (hDialog == g_hFileTransfer)
    {
        if (iEventCode == BVCSP_EVENT_DIALOG_OPEN)
        {
            printf("FileTransfer open result = %d\n", pParam->iResult);
            if (BVCU_Result_SUCCEEDED(pParam->iResult))
            {
                g_file = fopen(pParam->pDialogParam->stFileTarget.pPathFileName, "rb");
#ifdef _WIN32
                unsigned threadID;
                _beginthreadex(NULL, 0, transferThread, NULL, 0, &threadID);
#else
                pthread_t tidp;
                pthread_create(&tidp, NULL, transferThread, NULL);
#endif
            }
        }
        else if (iEventCode == BVCSP_EVENT_DIALOG_CLOSE)
        {
            printf("AV close result = %d\n", pParam->iResult);
            g_hFileTransfer = NULL;
            /*if (g_file)
            {
                fclose(g_file);
                g_file = NULL;
            }*/
                
        }
    }
}

BVCU_Result FileTransfer_OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam)
{
    pParam->OnEvent = Dialog_OnEvent;
    FILE* fd = fopen(pParam->stFileTarget.pPathFileName, "rb");
    if (!fd)
        return BVCU_RESULT_E_NOTFOUND;
    fseek(fd, 0L, SEEK_END);
    pParam->stFileTarget.iStartTime_iOffset = 0;    
    pParam->stFileTarget.iEndTime_iFileSize = ftell(fd);
    fclose(fd);
    g_hFileTransfer = hDialog;
    g_stFileTarget = pParam->stFileTarget;
    //如果这里是异步，则返回BVCU_RESULT_S_PENDING,异步调用pParam->OnEvent
    return BVCU_RESULT_S_OK;
}
