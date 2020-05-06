#include "Command.h"
#include <BVSearch.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

BVCU_Result BV_OnCommand(BVCSP_HSession hSession, BVCSP_Command* pCommand)
{
    if (pCommand->iSubMethod == BVCU_SUBMETHOD_SEARCH_LIST)
    {
        BVCU_Search_Request *pRequest = (BVCU_Search_Request *)pCommand->stMsgContent.pData;
        if (pRequest)
        {
            if (pRequest->stSearchInfo.iType == BVCU_SEARCH_TYPE_FILE)
            {
                BVCSP_Event_SessionCmd param;
                memset(&param, 0, sizeof(param));
                param.iPercent = 100;
                param.iResult = BVCU_RESULT_S_OK;
                BVCU_Search_FileInfo fileInfo;
                memset(&fileInfo, 0, sizeof(fileInfo));
                BVCU_Search_Response response;
                memset(&response, 0, sizeof(response));
                response.pData.pFileInfo = &fileInfo;
                param.stContent.pData = &response;
                param.stContent.iDataCount = 1;
                response.iCount = 1;
                response.stSearchInfo = pRequest->stSearchInfo;
                response.stSearchInfo.iCount = 1;
                response.stSearchInfo.iTotalCount = 1;
                fileInfo.iFileType = BVCU_STORAGE_FILE_TYPE_AUDIO;
                FILE* fd = fopen("PU_1_00_20181012_101336.aac", "rb");
                if (fd)
                {
                    fseek(fd, 0L, SEEK_END);
                    fileInfo.iFileSize = ftell(fd);
                    fclose(fd);
                }
                
                fileInfo.iRecordType = BVCU_STORAGE_RECORDTYPE_AUTOSAVE;
                fileInfo.iTimeBegin = time(NULL) - 10000;
                fileInfo.iTimeEnd = time(NULL);
                strcpy(fileInfo.szFilePath, "PU_1_00_20181012_101336.aac");
                strcpy(fileInfo.szSourceID, "PU_1");
                pCommand->OnEvent(hSession, pCommand, &param);
            }
            
        }
        return BVCU_RESULT_S_OK;
    }
    return BVCU_RESULT_E_FAILED;
}
