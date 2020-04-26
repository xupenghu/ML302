版本：OneMO_ML302_OpenCPU_SDK_Release_V1.1.0
日期：2019\3\20
1.使用前请阅读ML302 OpenCPU 开发指导手册.pdf
2.用户程序在void ML302_OpenCPU_Entry(void)中添加，此函数运行时操作系统尚未开始调度，请不要阻塞此函数
3.请使用文档提供的接口函数或文档明确指定的头文件包含的接口函数进行开发，如使用除此以外的接口，不能保证软件稳定性。
4.本SDK仅支持在windows下使用

1.1.0版本更新说明：
1.合入最新基线代码。
2.增加TTS功能（离线科大讯飞库），相关功能接口请查看onemo_tts.h文件。
3.增加OneNet FOTA功能，使用方法参考《ML302_OpenCPU_ONENET FOTA用户手册》。启用OneNet FOTA功能时候，ML302_OpenCPU_Entry()函数中onemo_onenet_fota_regcbex()函数必须执行不可删除。
