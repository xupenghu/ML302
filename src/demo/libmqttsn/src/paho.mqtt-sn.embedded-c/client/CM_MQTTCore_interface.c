/*******************************************************************************
by liyanlun @2018
 *******************************************************************************/

#include "CM_MQTTCore_interface.h"
#include "CM_MQTTconstant.h"
//log_create_module(CMMQTT_INTERFACE, PRINT_LEVEL_INFO);
extern CMQTT_ERR_STAT cmmqtt_err_stat;
//extern atCmdEngine_t *g_atCmdEngine;
char *g_atCmdEngine = NULL;

extern void mqttsn_printf(const char * str,...);
//static cmmqtt_entry *topic = NULL;
int cmmqtt_connect(CMMQTTConmsg *msg)
{
	int len = 0;
	int rc = FAILURE;
	unsigned char buf[CMMQTT_CON_BUFSIZE];
	CMMQTTClient *myclient = cmmqtt_getclient();
	len = MQTTSerialize_connect(buf, CMMQTT_CON_BUFSIZE, &msg->connect_data);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS )
		{
			if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
			{
				cmqueue_node *con_node = (cmqueue_node *)cmmqtt_malloc(sizeof(cmqueue_node));
				con_node->node_free = cmmqtt_free_connode;
				con_node->node_timeout = cmmqtt_nodetimeout_cb;
		 		con_node->data = msg;
				con_node->msgtype = CONNECT;
				cmmqtt_countdown_ms(&con_node->node_timer,myclient->command_timeout_ms);
				cmmqtt_queue_push_back(&myclient->sendqueue,con_node);
				cmmqtt_mutex_give(myclient->queuemutex);
				return CMMQTT_AT_OK;
			}
			else
				return CMMQTT_AT_SDK_ERROR;
		}
		else
		{
			myclient->ipstack.neterror++;
			return CMMQTT_AT_NETWORK_ERROR;
		}
	}
}



int cmmqtt_subscribe(CMMQTTSubmsg *msg)
{
	int len = 0;
	int rc = FAILURE;
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned char* buf = NULL;

	//Judge the topic weather is subscribed already
    cmmqtt_entry *topic = cmmqtt_find_topic(&myclient->topictable,msg->topicName.cstring,cmmqtt_entry,hash_head);
	if(NULL != topic)
	{
		cmmqtt_err_stat.cmmqtt_sub_error = CMMQTT_TOPIC_SUB_DONE;
		return CMMQTT_AT_TOPIC_SUB_DONE;	
	}
	buf = (char*)calloc(1,CMMQTT_SUB_BUFSIZE);
	if(NULL == buf)
	{
		return CMMQTT_AT_SDK_ERROR;
	}
	len = MQTTSerialize_subscribe(buf,CMMQTT_SUB_BUFSIZE,msg->dup,myclient->next_packetid,1,&msg->topicName,&msg->qos);
	if (len <= 0)
	{
		free(buf);
		buf = NULL;
		return CMMQTT_AT_SDK_ERROR;
	}
	else
	{
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		
		free(buf);
		buf = NULL;
		
		if(rc == SUCCESS )
		{
			//myclient->publish_remainlen = 0;
			if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
			{
				cmqueue_node *sub_node = (cmqueue_node *)cmmqtt_malloc(sizeof(cmqueue_node));
				sub_node->node_free = cmmqtt_free_subnode;
				sub_node->node_timeout = cmmqtt_nodetimeout_cb;
				msg->id = myclient->next_packetid;
		 		sub_node->data = msg;
				sub_node->msgtype = SUBSCRIBE;
				cmmqtt_countdown_ms(&sub_node->node_timer,myclient->command_timeout_ms);
				cmmqtt_queue_push_back(&myclient->sendqueue,sub_node);
				myclient->next_packetid++;
				cmmqtt_mutex_give(myclient->queuemutex);
				return CMMQTT_AT_OK;
			}
			else
				return CMMQTT_AT_SDK_ERROR;
		}
		else
		{
			myclient->ipstack.neterror++;
			//cmmqtt_free_submsg(msg);
			return CMMQTT_AT_NETWORK_ERROR;
		}
	}
}

