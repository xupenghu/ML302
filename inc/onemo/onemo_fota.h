/*********************************************************
*  @file    onemo_fota.h
*  @brief   ML302 OpenCPU FOTA APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by yuyangyi 2020/01/08
********************************************************/
#ifndef __ONEMO_FOTA_H__
#define __ONEMO_FOTA_H__
  /**

 * @defgroup FOTA_FUNCTIONS
 * 
   FOTA API，请参照SDK中示例文件使用
 */
typedef void (*oc_error_cb_t) (int error_code);
typedef void (*oc_event_cb_t) (int state, int event);
typedef void (*oc_download_cb_t) (int state, int event, int current, int total, int percent);
typedef void (*oc_get_version) (char **version);

typedef struct oc_cb
{
	oc_error_cb_t onError;
	oc_event_cb_t onEvent;
	oc_download_cb_t onDownload;
  oc_get_version onVersion;
}oc_cb_t;

 /**
 *  \brief onenet fota 错误码回调函数
 * 
 * error_code
 * 1	网络异常，检查网络后手动重新触发查询下载
 * 2	Token失效，检测平台任务是否异常，需手动重新触发查询下载
 * 3	上报状态失败，由于升级任务已取消、升级任务已完成导致上报状态失败
 * 4	差分升级包大于800KB，升级退出，在平台取消并重新创建任务
 * 5	信号强度低于升级任务设定值
 * 6	差分包升级错误（差分包与当前版本不匹配或差分包文件错误）
 * 7	未知错误
 *  \details More details
 */
void onError_cb(int error_code);

/**
 *  \brief onenet fota 状态码回调函数
 * 
 * state，event
 * state 模组当前流程状态
 *-1	上报流程
 * 0	检测流程
 * 1	下载流程
 * 2	升级流程
 * event 模组上报事件 
 * 上报流程如下
 * 0	上报中断
 * 1	上报完成
 * 2	上报错误
 * 检测流程如下
 * 0	与平台交互后无升级包
 * 1	与平台交互后有升级包，并启动下载
 * 2	有升级包且已经下载
 * 3	升级包过大
 * 4	获取任务失败
 * 下载流程如下
 * 0	开始下载
 * 1	下载过程中
 * 2	下载重试中
 * 3	下载成功
 * 4	下载失败，进度丢失，需手动重新触发查询下载
 * 升级流程如下	
 * 0	开始升级
 * 1	升级成功
 * 2	升级失败，
 *  \details More details
 */
void onEvent_cb(int state, int event);

 /**
 *  \brief onenet fota 下载状态码回调函数
 * <state>,<event>，current/total, percent%
 * <state>,<event>参照onEvent_cb释义
 *  \details More details
 */
void onDownload_cb(int state, int event, int current, int total, int percent);

/**
 *  \brief onenet fota 固件版本号回调获取函数，由用户定义相关版本号
 * 
 *  \param [in] version 固件版本号
 * 
 *  \details More details
 */
void onVersion(char **version);

/**
 *  \brief OneNet FOTA 注册回调函数
 * 
 *  \param [in] oc_cb_tt 回调函数结构体
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_onenet_fota_regcb(oc_cb_t *oc_cb_tt);

/**
 *  \brief OneNet FOTA 回调函数封装函数，方便使用
 *  
 *  \details More details
 */
void onemo_onenet_fota_regcbex(void);

/**
 *  \brief OneNet FOTA 设置平台产品ID信息
 *  
 *  \param [in] product_ID  OneNet平台网页生成
 *  \param [in] APIkey   OneNet平台网页生成
 *  \param [in] access_key  OneNet平台网页生成
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_onenet_fota_setdevinfo(char *product_ID, char *APIkey, char *access_key);

/**
 *  \brief OneNet FOTA 获取设备ID，请确定模组正常驻网后执行该任务
 *  
 *  \param [in] imei  模组IMEI号
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_onenet_fota_getdevid(char *imei);

/**
 *  \brief OneNet FOTA 触发升级，请确定模组正常驻网后执行该任务
 *  
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_onenet_fota_exe(void);

/**
 *  \brief OneNet FOTA 清除升级任务信息
 *  
 *  \return 1：成功 0：失败
 *  
 *  \details OneNet FOTA升级流程中，模组会记录任务信息，在任务出错或任务过期情况下可删除记录信息
 */
int onemo_onenet_fota_deletetask(void);

/**
 *  \brief FOTA 保存升级包至文件系统。
 *  
 *  \param [in] buffer  固件包内容buffer
 *  \param [in] size   固件包大小
 *  \return 1：成功 0：失败
 *  
 *  \details （模组侧FOTA升级函数，用户使用自定义服务器平台时候，将版本包下载后通过该函数保存至文件系统，然后调用onemo_fota_updata()函数启动升级）
 */
int onemo_fota_saverom(char *buffer, size_t size);

/**
 *  \brief FOTA 保存好升级包后触发升级
 *  
 *  \return 1：成功 0：失败
 *  
 *  \details More details
 */
int onemo_fota_update(void);

#endif