/*********************************************************
*  @file    onemo_fs_demo.c
*  @brief   ML302 OpenCPU file system example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by XieGangLiang 2019/12/3
********************************************************/
#include "onemo_fs.h"

#define test_dir  "/onemo_test/"
#define test_file "/onemo_test/test.txt"

void onemo_test_delete_file()
{
    onemo_printf("delete:%d\n",onemo_fs_fdelete(test_file));
}

void onemo_test_create_file(unsigned char *str)
{
    int fd;
    int ret;
    fd = onemo_fs_fopen(test_file,O_CREAT|O_RDWR);
    if(fd < 0)
    {
        onemo_printf("[FWRITE]file open failed\n");
        return;
    }
    ret = onemo_fs_fwrite(fd,str,strlen(str));
    if(ret <0)
    {
        onemo_printf("[FWRITE]file write failed\n");
        return;
    }
    onemo_printf("[FWRITE]file write ok\n");
    onemo_fs_fflush(fd);
    onemo_fs_fclose(fd);    
}

void onemo_test_read_file(unsigned char **cmd,int len)
{
    int fd;
    int ret;
    unsigned char buf[100] = {0};
    fd = onemo_fs_fopen(test_file,O_RDWR);
    if(fd < 0)
    {
        onemo_printf("[FREAD]file open failed");
        return;
    }
    ret = onemo_fs_fread(fd,buf,20);
    if(ret <0)
    {
        onemo_printf("[FREAD]file read failed\n");
        return;
    }
    onemo_printf("[FREAD]%s\n",buf);
    onemo_fs_fclose(fd);    
    
}

void onemo_test_mkdir()
{
    onemo_fs_mkdir(test_dir);
}
void onemo_test_rmdir()
{
    onemo_fs_rmdir(test_dir);
}

void onemo_test_write_file(unsigned char **cmd,int len)
{
    onemo_test_mkdir();
    onemo_test_create_file(cmd[2]);
}
