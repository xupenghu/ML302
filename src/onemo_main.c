/*********************************************************
*  @file    onemo_main.c
*  @brief   ML302 OpenCPU Entry File
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/08
********************************************************/
#ifdef ONEMO_DEMO_SUPPORT

#include "onemo_main.h" 
osThreadId OC_Main_TaskHandle;
extern osMutexId cmd_mutex;
extern unsigned char * cmd_buf[20]; //最多接受20个参数
extern int cmd_len;

onemo_cmd_t cmd_vector[] = {
            {"GPIO_READ" , onemo_test_gpio_read}, //ONEMO:GPIO_READ:n
            {"GPIO_WRITE", onemo_test_gpio_write},//ONEMO:GPIO_WRITE:n
            {"GPIO_IRQ"  , onemo_test_gpio_irq},  //ONEMO:GPIO_IRQ:n:INTERRUPT_ON/INTERRUPT_OFF
            {"FORCEDL"   , onemo_force_download}, //ONEMO:FORCEDL
            {"I2C"       , onemo_test_i2c_bmp180},//ONEMO:I2C
            {"TCPCLIENT" , onemo_test_tcpclient}, //ONEMO:TCPCLIENT:106.54.97.79:2019
            {"TCPCLIENTSEND" , onemo_test_tcpclient_send}, //ONEMO:TCPCLIENTSEND:DATA
            {"UDP"       , onemo_test_udp},       //ONEMO:UDP:106.54.97.79:2019
            {"ADC"       , onemo_test_adc},       //ONEMO:ADC:n
            {"RP_ALARM"  , onemo_test_set_repeate_alarm}, //ONEMO:RP_ALARM:0:1:2:14:20:5  在周天、周一、周二的14点20分5s提示
            {"SG_ALARM"  , onemo_test_single_time_alarm}, //ONEMO:SG_ALARM:50  50s后提醒
            {"VIRT_AT"   , onemo_test_send_at},   //ONEMO:VIRT_AT:AT+CGACT?
            {"DUMP_AT"   , onemo_test_dump_at},   //ONEMO:DUMP_AT
            {"REBOOT"    , onemo_test_reboot},    //ONEMO:REBOOT
#ifdef  ONEMO_HTTP_SUPPORT
            {"HTTP"      , onemo_test_http},      //ONEMO:HTTP:n:www.baidu.com
#endif
#ifdef  ONEMO_EDP_SUPPORT
            {"EDP"       , onemo_test_edp},       //ONEMO:EDP
#endif
#ifdef  ONEMO_ALIYUN_SUPPORT
            {"ALIYUN"    , onemo_test_aliyun},    //ONEMO:ALIYUN
#endif
#ifdef  ONEMO_MQTT_SUPPORT                        //ONEMO:MQTT:server:port:client_id:keepAlive:user:passwd:clean:version:encrypt:rec:subtopic:subqos:pubtopic:pubqos:pubdup:pubmessage
            {"MQTT"      , onemo_test_mqtt},      //ONEMO:MQTT:::::::::::abcaa::abcaa:::aaaaaaaa
#endif
#ifdef  ONEMO_DM_SUPPORT
            {"DM"        , onemo_test_dm_register}, 
#endif

#ifdef  ONEMO_FOTA_SUPPORT
            {"ONENET_FOTA_DEV"   , onemo_test_onenet_fota_setdevinfo},    //ONEMO:ONENET_FOTA_DEV
            {"ONENET_FOTA"       , onemo_test_onenet_fota},       //ONEMO:ONENET_FOTA
            {"ONENET_FOTA_DEL"       , onemo_test_onenet_fota_deletetask},       //ONEMO:ONENET_FOTA_DEL
            {"FOTA_UPDATE"       , onemo_test_fota_update},   //ONEMO:FOTA_UPDATE:rom_buffer:rom_size
#endif

#ifdef ONEMO_GNSS_SUPPORT
            {"LBS"       , onemo_test_lbs},       //ONEMO:LBS
#endif

            {"FWRITE"    , onemo_test_write_file}, //ONEMO:FWRITE:TESTMESSAGE
            {"FREAD"     , onemo_test_read_file},  //ONEMO:FREAD
            {"DATE"      , onemo_test_get_date},   //ONEMO:DATE
            {"MD5"       , onemo_test_md5},        //ONEMO:MD5:test_message
            {"BASE64"    , onemo_test_base64},        //ONEMO:BASE64:test_message
            {"SHA"       , onemo_test_sha},        //ONEMO:SHA:test_message
            {"CJSON"     , onemo_test_cjson}, 
			{"TTS"   	 , onemo_test_tts},//onemo:TTS:str:coding
};
void onemo_main_task(void *p)
{  
   unsigned char buf[50] = {0};
   int i;
   struct l_tm t;
   onemo_test_uart_init();
   onemo_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
   onemo_printf("ML302 OpenCPU Starts\n");
   onemo_printf("Boot Cause:%d\n",onemo_sys_get_boot_cause());
   onemo_sys_get_sdk_swver(buf,50);
   onemo_printf("SDK VERSION:%s\n",buf);
   onemo_sys_get_base_swver(buf,50);
   onemo_printf("BASELINE VERSION:%s\n",buf);
   onemo_sys_get_hwver(buf,50);
   onemo_printf("HW VERSION:%s\n",buf);  
   onemo_printf("waiting for network...\n");
   onemo_test_vir_at_init(); 
   onemo_test_network_config();
   time_to_date(osiEpochSecond()+onemo_get_timezone()* 15*60,&t);
   onemo_printf("Now:%d-%d-%d:%d:%d:%d\n",t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
   onemo_test_get_imei();
   onemo_test_get_imsi();
   onemo_test_get_iccid();
   #ifdef ONEMO_FOTA_SUPPORT   //启用OneNet FOTA不可删除
    char *version = NULL;
    onVersion(&version);
    onemo_printf("OneNet FOTA version:%s\n", version);
    #endif
    while(1)
    {
        onemo_printf("\nplease input cmds:\n");
        osSignalWait(0x0004, osWaitForever);

        if((cmd_len < 2))
        {
            onemo_printf("CMD NOT DEFINE\n");    
        }
        else
        {
            for(i = 0;i< (sizeof(cmd_vector)/sizeof(onemo_cmd_t));i++)
            {                
                if(strcmp(cmd_buf[1],cmd_vector[i].cmdstr) == 0)
                {
                    (*(cmd_vector[i].cmdfunc))(cmd_buf,cmd_len);
                    onemo_printf("OK\n");
                    break;
                }
            }
            if( i >=  (sizeof(cmd_vector)/sizeof(onemo_cmd_t)))
            {
                onemo_printf("CMD NOT DEFINE\n");
            }
        }  
        osMutexRelease( cmd_mutex );
    }
}

#endif

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