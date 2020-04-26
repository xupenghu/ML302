/*********************************************************
*  @file    onemo_cjson_demo.c
*  @brief   ML302 OpenCPU cjson example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2020/01/07
********************************************************/
#include "onemo_main.h"
void onemo_test_cjson(unsigned char **cmd,int len)
{
    cJSON *root=NULL;
    cJSON *sub_js=NULL;
    char *out=NULL;
    
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ver", "VX.X.X");
    cJSON_AddStringToObject(root, "imei", "1xxxxxxxxxxxxxxxx");
    cJSON_AddStringToObject(root, "hardware_ver", "VX.X.X");
    cJSON_AddStringToObject(root, "software_ver", "VX.X.X");
    cJSON_AddItemToObject(root, "data", sub_js = cJSON_CreateObject());
    cJSON_AddNumberToObject(sub_js, "status", 1);
    cJSON_AddTrueToObject(sub_js, "material");
    cJSON_AddTrueToObject(sub_js, "power_on");
    cJSON_AddNumberToObject(sub_js, "qty", 123);
    cJSON_AddStringToObject(root, "dt", "2020-01-07T05:15:52");
    
    out=cJSON_Print(root);
    cJSON_Delete(root); 
    onemo_printf("[CJSON]:\n");
    onemo_printf("%s\n",out);    
    cJSON_free(out);

}