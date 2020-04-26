/*******************************************************************************
by liyanlun @2018
 *******************************************************************************/

#include "CM_MQTTClient.h"

void cmmqtt_queue_init(cmmqtt_queue* queue)
{
	genc_slq_init(queue);
}
int cmmqtt_queue_len(cmmqtt_queue* queue)
{
	return genc_slq_length( queue);
}
void cmmqtt_queue_push_back(cmmqtt_queue* queue,cmqueue_node *node)
{
	genc_slq_push_back(queue,&(node->head));
}
void cmmqtt_queue_push_front(cmmqtt_queue* queue,cmqueue_node *node)
{
	genc_slq_push_front(queue,&(node->head));
}
cmmqtt_head *cmmqtt_queue_pop_front(cmmqtt_queue* queue)
{
	return genc_slq_pop_front(queue);
}
cmmqtt_head *cmmqtt_queue_front(cmmqtt_queue* queue)
{
	return genc_slq_front(queue);
}
void cmmqtt_queue_remove_at(cmmqtt_queue* queue,cmmqtt_head** ref)
{
	genc_slq_remove_item_at(queue,ref);
} 
void cmmqtt_queue_remove_all(cmmqtt_queue* queue)
{
	//UNUSED(queue);
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmqueue_node *item = NULL;
	//cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
	while(cmmqtt_queue_len(&myclient->sendqueue) > 0)
	{
		item = cmqtt_pop_front_object(&myclient->sendqueue,cmqueue_node,head);
		if(item != NULL)
		{
			if(item->node_free != NULL)
			{
				item->node_free(item);
				item = NULL;
			}
		}
	}
}

void cmmqtt_queue_checktimeout(cmmqtt_queue* queue)
{
	//UNUSED(queue);
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmqueue_node *item = NULL;
	int count = 0; 
	if(cmmqtt_mutex_take(myclient->queuemutex) == SUCCESS)
	{
		//cmmqtt_log("got the queuemutex .......");
		unsigned int cur_time = 0;
   		cur_time = cmmqtt_current_time_ms();
		cmmqtt_queue_for_each(item,&myclient->sendqueue,cmqueue_node,head)
		{
			if(item != NULL)
			{
				//if(cmmqtt_left_ms(&item->node_timer) ==0)
				if(item->node_timer.end_time <= cur_time )
				{
					count++;
				}
				else
					break;
			}
		}
		//cmmqtt_log("timeout count %d",count);
		while(count > 0)
		{
			item = cmqtt_pop_front_object(&myclient->sendqueue,cmqueue_node,head);
			if(item != NULL)
			{
				if(item->node_timeout != NULL)
					item->node_timeout(item);
				if(item->node_free != NULL)
				{
					item->node_free(item);
					item = NULL;
				}
			}
			count--;
		}
		cmmqtt_mutex_give(myclient->queuemutex);
	}

}


