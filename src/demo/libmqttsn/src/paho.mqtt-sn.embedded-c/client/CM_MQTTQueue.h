/*******************************************************************************
 
 *******************************************************************************/

#ifndef __CMMQTT_QUEUE_C_
#define __CMMQTT_QUEUE_C_
#include "slist_queue.h"
#include "MQTTPacket.h"

typedef struct slist_queue cmmqtt_queue,CMMQTT_Queue;
typedef struct slist_head  cmmqtt_head;
typedef struct queue_node cmqueue_node;

typedef struct queue_node 
{
	void *data;
	void (*node_free)(cmqueue_node *);
	void (*node_timeout)(void *);
	cmmqtt_head   head;
	int msgtype;
	CMMQTT_Timer  node_timer;	
}cmqueue_node;


void cmmqtt_queue_init(cmmqtt_queue* queue);
int cmmqtt_queue_len(cmmqtt_queue* queue);
void cmmqtt_queue_push_back(cmmqtt_queue* queue,cmqueue_node *node);
void cmmqtt_queue_push_front(cmmqtt_queue* queue,cmqueue_node *node);
cmmqtt_head *cmmqtt_queue_pop_front(cmmqtt_queue* queue);
cmmqtt_head *cmmqtt_queue_front(cmmqtt_queue* queue);
void cmmqtt_queue_remove_at(cmmqtt_queue* queue,cmmqtt_head** ref);
void cmmqtt_queue_remove_all(cmmqtt_queue* queue);
void cmmqtt_queue_checktimeout(cmmqtt_queue* queue);

#define cmqtt_pop_front_object(queue, list_type, list_head_member_name) \
genc_container_of(cmmqtt_queue_pop_front(queue), list_type, list_head_member_name)

#define cmqtt_front_object(queue, list_type, list_head_member_name) \
genc_container_of(cmmqtt_queue_front(queue), list_type, list_head_member_name)

#define cmmqtt_queue_for_each(loop_var, queue, list_type, list_head_member_name) \
for (loop_var = genc_container_of(cmmqtt_queue_front(queue), list_type, list_head_member_name); loop_var != NULL; loop_var = genc_slist_next(loop_var, list_type, list_head_member_name))


#endif
