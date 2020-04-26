/*********************************************************
 *  @file    onemo_http_demo.c
 *  @brief   ML302 OpenCPU HTTP示例
 *  Copyright (c) 2019 China Mobile IOT.
 *  All rights reserved.
 *  created by XieGangLiang 2019/11/25
 ********************************************************/
 #ifdef ONEMO_HTTP_SUPPORT
#include "httpclient.h"
 
void onemo_test_http(unsigned char **cmd,int len)
{    
    //char* url = "https://www.baidu.com/";
    unsigned char url[100] = {0};
    int https = char_to_int(cmd[2]);
    int ret;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    if(https)
    {
        sprintf(url,"https://%s/",cmd[3]);
        onemo_printf("[HTTP]Site:%s\r\n",url);
    }
    else
    {
        sprintf(url,"http://%s/",cmd[3]);
        onemo_printf("[HTTP]Site:%s\r\n",url);
    }
    buf = malloc(600);
    if (buf == NULL) 
    {
        onemo_printf("[HTTP]Malloc failed.\r\n");
        return;
    }
    memset(buf, 0, sizeof(buf));
    client_data.response_buf = buf;  
    client.timeout_in_sec = 20;
    client_data.response_buf_len = 600;  
    onemo_printf("[HTTP]Get Start\n");
    ret = httpclient_get(&client, url, &client_data);
    if(ret < 0)
    {
        onemo_printf("[HTTP]Get Failed:%d\n",ret);
    }
    else
    {
        onemo_printf("[HTTP]Get:\n%s\n",client_data.response_buf);
    }
}
void onemo_test_http2()
{
    char* url = "https://www.baidu.com/";
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char *buf = NULL;
    buf = malloc(600);
    if (buf == NULL) 
    {
        onemo_printf("Malloc failed.\r\n");
        return;
    }
    memset(buf, 0, sizeof(buf));
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    onemo_printf("start http test\n");
    client_data.response_buf_len = 600;  //Sets the buffer size.
    onemo_printf("start connect\n");
    ret = httpclient_connect(&client, url);
    if (!ret) {
        onemo_printf("start send request\n");
        ret = httpclient_send_request(&client, url, HTTPCLIENT_GET, &client_data);

        if (!ret) {
            onemo_printf("start recv\n");
            ret = httpclient_recv_response(&client, &client_data);
            onemo_printf("Data received: %s\r\n", client_data.response_buf);
        }
    }
    onemo_printf("close\n");
    httpclient_close(&client);
}
#endif