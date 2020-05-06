#ifndef __BVAUTH_H__
#define __BVAUTH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_AUTH 1

#include "bvrauth.h"
#if ENABLE_AUTH

/**
    认证终端
    param->tagInfo.RandCode带回本地标识,
    更新认证信息时, 需要传入RandCode
*/
LIBBVCSP_API BVCU_Result BVCSP_Auth(BVRAuthParam* param);

/**
    设置认证服务器地址(当前版本不支持)
*/
LIBBVCSP_API BVCU_Result BVCSP_SetServer(char* host, int port);

/**
    获取认证结果信息
*/
LIBBVCSP_API BVCU_Result BVCSP_GetAuthInfo(BVRAuthParam* info);

/**
    获取需要加密的数据, 使用开发者的公钥进行加密. 初始化param->appInfo
*/
LIBBVCSP_API BVCU_Result BVCSP_GetEncryptedData(char* data, int* len);

/**
    申请试用(功能未开放, 目前仅支持本公司产品)
*/
LIBBVCSP_API BVCU_Result BVCSP_Trial(int authCode);

/**
    加密BVCSP_GetEncryptedData()获得的数据,
    (用于测试, 不推荐使用)
*/
LIBBVCSP_API BVCU_Result BVCSP_EncryptData(char* n, char* e, char *input, int ilen, char *output, int* olen);

/**
    获取模块共享数据(Base64数据)
*/ 
LIBBVCSP_API BVCU_Result BVCSP_GetModuleShareData(char *output, int* olen);

/**
    对BVRAuthParam.OnMessage()的回复
    注意 BVRTPMessage.SessionID、BVRTPMessage.Seq要一致
*/
LIBBVCSP_API BVCU_Result BVCSP_AuthSendMessage(BVRAuthParam* info, struct BVRTPMessage* msg);

/**
    执行命令, 如上传PU信息, CMS获取黑白名单等
*/
LIBBVCSP_API BVCU_Result BVCSP_AuthSendCommond(BVRAuthParam* info, struct BVRCommond* cmd);

#endif

#ifdef __cplusplus
}
#endif

#endif
