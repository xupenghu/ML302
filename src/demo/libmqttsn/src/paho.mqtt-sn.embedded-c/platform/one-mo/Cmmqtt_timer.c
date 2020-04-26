
/**
 * This implementation CAN NOT be called from the ISR method
 * Meanwhile, this implementation also be effective after the vTaskStartScheduler method
 * If need to call from ISR, the APIs should have another implementation
 */

#include "cmmqtt_platform.h"

//extern uint32_t get_current_count(void);
extern int gettimeofday(struct timeval *tv, void *tz);

unsigned int cmmqtt_current_time_ms(void)
{
  	struct timeval tv = { 0 };
    uint32_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

int cmmqtt_timer_expired(CMMQTT_Timer *timer)
{
    unsigned int cur_time = 0;
    cur_time = cmmqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) 
	{
        //cmmqtt_log("MQTT expired enter");
        return 1;
    } 
	else
	{
       // cmmqtt_log("MQTT not expired");
        return 0;
    }
}

void cmmqtt_countdown_ms(CMMQTT_Timer *timer, uint32_t timeout)
{
    timer->end_time = cmmqtt_current_time_ms() + timeout;
}

void cmmqtt_countdown_sec(CMMQTT_Timer *timer, uint32_t timeout)
{
   timer->end_time = cmmqtt_current_time_ms() + (timeout * 1000);

}

uint32_t cmmqtt_left_ms(CMMQTT_Timer *timer)
{
   unsigned int cur_time = 0;
   cur_time = cmmqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) 
	{
        return 0;
    } 
	else
    {
        return timer->end_time - cur_time;
    }
}

void cmmqtt_init_timer(CMMQTT_Timer *timer)
{
    timer->end_time = 0;
}



