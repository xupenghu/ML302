/*********************************************************
*  @file    onemo_adc_demo.c
*  @brief   ML302 OpenCPU ADC APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/21
********************************************************/
#include "onemo_main.h"


void onemo_test_adc(unsigned char **cmd,int len)
{
    if(strcmp(cmd[2],"1") == 0)
        onemo_printf("[ADC][%d]:%d\n",1,onemo_adc_get_vol(ONEMO_ADC_CHANNEL_1));
    else
        onemo_printf("[ADC][%d]:%d\n",2,onemo_adc_get_vol(ONEMO_ADC_CHANNEL_2));
}