int cmmqtt_unsubscribe(CMMQTTUnsubmsg *msg)
{
	int len = 0;
	int rc = FAILURE;
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned char buf[CMMQTT_UNSUB_BUFSIZE];
	len = MQTTSerialize_unsubscribe(buf,CMMQTT_UNSUB_BUFSIZE,msg->dup,myclient->next_packetid,1,&msg->topicName);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS )
		{
			//myclient->publish_remainlen = 0;
			if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
			{
				cmqueue_node *unsub_node = (cmqueue_node *)cmmqtt_malloc(sizeof(cmqueue_node));
				unsub_node->node_free = cmmqtt_free_unsubnode;
				unsub_node->node_timeout = cmmqtt_nodetimeout_cb;
				msg->id = myclient->next_packetid;
		 		unsub_node->data = msg;
				unsub_node->msgtype = UNSUBSCRIBE;
				cmmqtt_countdown_ms(&unsub_node->node_timer,myclient->command_timeout_ms);
				cmmqtt_queue_push_back(&myclient->sendqueue,unsub_node);
				myclient->next_packetid++;
				cmmqtt_mutex_give(myclient->queuemutex);
				return CMMQTT_AT_OK;
			}
			else
				return CMMQTT_AT_SDK_ERROR;
		}
		else
		{
			myclient->ipstack.neterror++;
			//cmmqtt_free_unsubmsg(msg);
			return CMMQTT_AT_NETWORK_ERROR;
		}
	}
}


int cmmqtt_publish(CMMQTTPubmsg *msg)
{

	int len = 0;
	int rc = FAILURE;
	CMMQTTClient *myclient = cmmqtt_getclient();
	//unsigned char buf[CMMQTT_PUB_BUFSIZE];
	unsigned char * buf = (unsigned char *)myclient->at_buf;
	memset(buf,0,myclient->bufsize*2 + CMMQTT_AT_RESPONSE_DATA_LEN);
	cmqueue_node *pub_node  = NULL;
	len = MQTTSerialize_publish(buf,CMMQTT_PUB_BUFSIZE,msg->dup,msg->qos,msg->retained,myclient->next_packetid,msg->topicName,msg->payload,msg->payloadlen);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{

		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS )
		{
			//myclient->publish_remainlen = 0;
			if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
			{
				if(msg->qos == QOS1 || msg->qos == QOS2)
				{
					pub_node = (cmqueue_node *)cmmqtt_malloc(sizeof(cmqueue_node));
					pub_node->node_free = cmmqtt_free_pubnode;
					pub_node->node_timeout = cmmqtt_nodetimeout_cb;
			 		pub_node->data = msg;
					pub_node->msgtype = PUBLISH;
					msg->id = myclient->next_packetid;
					cmmqtt_countdown_ms(&pub_node->node_timer,myclient->command_timeout_ms);
					cmmqtt_queue_push_back(&myclient->sendqueue,pub_node);
				}

				myclient->next_packetid++;
				cmmqtt_mutex_give(myclient->queuemutex);
				return CMMQTT_AT_OK;
			}
			else
				return CMMQTT_AT_SDK_ERROR;
		}
		else
		{
			myclient->ipstack.neterror++;
			//cmmqtt_free_pubmsg(msg);
			return CMMQTT_AT_NETWORK_ERROR;
		}
	}
}
int cmmqtt_ping()
{
	int len = 0;
	int rc = FAILURE;
	unsigned char buf[CMMQTT_PING_BUFSIZE];
	CMMQTTClient *myclient = cmmqtt_getclient();
	len = MQTTSerialize_pingreq(buf,CMMQTT_PING_BUFSIZE);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{
		
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS)
		{
			myclient->publish_remainlen = 0;
			return CMMQTT_AT_OK;
		}
		else
		{
			myclient->ipstack.neterror++;
			return CMMQTT_AT_NETWORK_ERROR;
		}
	}
}

