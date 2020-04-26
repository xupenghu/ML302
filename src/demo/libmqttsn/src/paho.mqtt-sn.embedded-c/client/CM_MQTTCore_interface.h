/*******************************************************************************
 by liyanlun  @2018
 *******************************************************************************/

#ifndef __CMMQTT_CORE_INTERFACE_C_
#define __CMMQTT_CORE_INTERFACE_C_
#include "CM_MQTTClient.h"
//int cmmqtt_connect(const char *client_id, uint32_t keep_alive, uint32_t clean_session, 
//						uint32_t will_flag, MQTTPacket_willOptions *will_options, const char*user_name, const char*password);
int cmmqtt_connect(CMMQTTConmsg *conmsg);
int cmmqtt_subscribe(CMMQTTSubmsg *msg);
int cmmqtt_unsubscribe(CMMQTTUnsubmsg *msg);
int cmmqtt_publish(CMMQTTPubmsg *msg);
int cmmqtt_puback(unsigned char packettype,unsigned short packetid,unsigned char dup);
int cmmqtt_ping(void);
int cmmqtt_disc(void);
void cmmqtt_deconack(unsigned char* buf);
void cmmqtt_desuback(unsigned char* buf);
void cmmqtt_depuback(unsigned char* buf);
void cmmqtt_deunsuback(unsigned char* buf);
void cmmqtt_depublish(unsigned char* buf,int recv_len);
void cmmqtt_depubrel(unsigned char* buf);
void cmmqtt_depingresp(void);
void cmmqtt_recpub_reserved(void *msg);		

#define ERR_MQTT_GPRS_NOTACT            600
#define ERR_MQTT_PARAM_INVALID          601
#define ERR_MQTT_CONNECTED_DONE         602
#define ERR_MQTT_CONNECT_FAIL           603
#define ERR_MQTT_CONNECT_TIMEOUT        604
#define ERR_MQTT_SUBACK_FAIL            605
#define ERR_MQTT_TOPIC_SUB_DONE         606
#define ERR_MQTT_UNKNOWN_DATA           607
#define ERR_MQTT_SENDPACK_FAIL          608
#define ERR_MQTT_PUBPACKET_FAIL         609
#define ERR_MQTT_PUBREC_FAIL            610
#define ERR_MQTT_PUBCOMP_FAIL           611
#define ERR_MQTT_PUBACK_FAIL            612
#define ERR_MQTT_SOCKET_FAIL            613
#define ERR_MQTT_USERPASSWORD_ERR       614
#define ERR_MQTT_RECONNECT_FAIL         615
#define ERR_MQTT_ACK_TIMEOUT            616
#define ERR_MQTT_CONNECT_BREAK          617
#define ERR_MQTT_UNSUBACK_FAIL          618
#define ERR_MQTT_PUBREL_FAIL            619
#define ERR_MQTT_DISCONNECTED_DONE      620
#define ERR_MQTT_UNSUB_ERROR      		621

#endif
