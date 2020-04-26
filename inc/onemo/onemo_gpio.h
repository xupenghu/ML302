/*********************************************************
*  @file    onemo_gpio.h
*  @brief   ML302 OpenCPU GPIO APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/10/11
********************************************************/
#ifndef __ONEMO_GPIO_H__
#define __ONEMO_GPIO_H__
  /**

 * @defgroup GPIO_FUNCTIONS
 * 
   串口API，请参照SDK中示例文件使用
 *@{
 */
 
 
/**
 *  \brief GPIO中断回调函数
 */
typedef void (*onemo_gpio_cb)(void *param);
/**
 *  @brief 支持的GPIO号
 */
typedef enum {
    ONEMO_GPIO_0,//可用
    ONEMO_GPIO_1,//可用
    ONEMO_GPIO_2,//可用
    ONEMO_GPIO_3,//可用
    ONEMO_GPIO_4,
    ONEMO_GPIO_5,
    ONEMO_GPIO_6,
    ONEMO_GPIO_7,
    ONEMO_GPIO_8,
    ONEMO_GPIO_9,//可用
    ONEMO_GPIO_10,
    ONEMO_GPIO_11,//可用
    ONEMO_GPIO_12,
    ONEMO_GPIO_13,
    ONEMO_GPIO_14,//可用
    ONEMO_GPIO_15,//可用
    ONEMO_GPIO_16,
    ONEMO_GPIO_17,//可用
    ONEMO_GPIO_18,
    ONEMO_GPIO_19,//可用
    ONEMO_GPIO_20,//可用
    ONEMO_GPIO_21,//可用
    ONEMO_GPIO_22,//可用
    ONEMO_GPIO_23,//可用
    ONEMO_GPIO_24,//可用
    ONEMO_GPIO_25,//可用
    ONEMO_GPIO_26,//可用
    ONEMO_GPIO_27,//可用
    ONEMO_GPIO_28,//可用
    ONEMO_GPIO_29,
    ONEMO_GPIO_30,
    ONEMO_GPIO_31,
    ONEMO_GPIO_END,
}onemo_gpio_id_t;

/**
 *  @brief GPIO方向
 */
typedef enum{
    ONEMO_GPIO_OUT,
    ONEMO_GPIO_IN,
}onemo_gpio_dir_t;
/**
 *  @brief GPIO输出电平
 */
typedef enum{
    ONEMO_GPIO_LOW,
    ONEMO_GPIO_HIGH,
}onemo_gpio_level_t;
/**
 *	@brief GPIO上下拉
 */
typedef enum{
	ONEMO_GPIO_PULL_DOWN = 1,
	ONEMO_GPIO_PULL_UP,
	ONEMO_GPIO_FLOAT,
}onemo_gpio_pull_up_down_t;

/**
 *  GPIO初始化配置
 */
typedef struct{
    onemo_gpio_id_t id; /// GPIO号
    onemo_gpio_dir_t dir; ///GPIO方向
    unsigned char irq_enable;
    unsigned char level_or_edge;
    unsigned char rising;
    unsigned char falling;
    onemo_gpio_cb cb;
    void * param;
}onemo_gpio_cfg_t;



/**
 *  \brief GPIO初始化配置
 *  
 *  \param [in] cfg GPIO初始化结构体指针
 *  \return 0:初始化成功 -1:初始化失败
 *  
 *  \details More details
 */
int onemo_gpio_init(onemo_gpio_cfg_t *cfg);
/**
 *  \brief GPIO输入读取
 *  
 *  \param [in] gpio 要读取的GPIO ID
 *  \return 1:高电平  0:低电平
 *  
 *  \details More details
 */
int onemo_gpio_read(onemo_gpio_id_t gpio);
/**
 *  \brief GPIO输出
 *  
 *  \param [in] gpio GPIO ID
 *  \param [in] level GPIO输出电平
 *  \return 空
 *  
 *  \details More details
 */
void onemo_gpio_write(onemo_gpio_id_t gpio, onemo_gpio_level_t level);
/**
 *  \brief GPIO关闭
 *  
 *  \param [in] gpio GPIO ID
 *  \return Return 空
 *  
 *  \details More details
 */
void onemo_gpio_deinit(onemo_gpio_id_t gpio);
/**
 *  \brief GPIO配置上下拉
 *  
 *  \param [in] gpio GPIO ID
 *  \param [in] pull_cfg 上下拉配置
 *  \return 0:配置成功 -1:配置失败
 *  
 *  \details More details
 */
int onemo_gpio_pull_config(onemo_gpio_id_t gpio, onemo_gpio_pull_up_down_t pull_cfg);
/**
 *  \brief GPIO上下拉复位
 *  
 *  \param [in] gpio GPIO ID
 *  \return Return 空
 *  
 *  \details More details
 */
void onemo_gpio_pull_disable(onemo_gpio_id_t gpio);
/** @} */ 
#endif
