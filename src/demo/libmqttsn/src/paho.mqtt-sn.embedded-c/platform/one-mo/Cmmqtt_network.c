/*******************************************************

*******************************************************/

#include <stdint.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/ip_addr.h"
#include "cmmqtt_platform.h"
#include "../../../../../../../inc/os/include/sys/errno.h"

#define LWIP_PROVIDE_ERRNO 1

#if LWIP_SOCKET
#include "lwip/sockets.h"
#include "lwip/api.h"
#endif
#include "mbedtls/debug.h"
#define DEBUG_LEVEL 3
#define CLK32_TICK_TO_MS (32)

extern CMQTT_ERR_STAT cmmqtt_err_stat;
//log_create_module(CMMQTT_NETWORK, PRINT_LEVEL_INFO);
extern void cmmqtt_printf(const char *fmt,...);
extern void cmmqtt_log(const char *fmt,...);

typedef enum {
	SOCKET_NW_ROUTE_ERROR          =  -7,
	SOCKET_NW_CONN_ABORT_ERROR     =  -6,
	SOCKET_NW_CONN_RESET_ERROR	   =  -5,
	SOCKET_NW_NOT_CONNECTED_ERROR  =  -4,
	SOCKET_NW_ILLEGAL_VALUE_ERROR  =  -3,
	SOCKET_NW_BUFFER_ERROR		   =  -2,
    SOCKET_NW_COMMON_ERROR         = -1,
    SOCKET_NW_NO_ERROR             =  0,
    SOCKET_NW_WOULD_BLOCK_ERROR    =  1,
    SOCKET_NW_ADDR_IN_USE_ERROR    =  2,
    SOCKET_NW_ALR_CONNECTING_ERROR =  3,
    SOCKET_NW_ALR_CONNECTED_ERROR  =  4,
    SOCKET_NW_EINPROGRESS_ERROR    =  5
}cmmqtt_result_t;

cmmqtt_result_t socket_net_error_code(int error_code)
{
    cmmqtt_result_t result_code = SOCKET_NW_COMMON_ERROR;
    if (error_code == 0)
	{
        result_code = SOCKET_NW_NO_ERROR;
        return result_code;
    }
	
    switch(error_code)
	{
        case ENOMEM:
        case ENOBUFS:
		{
            result_code = SOCKET_NW_BUFFER_ERROR;
            break;
        }
        case EHOSTUNREACH: 
		{
            result_code = SOCKET_NW_ROUTE_ERROR;
            break;
        }
        case EWOULDBLOCK: 
		{
            result_code = SOCKET_NW_WOULD_BLOCK_ERROR;
            break;
        }
        case EIO:
        case EINVAL:
		{
            result_code = SOCKET_NW_ILLEGAL_VALUE_ERROR;
            break;
        }
        case EADDRINUSE: 
		{
            result_code = SOCKET_NW_ADDR_IN_USE_ERROR;
            break;
        }
        case EALREADY: 
		{
            result_code = SOCKET_NW_ALR_CONNECTING_ERROR;
            break;
        }
        case EISCONN: 
		{
            result_code = SOCKET_NW_ALR_CONNECTED_ERROR;
            break;
        }
        case ECONNABORTED: 
		{
            result_code = SOCKET_NW_CONN_ABORT_ERROR;
            break;
        }
        case ECONNRESET: 
		{
            result_code = SOCKET_NW_CONN_RESET_ERROR;
            break;
        }
        case ENOTCONN: 
		{
            result_code = SOCKET_NW_NOT_CONNECTED_ERROR;
            break;
        }
		case EINPROGRESS: 
		{
            result_code = SOCKET_NW_EINPROGRESS_ERROR;
            break;			
		}
        case -1: {
            result_code = SOCKET_NW_COMMON_ERROR;
            break;
        }
        default: {
            result_code = SOCKET_NW_COMMON_ERROR;
            break;
        }
    }
    return result_code;
}


int cmmqtt_read(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms)
{
    int rc = 0;
    int recvlen = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);

	cmmqtt_log("mqtt read timer=%d ms", timeout_ms);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    ret = select(n->my_socket + 1, &fdset, NULL, NULL, &tv);
    if (ret < 0)
	{
       // cmmqtt_log("mqtt read(select) fail ret=%d", ret);
		n->neterror++;
        return -1;
    }
	else if (ret == 0)
    {
      	//cmmqtt_log("mqtt read(select) timeout");
        return -2;
    }
	else if (ret == 1)
    {
        do
		{
            rc = recv(n->my_socket, buffer + recvlen, len - recvlen, MSG_DONTWAIT );
            if (rc > 0)
           	 	recvlen += rc;
			else 
				return recvlen;
			
        } while (recvlen < len);    
    }
    return recvlen;
}


int cmmqtt_write(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms)
{
    int rc = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    cmmqtt_log("mqtt write timer=%d ms", timeout_ms);
    ret = select(n->my_socket + 1, NULL, &fdset, NULL, &tv);

    if (ret < 0)
	{
       /*	if (n->disconnect)
		{
	        n->disconnect(n);
		}
		*/
		n->neterror++;
        return -1;
    }
	else if (ret == 0)
    {
        return -2;
    }
	else if (ret == 1)
    {
        rc = write(n->my_socket, buffer, len);
    }
    return rc;

}
int cmmqtt_neterror(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms)
{
	int error = SOCKET_NW_NOT_CONNECTED_ERROR;
    fd_set errorfds;
	socklen_t socket_code_size = sizeof(error);
    struct timeval tv;
	//if(lwip_getPcbState(n->my_socket) < 0)
	//	return error;
    FD_ZERO(&errorfds);
    FD_SET(n->my_socket, &errorfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
   
    select(n->my_socket + 1, NULL, NULL, &errorfds, &tv);
	getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&socket_code_size);
	error = socket_net_error_code(error);
	return error;
}