int cmmqtt_disc()
{
	int len = 0;
	int rc = FAILURE;
	unsigned char buf[CMMQTT_DISC_BUFSIZE];
	len = MQTTSerialize_disconnect(buf,CMMQTT_DISC_BUFSIZE);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS)
			return CMMQTT_AT_OK;
		else
			return CMMQTT_AT_NETWORK_ERROR;
	}
}
void cmmqtt_deconack(unsigned char* buf)
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned char sessionPresent;
	unsigned char connack_rc;
	MQTTDeserialize_connack(&sessionPresent,&connack_rc, buf,myclient->bufsize);
	cmmqtt_head **pos = &myclient->sendqueue.head;
	CMMQTTConmsg *msg =	NULL;
	cmqueue_node *item;
	if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
	{
		cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
		{
			if(item != NULL)
			{
				if(item->msgtype == CONNECT)
				{
					msg = (CMMQTTConmsg *)item->data;

						if(pos != NULL)
							cmmqtt_queue_remove_at(&myclient->sendqueue,pos);
						if(msg!= NULL && msg->queueout_ackcb != NULL)
							msg->queueout_ackcb(sessionPresent,connack_rc,item);
						break;

				}
				pos =(cmmqtt_head **) &item->head;
			}
		}
		cmmqtt_mutex_give(myclient->queuemutex);
	}
	//cmmqtt_log("session: %d,rc: %d",sessionPresent,connack_rc);
}

void cmmqtt_desuback(unsigned char* buf)
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned short packetid;
	int count ;
	int qos;
	
	if(MQTTDeserialize_suback(&packetid,1,&count,&qos,buf,myclient->bufsize))
	{
		//cmmqtt_log("sub id: %d,qos: %d",packetid,qos);
		cmmqtt_head **pos = &myclient->sendqueue.head;
		CMMQTTSubmsg *msg =	NULL;
		cmqueue_node *item;
		if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
		{
			cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
			{
				if(item != NULL)
				{
					if(item->msgtype == SUBSCRIBE)
					{
						msg = (CMMQTTSubmsg *)item->data;
						if(packetid == msg->id && qos == msg->qos)
						{

							if(pos != NULL)
								cmmqtt_queue_remove_at(&myclient->sendqueue,pos);
							if(msg!= NULL && msg->queueout_ackcb != NULL)
								msg->queueout_ackcb(packetid,qos,item);
							cmmqtt_err_stat.cmmqtt_sub_error = CMMQTT_SUB_OK;
							break;
						}
					}
					pos =(cmmqtt_head **) &item->head;
				}
				else
					break;
			}
			cmmqtt_mutex_give(myclient->queuemutex);
		}
	}
	
	if((CMMQTT_SUB_OK != cmmqtt_err_stat.cmmqtt_sub_error) && (CMMQTT_TOPIC_SUB_DONE != cmmqtt_err_stat.cmmqtt_sub_error))
	{
		cmmqtt_err_stat.cmmqtt_sub_error = CMMQTT_SUBACK_FAIL;
	}
}

void cmmqtt_deunsuback(unsigned char* buf)
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned short packetid;

	if(MQTTDeserialize_unsuback(&packetid, buf,myclient->bufsize) == 0)
	{
		cmmqtt_err_stat.cmmqtt_unsub_error = CMMQTT_UNSUBACK_FAIL;
	}
	else
	{
		cmmqtt_err_stat.cmmqtt_unsub_error = CMMQTT_UNSUB_OK;
	}
	//cmmqtt_log("id: %d",packetid);
	cmmqtt_head **pos = &myclient->sendqueue.head;
	CMMQTTSubmsg *msg =	NULL;
	cmqueue_node *item;
	if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
	{
		cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
		{
			if(item != NULL)
			{
				if(item->msgtype == UNSUBSCRIBE)
				{
					msg = (CMMQTTUnsubmsg *)item->data;
					if(packetid == msg->id )
					{
						if(pos != NULL)
							cmmqtt_queue_remove_at(&myclient->sendqueue,pos);
						if(msg!= NULL && msg->queueout_ackcb != NULL)
							msg->queueout_ackcb(packetid,QOS0,item);
						break;
					}
				}
				pos =(cmmqtt_head **) &item->head;
			}
			else
				break;
		}
		cmmqtt_mutex_give(myclient->queuemutex);
	}
}

