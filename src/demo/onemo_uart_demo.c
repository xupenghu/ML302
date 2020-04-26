/*********************************************************
*  @file    onemo_uart_demo.c
*  @brief   ML302 OpenCPU uart APIs example
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/17
********************************************************/
#include "onemo_main.h"
#define OPENCPU_MAIN_URAT ONEMO_UART_0

osThreadId OC_Uart_TaskHandle;
unsigned int data_arrived = 0;
unsigned char uart_buf[1024];
int uart_buf_len = 0;

osMutexDef( cmdMutex );
osMutexId cmd_mutex;


unsigned char * cmd_buf[20]; //最多接受20个参数
int cmd_len;
int char_to_int(unsigned char *s)
{
	int i;  
    int n = 0;  
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)  
    {  
        n = 10 * n + (s[i] - '0');  
    }  
    return n;  
}
static void test_oc_uart_cb(void *param,unsigned int evt)
{
   data_arrived =1;
}
static uart_recv_task(void *param)
{
    unsigned char buf[64] = {0};
    unsigned char * p;
    int len ;
    while(true)
    {
        if(data_arrived == 1)
        {             
            uart_buf_len = onemo_uart_receive(OPENCPU_MAIN_URAT,uart_buf,1024);
            
            while(uart_buf_len < 1024)
            {
                len = onemo_uart_receive(OPENCPU_MAIN_URAT,uart_buf+uart_buf_len,1024-uart_buf_len);
                
                if(len == 0)
                    break;
                uart_buf_len += len;
            } 
            
            if(osMutexWait( cmd_mutex, 0 ) != 0)
            {
                onemo_printf("busy\n");
                uart_buf_len = 0;
                data_arrived = 0;
                continue;
            }
            if(onemo_cmd_engine(uart_buf,&uart_buf_len,cmd_buf,&cmd_len) == -1)
            {
                onemo_printf("error\n");
                uart_buf_len = 0;
                osMutexRelease( cmd_mutex );
            }
            else
            {
                uart_buf_len = 0;
                osSignalSet(OC_Main_TaskHandle, 0x0004);   
            }
          
            data_arrived = 0;
        }
        osDelay(20);   
    }
}

void onemo_printf (char *str, ...)
{

    static  char s[600]; //This needs to be large enough to store the string TODO Change magic number
    va_list args;
    int len;
    if ((str == NULL) || (strlen(str) == 0))
    {
        return;
    }
    va_start (args, str);
    len = vsprintf ((char*)s, str, args);
    va_end (args);
    onemo_uart_send_no_cache(OPENCPU_MAIN_URAT,s,len,100);
  
}

void onemo_test_uart_init()
{
    onemo_uart_cfg_t cfg;
    
    osThreadDef(OC_Uart_Task, uart_recv_task, osPriorityNormal, 0, 1024);
    OC_Uart_TaskHandle = osThreadCreate(osThread(OC_Uart_Task), 0);
    cmd_mutex = osMutexCreate( osMutex(cmdMutex) );
    cfg.id = OPENCPU_MAIN_URAT;
    cfg.baud = ONEMO_UART_BAUD_115200;
    cfg.databit = ONEMO_UART_DATA_BITS_8;
    cfg.stopbit = ONEMO_UART_STOP_BITS_1;
    cfg.parity = ONEMO_UART_NO_PARITY;
    cfg.cb = test_oc_uart_cb;
    cfg.param = "test param";
    cfg.event = ONEMO_UART_EVENT_RX_ARRIVED;
    cfg.rx_buf_size = 4*1024;
    cfg.tx_buf_size = 4*1024;
    onemo_uart_init(&cfg);
    
    
    
}