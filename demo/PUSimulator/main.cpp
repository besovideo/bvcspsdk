#include "loginout.h"
#include "AVDialog.h"
#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#ifdef _MSC_VER
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\lib\\BVCSP_x64.lib")
#pragma comment(lib, "..\\..\\lib\\libSAV_x64.lib")
#else
#pragma comment(lib, "..\\..\\lib\\BVCSP_x64.lib")
#pragma comment(lib, "..\\..\\lib\\libSAV_x64.lib")
#endif
#elif _WIN32
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\lib\\BVCSP.lib")
#pragma comment(lib, "..\\..\\lib\\libSAV.lib")
#else
#pragma comment(lib, "..\\..\\lib\\BVCSP.lib")
#pragma comment(lib, "..\\..\\lib\\libSAV.lib")
#endif
#endif // _WIN32
#endif // _MSC_VER

void Log_Callback(int level, const char* log)
{
    printf("%s\n", log);
}
#ifdef _WIN32
static unsigned __stdcall
#else
static void *
#endif
Wall_SEProtTask(void* arg)
{
    while(1){
    BVCSP_HandleEvent();
    }
    return NULL;
}

#ifdef _WIN32
static unsigned threadID;
#else
static pthread_t  lo_task;
#endif

int main()
{
    if(!ReadMyAAC())
    {
        printf("¶ÁÈ¡MyAACÎÄ¼þÊ§°Ü£¡£¡£¡\n");
        system("pause");
        return -1;
    }
    BVCSP_SetLogCallback(Log_Callback, BVCU_LOG_LEVEL_INFO);
    BVCSP_Initialize(0, 0);
    printf("bvcsp task thread create  \n");
#ifdef _WIN32
    _beginthreadex(NULL, 0, Wall_SEProtTask, NULL, 0, &threadID);
#else
    pthread_create(&lo_task, NULL, Wall_SEProtTask, (void*)NULL);
    if (NULL == (void *)lo_task) {
        printf("Failed to create task!(VideoIO)");
        return 0;
    }
#endif

    printf("bvcsp task thread create success \n");

    while (1)
    {
        int iType;
        printf("0:PU test, 1 : getavdialoginfo, 2£ºauth, 5:exit\r\n");
        scanf("%d", &iType);
        if (iType == 5)
            break;
        if (0 == iType)
            loginout("192.168.6.76", 9702);
        else if (1 == iType)
            GetDialogInfo();
        else if (2 == iType)
            test_auth();
    }
    BVCSP_Finish();
    return 0;
}
