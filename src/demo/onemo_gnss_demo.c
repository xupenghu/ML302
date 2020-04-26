/*********************************************************
*  @file    onemo_gnss_demo.c
*  @brief   ML302 OpenCPU GNSS demo file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by yuyangyi 2020/01/08
********************************************************/
#ifdef ONEMO_GNSS_SUPPORT
#include "onemo_main.h"
#include "onemo_gnss.h"

void onemo_test_lbs(void)
{
    char *cellloc = NULL;
    onemo_lbs_getlocation(&cellloc);
    onemo_printf("LBS:%s\n", cellloc);
    onemo_sys_log("LBS:%s", cellloc);

    return;
}

#endif