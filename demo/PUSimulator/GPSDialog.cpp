#ifdef _WIN32
#include <Windows.h>
#endif
#include "GPSDialog.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <stdlib.h>


static BVCSP_HDialog g_hGPS = NULL;

#ifdef _WIN32
static unsigned __stdcall
#else
static void *
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
        struct tm* t = gmtime(&now);
        struct tm temp = *t;
        gpsdata.stTime.iYear = temp.tm_year + 1900;
        gpsdata.stTime.iMonth = temp.tm_mon + 1;
        gpsdata.stTime.iDay = temp.tm_mday;
        gpsdata.stTime.iHour = temp.tm_hour + 1;
        gpsdata.stTime.iMinute = temp.tm_min + 1;
        gpsdata.stTime.iSecond = temp.tm_sec + 1;

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

static void Dialog_OnEvent(BVCSP_HDialog hDialog, int iEventCode, BVCSP_Event_DialogCmd* pParam)
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
}
BVCU_Result GPS_OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam)
{
    pParam->OnEvent = Dialog_OnEvent;
    g_hGPS = hDialog;
    //如果这里是异步，则返回BVCU_RESULT_S_PENDING,异步调用pParam->OnEvent
    return BVCU_RESULT_S_OK;
}
