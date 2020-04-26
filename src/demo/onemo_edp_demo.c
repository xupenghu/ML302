/*********************************************************
*  @file    onemo_edp_demo.c
*  @brief   ML302 OpenCPU EDP demo file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by Jinjun 2019/11/26
********************************************************/
#ifdef ONEMO_EDP_SUPPORT
#include "onemo_main.h"
#include "Openssl.h"
#include "EdpKit.h"
#include "onemo_main.h"

/*----------------------------错误码-----------------------------------------*/
#define ERR_CREATE_SOCKET -1
#define ERR_HOSTBYNAME -2
#define ERR_CONNECT -3
#define ERR_SEND -4
#define ERR_TIMEOUT -5
#define ERR_RECV -6

#define Socket(a, b, c) socket(a, b, c)
#define Connect(a, b, c) connect(a, b, c)
#define Close(a) close(a)
#define Read(a, b, c) read(a, b, c)
#define Recv(a, b, c, d) recv(a, (void *)b, c, d)
#define Select(a, b, c, d, e) select(a, b, c, d, e)
#define Send(a, b, c, d) send(a, (const int8 *)b, c, d)
#define Write(a, b, c) write(a, b, c)
#define GetSockopt(a, b, c, d, e) getsockopt((int)a, (int)b, (int)c, (void *)d, (socklen_t *)e)
#define SetSockopt(a, b, c, d, e) setsockopt((int)a, (int)b, (int)c, (const void *)d, (int)e)
#define GetHostByName(a) gethostbyname((const char *)a)
#define bzero(a, b) memset(a, 0, b)
#define bcopy(a, b, c) memcpy(b, a, c)

static int g_is_encrypt = 0;
int g_sockfd;
osThreadId OC_edp_TaskHandle;

static fd_set iot_tcp_readfds;
static fd_set iot_tcp_exptfds;  

static osTimerId edptimer; 
static int edp_timeout_flag = 0;
static int edp_recv_data_flag = 0;

void hexdump(const unsigned char *buf, uint32 num)
{
    onemo_printf("begin hexdump\n");

    uint32 i = 0;
    for (; i < num; i++)
    {
        onemo_printf("%02X ", buf[i]);
        if ((i + 1) % 8 == 0)
            onemo_printf("\n");
    }
    onemo_printf("\n");
}

int32 Open(const uint8 *addr, int16 portno)
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    char portstr[32] = {
        0,
    };
    sprintf(portstr, "%d", portno);
    if (getaddrinfo(addr, portstr, &hints, &addr_list) != 0)
        return (ERR_HOSTBYNAME);

    /* Try the sockaddrs until a connection succeeds */
    ret = ERR_HOSTBYNAME;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next)
    {
        g_sockfd = (int)socket(cur->ai_family, cur->ai_socktype,
                             cur->ai_protocol);
        if (g_sockfd < 0)
        {
            ret = ERR_CREATE_SOCKET;
            continue;
        }

        if (connect(g_sockfd, cur->ai_addr, cur->ai_addrlen) == 0)
        {
            ret = g_sockfd;
            break;
        }

        close(g_sockfd);
        ret = ERR_CONNECT;
    }

    freeaddrinfo(addr_list);

    return (ret);
}

int32 DoSend(int32 sockfd, const char *buffer, uint32 len)
{
    int32 total = 0;
    int32 n = 0;
    while (len != total)
    {
        /* 试着发送len - total个字节的数据 */
        n = Send(sockfd, buffer + total, len - total, 0);
        if (n <= 0)
        {
            onemo_printf("[EDP][ERROR]: writing to socket\n");
            return n;
        }
        /* 成功发送了n个字节的数据 */
        total += n;
        onemo_printf("[EDP]:send %d bytes\n", n);
    }
    /* wululu test print send bytes */
    //hexdump((const unsigned char *)buffer, len);
    return total;
}

