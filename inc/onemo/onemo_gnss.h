/*********************************************************
*  @file    onemo_gnss.h
*  @brief   ML302 OpenCPU GNSS APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by yuyangyi 2020/01/19
********************************************************/
#ifndef __ONEMO_GNSS_H__
#define __ONEMO_GNSS_H__
  /**

 * @defgroup GNSS_FUNCTIONS
 * 
   GNSS API，请参照SDK中示例文件使用
 */

/**
 *  \brief lbs 设置key
 *  
 *  \param [in] key  用户自己的key
 *  \return 1：成功 0：失败
 *  
 *  \details 
 * Users should apply for autonavi account by themselves and use their own key. The key applied by our company is built into the module, which can be used for user function development trial, but the key of our company cannot be directly used. 
 *  Simple key acquisition process:
 *  1.Apply for autonavi enterprise developer account;
 *  2.Click personal center -> application management -> to create an application;
 *  3.Click "add new key" on the application page, select intelligent hardware on the service platform, and there is no IP whitelist;
 *  After submission, the key can be obtained.
*/
int onemo_lbs_setkey(char *key_buffer);

/**
 *  \brief lbs obtain base station location data
 *  
 *  \param [out] cellloc  返回lbs定位信息 经度 + 纬度<longitude>,<latitude>
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_lbs_getlocation(char **cellloc);

#endif