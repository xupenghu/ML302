/*******************************************************************************
by liyanlun @2018
 *******************************************************************************/

#include "CM_MQTTClient.h"
extern uint32_t cmmqtt_bin_to_hex(char *dest, const uint8_t *source, uint32_t max_dest_len);
extern CMQTT_ERR_STAT cmmqtt_err_stat;

//用于扩展，不同的topic根据实际需要选用不同的pub_cb来处理收到的publish信息，默认为按text模式打印接收到的publish信息
CMMQTT_PUBCB cmmqtt_recpubcb[CMMQTT_PUBCB_NUM] = {cmmqtt_recpub_long_text_cb,NULL};

void cmmqtt_conack_cb(unsigned char sessionPresent, unsigned char connack_rc,void *node)
{
	//cmmqtt_log("enter conack cb");	
	cmmqtt_err_stat.cmmqtt_conpasswd_error = 0;
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmqueue_node *item = (cmqueue_node*)node;
	if(connack_rc == 0)
	{
		cmmqtt_init_timer(&myclient->ping_timer);
		cmmqtt_countdown_ms(&myclient->ping_timer,myclient->keepAliveInterval);
		myclient->connectnum = 0;
		myclient->pinglostnum = 0;
		if(sessionPresent == 0 || myclient->cleansession == 1)
		{
			cmmqtt_queue_remove_all(&myclient->sendqueue);
			cmmqtt_topictable_clean(&myclient->topictable);
		}		
		cmmqtt_err_stat.cmmqtt_conpasswd_error = CMMQTT_CONPASSWD_OK;
		myclient->state = CONNECTED;
		cmmqtt_printf("+MQTTOPEN:OK");
	}
	else
	{
		if((connack_rc==0x04)||(connack_rc==0x02))
		{
			cmmqtt_err_stat.cmmqtt_conpasswd_error = CMMQTT_CONPASSWD_FAIL;
		}

		myclient->state = DISCONNECTED;
        cmmqtt_printf("+MQTTOPEN:FAIL");
	}
	if(item != NULL)
		item->node_free(item);
    //apb_proxy_urc_string(APB_PROXY_INVALID_CMD_ID,"+MQTTCONACK: %d,%d",connack_rc,sessionPresent);
    
	if(cmmqtt_err_stat.cmmqtt_conpasswd_error == 0)
	{
		cmmqtt_err_stat.cmmqtt_conpasswd_error = -1;
	}
}

void cmmqtt_suback_cb(unsigned short id, int qos,void *node)
{
	int rc = 0;
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmqueue_node *item = (cmqueue_node*)node;
	CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)item->data;
   
   
    cmmqtt_entry *topic = cmmqtt_find_topic(&myclient->topictable,submsg->topicName.cstring,cmmqtt_entry,hash_head);
    if(topic == NULL)
	{
		cmmqtt_entry *e = (cmmqtt_entry*)cmmqtt_malloc(sizeof(cmmqtt_entry));
		e->key = (char*)cmmqtt_malloc(strlen(submsg->topicName.cstring)+1);
		strcpy(e->key,submsg->topicName.cstring);

		CMMQTTSubmsg *topicmsg = (CMMQTTSubmsg *)cmmqtt_malloc(sizeof(CMMQTTSubmsg));
		topicmsg->topicName.lenstring.len = 0;
		topicmsg->topicName.lenstring.data = NULL;
		topicmsg->topicName.cstring = (char*)cmmqtt_malloc(strlen(submsg->topicName.cstring)+1);
		strcpy(topicmsg->topicName.cstring,submsg->topicName.cstring);
		topicmsg->dup = submsg->dup;
		topicmsg->qos = submsg->qos;
		topicmsg->queueout_ackcb = submsg->queueout_ackcb;
		topicmsg->topic_matchcb = submsg->topic_matchcb;

		e->data = topicmsg;
		e->entry_free = cmmqtt_free_subtopic;
		rc = cmmqtt_add_subtopic(&myclient->topictable,&e->hash_head);
		if(rc == true)
		{	
			cmmqtt_log("add topic ok");	
		}		
			
	}
	else
	{
		CMMQTTSubmsg *sub = (CMMQTTSubmsg *)topic->data;
		sub->qos = submsg->qos;
		sub->topic_matchcb = submsg->topic_matchcb;
	}
	
   // apb_proxy_urc_string(APB_PROXY_INVALID_CMD_ID,"+MQTTSUBACK: %d,%d,%s",id,qos,submsg->topicName.cstring);
   	onemo_printf("[MQTT]MQTTSUBACK:%d,%d,%s\r\n",id,qos,submsg->topicName.cstring);
	if(item != NULL)
		item->node_free(item);

}

