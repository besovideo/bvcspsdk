// stdafx.cpp : source file that includes just the standard includes
// BVCSPTest.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

// ȫ�ֳ�������
const char* base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

/*
* const unsigned char * sourcedata�� Դ����
* char * base64 �����ֱ���
*/
int base64_encode(const unsigned char* sourcedata, int datalength, char* base64)
{
    int i = 0, j = 0;
    unsigned char trans_index = 0;    // ������8λ�����Ǹ���λ��Ϊ0
    for (; i < datalength; i += 3)
    {
        // ÿ����һ�飬���б���
        // Ҫ��������ֵĵ�һ��
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // �ڶ���
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength) {
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }
        else {
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   // �����ܳ��ȣ�����ֱ��break
        }
        // ������
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength) { // �еĻ���Ҫ����2��
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
