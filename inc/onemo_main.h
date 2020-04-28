/*********************************************************
*  @file    onemo_main.h
*  @brief   ML302 OpenCPU main header file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/08
********************************************************/
#ifndef __ONEMO_MAIN_H__
#define __ONEMO_MAIN_H__
#define false 0
#define true  1
#define bool int
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sockets.h"
#include "other.h"
#include "onemo/onemo_gpio.h"
#include "onemo/onemo_uart.h"
#include "onemo/onemo_adc.h"
#include "onemo/onemo_alarm.h"
#include "onemo/onemo_i2c.h"
#include "cmsis_os.h"
#include "onemo_sys.h"
#include "onemo_fota.h"
#include "onemo_gnss.h"
#include "cJSON.h"
#include "onemo_tts.h"

void onemo_test_cjson(unsigned char **cmd,int len);
void onemo_test_sha(unsigned char **cmd,int len);
void onemo_test_base64(unsigned char **cmd,int len);
void onemo_test_md5(unsigned char **cmd,int len);
void onemo_printf(char *format,...);
void onemo_test_udp(unsigned char **cmd,int len);
void onemo_test_tcpclient_send(unsigned char **cmd,int len);
void onemo_test_tcpclient(unsigned char **cmd,int len);
void onemo_test_gpio_write(unsigned char **cmd,int len);
void onemo_test_gpio_read(unsigned char **cmd,int len);
void onemo_test_gpio_irq(unsigned char **cmd,int len);
void onemo_force_download(unsigned char **cmd,int len);
void onemo_test_i2c_bmp180(unsigned char **cmd,int len);
void onemo_test_uart_init(void);
void onemo_test_alarm_init(void);
void onemo_test_single_time_alarm(unsigned char **cmd,int len);
void onemo_test_set_repeate_alarm(unsigned char **cmd,int len);
void onemo_test_get_repeate_alarm_info(unsigned char **cmd,int len);
void onemo_test_adc(unsigned char **cmd,int len);
void onemo_test_reboot(unsigned char **cmd,int len);
#ifdef  ONEMO_HTTP_SUPPORT
void onemo_test_http(unsigned char **cmd,int len);
#endif
#ifdef  ONEMO_EDP_SUPPORT
void onemo_test_edp(unsigned char **cmd,int len);
#endif
#ifdef  ONEMO_ALIYUN_SUPPORT
void onemo_test_aliyun(unsigned char **cmd,int len);
#endif
#ifdef  ONEMO_DM_SUPPORT
void onemo_test_dm_register(unsigned char **cmd,int len);
#endif
#ifdef  ONEMO_MQTT_SUPPORT
void onemo_test_mqtt(unsigned char **cmd,int len);
#endif

#ifdef  ONEMO_FOTA_SUPPORT
void onemo_test_onenet_fota_setdevinfo(void);
void onemo_test_onenet_fota(void);
void onemo_test_onenet_fota_deletetask(void);
void onemo_test_fota_update(unsigned char **cmd,int len);
#endif

#ifdef ONEMO_GNSS_SUPPORT
void onemo_test_lbs(void);
#endif
void onemo_test_tts(unsigned char **cmd,int len);
void onemo_test_write_file(unsigned char **cmd,int len);
void onemo_test_read_file(unsigned char **cmd,int len);
void onemo_test_get_date(unsigned char **cmd,int len);
extern osThreadId OC_Main_TaskHandle;
extern osThreadId OC_AT_TaskHandle;
extern unsigned char virt_at_buffer[2048];
extern int virt_at_len;
#endif