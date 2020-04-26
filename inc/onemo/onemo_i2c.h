/*********************************************************
*  @file    onemo_i2c.h
*  @brief   ML302 OpenCPU i2c APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by Yu Yansen 2019/11/5
********************************************************/
#ifndef __ONEMO_I2C_H__
#define __ONEMO_I2C_H__
/**

 * @defgroup I2C_FUNCTIONS 
 * 
   ML302 I2C功能
 *@{
 */
typedef enum{
    ONEMO_I2C_MASTER_1,//当前不支持I2C_MASTER_1
    ONEMO_I2C_MASTER_2,
    ONEMO_I2C_MASTER_3,//当前不支持I2C_MASTER_3
}onemo_i2c_master_id_t;

typedef enum
{
    ONEMO_I2C_BPS_100K, ///< normal 100Kbps
    ONEMO_I2C_BPS_400K, ///< fast 400Kbps
    ONEMO_I2C_BPS_3P5M, ///< high speed 3.5Mbps
} onemo_i2c_bps_t;;

/**
 *  \brief I2C初始化函数
 *  
 *  \param [in] id 主设备id ,暂只支持Master 2
 *  \param [in] i2c_bps i2c 传输速率 
 *  \return 0:成功 -1:失败
 *  
 *  \details More details
 */

int onemo_i2c_init(onemo_i2c_master_id_t id, onemo_i2c_bps_t i2c_bps)
;
/**
 *  \brief I2C反初始化函数
 *  
 *  \param [in] id 主设备id ,暂只支持Master 2
 *  \return 0:成功 -1:失败
 *  
 *  \details More details
 */
int onemo_i2c_deinit(onemo_i2c_master_id_t id);
/**
 *  \brief i2c读取数据
 *  
 *  \param [in] id 主设备id,暂只支持Master 2
 *  \param [in] slave_addr I2C外设地址
 *  \param [in] length 要读取的数据长度
 *  \param [out] buf 要读取的数据地址
 *  \return 0:成功 -1:失败
 *  
 *  \details More details
 */
int onemo_i2c_read(onemo_i2c_master_id_t id, unsigned char slave_addr, unsigned char *buf, unsigned int length);
/**
 *  \brief i2c写数据
 *  
 *  \param [in] id 主设备id,暂只支持Master 2
 *  \param [in] slave_addr I2C外设地址
 *  \param [in] length 要发送的数据长度
 *  \param [in] data 要发送的数据地址
 *  \return 0:成功 -1:失败
 *  
 *  \details More details
 */
int onemo_i2c_write(onemo_i2c_master_id_t id, unsigned char slave_addr, const unsigned char * data, unsigned int length);
/** @} */ 
#endif