void cmmqtt_unsuback_cb(unsigned short id, int qos,void *node)
{
	//UNUSED(qos);
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmqueue_node *item = (cmqueue_node*)node;
	CMMQTTUnsubmsg *unsubmsg = (CMMQTTUnsubmsg *)item->data;
	cmmqtt_entry *topic = cmmqtt_find_topic(&myclient->topictable,unsubmsg->topicName.cstring,cmmqtt_entry,hash_head);
	if(topic == NULL)
	{
		cmmqtt_log("topic not sub !");
	}
	else
	{
		cmmqtt_remove_topic(&myclient->topictable,unsubmsg->topicName.cstring,cmmqtt_entry,hash_head);
		topic->entry_free(topic);
		cmmqtt_log("topic remove !");
	}
	onemo_printf("[MQTT]MQTTUNSUBACK:%d,%s\r\n",id,unsubmsg->topicName.cstring);
	if(item != NULL)
		item->node_free(item);
   
}

void cmmqtt_puback_cb(unsigned short id, unsigned char packettype,unsigned char dup,void * node)
{
	
	cmqueue_node *item = (cmqueue_node *)node;
	int ret_value = 0;
	if(packettype == PUBACK)
	{
        onemo_printf("[MQTT]MQTTPUBACK:%d,%d\r\n",dup,id);
		if(item != NULL)
		{
			item->node_free(item);
			item = NULL;
		}
		else
		{
			cmmqtt_log("pub node null");
		}
	}
	else if(packettype == PUBREC)
	{
        ret_value = cmmqtt_puback(PUBREL,id,dup);
		if(CMMQTT_AT_OK == ret_value)
		{
			cmmqtt_err_stat.cmmqtt_pub_error[1] = CMMQTT_PUB_OK;
		}
		else
		{
			cmmqtt_err_stat.cmmqtt_pub_error[1] = CMMQTT_PUBACK_FAIL;
		}
			
		onemo_printf("[MQTT]MQTTPUBREC:%d,%d\r\n",dup,id);
	}
	else
	{
		if(packettype == PUBCOMP)
		{
            onemo_printf("[MQTT]MQTTPUBCOMP:%d,%d\r\n",dup,id);
			if(item != NULL)
			{
				item->node_free(item);
				item = NULL;
			}
		}
	}
}
void cmmqtt_recpub_text_cb(void *pubmsg)		
{
    CMMQTTClient *myclient = cmmqtt_getclient();
    CMMQTTPubmsg *msg = pubmsg;
	if(myclient->publish_remainlen <= 0)
	{
		if(msg->qos == QOS1)
		{
			cmmqtt_puback(PUBACK,msg->id,msg->dup);
		}
		if(msg->qos == QOS2)
		{
			cmmqtt_puback(PUBREC,msg->id,msg->dup);
		}
		char topicname[msg->topicName.lenstring.len];
		strncpy(topicname,msg->topicName.lenstring.data,msg->topicName.lenstring.len);
		topicname[msg->topicName.lenstring.len] = '\0';
		/*mqtt_printf_normal("\r\n");
		mqtt_printf_normal("+MQTTPUBLISH:%d,%d,%d,%d,%s,%d,",msg->dup,msg->qos,msg->retained,msg->id,topicname,msg->payloadlen);
		mqtt_printf_longstring(msg->payload,msg->firstpacket_payloadlen);
		myclient->publish_remainlen = msg->payloadlen - msg->firstpacket_payloadlen;*/
		cmmqtt_printf("+MQTTPUBLISH:%d,%d,%d,%d,%s,%d,%s",msg->dup,msg->qos,msg->retained,msg->id,topicname,msg->payloadlen,msg->payload);
		//mqtt_printf_longstring(msg->payload,msg->firstpacket_payloadlen);
		myclient->publish_remainlen = msg->payloadlen - msg->firstpacket_payloadlen;
	}
	else
	{
		//mqtt_printf_longstring(msg->payload,msg->payloadlen);
		cmmqtt_printf((const char *)msg->payload,msg->payloadlen);
		myclient->publish_remainlen = myclient->publish_remainlen - msg->payloadlen;
	}
	cmmqtt_log("pub remain:%d",myclient->publish_remainlen);
}




