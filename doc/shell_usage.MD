# ML302 shell使用方法
## 引言
首先感谢letter shell作者开源了简单好用的shell工具，详情可以访问作者的[GitHub](https://github.com/NevermindZZT/letter-shell)获取。

其次感谢rt-thread团队的dbg着色支持，更多rtthread的资料，请访问[官网](https://www.rt-thread.org/)
获取。

## 使用
### 1 需要串口打印log信息
在需要log打印的*.c文件头部包含dbg头文件，如下所示：

```c
#define DBG_TAG "MAIN"
#define DBG_LVL DBG_LOG
#include "shell/dbg.h"
```
然后就可以在当前文件中使用`LOG_D LOG_W LOG_I LOGE`打印不同等级的log，如下：

```c
   LOG_D("\r\nML302 OpenCPU Starts\n");
   LOG_D("Boot Cause:%d\n",onemo_sys_get_boot_cause());
   onemo_sys_get_sdk_swver(buf,50);
   LOG_D("SDK VERSION:%s\n",buf);
   onemo_sys_get_base_swver(buf,50);
   LOG_D("BASELINE VERSION:%s\n",buf);
```
串口打印输出：

```c
letter:/$ [I/MAIN] 
ML302 OpenCPU Starts

[I/MAIN] Boot Cause:2

[I/MAIN] SDK VERSION:V1.1.0

[I/MAIN] BASELINE VERSION:8910_MODULE_V1_W20.11.1_CMIOT

```

如果在调试阶段，推荐将DBG_LVL设置为DBG_LOG 打印所有调试信息，当发布的时候，可以将相应的调试等级调高。

### 2 调试函数
比如我们想使用ML302的虚拟AT功能，则可以定义如下函数在`onemo_vir_at_demo.c`

```c
void onemo_test_send_at(int argc, char *agrv[])
{
    if (argc != 2)
    {
        LOG_E("error, argc = %d, usage: AT+CGSN=1", argc);
    }
    else
    {
        onemo_clear_virt_at_buffer();
        unsigned char p[100] = {0};
        sprintf(p, "%s\r\n", agrv[1]);
        LOG_I("[VIRT_AT]:%d\n", onemo_vir_at_send(p));
    }
}

void onemo_test_dump_at(int argc, char *agrv[])
{
    int i;
    LOG_I("[DUMP_AT]:Start\n");
    for (i = 0; i < virt_at_len; i++)
    {
        onemo_printf("%c", virt_at_buffer[i]);
    }
}
```
然后在`shell_cmd_list.c`中导入刚才的函数，如下：

```c
extern void onemo_test_send_at(int argc, char *agrv[]);
extern void onemo_test_dump_at(int argc, char *agrv[]);
```

```c
        SHELL_CMD_ITEM(
            SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
            cls, shellClear, clear console),
            SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
            send_at, onemo_test_send_at, vir at send),
        SHELL_CMD_ITEM(
            SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
            dump_at, onemo_test_dump_at, at dump),
```

然后编译下载测试：

```c
letter:/$ 
letter:/$ send_at AT+CSQ
[I/VIR_AT] [VIRT_AT]:8


letter:/$ dump_at
[I/VIR_AT] [DUMP_AT]:Start

AT+CSQ

+CSQ: 28,99

OK

letter:/$ 

```

这样就可以方便的做AT测试，其他的demo也可以使用同样的方法来做测试，这里就不再赘述。
