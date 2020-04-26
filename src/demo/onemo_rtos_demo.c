/*********************************************************
*  @file    onemo_rtos_demo.c
*  @brief   ML302 OpenCPU rtos example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/11/12
********************************************************/
#include "onemo_main.h"

static int cnt = 0;
static osTimerId timer; 
static void timerHandler( void * arg )
{
   cnt++;
   onemo_printf("timer:%d\n",cnt);
   osTimerStart( timer, 2000 );
}

osTimerDef( timer, timerHandler );

void onemo_test_os_timer()
{
    timer = osTimerCreate( osTimer(timer), osTimerOnce, 0 );
    osTimerStart( timer, 2000 );
    onemo_printf("timer start ok\n");
}