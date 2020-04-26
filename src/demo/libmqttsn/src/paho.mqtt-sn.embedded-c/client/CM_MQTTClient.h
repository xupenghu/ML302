/*******************************************************************************

 *******************************************************************************/

#ifndef __CMMQTT_CLIENT_C_
#define __CMMQTT_CLIENT_C_
#include "cmmqtt_platform.h"
#include "MQTTPacket.h"
#include "CM_MQTTTopic.h"
#include "CM_MQTTQueue.h"
#include "CM_MQTTCbfun.h"
//#include "at_response.h"
//#include "at_command.h"

#define MQTT_DEFALUT_PORT           (1883)
#define MQTT_DEFALUT_KEEPALIVE      (30) //second
#define MQTT_DEFAULT_CMD_TIMEOUT    (10) //second
#define MQTT_DEFAULT_BUFSIZE        (512) //bytes
#define MQTT_DEFAULT_CLEAN_SESSION  (1) //bytes
#define MQTT_DEFAULT_USER_FLAG      (1)
#define MQTT_DEFAULT_PWD_FLAG       (1)
#define MQTT_DEFAULT_WILL_FLAG      (1)
#define MQTT_DEFAULT_QOS            (QOS0)
#define MQTT_DEFAULT_RETAIN         (1)
#define MQTT_DEFAULT_DUP            (0)
#define MQTT_DEFAULT_PING_ENABLE    (1)
#define MQTT_DEFAULT_ENCRYPT        (0)

enum QoS { QOS0, QOS1, QOS2 };
typedef enum msgTypes cmqttmsgtype;
typedef unsigned int uint32;
typedef unsigned char uint8;

enum MQTT_TO_E
{
    MQTT_TO_CONN = 1,
    MQTT_TO_PUBLISH,
    MQTT_TO_SUB,
    MQTT_TO_UNSUB,
    MQTT_TO_PING,
    MQTT_TO_UNKOWN,
};

typedef struct MQTTConmsg CMMQTTConmsg;
typedef struct MQTTConmsg
{
    MQTTPacket_connectData connect_data;
	void (*queueout_ackcb)(unsigned char, unsigned char,void *);
}CMMQTTConmsg;

typedef struct MQTTPubmsg		//M5310A 字节对其  MQTTString放置位置会造成影响??
{
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
	int qos;
	MQTTString topicName;
	int firstpacket_payloadlen;
    int payloadlen;
	void (*queueout_ackcb)(unsigned short, unsigned char,unsigned char,void *);
	unsigned char *payload;
}CMMQTTPubmsg;

typedef struct MQTTSubmsg
{
    char dup;
    unsigned short id;
    int qos;
	MQTTString topicName;
	void (*queueout_ackcb)(unsigned short, int,void *);
	void (*topic_matchcb)(void *);
}CMMQTTSubmsg,CMMQTTUnsubmsg;



#include "CM_MQTTCore_interface.h"

//typedef (*CMMQTT_INTERFACE)(void *data);


typedef struct __Client
{
    char *client_id;
    char *usr;
	char *password;
	char ping_echo;
	unsigned char *at_buf;
    unsigned char *readbuf;
	unsigned int bufsize;
	unsigned int next_packetid;
    unsigned int command_timeout_ms;
    unsigned int keepAliveInterval;
	unsigned int connectnum;
	unsigned int pinglostnum;
	unsigned int cleansession;
	unsigned int state;
	//unsigned int processflag;
	unsigned int processalive;
	unsigned int encrypt;
    //char ping_outstanding;
	//CMMQTT_INTERFACE *basic_fun;
    CMMQTT_Network ipstack;
    CMMQTT_Timer ping_timer;
	CMMQTT_Mutex *queuemutex;

	CMMQTT_Mutex *taskmonitormutex;
	CMMQTT_Mutex *taskprocessmutex;

	CMMQTT_Queue  sendqueue;
	CMMQTT_Topictable topictable;
	CMMQTT_Task	  cmmqtt_task_process;
	CMMQTT_Task	  cmmqtt_task_monitor;
	CMMQTTConmsg *cmmqtt_conmsg;
	volatile char sock_protect;		//用于进程间socket操作同步，for RDA
	volatile char mutex_protect;		//用于进程间操作同步，for RDA
	char *ca;
	char *client_ca;
	char *client_key;
	int publish_remainlen;
}CMMQTTClient;


CMMQTTClient *cmmqtt_getclient(void);

int32_t cmmqtt_init_cfg(const char *server, const uint16_t port, const char *client_id,//uint32_t command_timeout_s,
							  uint32_t keep_alive,const char*user_name, const char*password, uint32_t clean, uint32_t bufsize,uint32_t encrypt);

void cmmqtt_deinit(void);
void cmmqtt_task_processing(void *arg);
void cmmqtt_task_monitor(void *arg);
void cmmqtt_cktimeout_processing(void *arg);
void cmmqtt_test(void);
int cmmqtt_sendPacket( unsigned char *buf,int length, CMMQTT_Timer* timer);
int cmmqtt_decodePacket(CMMQTTClient* c, uint32* value, int timeout);
int cmmqtt_readPacket(CMMQTTClient* c, CMMQTT_Timer* timer, int *recvlength);
int cmmqtt_recvPacket(CMMQTT_Timer *timer);
void cmmqtt_dispatchpacket(uint8 packet_type,int recv_len );
void cmmqtt_clientalive(void);
int cmmqtt_monitor(CMMQTTClient *c);
void cmmqtt_reconnect(CMMQTTClient *c);

void cmmqtt_printf(const char * str,...);
void cmmqtt_printf_normal(const char * fmt,int length);
void cmmqtt_printf_longstring(const uint8 *buffer, uint32 length);
void cmmqtt_log(const char *buffer,...);

uint32_t cmmqtt_bin_to_hex(char *dest, const uint8_t *source, uint32_t max_dest_len);
uint32_t cmmqtt_hex_to_bin(uint8_t *dest, const char *source, uint32_t max_dest_len);
extern void mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );
extern void cmmqtt_suspendtask(CMMQTT_Task *task);
extern void cmmqtt_suspendtask(CMMQTT_Task *task);
extern int cmmqtt_neterror(CMMQTT_Network *n, unsigned char *buffer, int len, uint32_t timeout_ms);
extern void cmmqtt_printf_special(const char * buff,int length);
extern void cmmqtt_resumetask(CMMQTT_Task *task);

void cmhmac_sha1(  
                unsigned char *key,  
                int key_length,  
                unsigned char *data,  
                int data_length,  
                unsigned char *digest  
                );


void cmmqtt_at_cmds_register(void);

#define CMdefaultClient {NULL, NULL, NULL, NULL,0, 0, 0, 0, 0, 0,0,0,0,0,0,NULL,0,0,0}

#endif
