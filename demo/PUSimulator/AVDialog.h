#ifndef __AVDIALOG_H__
#define __AVDIALOG_H__
#include <BVCSP.h>
void GenMyAAC();
bool ReadMyAAC();
BVCU_Result AV_OnDialogCmd(BVCSP_HDialog hDialog, int iEventCode, BVCSP_DialogParam* pParam);
BVCU_Result GetDialogInfo();
#endif
