/*********************************************************
*  @file    onemo_tts_demo.c
*  @brief   ML302 OpenCPU ADC APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/21
********************************************************/
#include "onemo_main.h"

#define MAX_SIZE 512
void onemo_test_tts(unsigned char **cmd,int len)
{
	int textlen = strlen(cmd[2]);
	unsigned  coding;
	if(char_to_int(cmd[3]) == 0)
	{
		coding = ML_UTF8;
	}
	else
	{
		coding = ML_CP936;
	}
	if(onemo_tts_playing())
	{
		onemo_printf("[TTS]:busy\n");
		return;
	}
	if(textlen > MAX_SIZE)
	{
		onemo_printf("[TTS]:size too long\n");
		return;
	}
	onemo_printf("[TTS]:\n",ttsPlayText(cmd[2], textlen, coding));

}
