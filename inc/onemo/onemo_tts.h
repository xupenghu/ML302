/*********************************************************
*  @file    onemo_tts.h
*  @brief   ML302 OpenCPU TTS APIs header
*  Copyright (c) 2020 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2020/03/09
********************************************************/
#ifndef __ONEMO_TTS_H__
#define __ONEMO_TTS_H__
  /**

 * @defgroup TTS_FUNCTIONS
 * 
   TTS API，请参照SDK中示例文件使用
 *@{
 */
 /**
 * @brief TTS文字编码，目前支持ML_UTF8和ML_CP936
 */
enum
{
    ML_ISO8859_1,
    ML_UTF8,
    ML_UTF16BE,
    ML_UTF16LE,
    ML_GSM,
    ML_CP936
};
/**
 *  \brief 停止播放
 *  
 *  \return 空
 *  
 *  \details More details
 */
void onemo_tts_stop(void);
/**
 *  \brief 查询TTS是否在播放
 *  
 *  \return true:在播放 false:没在播放
 *  
 *  \details More details
 */
bool onemo_tts_playing(void);
/**
 *  \brief TTS播放文字函数
 *  
 *  \param [in] text 将要播放的文字
 *  \param [in] len text的字符串长度，当等于-1时，函数内部自动判断
 *  \param [in] encoding 编码，当前支持ML_UTF-8和ML_CP936
 *  \return true:成功 false:失败
 *  
 *  \details 这是异步函数，播放任务建立后，此函数会立即返回
 */
bool onemo_tts_playText(const void * text,int len,unsigned encoding);

/** @} */ 
#endif