void recv_thread_func(void *arg)
{
    int error = 0;
    int n, rtn;
    uint8 mtype, jsonorbin;
    char buffer[1024];
    RecvBuffer *recv_buf = NewBuffer();
    EdpPacket *pkg;

    char *src_devid;
    char *push_data;
    uint32 push_datalen;

    cJSON *save_json;
    char *save_json_str;

    cJSON *desc_json;
    char *desc_json_str;
    char *save_bin;
    uint32 save_binlen;
    unsigned short msg_id;
    unsigned char save_date_ret;

    char *cmdid;
    uint16 cmdid_len;
    char *cmd_req;
    uint32 cmd_req_len;
    EdpPacket *send_pkg;
    char *ds_id;
    double dValue = 0;
    int iValue = 0;
    char *cValue = NULL;

    char *simple_str = NULL;
    char cmd_resp[] = "ok";
    unsigned cmd_resp_len = 0;

    DataTime stTime = {0};

    FloatDPS *float_data = NULL;
    int count = 0;
    int i = 0;

    struct UpdateInfoList *up_info = NULL;

    struct timeval timeout={0,0};
    int ret = 0;

    onemo_printf("[EDP]: recv thread start ...\n");

    while (error == 0)
    {
        FD_ZERO(&iot_tcp_readfds);
        FD_ZERO(&iot_tcp_exptfds);

        FD_SET(g_sockfd, &iot_tcp_readfds);
        FD_SET(g_sockfd, &iot_tcp_exptfds);

        timeout.tv_sec = 1;

        ret = select(g_sockfd + 1, &iot_tcp_readfds, NULL, &iot_tcp_exptfds, &timeout);
        if(ret < 0)
        {
            onemo_printf("[EDP]: %s() read select error ,ret=%d\n", __func__, ret);

            Close(g_sockfd);
            break;
        }

        else if(ret == 0)
        {
            //onemo_printf("[EDP] : %s() ret == 0\n", __func__);
            osDelay(10);
        }
        else
        {
            if(FD_ISSET(g_sockfd, &iot_tcp_exptfds))
            {
                onemo_printf("[EDP]: %s() tcp_exptfds error\n", __func__);
                Close(g_sockfd);
                break;
            }

            if(0 == FD_ISSET(g_sockfd, &iot_tcp_readfds))
            {
                /* program should not reach current branch */
                onemo_printf("[EDP]: %s() tcp_readfds unkwon error\n", __func__);
                osDelay(1000);
                continue;
            }

            memset(buffer,0,sizeof(buffer));

            /* 试着接收1024个字节的数据 */
            n = Recv(g_sockfd, buffer, 1024, 0);
            if (n <= 0)
            {
                onemo_printf("[EDP]: recv from server, error");
                break;
            }
            onemo_printf("[EDP]:recv from server, bytes: %d\n", n);
            /* wululu test print send bytes */
            //hexdump((const unsigned char *)buffer, n);
            /* 成功接收了n个字节的数据 */
            WriteBytes(recv_buf, buffer, n);
            while (1)
            {
                /* 获取一个完成的EDP包 */
                if ((pkg = GetEdpPacket(recv_buf)) == 0)
                {
                    onemo_printf("[EDP]:need more bytes...\n");
                    break;
                }
                /* 获取这个EDP包的消息类型 */
                mtype = EdpPacketType(pkg);
                if (mtype != ENCRYPTRESP)
                {
                    if (g_is_encrypt)
                    {
                        SymmDecrypt(pkg);
                    }
                }
                onemo_printf("[EDP]:mtype(%d)\n",mtype);
                /* 根据这个EDP包的消息类型, 分别做EDP包解析 */
                switch (mtype)
                {
                case ENCRYPTRESP:
                    UnpackEncryptResp(pkg);
                    break;
                case CONNRESP:
                    /* 解析EDP包 - 连接响应 */
                    rtn = UnpackConnectResp(pkg);
                    onemo_printf("[EDP]:recv connect resp, rtn: %d\n", rtn);
                    break;
                case PUSHDATA:
                    /* 解析EDP包 - 数据转发 */
                    UnpackPushdata(pkg, &src_devid, &push_data, &push_datalen);
                    onemo_printf("[EDP]:recv push data, src_devid: %s, push_data: %s, len: %d\n",
                           src_devid, push_data, push_datalen);
                    free(src_devid);
                    free(push_data);
                    break;
                case UPDATERESP:
                    UnpackUpdateResp(pkg, &up_info);
                    while (up_info)
                    {
                        onemo_printf("[EDP]:name = %s\n", up_info->name);
                        onemo_printf("[EDP]:version = %s\n", up_info->version);
                        onemo_printf("[EDP]:url = %s\nmd5 = ", up_info->url);
                        for (i = 0; i < 32; ++i)
                        {
                            onemo_printf("%c", (char)up_info->md5[i]);
                        }
                        onemo_printf("\n");
                        up_info = up_info->next;
                    }
                    FreeUpdateInfolist(up_info);
                    break;

                case SAVEDATA:
                    /* 解析EDP包 - 数据存储 */
                    if (UnpackSavedata(pkg, &src_devid, &jsonorbin) == 0)
                    {
                        if (jsonorbin == kTypeFullJson || jsonorbin == kTypeSimpleJsonWithoutTime || jsonorbin == kTypeSimpleJsonWithTime)
                        {
                            onemo_printf("[EDP]:json type is %d\n", jsonorbin);
                            /* 解析EDP包 - json数据存储 */
                            /* UnpackSavedataJson(pkg, &save_json); */
                            /* save_json_str=cJSON_Print(save_json); */
                            /* printf("recv save data json, src_devid: %s, json: %s\n", */
                            /*     src_devid, save_json_str); */
                            /* free(save_json_str); */
                            /* cJSON_Delete(save_json); */

                            /* UnpackSavedataInt(jsonorbin, pkg, &ds_id, &iValue); */
                            /* printf("ds_id = %s\nvalue= %d\n", ds_id, iValue); */

                            UnpackSavedataDouble(jsonorbin, pkg, &ds_id, &dValue);
                            onemo_printf("[EDP]:ds_id = %s\nvalue = %f\n", ds_id, dValue);

                            /* UnpackSavedataString(jsonorbin, pkg, &ds_id, &cValue); */
                            /* printf("ds_id = %s\nvalue = %s\n", ds_id, cValue); */
                            /* free(cValue); */

                            free(ds_id);
                        }
                        else if (jsonorbin == kTypeBin)
                        { /* 解析EDP包 - bin数据存储 */
                            UnpackSavedataBin(pkg, &desc_json, (uint8 **)&save_bin, &save_binlen);
                            desc_json_str = cJSON_Print(desc_json);
                            onemo_printf("[EDP]:recv save data bin, src_devid: %s, desc json: %s, bin: %s, binlen: %d\n",
                                   src_devid, desc_json_str, save_bin, save_binlen);
                            free(desc_json_str);
                            cJSON_Delete(desc_json);
                            free(save_bin);
                        }
                        else if (jsonorbin == kTypeString)
                        {
                            UnpackSavedataSimpleString(pkg, &simple_str);

                            onemo_printf("[EDP]:%s\n", simple_str);
                            free(simple_str);
                        }
                        else if (jsonorbin == kTypeStringWithTime)
                        {
                            UnpackSavedataSimpleStringWithTime(pkg, &simple_str, &stTime);

                            onemo_printf("[EDP]:time:%u-%02d-%02d %02d-%02d-%02d\nstr val:%s\n",
                                   stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second, simple_str);
                            free(simple_str);
                        }
                        else if (jsonorbin == kTypeFloatWithTime)
                        {
                            if (UnpackSavedataFloatWithTime(pkg, &float_data, &count, &stTime))
                            {
                                onemo_printf("[EDP]:UnpackSavedataFloatWithTime failed!\n");
                            }

                            onemo_printf("[EDP]:read time:%u-%02d-%02d %02d-%02d-%02d\n",
                                   stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second);
                            onemo_printf("[EDP]:read float data count:%d, ptr:[%p]\n", count, float_data);

                            for (i = 0; i < count; ++i)
                            {
                                onemo_printf("[EDP]:ds_id=%u,value=%f\n", float_data[i].ds_id, float_data[i].f_data);
                            }

                            free(float_data);
                            float_data = NULL;
                        }
                        free(src_devid);
                    }
                    else
                    {
                        onemo_printf("[EDP]: SAVEDATA error\n");
                    }
                    break;
                case SAVEACK:
                    UnpackSavedataAck(pkg, &msg_id, &save_date_ret);
                    onemo_printf("[EDP]:save ack, msg_id = %d, ret = %d\n", msg_id, save_date_ret);
                    break;
                case CMDREQ:
                    if (UnpackCmdReq(pkg, &cmdid, &cmdid_len,
                                     &cmd_req, &cmd_req_len) == 0)
                    {
                        /*
                         * 用户按照自己的需求处理并返回，响应消息体可以为空，此处假设返回2个字符"ok"。
                         * 处理完后需要释放
                         */
                         char RecvCmdStr[256];
                         memset(RecvCmdStr, 0, sizeof(RecvCmdStr));
                         memcpy(RecvCmdStr, cmd_req, cmd_req_len);
                         onemo_printf("[EDP]:recvlen(%d),recvdata(%s)\n", cmd_req_len,RecvCmdStr);

                         edp_recv_data_flag = 1;
                         
                        cmd_resp_len = strlen(cmd_resp);
                        send_pkg = PacketCmdResp(cmdid, cmdid_len,
                                                 cmd_resp, cmd_resp_len);
                        if (g_is_encrypt)
                        {
                            SymmEncrypt(send_pkg);
                        }
                        
                        DoSend(g_sockfd, (const char *)send_pkg->_data, send_pkg->_write_pos);
                        DeleteBuffer(&send_pkg);

                        free(cmdid);
                        free(cmd_req);
                    }
                    break;
                case PINGRESP:
                    /* 解析EDP包 - 心跳响应 */
                    UnpackPingResp(pkg);
                    onemo_printf("[EDP]:recv ping resp\n");
                    break;

                default:
                    /* 未知消息类型 */
                    error = 1;
                    onemo_printf("[EDP]:recv failed...\n");
                    break;
                }
                DeleteBuffer(&pkg);
            }
        }
   }
    DeleteBuffer(&recv_buf);

    onemo_printf("[EDP]:recv thread end ...\n");
    osThreadTerminate(OC_edp_TaskHandle);
}

