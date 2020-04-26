/*********************************************************
*  @file    onemo_sys_demo.c
*  @brief   ML302 OpenCPU network example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/15
********************************************************/
#include "onemo_main.h"
unsigned char global_imei[30]= {0};
unsigned char global_imsi[30] = {0};
unsigned char global_iccid[30] = {0};
void onemo_force_download(unsigned char **cmd,int len)
{
    onemo_printf("[FORCEDL]go to download mode\n");
    onemo_vir_at_send("AT^FORCEDNLD\r\n");
}
void onemo_test_get_imei()
{
    unsigned char *p1,*p2;
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CGSN=1\r\n");
    p1= onemo_check_at_str("CGSN",10);
    if(p1 == 0)
    {
        onemo_printf("imei read error\n");
        return;
    }        
    if(virt_at_len < 30) 
    {
        onemo_printf("imei read error");
    }
    p2 = strstr(p1+10,"\r\n");
    *p2 =  0;
    strcpy(global_imei,p1+10);
    onemo_printf("IMEI:%s\n",global_imei);
}
void onemo_test_get_imsi()
{
    unsigned char *p1,*p2;
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CIMI\r\n");
    p1= onemo_check_at_str("CIMI",10);
    if(p1 == 0)
    {
        onemo_printf("imsi read error\n");
        return;
    }        
    if(virt_at_len < 30) 
    {
        onemo_printf("imsi read error");
    }
    p2 = strstr(p1+8,"\r\n");
    *p2 =  0;
    strcpy(global_imsi,p1+8);
    onemo_printf("IMSI:%s\n",global_imsi);
}



void onemo_test_get_iccid()
{
    unsigned char *p1,*p2;
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CCID\r\n");
    p1= onemo_check_at_str("+CCID:",10);
    if(p1 == 0)
    {
        onemo_printf("iccid read error\n");
        return;
    }        
    if(virt_at_len < 30) 
    {
        onemo_printf("iccid read error");
    }
    p2 = strstr(p1+7,"\r\n");
    *p2 =  0;
    strcpy(global_iccid,p1+7); 
    onemo_printf("ICCID:%s\n",global_iccid);
}

void onemo_test_reboot(unsigned char **cmd,int len)
{
    
    onemo_printf("[REBOOT]rebooting...\n");
    onemo_sys_reboot();   
}