void cmmqtt_recpub_long_text_cb(void *msg)		//string 	打印类型的回调函数,改进后可接收大于bufsize的数据长度。
{
    CMMQTTClient *myclient = cmmqtt_getclient();
    CMMQTTPubmsg *pubmsg =(CMMQTTPubmsg *)msg;
    uint32_t len = 0;
	uint32_t expect_len = 0;
	//int fixheader_len = 0;
	int remain_len = 0 ;
	int original_len = 0 ;
    unsigned char *rsp_buf = myclient->at_buf;
	char topicname[pubmsg->topicName.lenstring.len];
	CMMQTT_Timer timer;
	int main_loop = 0;
	strncpy(topicname,pubmsg->topicName.lenstring.data,pubmsg->topicName.lenstring.len);
	topicname[pubmsg->topicName.lenstring.len] = '\0';
	original_len = pubmsg->payloadlen;
		
	main_loop = pubmsg->payloadlen / myclient->bufsize + 3;		
	remain_len = pubmsg->payloadlen - (pubmsg->firstpacket_payloadlen - 2 - pubmsg->topicName.lenstring.len -2); //计算剩余的payload 
	pubmsg->payloadlen = pubmsg->firstpacket_payloadlen - 2 - pubmsg->topicName.lenstring.len -2;	       
	if(pubmsg->qos ==0)				//qos = 0 少两位packet id
	{
		remain_len = remain_len - 2;
		pubmsg->payloadlen = pubmsg->payloadlen +2;
	}
	memset(rsp_buf,0,myclient->bufsize*2 + CMMQTT_AT_RESPONSE_DATA_LEN);
	len = sprintf((char *)rsp_buf, "[MQTT]MQTTPUBLISH: %d,%d,%d,%d,",pubmsg->dup,pubmsg->qos,pubmsg->retained,pubmsg->id);
    memcpy(rsp_buf + len, topicname, pubmsg->topicName.lenstring.len);
    len += pubmsg->topicName.lenstring.len;
    len += sprintf((char *)rsp_buf + len , ",%d,",original_len);
	onemo_printf((const char *)rsp_buf,len);
	do
	{
	    //prepare +MQTTPUBLISH messagem
	    main_loop--;
	  /*  memset(rsp_buf,0,myclient->bufsize*2 + CMMQTT_AT_RESPONSE_DATA_LEN);
		len = sprintf(rsp_buf, "+MQTTPUBLISH: %d,%d,%d,%d,\"",pubmsg->dup,pubmsg->qos,pubmsg->retained,pubmsg->id);
	    memcpy(rsp_buf + len, topicname, pubmsg->topicName.lenstring.len);
	    len += pubmsg->topicName.lenstring.len;
	    len += sprintf(rsp_buf + len , "\",%d\r\n",pubmsg->payloadlen);
	    */
	    //copy payload data
	    memset(rsp_buf,0,myclient->bufsize*2 + CMMQTT_AT_RESPONSE_DATA_LEN);
	    len = 0;
	    memcpy(rsp_buf + len, pubmsg->payload, pubmsg->payloadlen);
	    len += pubmsg->payloadlen;
	    //TODO: how to do with long message
	    onemo_printf((const char *)rsp_buf,len);
		memset(myclient->readbuf,0,myclient->bufsize);
		if(remain_len > myclient->bufsize)
			expect_len = myclient->bufsize;
		else
			expect_len = remain_len;
		if(expect_len > 0)
		{
			int recv_len = 0;
			int rc = 0;
			int loop = 0;
			while(recv_len < expect_len && loop < 8)
			{
				rc = myclient->ipstack.mqttread(&myclient->ipstack, myclient->readbuf  + recv_len, expect_len - recv_len, cmmqtt_left_ms(&timer)); 
				if(rc <= 0)
					cmmqtt_countdown_ms(&timer, CMMQTT_RECV_TIME);	
				else
				recv_len = recv_len + rc;
				loop++;
				myclient->processalive = 1;
			}
			//pubmsg->payloadlen = myclient->ipstack.mqttread(&myclient->ipstack, myclient->readbuf, expect_len, cmmqtt_left_ms(&timer));
			pubmsg->payloadlen = recv_len;
			pubmsg->payload = myclient->readbuf;
			remain_len = remain_len - pubmsg->payloadlen;
		}
		cmmqtt_log("remain_len: %d ..",remain_len);
	}while(expect_len > 0 && main_loop > 0);
	onemo_printf("\r\n",2);
	if(pubmsg->qos == QOS1)
	{
		cmmqtt_puback(PUBACK,pubmsg->id,pubmsg->dup);
	}
	if(pubmsg->qos == QOS2)
	{
		cmmqtt_puback(PUBREC,pubmsg->id,pubmsg->dup);
	}
}
/*
void cmmqtt_recpub_hex_cb(void *msg)		//hex  打印类型的回调函数
{
    apb_proxy_at_cmd_result_t response;
    uint32_t len = 0;
    CMMQTTPubmsg *pubmsg =(CMMQTTPubmsg *)msg;
	char topicname[pubmsg->topicName.lenstring.len];
	strncpy(topicname,pubmsg->topicName.lenstring.data,pubmsg->topicName.lenstring.len);
	topicname[pubmsg->topicName.lenstring.len] = '\0';
    char *hex_buf = pvPortMalloc(pubmsg->payloadlen * 2 + CMMQTT_AT_RESPONSE_DATA_LEN);
    if(hex_buf == NULL)
        return;

	response.result_code = APB_PROXY_RESULT_UNSOLICITED;
	response.cmd_id = APB_PROXY_INVALID_CMD_ID;

	if(pubmsg->qos == QOS1)
	{
		cmmqtt_puback(PUBACK,pubmsg->id,pubmsg->dup);
	}
	if(pubmsg->qos == QOS2)
	{
		cmmqtt_puback(PUBREC,pubmsg->id,pubmsg->dup);
	}
	len = sprintf(hex_buf, "+MQTTPUBLISH: %d,%d,%d,%d,%s,%d\r\n",
                pubmsg->dup,  pubmsg->qos,pubmsg->retained,pubmsg->id,topicname,pubmsg->payloadlen);

    _get_data_to_hex(hex_buf + len,pubmsg->payload,pubmsg->payloadlen);
    //+MQTTPUBLISH head + data
    response.pdata = hex_buf;
	response.length = pubmsg->payloadlen * 2 + len;
    apb_proxy_send_at_cmd_result(&response);

    vPortFree(hex_buf);

}
*/