static void timerHandler( void * arg )
{
   edp_timeout_flag = 1;
}

osTimerDef( edptimer, timerHandler );

void onemo_test_edp(unsigned char **cmd,int len)
{
    int n, ret;
    char *dst_dev = NULL;
    
    EdpPacket *send_pkg;
    char push_data[] = {'a', 'b', 'c'};
    char text1[] = "{\"name\": \"Jack\"}";
    /* cJSON中文只支持unicode编码   */
    char text2[] = "{\"ds_id\": \"temperature\"}";
    cJSON *save_json, *desc_json;
    char save_bin[] = {'c', 'b', 'a'};
    char send_str[] = ",;temperature,2015-03-22 22:31:12,22.5;humidity,35%;pm2.5,89;1001";
    FloatDPS send_float[] = {{1, 0.5}, {2, 0.8}, {3, -0.5}};
    struct UpdateInfoList *up_info = NULL;

    SaveDataType data_type;

    char edp_server_ip[32] = "jjfaedp.hedevice.com";
    int edp_server_port = 876;

    //char src_api_key[36]="ExYp=ztqZKJx433s02ctSpmNCp0=";
    //char src_dev[16]="518978058";

    char src_api_key[36]={0};
    char src_dev[16]={0};

    memcpy(src_api_key,cmd[2],sizeof(src_api_key));
    memcpy(src_dev,cmd[3],sizeof(src_dev));

    //onemo_printf("edp demo start\n");
    
        /* create a socket and connect to server */
    g_sockfd = Open((const uint8 *)edp_server_ip, edp_server_port);
    if (g_sockfd < 0)
    {
        //exit(0);
        onemo_printf("[EDP][ERROR]:open socket fail\n");
    }
    //onemo_printf("Open sock successed sockfd=%d\n", g_sockfd);

    osThreadDef(OC_edp_Task, recv_thread_func, osPriorityNormal, 0, 4096*4);
    OC_edp_TaskHandle = osThreadCreate(osThread(OC_edp_Task), 0);

    if (g_is_encrypt)
    {
        send_pkg = PacketEncryptReq(kTypeAes);
        /* 向设备云发送加密请求 */
        //onemo_printf("send encrypt to server, bytes: %d\n", send_pkg->_write_pos);
        ret = DoSend(g_sockfd, (const char *)send_pkg->_data, send_pkg->_write_pos);
        DeleteBuffer(&send_pkg);
        osDelay(1000);
    }

    /* connect to server */
    /*    send_pkg = PacketConnect1(src_dev, "Bs04OCJioNgpmvjRphRak15j7Z8=");*/
    send_pkg = PacketConnect1(src_dev, src_api_key);

    if (g_is_encrypt)
    {
        SymmEncrypt(send_pkg);
    }

    /* send_pkg = PacketConnect2("433223", "{ \"SYS\" : \"0DEiuApATHgLurKNEl6vY4bLwbQ=\" }");*/
    /* send_pkg = PacketConnect2("433223", "{ \"13982031959\" : \"888888\" }");*/

    /* 向设备云发送连接请求 */
    //onemo_printf("send connect to server, bytes: %d\n", send_pkg->_write_pos);
    ret = DoSend(g_sockfd, (const char *)send_pkg->_data, send_pkg->_write_pos);
    DeleteBuffer(&send_pkg);

    osDelay(1000);

    /*send simple format (string)*/
    send_pkg = PacketSavedataSimpleString(dst_dev, send_str, 0);

    /*send ping*/
    //send_pkg = PacketPing();

    /*send push data*/
    //send_pkg = PacketPushdata("533236972", push_data, sizeof(push_data));

    /*send save json*/
    //data_type = kTypeFullJson;
    
    /*send save json simple format without time*/
    //data_type = kTypeSimpleJsonWithoutTime;
    
    /*send save json simple format with time*/
    //data_type = kTypeSimpleJsonWithTime;
    
    //send_pkg = PacketSavedataInt(data_type, dst_dev, "123", 1234, 0, 0);

    /*send save bin*/
    //desc_json = cJSON_Parse(text2);
    //send_pkg = PacketSavedataBin(dst_dev, desc_json, (const uint8 *)save_bin, sizeof(save_bin), 0);

    /*send simple format (string) with datetime*/
    //send_pkg = PacketSavedataSimpleStringWithTime(dst_dev, send_str, NULL, 0);

    /*send float data with datetime*/
    //send_pkg = PackSavedataFloatWithTime(dst_dev, send_float, 3, NULL, 0);

    /*send update info*/
    //send_pkg = PacketUpdateReq(up_info);

    if (g_is_encrypt)
    {
        SymmEncrypt(send_pkg);
    }
    
    //onemo_printf("send simple format (string)\n");
    DoSend(g_sockfd, (const char *)send_pkg->_data, send_pkg->_write_pos);
    DeleteBuffer(&send_pkg);
    onemo_printf("[EDP]:send string\n");

    edptimer = osTimerCreate( osTimer(edptimer), osTimerOnce, 0 );
    osTimerStart( edptimer, 2*60*1000 );

    while(1)
    {

        if(edp_timeout_flag > 0)
        {
            edp_timeout_flag = 0;
            onemo_printf("[EDP][ERROR]:timeout\n");
            break;
        }

        if(edp_recv_data_flag > 0)
        {
            edp_recv_data_flag = 0;
            onemo_printf("[EDP][OK]:recv data\n");
            break;
        }

        osDelay(500);
        
    }

    osTimerStop( edptimer);

    /* close socket */
    Close(g_sockfd);

    
    onemo_printf("[EDP]:test end\n");
}
#endif