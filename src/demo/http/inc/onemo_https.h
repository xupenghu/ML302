/*********************************************************
 *  @file    onemo_https.h
 *  @brief   ML302 OpenCPU HTTPS SSL相关头文件
 *  Copyright (c) 2019 China Mobile IOT.
 *  All rights reserved.
 *  created by XieGangLiang 2019/12/2
 ********************************************************/
 #ifndef __ONEMO_HTTPS_H__
 #define __ONEMO_HTTPS_H__
 
typedef struct ssl_file {
	uint16_t file_len;
	uint8_t data[0];
} ssl_file_t;



 #endif