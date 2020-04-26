/*******************************************************************************

 *******************************************************************************/

#ifndef __CMMQTT_PLATFORM_C_
#define __CMMQTT_PLATFORM_C_
#include "cmsis_os.h"
#include "CM_MQTTConstant.h"
//#include "task.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "string.h"
//#include "mbedtls/compat-1.3.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"

//#include "osi_api.h"
/*
#ifndef CMMQTT_PLATFORM_DEBUG_LEVEL_NONE
#define CMMQTT_P_DEBUG 1
#else
#define CMMQTT_P_DEBUG 0
#endif

#if CMMQTT_P_DEBUG
#define cmmqtt_log(fmt,arg...)  LOG_I(fmt,##arg)
#define CMMQTT_P_WARN(fmt,arg...) LOG_V(fmt,##arg)
#define CMMQTT_P_ERR(fmt,arg...)  LOG_E(fmt,##arg)
#else
#define VMMQTT_P_DBG(x, ...)
#define CMMQTT_P_WARN(x, ...)
#define CMMQTT_P_ERR(x, ...)
#endif
*/

#define bool	_Bool
#define true	1
#define false	0

typedef unsigned int HANDLE;
typedef void (*TaskFunction_t)( void * );
typedef void * QueueHandle_t;
typedef void * TaskHandle_t;

typedef struct _CMMQTT_Mutex_t
{
	//u8_t id;
	QueueHandle_t	lock;
}CMMQTT_Mutex;
Cmmqtt_Error_t cmmqtt_mutex_create(const osMutexDef_t *mutex_name, CMMQTT_Mutex *mutex);
Cmmqtt_Error_t cmmqtt_mutex_take(CMMQTT_Mutex *mutex);
Cmmqtt_Error_t cmmqtt_mutex_give(CMMQTT_Mutex *mutex);
Cmmqtt_Error_t cmmqtt_mutex_destory(CMMQTT_Mutex *mutex);


typedef struct _CMMQTT_Task_t
{
	TaskFunction_t pvTaskCode;
	const char*  pcName;
	uint16_t usStackDepth;
	volatile TaskHandle_t pvCreatedTask;
	void *pvParameters;
	uint8_t uxPriority;
	void *taskid;
}CMMQTT_Task;
void  cmmqtt_createtask(CMMQTT_Task *task);
void cmmqtt_deltask(CMMQTT_Task *task);

typedef enum
{
    TIMER_TYPE_MS   = 0x00,
    TIMER_TYPE_S    = 0x01,
} timer_type;
typedef struct _CMMQTT_Timer_t
{
	unsigned int systick_period;
    unsigned int end_time;
} CMMQTT_Timer;
unsigned int cmmqtt_current_time_ms(void);
int cmmqtt_timer_expired(CMMQTT_Timer *timer);
void cmmqtt_countdown_ms(CMMQTT_Timer *timer, uint32_t timeout);
void cmmqtt_countdown_sec(CMMQTT_Timer *timer, uint32_t timeout);
uint32_t cmmqtt_left_ms(CMMQTT_Timer *timer);
void cmmqtt_init_timer(CMMQTT_Timer *timer);

typedef struct CMMQTT_Network_t
{
	char *server;
	uint16_t port;
    int my_socket;                                                /**< Connect the socket handle. */
	int neterror;
    int (*mqttread)(struct CMMQTT_Network_t *, unsigned char *, int, int);        /**< Read data from server function pointer. */
    int (*mqttwrite)(struct CMMQTT_Network_t *, unsigned char *, int, int);       /**< Send data to server function pointer. */
    void (*disconnect)(struct CMMQTT_Network_t *);    /**< Disconnect the network function pointer. */
    void (*disconnected_cb)(struct CMMQTT_Network_t *);
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
}CMMQTT_Network;
 int cmmqtt_read(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms);
 int cmmqtt_write(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms);
 void cmmqtt_disconnect(CMMQTT_Network *n);
 void cmmqtt_init_network(CMMQTT_Network *n,const char *addr,  const uint16_t port);
 int cmmqtt_connect_network(CMMQTT_Network *n, char *addr,  uint16_t port);
 void cmmqtt_deinit_network(CMMQTT_Network *n);
 void cmmqtt_disconnect_cb(CMMQTT_Network *n);
 int cmmqtt_real_confirm(int verify_result);
uint32_t cmmqtt_avRandom();
void cmmqtt_ssl_debug( void *ctx, int level, const char *file, int line, const char *str );
int cmmqtt_ssl_random(void *p_rng, unsigned char *output, size_t output_len);
int cmmqtt_ssl_parse_crt(mbedtls_x509_crt *crt);
int cmmqtt_ssl_init(mbedtls_ssl_context *ssl,
                         mbedtls_net_context *tcp_fd,
                         mbedtls_ssl_config *conf,
                         mbedtls_x509_crt *crt509_ca, const char *ca_crt, size_t ca_len,
                         mbedtls_x509_crt *crt509_cli, const char *cli_crt, size_t cli_len,
                         mbedtls_pk_context *pk_cli, const char *cli_key, size_t key_len,  const char *cli_pwd, size_t pwd_len
                        );

int cmmqtt_connect_network_tls(CMMQTT_Network *n, const char *addr, const char *port,
                      const char *ca_crt, size_t ca_crt_len,
                      const char *client_crt,	size_t client_crt_len,
                      const char *client_key,	size_t client_key_len,
                      const char *client_pwd, size_t client_pwd_len);

void cmmqtt_get_encryptinfo(char **buf,const char *item,const char *item_name);

void* cmmqtt_malloc(size_t bufsize);
void  cmmqtt_free(void *buf);

typedef struct CMQTT_ERR_STAT_t{
	volatile int cmmqtt_connect_error;
	volatile int cmmqtt_sub_error;
	volatile char cmmqtt_pub_error[3];
	volatile int cmmqtt_unsub_error;
	volatile int cmmqtt_conpasswd_error;
	volatile int cmmqtt_response_error;
}CMQTT_ERR_STAT;


#endif
