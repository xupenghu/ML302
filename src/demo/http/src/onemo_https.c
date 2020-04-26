/*********************************************************
 *  @file    onemo_https.c
 *  @brief   ML302 OpenCPU HTTPS SSL相关函数源文件
 *  Copyright (c) 2019 China Mobile IOT.
 *  All rights reserved.
 *  created by XieGangLiang 2019/12/2
 ********************************************************/
#include "wolfssl/openssl/crypto.h"
#include "wolfssl/openssl/ssl.h"
#include "wolfssl/openssl/err.h"
#include "wolfssl/openssl/rand.h"
#include "wolfssl/internal.h"
#include "onemo_https.h"
#include "onemo_sys.h"
 typedef struct ssl_ctx {
	WOLFSSL_CTX *wolf_ctx;

	uint16_t cipher_suit;
	uint8_t ssl_ver;
	uint8_t sec_lvl;

	uint8_t *ca_cert;
	uint8_t *client_cert;
	uint8_t *client_key;

	uint8_t ign_rtctime;

	uint8_t http_enable;
	int8_t http_index;

	uint8_t smtp_style;
	int8_t smtp_index;

	uint8_t used_cnt;
	uint8_t init_flg;
} ssl_ctx_t;
static uint8_t global_cipher_list[1024] = {0};
void set_ssl_ctx_default(ssl_ctx_t *ssl_ctx)
{
	ssl_ctx->init_flg = 0;
	ssl_ctx->ssl_ver = 4;
	ssl_ctx->cipher_suit = 0xffff;
	ssl_ctx->sec_lvl = 0;
	ssl_ctx->ca_cert = NULL;
	ssl_ctx->client_cert = NULL;
	ssl_ctx->client_key = NULL;
	ssl_ctx->ign_rtctime = 0;
	ssl_ctx->http_enable = 0;
	ssl_ctx->http_index = -1;
	ssl_ctx->smtp_style = 0;
	ssl_ctx->smtp_index = -1;
	ssl_ctx->wolf_ctx = NULL;

	ssl_ctx->used_cnt = 0;
}
static wolfSSL_method_func ssl_get_ssl_version_method(ssl_ctx_t *ssl_ctx)
{
	if (ssl_ctx == NULL)
		return (wolfSSL_method_func)wolfTLSv1_1_client_method_ex;

	if (ssl_ctx->ssl_ver == 0)
		return (wolfSSL_method_func)wolfSSLv3_client_method;
	if (ssl_ctx->ssl_ver == 1)
		return (wolfSSL_method_func)wolfTLSv1_client_method;
	if (ssl_ctx->ssl_ver == 2)
		return (wolfSSL_method_func)wolfTLSv1_1_client_method;
	if (ssl_ctx->ssl_ver == 3)
		return (wolfSSL_method_func)wolfTLSv1_2_client_method;
	if (ssl_ctx->ssl_ver == 4)
		return (wolfSSL_method_func)wolfSSLv23_client_method; //use highest possible version
		
	return (wolfSSL_method_func)NULL;
}
static int8_t ssl_get_full_cipher_list(uint8_t *buf)
{
	uint8_t *cipher_item = NULL;
	uint16_t len = 0;

	if (buf == NULL)
		return -1;

	cipher_item = (uint8_t *)GetCipherNameInternal(0, TLS_RSA_WITH_AES_256_CBC_SHA);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s:", cipher_item);

	cipher_item = (uint8_t *)GetCipherNameInternal(0, TLS_RSA_WITH_AES_128_CBC_SHA);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s:", cipher_item);

	cipher_item = (uint8_t *)GetCipherNameInternal(0, SSL_RSA_WITH_RC4_128_SHA);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s:", cipher_item);

	cipher_item = (uint8_t *)GetCipherNameInternal(0, SSL_RSA_WITH_RC4_128_MD5);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s:", cipher_item);

	cipher_item = (uint8_t *)GetCipherNameInternal(0, SSL_RSA_WITH_3DES_EDE_CBC_SHA);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s:", cipher_item);

	cipher_item = (uint8_t *)GetCipherNameInternal(0, TLS_RSA_WITH_AES_256_CBC_SHA256);
	if (cipher_item != NULL)
		len += sprintf(buf + len, "%s", cipher_item);

	if (strlen(buf) == 0)
		return -1;

	return 0;
}

static uint8_t wolf_ssl_init_flg = 0;

int8_t wolf_ssl_init(ssl_ctx_t *ssl_ctx)
{
	wolfSSL_method_func method = NULL;
	int32_t ret = 0;

	if (ssl_ctx == NULL)
	{
		onemo_sys_log("wolf_ssl_init input error");
		return -1;
	}

	if (wolf_ssl_init_flg == 0)
	{
		wolf_ssl_init_flg = 1;
		wolfSSL_Debugging_ON();
		wolfSSL_Init();
	}

	method = ssl_get_ssl_version_method(ssl_ctx);
	if (ssl_ctx->wolf_ctx == NULL)
	{
		ssl_ctx->wolf_ctx = SSL_CTX_new(method(NULL)); 
		if (ssl_ctx->wolf_ctx == NULL)
		{
			onemo_sys_log("wolf_ssl_init ssl ctx new error");
			return -1;
		}
	}
	else if (ssl_ctx->used_cnt > 0)
	{
		onemo_sys_log("wolf_ssl_init ssl ctx busy, can't reconfig");
		return 0;
	}

	if (ssl_ctx->cipher_suit != 0xffff)
	{
		ret = wolfSSL_CTX_set_cipher_list(ssl_ctx->wolf_ctx, GetCipherNameInternal(0, ssl_ctx->cipher_suit));
	}
	else
	{
		
		ssl_get_full_cipher_list(global_cipher_list);
		ret = wolfSSL_CTX_set_cipher_list(ssl_ctx->wolf_ctx, (const char *)global_cipher_list);
	}

	if (ret != WOLFSSL_SUCCESS)
	{
		onemo_sys_log("wolf_ssl_init cipher set error");
		return -1;
	}

	if (ssl_ctx->sec_lvl == 0)
		wolfSSL_CTX_set_verify(ssl_ctx->wolf_ctx, WOLFSSL_VERIFY_NONE, NULL);
	else
	{
        //加载密钥证书
	}

	if (ssl_ctx->sec_lvl == 2) 
	{
		//加载密钥证书
	}

	return 0;
}