void cmmqtt_nodetimeout_cb(void *node)
{
	CMMQTTClient *c = cmmqtt_getclient();
	cmqueue_node *item = (cmqueue_node *)node;
	switch(item->msgtype)
	{
		case PUBLISH:
			{
				
				 cmmqtt_printf("+MQTTTO:%d", MQTT_TO_PUBLISH);
			}
		break;
		case SUBSCRIBE:
			{
				//CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)item->data;
                 cmmqtt_printf("+MQTTTO:%d", MQTT_TO_SUB);
			}
		break;
		case UNSUBSCRIBE:
			{
				//CMMQTTUnsubmsg *unsubmsg = (CMMQTTUnsubmsg *)item->data;
                 cmmqtt_printf("+MQTTTO:%d", MQTT_TO_UNSUB);
			}
		break;
		case CONNECT:
			{
                cmmqtt_printf("+MQTTTO:%d", MQTT_TO_CONN);
				//c->ipstack.disconnect(&c->ipstack);
				c->state = RECONNECTING;
			}
		break;
      /*  case PINGREQ:
			{
				apb_proxy_urc_string(APB_PROXY_INVALID_CMD_ID,"+MQTTTO: %d", MQTT_TO_PING);
			}
         break;
        */
		default:
			cmmqtt_printf("+MQTTTO:%d", MQTT_TO_UNKOWN);
		break;
	}
}