void cmmqtt_depuback(unsigned char* buf)
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned char  packettype;
	unsigned short packetid;
	unsigned char dup;
	unsigned char des_ret;
	
	des_ret = MQTTDeserialize_ack(&packettype,&dup,&packetid,buf,myclient->bufsize);
	cmmqtt_head **pos = &myclient->sendqueue.head;
	CMMQTTPubmsg *msg =	NULL;
	cmqueue_node *item;

	if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
	{
		cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
		{
			if(item != NULL)
			{
				if(item->msgtype == PUBLISH)
				{
					msg = (CMMQTTPubmsg *)item->data;
					if(packetid == msg->id )
					{
						if((msg->qos == QOS1) )
						{
							if(des_ret == 0)
							{
								cmmqtt_err_stat.cmmqtt_pub_error[0] = CMMQTT_PUBACK_FAIL;
							}
							else if(des_ret == 1)
							{
								cmmqtt_err_stat.cmmqtt_pub_error[0] = CMMQTT_PUB_OK;
							}
						}
						
						if((msg->qos == QOS2) )
						{
							if((des_ret == 0) && (packettype == PUBREC))
							{
								cmmqtt_err_stat.cmmqtt_pub_error[0] = CMMQTT_PUBACK_FAIL;
							}
							if((des_ret != 0) && (packettype == PUBREC))
							{
								cmmqtt_err_stat.cmmqtt_pub_error[0] = CMMQTT_PUB_OK;
							}
							if((des_ret == 0) && (packettype == PUBCOMP))
							{
								cmmqtt_err_stat.cmmqtt_pub_error[2] = CMMQTT_PUBACK_FAIL;
							}
							if((des_ret != 0) && (packettype == PUBCOMP))
							{
								cmmqtt_err_stat.cmmqtt_pub_error[2] = CMMQTT_PUB_OK;
							}

						}

						
						if(pos != NULL &&(packettype == PUBACK || packettype == PUBCOMP)) ////qos =1 收到ack 或qos=2 收到comp 时再出队
							cmmqtt_queue_remove_at(&myclient->sendqueue,pos);
						if(msg != NULL && msg->queueout_ackcb != NULL)
							msg->queueout_ackcb(packetid,packettype,dup,item);
						break;
					}
				}
				pos =(cmmqtt_head **) &item->head;
			}
			else
				break;
		}
		cmmqtt_mutex_give(myclient->queuemutex);
	}

}
int cmmqtt_puback(unsigned char packettype,unsigned short packetid,unsigned char dup)
{
	int len = 0;
	int rc = FAILURE;
	unsigned char buf[CMMQTT_ACK_BUFSIZE];
	len = MQTTSerialize_ack( buf,CMMQTT_ACK_BUFSIZE,packettype, dup,packetid);
	if (len <= 0)
		return CMMQTT_AT_SDK_ERROR;
	else
	{
		CMMQTT_Timer  timer;
		cmmqtt_init_timer(&timer);
		cmmqtt_countdown_ms(&timer,CMMQTT_SEND_TIME);
		rc = cmmqtt_sendPacket(buf,len,&timer);
		if(rc == SUCCESS)
			return CMMQTT_AT_OK;
		else
			return CMMQTT_AT_NETWORK_ERROR;
	}

}

void (*topic_matchcallback)(void *) ;
volatile int pub_lock_flag = 0;
extern CMMQTT_Mutex *g_pubMutex;

