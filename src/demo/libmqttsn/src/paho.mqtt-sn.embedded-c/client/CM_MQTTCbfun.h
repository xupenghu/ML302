/*******************************************************************************
 
 *******************************************************************************/

#ifndef __CMMQTT_CB_C_
#define __CMMQTT_CB_C_
void cmmqtt_conack_cb(unsigned char sessionPresent, unsigned char connack_rc,void *node);
void cmmqtt_suback_cb(unsigned short id, int qos,void *node);
void cmmqtt_unsuback_cb(unsigned short id, int qos,void *node);
void cmmqtt_puback_cb(unsigned short id, unsigned char packettype,unsigned char dup,void *msg);
void cmmqtt_recpub_text_cb(void *msg);	
void cmmqtt_recpub_hex_cb(void *msg);
void cmmqtt_recpub_long_text_cb(void *msg);
void cmmqtt_nodetimeout_cb(void *node);
void cmmqtt_free_pingnode(cmqueue_node *node);
void cmmqtt_free_conmsg(void *msg);
void cmmqtt_free_connode(cmqueue_node *node);
void cmmqtt_free_pubmsg(void *msg);
void cmmqtt_free_pubnode(cmqueue_node *node);
void cmmqtt_free_submsg(void *submsg);
void cmmqtt_free_subnode(cmqueue_node *node);
void cmmqtt_free_unsubmsg(void *msg);
void cmmqtt_free_unsubnode(cmqueue_node *node);
void cmmqtt_free_subtopic(cmmqtt_entry *topic);



typedef void (*CMMQTT_PUBCB)(void *data);

#endif
