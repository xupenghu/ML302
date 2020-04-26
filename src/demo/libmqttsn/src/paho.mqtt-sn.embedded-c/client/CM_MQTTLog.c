/******************************************************************************
 * @brief
 * Copyright (c) 2016 cmiot
******************************************************************************/
#include "CM_MQTTClient.h"
#include "string.h"
#include "stdio.h"
//#include "at_engine.h"
//#include "osi_log.h"

extern void at_cmmqtt_urctext(void *param);
extern char *g_atCmdEngine;
extern void atCmdRespUrcText(char *engine, const char *text);

void cmmqtt_printf(const char * fmt,...)
{
 	va_list args;
    char buff[1024] = {0};

	if(NULL == g_atCmdEngine)
	{
		return;
	}

	va_start(args, fmt);
    vsnprintf(buff, sizeof(buff) - 1, fmt, args);
    va_end(args);

	atCmdRespUrcText(g_atCmdEngine, buff);
}

void cmmqtt_printf_normal(const char * buff,int length)
{
 
	//at_CmdRespUrcNText(at_CmdGetByChannel(g_cm_dlci),buff, length);
	
}

void cmmqtt_printf_special(const char * buff,int length)
{
	if(NULL == g_atCmdEngine)
	{
		return;
	}

	atCmdWrite(g_atCmdEngine, buff, length);
}

void cmmqtt_printf_longstring(const uint8 *buffer, uint32 length)
{
	//at_uart_write(buffer, length );
	;
}	

void cmmqtt_log(const char *fmt,...)
{
	va_list args;
    char buff[1024*3] = {0};
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff) - 1, fmt, args);
    va_end(args);
    sys_arch_printf("cmmqtt: %s", buff);
    fflush(stdout);

}
