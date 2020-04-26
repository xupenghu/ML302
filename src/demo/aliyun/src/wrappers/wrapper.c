/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include <lwip/sockets.h>
#include <unistd.h>
#include <fcntl.h>
#include <lwip/inet.h>
#include <lwip/netdb.h>

#include <os/include/sys/errno.h>

char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1pnQWj9dvs";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "1ucJGc0VVPlYRBvA";
char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "test";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "C5LTDEyQj6wMXh8KaNICzvCSgi69Ysj8";

unsigned int g_tls_enable = 0;

void HAL_Free(void *ptr)
{
	vPortFree(ptr);
}

int HAL_SetProductKey(char *product_key)
{
    int len = strlen(product_key);

    if (len > IOTX_PRODUCT_KEY_LEN) {
        return -1;
    }
    memset(_product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);
    strncpy(_product_key, product_key, len);

    return len;
}


int HAL_SetDeviceName(char *device_name)
{
    int len = strlen(device_name);

    if (len > IOTX_DEVICE_NAME_LEN) {
        return -1;
    }
    memset(_device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);
    strncpy(_device_name, device_name, len);

    return len;
}

int HAL_SetProductSecret(char *product_secret)
{
    int len = strlen(product_secret);

    if (len > IOTX_PRODUCT_SECRET_LEN) {
        return -1;
    }
    memset(_product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);
    strncpy(_product_secret, product_secret, len);

    return len;
}

int HAL_SetDeviceSecret(char *device_secret)
{
    int len = strlen(device_secret);

    if (len > IOTX_DEVICE_SECRET_LEN) {
        return -1;
    }
    memset(_device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);
    strncpy(_device_secret, device_secret, len);

    return len;
}

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1])
{
    int len = strlen(_product_key);
    memset(product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);

    strncpy(product_key, _product_key, len);

    return len;
}

int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1])
{
    int len = strlen(_product_secret);
    memset(product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);

    strncpy(product_secret, _product_secret, len);

    return len;
}

int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int len = strlen(_device_name);
    memset(device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);

    strncpy(device_name, _device_name, len);

    return strlen(device_name);
}

int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int len = strlen(_device_secret);
    memset(device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);

    strncpy(device_secret, _device_secret, len);

    return len;
}



int HAL_GetFirmwareVersion(char *version)
{
	return (int)1;
}

void HAL_SetTLSEnable(int enable)
{
    g_tls_enable = enable;
}

int HAL_GetTLSEnable(void)
{
    return g_tls_enable;
}

int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
	return (int)1;
}

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
	return (int)1;
}

void *HAL_Malloc(uint32_t size)
{
	return pvPortMalloc(size);
}

void *HAL_MutexCreate(void)
{
    QueueHandle_t sem;

    sem = xSemaphoreCreateMutex();
    if (0 == sem) {
        return NULL;
    }

    return sem;

}

void HAL_MutexDestroy(void *mutex)
{
    QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }
    sem = (QueueHandle_t)mutex;
    vSemaphoreDelete(sem);

}

void HAL_MutexLock(void *mutex)
{
    BaseType_t ret;
    QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }

    sem = (QueueHandle_t)mutex;
    ret = xSemaphoreTake(sem, 0xffffffff);
    while (pdPASS != ret) {
        ret = xSemaphoreTake(sem, 0xffffffff);
    }

}

void HAL_MutexUnlock(void *mutex)
{
    QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }
    sem = (QueueHandle_t)mutex;
    (void)xSemaphoreGive(sem);

}

 char g_print_buffer[1024] = {0};
void HAL_Printf(const char *fmt, ...)
{
    //sys_arch_printf(fmt);
    va_list args;
	char buffer[1024] = {0};

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer) - strlen(g_print_buffer), fmt, args);
    va_end(args);

    strcat(g_print_buffer,buffer);

    if((g_print_buffer[strlen(g_print_buffer)-2] == '\r') && (g_print_buffer[strlen(g_print_buffer)-1] == '\n'))
    {
        sys_arch_printf("[aliyun]: %s", g_print_buffer);
        memset(g_print_buffer,0,1024);
    }
}