void depublish_callback_func(void *arg)
{
	osDelay(50);
	topic_matchcallback(arg);
	pub_lock_flag = 1;
	while(1)
	{
		;
	}

}

void cmmqtt_depublish(unsigned char* buf,int recv_len)
{

    char str_rsp[50] = {0};
	CMMQTTClient *myclient = cmmqtt_getclient();
	CMMQTTPubmsg msg ;
	msg.id = 0;

	if(MQTTDeserialize_publish(&msg.dup,&msg.qos,&msg.retained,&msg.id,&msg.topicName,&msg.payload,&msg.payloadlen,buf,myclient->bufsize) != 1)
	{
		memset(str_rsp, 0, sizeof(str_rsp));
		sprintf(str_rsp, "\r\n+CME ERROR: %d\r\n", ERR_MQTT_PUBPACKET_FAIL);
		if(NULL != g_atCmdEngine)
		{
			atCmdRespInfoText(g_atCmdEngine, (char*)str_rsp);
		}
		return;
	}
	
	cmmqtt_entry *topic = NULL;
	char topickey[msg.topicName.lenstring.len];
	strncpy(topickey,msg.topicName.lenstring.data,msg.topicName.lenstring.len);
	topickey[msg.topicName.lenstring.len] = '\0';
	topic = cmmqtt_find_topic(&myclient->topictable,topickey,cmmqtt_entry,hash_head);
	msg.firstpacket_payloadlen = recv_len;
    //TODO: topic match
	if(topic != NULL)
	{
		cmmqtt_log("get the topic  :%s",topickey);
		CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)topic->data;
		
#if 0
		//1.Not create lock, create thread first
		//2.Add lock
		pub_lock_flag = 0;		
		TaskHandle_t xHandle_pub;

		if(cmmqtt_mutex_take(g_pubMutex) == SUCCESS)
		{
			topic_matchcallback = submsg->topic_matchcb;
			xTaskCreate(depublish_callback_func,
	               	 "PUBLISH_CALLBACK",
	                 CMMQTT_TASK_MONITOR_STACK_SIZE,
	                 &msg,
	                 CMMQTT_TASK_PROCESS_PRIORITY,
	                 &xHandle_pub);
			
			while(pub_lock_flag == 0);
			if( xHandle_pub !=NULL )
			{
				vTaskDelete( xHandle_pub );
			}
			
			cmmqtt_mutex_give(g_pubMutex);

		}
#else		
		submsg->topic_matchcb(&msg);
#endif
	}
	else
	{
		onemo_printf("[MQTT]no such topic .......: %d,name :%s,topic %s",msg.topicName.lenstring.len,msg.topicName.lenstring.data,topickey);
		//cmmqtt_recpub_reserved(&msg);
		cmmqtt_recpub_long_text_cb(&msg);
	}
}
void cmmqtt_depubrel(unsigned char* buf)
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	unsigned char  packettype;
	unsigned short packetid;
	unsigned char dup;
	int ret_value = -1;
    char str_rsp[50] = {0};
	
	ret_value = MQTTDeserialize_ack(&packettype,&dup,&packetid,buf,myclient->bufsize);
	if(ret_value == 0)
	{
		memset(str_rsp, 0, sizeof(str_rsp));
		sprintf(str_rsp, "\r\n+CME ERROR: %d\r\n", ERR_MQTT_PUBREL_FAIL);
		if(NULL != g_atCmdEngine)
		{
			atCmdRespInfoText(g_atCmdEngine, (char*)str_rsp);
		}
	}
	else if(ret_value == 1)
	{
		if(packettype == PUBREL)
		{
			cmmqtt_puback(PUBCOMP,packetid,dup);
		}
	}

}
void cmmqtt_depingresp()
{
	CMMQTTClient *myclient = cmmqtt_getclient();
	myclient->pinglostnum = 0;
	if(myclient->ping_echo)
	{
        cmmqtt_printf("+MQTTPINGRESP:OK");	
	}
}