void cmmqtt_free_conmsg(void *msg)
{
	if(msg != NULL)
	{
		CMMQTTConmsg *conmsg = (CMMQTTConmsg *)msg;
		if(conmsg != NULL)
		{
			if(conmsg->connect_data.will.topicName.cstring != NULL)
			{
				cmmqtt_free(conmsg->connect_data.will.topicName.cstring);
				conmsg->connect_data.will.topicName.cstring = NULL;
			}
			if(conmsg->connect_data.will.topicName.lenstring.data != NULL)
			{
				cmmqtt_free(conmsg->connect_data.will.topicName.lenstring.data);
				conmsg->connect_data.will.topicName.lenstring.len =0;
				conmsg->connect_data.will.topicName.lenstring.data = NULL;
			}
			if(conmsg->connect_data.will.message.cstring != NULL)
			{
				cmmqtt_free(conmsg->connect_data.will.message.cstring);
				conmsg->connect_data.will.message.cstring = NULL;
			}
			if(conmsg->connect_data.will.message.lenstring.data != NULL)
			{
				cmmqtt_free(conmsg->connect_data.will.message.lenstring.data);
				conmsg->connect_data.will.message.lenstring.len =0;
				conmsg->connect_data.will.message.lenstring.data = NULL;
			}
			cmmqtt_free(conmsg);
			//conmsg = NULL;
		}
		//msg = NULL;
	}

}

void cmmqtt_free_pingnode(cmqueue_node *node)
{
	if(node != NULL)
	{
		cmmqtt_free(node);
		node = NULL;
	}
}


void cmmqtt_free_connode(cmqueue_node *node)
{
	if(node != NULL)
	{
		//CMMQTTConmsg *conmsg = (CMMQTTConmsg *)node->data;
		/*if(conmsg != NULL)
		{
			cmmqtt_free_conmsg(conmsg);
		}
		*/
		cmmqtt_free(node);
		node = NULL;
	}
}
void cmmqtt_free_pubmsg(void *msg)
{
	if(msg != NULL)
	{
		CMMQTTPubmsg *pubmsg = (CMMQTTPubmsg *)msg;
		if(pubmsg != NULL)
		{
			if(pubmsg->payload != NULL)
			{
				cmmqtt_free(pubmsg->payload);
				pubmsg->payload = NULL;
			}
			if(pubmsg->topicName.cstring != NULL)
			{
				cmmqtt_free(pubmsg->topicName.cstring);
				pubmsg->topicName.cstring = NULL;
			}
			if(pubmsg->topicName.lenstring.data != NULL)
			{
				cmmqtt_free(pubmsg->topicName.lenstring.data);
				pubmsg->topicName.lenstring.data = NULL;
				pubmsg->topicName.lenstring.len = 0;
			}
			if(pubmsg->queueout_ackcb != NULL)
				pubmsg->queueout_ackcb = NULL;
			cmmqtt_free(pubmsg);
			//pubmsg = NULL;
		}
		//msg = NULL;
	}

}

