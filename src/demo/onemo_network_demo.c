/*********************************************************
*  @file    onemo_network_demo.c
*  @brief   ML302 OpenCPU network example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/15
********************************************************/
#include "onemo_main.h"

struct   in_addr test_remote_addr;
uint16_t test_remote_port;
char test_msg[] = "network test\n";
char test_rx_buf[100];
int onemo_global_socket_id = -1;
osThreadId onemo_socket_TaskHandle = 0;
unsigned char onemo_socket_buf[1024] = {0};
static void onemo_socket_select_task(void *not_used);
/*
网络初始化配置
*/
void onemo_test_network_config(void)
{
    int i;
    /**************************************************/
    for(i = 0;i< 100;i++)
    {
        onemo_clear_virt_at_buffer();
        onemo_vir_at_send("AT+CPIN?\r\n");
        if(onemo_check_at_str("READY",2) != 0)
        {
            break;
            
        }   
    }        
    if(i>=100)
    {
        onemo_printf("sim card error\n");
        onemo_test_dump_at(0,0);
        return;
    }
    /**************************************************/
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CFUN=1\r\n");
    if(onemo_check_at_str("OK",10) == 0)
    {
        onemo_printf("cfun error\n");
        return;
    }    
    /**************************************************/
    for(i = 0;i< 30;i++)
    { 
        onemo_clear_virt_at_buffer();
        onemo_vir_at_send("AT+CEREG?\r\n");
        if(onemo_check_at_str("1,1",10) != 0)
        {
            break;       
        }

        onemo_clear_virt_at_buffer();
        onemo_vir_at_send("AT+CEREG?\r\n");
        if(onemo_check_at_str("0,1",10) != 0)
        {
            break;       
        }        
    }        
    if(i>=30)
    {
        onemo_printf("cereg error\n");
        onemo_test_dump_at(0,0);
        return;
    }
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
    if(onemo_check_at_str("OK",10) == 0)
    {
        onemo_printf("CGDCONT error\n");
        return;
    }    
    onemo_clear_virt_at_buffer();
    onemo_vir_at_send("AT+CGACT=1,1\r\n");
    if(onemo_check_at_str("OK",30) == 0)
    {
        onemo_printf("cgact error\n");
        return;
    }    
    onemo_printf("network ready\n");
    
}
/*
   测试UDP
*/
void onemo_test_udp(unsigned char **cmd,int len)
{
    struct sockaddr_in server_addr;  
    int sock_fd; 
    struct sockaddr_in from;
    int data_len;
	
    inet_aton(cmd[2], &test_remote_addr);  
    test_remote_port = char_to_int(cmd[3]);
	socklen_t fromlen = sizeof(struct sockaddr_in);
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1)
	{
		onemo_printf ( "[UDP]socket create error\n");
		return;
	}
	memset(&server_addr, 0, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = test_remote_addr.s_addr;  
    server_addr.sin_port = htons(test_remote_port);  

	connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)); 
	onemo_printf ("[UDP]data sending\n");
	send(sock_fd, (char *)test_msg, sizeof(test_msg), 0);
	 memset(test_rx_buf,0,100);
	/*data_len = recvfrom(sock_fd, test_rx_buf,
                    100, MSG_TRUNC | MSG_DONTWAIT, (struct sockaddr*)&from, &fromlen);*/
    onemo_printf ( "[UDP]waiting...\n");
    data_len = recvfrom(sock_fd, test_rx_buf,
                    100, MSG_TRUNC, (struct sockaddr*)&from, &fromlen);
    onemo_printf("[UDP]waiting end\n");                   
	if(data_len >0)
		{
		    onemo_printf ( "[UDP]get:%s\n",test_rx_buf);
			onemo_printf ( "[UDP]len:%d\n",data_len);			 
		}
		
	close(sock_fd);	
}
/*
    测试TCP
*/
void onemo_test_tcpclient_send(unsigned char **cmd,int len)
{
    int ret;
    ret = send(onemo_global_socket_id, cmd[2], strlen(cmd[2]), 0);
    onemo_printf("[Send]:%d\r\n",ret);
}
void onemo_test_tcpclient(unsigned char **cmd,int len)
{
    struct sockaddr_in server_addr;  
    int data_len;
    test_remote_port = char_to_int(cmd[3]);
    inet_aton(cmd[2], &test_remote_addr);  
    if(onemo_global_socket_id != -1)
    {
        return;
    }
	onemo_global_socket_id = socket(AF_INET, SOCK_STREAM, 0);  
	if (onemo_global_socket_id == -1) {  
        onemo_printf ("[TCPCLIENT]socket create error\n");
		 return;
    }  
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = test_remote_addr.s_addr;
    server_addr.sin_port = htons(test_remote_port);
    if(connect(onemo_global_socket_id, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)))
	{
		onemo_printf ( "[TCPCLIENT]tcp connect error\n");
		return;
	}
    if(onemo_socket_TaskHandle == 0)
    {
        osThreadDef(OC_Socket_Task, onemo_socket_select_task, osPriorityNormal, 0, 1024);
        onemo_socket_TaskHandle = osThreadCreate(osThread(OC_Socket_Task), 0);  
    }   
	onemo_printf ( "[TCPCLIENT]Socket init done\n");	
    
}
void onemo_socket_callback()
{
    int data_len = 0;
    int i;
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(struct sockaddr_in);
    data_len = recvfrom(onemo_global_socket_id, onemo_socket_buf,1024, MSG_TRUNC | MSG_DONTWAIT, (struct sockaddr*)&from, &fromlen);
    if(data_len <=0)
    {
        onemo_printf("[TCPCLIENT]socket closed\n",data_len);
        close(onemo_global_socket_id);
        onemo_global_socket_id = -1;
        return;
    }
    onemo_printf("[TCPCLIENT]Data Arrives:%d\n",data_len);
    for(i=0;i<data_len;i++)
        onemo_printf("%c",onemo_socket_buf[i]);
    onemo_printf("\n");
    
}

