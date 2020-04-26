/*******************************************************************************
 by liyanlun @2018
 *******************************************************************************/

#ifndef __CMMQTT_TOPIC_C_
#define __CMMQTT_TOPIC_C_
#include "chaining_hash_table.h" 
#include "hash_shared.h" 

typedef struct mqtt_entry cmmqtt_entry;

typedef struct mqtt_entry
{
	struct slist_head hash_head;
	char *key;
	void *data;
	void (*entry_free)(cmmqtt_entry *);
}cmmqtt_entry;
typedef struct genc_chaining_hash_table CMMQTT_Topictable;

size_t BKDRHash(char *str) ;
genc_hash_t cmmqtt_hash(void* key, void* opaque);
void* cmmqtt_get_key(struct slist_head* hash_head, void* opaque);
genc_bool_t cmmqtt_keys_equal(void* key1, void* key2, void* opaque);
void* cmmqtt_realloc(void* old, size_t old_size, size_t new_size, void* opaque);
int  cmmqtt_topictable_init(CMMQTT_Topictable* table,void* opaque,size_t initial_capacity_pow2);
int	cmmqtt_add_subtopic(CMMQTT_Topictable* table, struct slist_head* item);
struct slist_head** cmmqtt_get_subtopic(CMMQTT_Topictable* table, void* key);
void cmmqtt_topictable_clean(CMMQTT_Topictable* table);
void cmmqtt_topictable_destory(CMMQTT_Topictable* table);
#define cmmqtt_find_topic(table, key, type, header_name) genc_cht_find_obj(table, key, type, header_name)
#define cmmqtt_remove_topic(table, key, type, header_name) genc_cht_remove_obj(table, key, type, header_name)
#define	cmmqtt_table_for_each_obj_ref(TABLE,ENTRY_VAR,CUR_HEAD_PTR_VAR,BUCKET_VAR,ENTRY_TYPE,TABLE_HEAD_MEMBER_NAME) \
	genc_cht_for_each_obj_ref(TABLE,ENTRY_VAR,CUR_HEAD_PTR_VAR,BUCKET_VAR,ENTRY_TYPE,TABLE_HEAD_MEMBER_NAME)


#endif
