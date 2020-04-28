/*********************************************************
*  @file    onemo_vir_at_demo.c
*  @brief   ML302 OpenCPU virture AT channel example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/08
********************************************************/
#define DBG_TAG "VIR_AT"
#define DBG_LVL DBG_LOG
#include "shell/dbg.h"
#include "shell/shell.h"
#include "onemo_main.h"
unsigned char virt_at_buffer[2048];
int virt_at_len = 0;
osThreadId OC_AT_TaskHandle;

osMutexDef(VIR_AT_Mutex);
osMutexId vir_at_mutex;

void onemo_clear_virt_at_buffer()
{
    osMutexWait(vir_at_mutex, osWaitForever);
    virt_at_len = 0;
    osMutexRelease(vir_at_mutex);
}
unsigned char *onemo_check_at_str(const char *str, int delay)
{
    int i, j;
    unsigned char *p;
    j = 0;
    while (j < delay)
    {
        osDelay(100);
        j++;
        if (virt_at_len > 0)
        {
            for (i = 0; i < virt_at_len; i++) //将有效长度内所有含0的地方替换成'!'，以方便进行字符串检测
            {
                if (virt_at_buffer[i] == 0)
                {
                    virt_at_buffer[i] = '!';
                }
            }
            virt_at_buffer[virt_at_len] = 0;
            p = strstr((const char *)virt_at_buffer, str);
            if (p != 0)
                return p;
        }
    }
    return 0;
}
static void virture_at_task()
{
    while (1)
    {
        osSignalWait(0x0004, osWaitForever);
        if (virt_at_len >= 2048)
        {
            osMutexWait(vir_at_mutex, osWaitForever);
            virt_at_len = 0;
            osMutexRelease(vir_at_mutex);
        }
        osMutexWait(vir_at_mutex, osWaitForever);
        virt_at_len += onemo_vir_at_get(virt_at_buffer + virt_at_len, 2048 - virt_at_len);
        osMutexRelease(vir_at_mutex);
    }
}
void onemo_test_send_at(int argc, char *agrv[])
{
    if (argc != 2)
    {
        LOG_E("error, argc = %d, usage: AT+CGSN=1", argc);
    }
    else
    {
        onemo_clear_virt_at_buffer();
        unsigned char p[100] = {0};
        sprintf(p, "%s\r\n", agrv[1]);
        LOG_I("[VIRT_AT]:%d\n", onemo_vir_at_send(p));
    }
}

void onemo_test_dump_at(int argc, char *agrv[])
{
    int i;
    LOG_I("[DUMP_AT]:Start\n");
    for (i = 0; i < virt_at_len; i++)
    {
        onemo_printf("%c", virt_at_buffer[i]);
    }
}

/*
    虚拟AT回调函数，当有AT命令返回或者URC消息时，此函数会被调用
*/
void onemo_sys_at_callback(void *param)
{
    osSignalSet(OC_AT_TaskHandle, 0x0004);
}
void onemo_test_vir_at_init(void)
{
    vir_at_mutex = osMutexCreate(osMutex(VIR_AT_Mutex));
    osThreadDef(OC_AT_Task, virture_at_task, osPriorityNormal, 0, 1024);
    OC_AT_TaskHandle = osThreadCreate(osThread(OC_AT_Task), 0);
    onemo_vir_at_init(onemo_sys_at_callback);
}