static void onemo_socket_select_task(void *not_used)
{
    int result, i;
    int maxfd;
    uint32_t resp_len = 0;
    uint32_t resp_reserve_len = 0; //输出信息提示头长度
    int error;
    socklen_t socket_code_size = sizeof(error);
    fd_set readfds, errorfds, writefds;

	
    while(1)
    {   
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&errorfds);
        FD_SET(onemo_global_socket_id,&readfds);						
        FD_SET(onemo_global_socket_id,&errorfds);

        //wait forever
        struct timeval tv;
        tv.tv_sec  = 5;
        tv.tv_usec = 0;
        result = select(onemo_global_socket_id + 1, &readfds, &writefds, &errorfds, &tv);		
        if (result > 0)
        {
            uint8_t* outbuf = NULL;

            //检测socket可读性
            if ((onemo_global_socket_id!= -1)&& FD_ISSET(onemo_global_socket_id, &readfds))
            {
                onemo_socket_callback();
            }

            //检测connect
            if((onemo_global_socket_id!= -1)&&FD_ISSET(onemo_global_socket_id, &writefds))
            {
                getsockopt(onemo_global_socket_id, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&socket_code_size);
                if(error == 0)
                {
                    onemo_printf("[TCPCLIENT]CONNECT OK\n");
                }
                else
                {
                    onemo_printf("[TCPCLIENT]CONNECT FAIL");
                }
            }

            //检测socket错误
            if ((onemo_global_socket_id!= -1)&&FD_ISSET(onemo_global_socket_id, &errorfds))
            {

                getsockopt(onemo_global_socket_id, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&socket_code_size);
                if(error != 0)
                {
                    onemo_printf("[TCPCLIENT]socket closed\n");
                    close(onemo_global_socket_id);
                    onemo_global_socket_id = -1;
                }
            }
        }
    }
}
