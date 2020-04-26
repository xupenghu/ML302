/*******************************************************************************
by liyanlun @2018
 *******************************************************************************/

#include "CM_MQTTClient.h"

 size_t BKDRHash(char *str)  
 {	
	 register size_t hash = 0;
	 size_t ch;
	 //while (ch = (size_t)*str++)  
	 while(*str)
	 {		   
		 ch = (size_t)*str++;
		 hash = hash * 131 + ch;   		
	 }	
	 return hash;
 }	


 genc_hash_t cmmqtt_hash(void* key, void* opaque)
{
	//return genc_hash_uint32(*(unsigned*)key);
	//UNUSED(opaque);
	return BKDRHash(key); 
}
 void* cmmqtt_get_key(struct slist_head* hash_head, void* opaque)
{
	//UNUSED(opaque);
	struct mqtt_entry* e = genc_container_of(hash_head, struct mqtt_entry, hash_head);
	return e->key;
}
 genc_bool_t cmmqtt_keys_equal(void* key1, void* key2, void* opaque) 
{
	//UNUSED(opaque);
	if(strcmp((char *)key1,(char*)key2) == 0)
		return true;
	else
		return false;
	//return (*(char*)key1) == (*(unsigned int*)key2);
}
 void* cmmqtt_realloc(void* old, size_t old_size, size_t new_size, void* opaque)
{
	//UNUSED(opaque);
	//UNUSED(old_size);
	return realloc(old, new_size);
}

int cmmqtt_topictable_init(CMMQTT_Topictable* table,void* opaque,size_t initial_capacity_pow2)
{
	
	return genc_chaining_hash_table_init(table, cmmqtt_hash, cmmqtt_get_key, cmmqtt_keys_equal, cmmqtt_realloc,opaque, initial_capacity_pow2);
	
}

int	cmmqtt_add_subtopic(CMMQTT_Topictable* table, struct slist_head* item)
{
	return genc_cht_insert_item(table, item);

}

struct slist_head** cmmqtt_get_subtopic(CMMQTT_Topictable* table, void* key)
{

	return genc_cht_find_ref(table, key);
}

void cmmqtt_topictable_clean(CMMQTT_Topictable* table)
{
	//UNUSED(table);
	CMMQTTClient *myclient = cmmqtt_getclient();
	cmmqtt_entry *cur;
	cmmqtt_entry *temp;
	cmmqtt_head **ref = NULL;
	uint32 bucket = 0;
	cmmqtt_table_for_each_obj_ref(&myclient->topictable, cur, ref, bucket, cmmqtt_entry, hash_head)
	{
		if(cur != NULL)
		{
			temp = cur;
			CMMQTTSubmsg *msg = (CMMQTTSubmsg *)cur->data;
			cmmqtt_remove_topic(&myclient->topictable,msg->topicName.cstring,cmmqtt_entry,hash_head);
			cur->entry_free(cur);
			cur = temp;
		}
	}
	
}

void cmmqtt_topictable_destory(CMMQTT_Topictable* table)
{
	//genc_cht_destroy(table);
	if(table->buckets != NULL)
	{
		cmmqtt_free(table->buckets);
		table->buckets = NULL;
	}
	table->capacity = 0;
	table->item_count = 0;
}
