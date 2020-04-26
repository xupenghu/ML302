/*******************************************************************************
  
 *******************************************************************************/
#include "cmmqtt_platform.h"
#include "cmsis_os.h"

extern osMutexId osMutexCreate (const osMutexDef_t *mutex_def);

extern osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec);

extern osStatus osMutexRelease (osMutexId mutex_id);

Cmmqtt_Error_t cmmqtt_mutex_create(const osMutexDef_t *mutex_name, CMMQTT_Mutex *mutex)
{
	if (mutex == NULL)
	{
        return NULL_VALUE_ERROR;
    }
	
    mutex->lock = osMutexCreate((const osMutexDef_t *)mutex_name);
	if (mutex->lock == NULL)
	{
        return MUTEX_INIT_ERROR;
       
    }
	
    return SUCCESS;	
}

Cmmqtt_Error_t cmmqtt_mutex_take(CMMQTT_Mutex *mutex)
{
	if (mutex == NULL || mutex->lock == NULL)
	{
        return NULL_VALUE_ERROR;
       
    }

	if(osMutexWait(mutex->lock, osWaitForever) != 0)
	{
        return MUTEX_TAKE_ERROR;
    }
    
    return SUCCESS;
}

Cmmqtt_Error_t cmmqtt_mutex_give(CMMQTT_Mutex *mutex)
{
	if (mutex == NULL || mutex->lock == NULL) 
	{
        return NULL_VALUE_ERROR;
       
    }

	if(osMutexRelease(mutex->lock) != 0)
	{
        return MUTEX_GIVE_ERROR;
    }
    return SUCCESS;

}

Cmmqtt_Error_t cmmqtt_mutex_destory(CMMQTT_Mutex *mutex)
{
	if (mutex == NULL || mutex->lock == NULL) 
	{
        return NULL_VALUE_ERROR;
    }

	osMutexDelete(mutex->lock);
    return SUCCESS;	
}