void HAL_SleepMs(uint32_t ms)
{
	vTaskDelay(osiMsToOSTick(ms));

}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);
    return rc;

}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}


static uint64_t _linux_get_time_ms(void)
{
    struct timeval tv = { 0 };
    uint64_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

static uint64_t _linux_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

int HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int) fd, 2);
    if (0 != rc) {
        sys_arch_printf("[aliyun]:shutdown error\n");
        return -1;
    }

    rc = close((int) fd);
    if (0 != rc) {
        sys_arch_printf("[aliyun]:closesocket error\n");
        return -1;
    }

    return 0;

}

uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    int fd = 0;
    int rc = 0;
    char service[6];
    uint8_t dns_retry = 0;

    memset(&hints, 0, sizeof(hints));

    sys_arch_printf("[aliyun]:establish tcp connection with server(host='%s', port=[%u])\n", host, port);

    hints.ai_family = AF_INET; /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    while(dns_retry++ < 1) {
        rc = getaddrinfo(host, service, &hints, &addrInfoList);
        if (rc != 0) {
            sys_arch_printf("[aliyun]:getaddrinfo error[%d], res: %d, host: %s, port: %s\n", dns_retry, rc, host, service);
            osiThreadSleep(1000);
            continue;
        }else{
            break;
        }
    }

    if (rc != 0) {
        sys_arch_printf("[aliyun]:getaddrinfo error(%d), host = '%s', port = [%d]\n", rc, host, port);
        return (uintptr_t)(-1);
    }

    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            sys_arch_printf("[aliyun]:socket type error\n");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            sys_arch_printf("[aliyun]:create socket error\n");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        sys_arch_printf("[aliyun]:connect error\n");
        rc = -1;
    }

    if (-1 == rc) {
        sys_arch_printf("[aliyun]:fail to establish tcp\n");
    } else {
        sys_arch_printf("[aliyun]:success to establish tcp, fd=%d\n", rc);
    }
    freeaddrinfo(addrInfoList);

    return (uintptr_t)rc;

}

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code, tcp_fd;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());
        if (0 == t_left) {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(tcp_fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(tcp_fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(tcp_fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                sys_arch_printf("[aliyun]:connection is closed\n");
                err_code = -1;
                break;
            } else {
                if (EINTR == errno) {
                    continue;
                }
                sys_arch_printf("[aliyun]:recv fail\n");
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (EINTR == errno) {
                continue;
            }
            sys_arch_printf("[aliyun]:select-recv fail\n");
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;

}

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret,tcp_fd;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;
    int net_err = 0;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(tcp_fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(tcp_fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(tcp_fd, &sets)) {
                    sys_arch_printf("[aliyun]:Should NOT arrive\n");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                sys_arch_printf("[aliyun]:select-write timeout %d\n", tcp_fd);
                break;
            } else {
                if (EINTR == errno) {
                    sys_arch_printf("[aliyun]:EINTR be caught\n");
                    continue;
                }

                sys_arch_printf("[aliyun]:select-write fail, ret = select() = %d\n", ret);
                net_err = 1;
                break;
            }
        }

        if (ret > 0) {
            ret = send(tcp_fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                sys_arch_printf("[aliyun]:No data be sent\n");
            } else {
                if (EINTR == errno) {
                    sys_arch_printf("[aliyun]:EINTR be caught\n");
                    continue;
                }

                sys_arch_printf("[aliyun]:send fail, ret = send() = %d\n", ret);
                net_err = 1;
                break;
            }
        }
    } while (!net_err && (len_sent < len) && (_linux_time_left(t_end, _linux_get_time_ms()) > 0));

    if (net_err) {
        return -1;
    } else {
        return len_sent;
    }

}

uint64_t HAL_UptimeMs(void)
{
    struct timeval time_val = {0};
    uint32_t time_ms;

    gettimeofday(&time_val, NULL);
    time_ms = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;

    return time_ms;


}


