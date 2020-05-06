#include <stdio.h>
#include <string>
#include "auth.h"

// 全局常量定义
const char* base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

/*
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int base64_encode(const unsigned char* sourcedata, int datalength, char* base64)
{
    int i = 0, j = 0;
    unsigned char trans_index = 0;    // 索引是8位，但是高两位都为0
    for (; i < datalength; i += 3)
    {
        // 每三个一组，进行编码
        // 要编码的数字的第一个
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // 第二个
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength) {
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }
        else {
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   // 超出总长度，可以直接break
        }
        // 第三个
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength) { // 有的话需要编码2个
            trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
            base64[j++] = base64char[(int)trans_index];

            trans_index = sourcedata[i + 2] & 0x3f;
            base64[j++] = base64char[(int)trans_index];
        }
        else {
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            break;
        }
    }

    base64[j] = '\0';

    return j;
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
        {  // 设备硬件信息。根据您的硬件情况填写。
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
