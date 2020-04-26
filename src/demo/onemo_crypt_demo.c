/*********************************************************
 *  @file    onemo_crypt_demo.c
 *  @brief   ML302 OpenCPU 数据加密示例
 *  Copyright (c) 2019 China Mobile IOT.
 *  All rights reserved.
 *  created by XieGangLiang 2020/01/02
 ********************************************************/
#include "onemo_main.h"
#include "wolfssl/wolfcrypt/md5.h"
#include "wolfssl/wolfcrypt/sha.h"
#include "mbedtls/base64.h"
char sum[1024];

void onemo_test_md5(unsigned char **cmd,int len)
{
    int i = 0;
    Md5 md5;
    
    memset(sum,0,1024);
    wc_InitMd5(&md5);
    wc_Md5Update(&md5, cmd[2], strlen(cmd[2]));
    wc_Md5Final(&md5, sum);
    onemo_printf("[MD5]%s\n",sum);
}
void onemo_test_base64(unsigned char **cmd,int len)
{
    int i = 0;
    int res_len = 0;
    memset(sum,0,1024);
    mbedtls_base64_encode(sum,1024,&res_len,cmd[2],strlen(cmd[2]));
    onemo_printf("[BASE64]:%s\n",sum);
}

void onemo_test_sha(unsigned char **cmd,int len)
{
    int i = 0;
    memset(sum,0,1024);
    Sha sha;
    wc_InitSha(&sha);
    wc_ShaUpdate(&sha, cmd[2], strlen(cmd[2]));
    wc_ShaFinal(&sha, sum);
    onemo_printf("[SHA]:%s\n",sum);
}