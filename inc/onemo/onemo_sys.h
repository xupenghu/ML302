/*********************************************************
*  @file    onemo_sys.h
*  @brief   ML302 OpenCPU base header file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/11/11
********************************************************/
#ifndef __ONEMO_SYS_H__
#define __ONEMO_SYS_H__
/*! \mainpage 手册须知
 * \section 重要声明
 * <H2>免责声明</H2>
 *
 * 除非合同另有约定，中移物联网有限公司对本文档内容不做任何明示或暗示的声明或保证，并且不对特定目的适销性及适用性或者任何间接、特殊或连带的损失承担任何责任。
由于产品版本升级或其他原因，本文档内容会不定期进行更新。除非另有约定，本文档仅作为使用指导，本文档中的所有陈述、信息和建议不构成任何明示或暗示的担保。公司保留随时修改本手册中任何信息的权利，无需进行提前通知且不承担任何责任。

 *
 * <H2>保密声明</H2>
 本文档（包括任何附件）包含的信息是保密信息。接收人了解其获得的本文档是保密的，除用于规定的目的外不得用于任何目的，也不得将本文档泄露给任何第三方。
 *
 * <H2>版权所有©中移物联网有限公司。保留一切权利。</H2> 
 * 本手册中描述的产品，可能包含中移物联网公司及其存在的许可人享有版权的软件，除非获得相关权利人的许可，否则，非经本公司书面同意，任何单位和个人不得擅自摘抄、复制本手册内容的部分或全部，并以任何形式传播。
 *  
 *  
 *    
 */
 /**

 * @defgroup SYS_FUNCTIONS
 * 
   基础或杂项API
 *@{
 */
 struct l_tm {
    int tm_sec; /* 秒 – 取值区间为[0,59] */   
    int tm_min; /* 分 - 取值区间为[0,59] */
    int tm_hour; /* 时 - 取值区间为[0,23] */
    int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
    int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
    int tm_year; /* 年份，其值等于实际年份减去1900 */
};
/**
 *  \brief 虚拟AT通道回调函数，param暂无意义
 */
typedef void (*onemo_vir_at_cb)(void *param);
typedef enum onemo_boot_cause
{
    ONEMO_BOOTCAUSE_UNKNOWN = 0,           ///0,< placeholder for unknown reason
    ONEMO_BOOTCAUSE_PWRKEY = (1 << 0),     ///1,< boot by power key
    ONEMO_BOOTCAUSE_PIN_RESET = (1 << 1),  ///2,< boot by pin reset
    ONEMO_BOOTCAUSE_ALARM = (1 << 2),      ///4,< boot by alarm
    ONEMO_BOOTCAUSE_CHARGE = (1 << 3),     ///8,< boot by charge in
    ONEMO_BOOTCAUSE_WDG = (1 << 4),        ///16,< boot by watchdog
    ONEMO_BOOTCAUSE_PIN_WAKEUP = (1 << 5), ///32,< boot by wakeup
    ONEMO_BOOTCAUSE_PSM_WAKEUP = (1 << 6), ///64,< boot from PSM wakeup
} onemo_boot_cause_t;
typedef struct onemo_cmd {
    unsigned char * cmdstr;
    void (* cmdfunc)(char **,int);
} onemo_cmd_t;
#define onemo_sys_log(fmt,arg...)  sys_arch_printf("[ONEMO_OC]"fmt,##arg);
/**
 *  \brief us级精确延时接口
 *  
 *  \param [in] delay_us 延时周期个数
 *  \return 空
 *  
 *  \details More details
 */
void onemo_sys_delay_us(int delay_us);
/**
 *  \brief 获取模组启动原因
 *  
 *  \return 模组启动原因
 *  
 *  \details More details
 */
onemo_boot_cause_t onemo_sys_get_boot_cause(void);
/**
 *  \brief 重启模组
 *  
 *  \return 空
 *  
 *  \details More details
 */
void onemo_sys_reboot(void);
/**
 *  \brief 虚拟AT通道初始化，必须在线程中调用，不能在OpenCPU入口函数中调用
 *  
 *  \param [in] cb AT回调函数
 *  \return 0:成功 -1:失败
 *  
 *  \details AT虚拟通道仅支持ML302 AT文档中第2到9章包含的AT命令
 */
int onemo_vir_at_init(onemo_vir_at_cb cb);
/**
 *  \brief 虚拟AT通道发送接口
 *  
 *  \param [in] at_str 要发送的AT命令字符串
 *  \return 发送成功的字节数
 *  
 *  \details AT虚拟通道仅支持ML302 AT文档中第2到8章包含的AT命令
 */
int onemo_vir_at_send(unsigned char * at_str);
/**
 *  \brief 虚拟AT通道接收函数
 *  
 *  \param [in] buf 接收返回的内存指针
 *  \param [in] max_len 内存长度
 *  \return Return 实际接收到的长度
 *  
 *  \details AT虚拟通道仅支持ML302 AT文档中第2到8章包含的AT命令
 */
int onemo_vir_at_get(unsigned char *buf,int max_len);
/**
 *  \brief 关闭AT虚拟通道
 *  
 *  \return 空
 *  
 *  \details AT虚拟通道仅支持ML302 AT文档中第2到8章包含的AT命令
 */
void onemo_virt_at_deinit(void);
/**
 *  \brief 获取当前时区
 *  
 *  \return 空
 *  
 *  \details More details
 */
int onemo_get_timezone(void);
/**
 *  \brief 把1970年到现在的秒数转换为年月日时分秒
 *  
 *  \param [in] time 秒数
 *  \param [in] t 时间结构体
 *  \return 空
 *  
 *  \details More details
 */
void time_to_date(long time,struct l_tm *t);
/** @} */ 
#endif