void cmmqtt_free_pubnode(cmqueue_node *node)
{
	if(node != NULL)
	{
		CMMQTTPubmsg *pubmsg = (CMMQTTPubmsg *)node->data;
		//cmmqtt_log("id: %d",pubmsg->id);
		if(pubmsg != NULL)
		{
			cmmqtt_free_pubmsg(pubmsg);
			pubmsg = NULL;
		}
		cmmqtt_free(node);
	}

}
void cmmqtt_free_unsubmsg(void *msg)
{
	if(msg != NULL)
	{
		CMMQTTUnsubmsg *unsubmsg = (CMMQTTUnsubmsg *)msg;
		if(unsubmsg != NULL)
		{

			if(unsubmsg->topicName.cstring != NULL)
			{
				cmmqtt_free(unsubmsg->topicName.cstring);
				unsubmsg->topicName.cstring = NULL;
			}
			if(unsubmsg->topicName.lenstring.data != NULL)
			{
				cmmqtt_free(unsubmsg->topicName.lenstring.data);
				unsubmsg->topicName.lenstring.data = NULL;
				unsubmsg->topicName.lenstring.len = 0;
			}
			if(unsubmsg->queueout_ackcb != NULL)
				unsubmsg->queueout_ackcb = NULL;
			if(unsubmsg->topic_matchcb != NULL)
				unsubmsg->topic_matchcb = NULL;
			cmmqtt_free(unsubmsg);
			//unsubmsg = NULL;
		}
		//msg = NULL;
	}

}

void cmmqtt_free_unsubnode(cmqueue_node *node)
{
	if(node != NULL)
	{
		CMMQTTUnsubmsg *unsubmsg = (CMMQTTUnsubmsg *)node->data;
		if(unsubmsg != NULL)
		{
			cmmqtt_free_unsubmsg(unsubmsg);
			unsubmsg = NULL;
		}
		cmmqtt_free(node);
		//node = NULL;
	}
}

void cmmqtt_free_submsg(void *msg)
{
	if(msg != NULL)
	{
		CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)msg;
		if(submsg != NULL)
		{
			if(submsg->topicName.cstring != NULL)
			{
				cmmqtt_free(submsg->topicName.cstring);
				submsg->topicName.cstring = NULL;
			}
			if(submsg->topicName.lenstring.data != NULL)
			{
				cmmqtt_free(submsg->topicName.lenstring.data);
				submsg->topicName.lenstring.data = NULL;
				submsg->topicName.lenstring.len = 0;
			}
			if(submsg->queueout_ackcb != NULL)
				submsg->queueout_ackcb = NULL;
			if(submsg->topic_matchcb != NULL)
				submsg->topic_matchcb = NULL;
			cmmqtt_free(submsg);
			//submsg = NULL;
		}
		//msg = NULL;
	}

}

void cmmqtt_free_subnode(cmqueue_node *node)
{
	if(node != NULL)
	{
		CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)node->data;
		if(submsg != NULL)
		{
			cmmqtt_free_submsg(submsg);
			submsg = NULL;
		}
		cmmqtt_free(node);
		//node = NULL;
	}

}

void cmmqtt_free_subtopic(cmmqtt_entry *topic)
{
	if(topic != NULL)
	{
		CMMQTTSubmsg *submsg = (CMMQTTSubmsg *)topic->data;
		if(submsg != NULL)
		{
			cmmqtt_free_submsg(submsg);
			submsg = NULL;
		}
		if(topic->key != NULL)
		{
			cmmqtt_free(topic->key);
			topic->key = NULL;
		}
		cmmqtt_free(topic);
		//topic = NULL;
	}
}




