/*********************************************************
*  @file    onemo_fs.h
*  @brief   ML302 OpenCPU file system APIs header
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/12/3
********************************************************/
#ifndef __ONEMO_FS_H__
#define __ONEMO_FS_H__
#include "stat.h"
  /**

 * @defgroup FILE_SYSTEM_FUNCTIONS
 * 
   
 *@{
 */
#define	O_RDONLY	0	
#define	O_WRONLY	1	
#define	O_RDWR		2
#define	O_APPEND	0x0008
#define	O_CREAT		0x0200
#define	O_TRUNC		0x0400
#define	O_EXCL		0x0800
#define O_SYNC		0x2000

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define DT_UNKOWN 0 ///< unknown type
#define DT_DIR 4    ///< directory
#define DT_REG 8    ///< regular file

struct dirent
{
    int d_ino;            ///< inode number, file system implementation can use it for any purpose
    unsigned char d_type; ///< type of file
    char d_name[256];     ///< file name
};
typedef struct
{
    int16_t fs_index;  ///< internal fs index
    int16_t _reserved; ///< reserved
} DIR;
 /**
 *  \brief 打开一个文件
 *  
 *  \param [in] filename 文件路径
 *  \param [in] flag 打开参数
 *  \return 0:成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_fopen(const char * filename,int flag);
/**
 *  \brief 关闭一个文件
 *  
 *  \param [in] fd 文件描述符
 *  \return 0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_fclose(int fd);
/**
 *  \brief 读取文件内容
 *  
 *  \param [in] fd 文件描述符
 *  \param [in] buf 存储数据的缓冲区指针
 *  \param [in] len 要读取的数据长度
 *  \return 大于等于0：实际的读取长度 小于0:读取失败
 *  
 *  \details More details
 */
int onemo_fs_fread(int fd,unsigned char* buf,int len);
/**
 *  \brief 写入文件内容
 *  
 *  \param [in] fd 文件描述符
 *  \param [in] buf 存储数据的缓冲区指针
 *  \param [in] len 要写入的数据长度
 *  \return 大于等于0：实际的写入长度 小于0:写入失败
 *  
 *  \details More details
 */
int onemo_fs_fwrite(int fd,unsigned char* buf,int len);
/**
 *  \brief 删除文件
 *  
 *  \param [in] filename 文件路径
 *  \return 0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_fdelete(const char * filename);
/**
 *  \brief 同步文件
 *  
 *  \param [in] fd 文件描述符
 *  \return 0：同步成功 小于0：同步失败
 *  
 *  \details More details
 */
int onemo_fs_fflush(int fd);
/**
 *  \brief 文件指针定位
 *  
 *  \param [in] fd 文件描述符
 *  \param [in] offset 指针偏移量
 *  \param [in] base 偏移起始点，0：文件开头 1：当前位置 2：文件末尾
 *  \return 大于等于0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_fseek(int fd,int offset,int base);
/**
 *  \brief 创建文件夹
 *  
 *  \param [in] dir 文件夹路径
 *  \return 0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_mkdir(const char * dir);
/**
 *  \brief 删除文件夹
 *  
 *  \param [in] dir 文件夹路径
 *  \return 0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_rmdir(const char *dir);
/**
 *  \brief 打开一个文件夹
 *  
 *  \param [in] dir 文件夹的路径
 *  \return 文件夹的对象指针
 *  
 *  \details More details
 */
DIR* onemo_fs_opendir(const char * dir);
/**
 *  \brief 关闭一个文件夹
 *  
 *  \param [in] dir 文件夹对象指针
 *  \return 0：成功 小于0：失败
 *  
 *  \details More details
 */
int onemo_fs_closedir(DIR * dir);
/**
 *  \brief 读取一个文件夹
 *  
 *  \param [in] dir 文件夹对象指针
 *  \return 文件夹内容对象指针
 *  
 *  \details More details
 */
struct dirent * onemo_fs_readdir(DIR * dir);
/**
 *  \brief 读取文件系统剩余空间
 *  
 *  \return 大于等于0：实际大小 -1：失败
 *  
 *  \details More details
 */
int onemo_fs_partion_size(void);

/** @} */ 
#endif