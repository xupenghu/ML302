/*********************************************************
*  @file    onemo_main.c
*  @brief   ML302 OpenCPU Entry File
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/08
********************************************************/

#define DBG_TAG "MAIN"
#define DBG_LVL DBG_LOG
#include "shell/dbg.h"
#include "onemo_main.h" 
osThreadId OC_Main_TaskHandle;

void onemo_main_task(void *p)
{  
   unsigned char buf[50] = {0};
   int i;
   struct l_tm t;
   onemo_test_uart_init();
   LOG_D("\r\nML302 OpenCPU Starts\n");
   LOG_D("Boot Cause:%d\n",onemo_sys_get_boot_cause());
   onemo_sys_get_sdk_swver(buf,50);
   LOG_D("SDK VERSION:%s\n",buf);
   onemo_sys_get_base_swver(buf,50);
   LOG_D("BASELINE VERSION:%s\n",buf);
   onemo_sys_get_hwver(buf,50);
   LOG_D("HW VERSION:%s\n",buf);  
   LOG_D("waiting for network...\n");
   onemo_test_vir_at_init(); 
   onemo_test_network_config();
   time_to_date(osiEpochSecond()+onemo_get_timezone()* 15*60,&t);
   LOG_D("Now:%d-%d-%d:%d:%d:%d\n",t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
   onemo_test_get_imei();
   onemo_test_get_imsi();
   onemo_test_get_iccid();
   #ifdef ONEMO_FOTA_SUPPORT   //启用OneNet FOTA不可删除
   char *version = NULL;
   onVersion(&version);
   LOG_D("OneNet FOTA version:%s\n", version);
#endif
   while (1)
   {
       osDelay(1000);
   }
}


void ML302_OpenCPU_Entry()
{
#ifdef ONEMO_DEMO_SUPPORT
    onemo_test_alarm_init();
    osThreadDef(OC_Main_Task, onemo_main_task, osPriorityNormal, 0, 8192);
    OC_Main_TaskHandle = osThreadCreate(osThread(OC_Main_Task), 0);  
#endif
#ifdef ONEMO_FOTA_SUPPORT   //启用OneNet FOTA不可删除
    onemo_onenet_fota_regcbex();
#endif
}
//SDK中调用，不可删除
int onemo_wdt_config_cb(void)
{
    return 0;//关闭看门狗
    //return 1;//开启看门狗
}