/*******************************************************************************
  
 *******************************************************************************/
#include "cmmqtt_platform.h"
void* cmmqtt_malloc(size_t size)
{
	
	void *mem = NULL;
    mem = malloc(size);
    if(mem == NULL) return NULL;
    memset(mem, 0, size);
    return mem;
}

void cmmqtt_free(void *buf)
{
	free(buf);
}