void cmmqtt_disconnect(CMMQTT_Network *n)
{
	//if(n->my_socket >= 0)
	//{
	//shutdown( n->my_socket, 2 );
	close(n->my_socket);
		//n->my_socket = -1;
	//}
}

void cmmqtt_disconnect_cb(CMMQTT_Network *n)
{
    //apb_proxy_urc_string(APB_PROXY_INVALID_CMD_ID,"+MQTTDISC: OK");
    cmmqtt_printf("+MQTTDISC: OK");
}

void cmmqtt_init_network(CMMQTT_Network *n,const char *addr, uint16_t port)
{
	cmmqtt_log("init network");
	if(n->server != NULL)
	{
		cmmqtt_free(n->server);
		n->server = NULL;
	}
	n->server = (char *)cmmqtt_malloc(strlen(addr)+1);
	//memcpy(n->server ,addr,strlen(addr));
	strcpy(n->server ,addr);
	n->port = port;
	n->my_socket = -1;
	n->neterror = 0;
    n->mqttread = (void*)cmmqtt_read;
    n->mqttwrite = (void*)cmmqtt_write;
    n->disconnect = cmmqtt_disconnect;
	n->disconnected_cb = cmmqtt_disconnect_cb;
}
void cmmqtt_deinit_network(CMMQTT_Network *n)
{
	//if(n->disconnect != NULL)
	//	n->disconnect(n);
	if(n->server != NULL)
	{
		cmmqtt_free(n->server);
		n->server = NULL;
	}
	n->port = 0;
	n->mqttread = NULL;
    n->mqttwrite = NULL;
    n->disconnect = NULL;
	n->disconnected_cb = NULL;
}

//tcp client
int cmmqtt_connect_network(CMMQTT_Network *n, char *addr,  uint16_t port)
{
	int type = SOCK_STREAM;
    struct sockaddr_in address;
    int rc = -1;
	int errMode = 1;
	struct timeval send_timeout = {0};
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
	{
        struct addrinfo *res = result;
        while (res)
		{
            if (res->ai_family == AF_INET)
			{
                result = res;
                break;
            }
            res = res->ai_next;
        }
        if (result->ai_family == AF_INET)
		{
            address.sin_port = htons(port);
            address.sin_family  = AF_INET;
            address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;

        } 
		else
        {
            rc = -1;
        }
        freeaddrinfo(result);
    }
    /* create client socket */
    if (rc == 0)
	{
        int opval = 1;
		u_long sock_argp = 1;

		n->my_socket = socket(family, type, 0);
        if (n->my_socket < 0)
		{
            sys_arch_printf("mqtt socket create fail");
			cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
            return -1;
    	}

		if(ioctlsocket(n->my_socket, FIONBIO, &sock_argp) < 0)
		{
			sys_arch_printf("mqtt ioctlsocket set fail");
			close(n->my_socket);
			cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
			return -1;
		}
		
		setsockopt(n->my_socket ,IPPROTO_TCP, TCP_NODELAY, &opval, sizeof(opval));
		send_timeout.tv_sec = 20;
        send_timeout.tv_usec = 0;
        setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
		setsockopt(n->my_socket, SOL_SOCKET, SO_CONTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
		//int reuseaddr = 1;
   		//setsockopt(n->my_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
		setsockopt(n->my_socket, SOL_SOCKET,SO_ERROR, &errMode, sizeof(errMode));
        /* connect remote servers*/
        rc = connect(n->my_socket, (struct sockaddr *)&address, sizeof(address));

	    if (rc < 0)
		{

			fd_set mqtt_sock_set;
			int len = -1;
			int error = -1;
			int ret_val = -1;
			
			FD_ZERO(&mqtt_sock_set);
			FD_SET(n->my_socket, &mqtt_sock_set);

			ret_val = select(n->my_socket+1, NULL, &mqtt_sock_set, NULL, &send_timeout);
			if(ret_val > 0)
			{
				len = sizeof(int);

				if(getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0)//fail
				{
					cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
					sys_arch_printf("mqtt socket connect fail:rc=%d,socket = %d", rc, n->my_socket);
				}
				else
				{
					if(error == 0)
					{
						sys_arch_printf("mqtt socket connect OK");
						//cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_OK;
					}
					else
					{
						sys_arch_printf("mqtt socket connect fail");
						cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
					}
				}
			}
			else 
			if(ret_val == 0)
			{
				sys_arch_printf("mqtt socket connect timeout:rc=%d,socket = %d", rc, n->my_socket);
				cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_TIMEOUT;//time out
			}
			else
			{
				cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
		        sys_arch_printf("mqtt socket connect fail666:rc=%d,socket = %d", rc, n->my_socket);
			}

	    }
		else
		{
			//cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_OK;
		}

		sock_argp = 0;
		if(ioctlsocket(n->my_socket, FIONBIO, &sock_argp) < 0)
		{
			sys_arch_printf("mqtt ioctlsocket set fail");
			cmmqtt_err_stat.cmmqtt_connect_error = CMMQTT_CONMQTTSERVER_FAIL;
		}

		//setsockopt(n->my_socket, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt));
    }

	if((CMMQTT_CONMQTTSERVER_FAIL != cmmqtt_err_stat.cmmqtt_connect_error)&&(CMMQTT_CONMQTTSERVER_TIMEOUT != cmmqtt_err_stat.cmmqtt_connect_error))
	{		
		sys_arch_printf("mqtt succeed");
		return 0;
	}
	else
	{	
		sys_arch_printf("mqtt fail");
		close(n->my_socket);
		return -1;
	}
	
    return rc;
}
















