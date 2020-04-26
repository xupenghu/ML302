/*********************************************************
*  @file    onemo_i2c_demo.c
*  @brief   ML302 OpenCPU i2c example file
*  Copyright (c) 2019 China Mobile IOT.
*  All rights reserved.
*  created by Yu Yansen 2019/11/5
********************************************************/
#include "onemo_main.h"

//存储BMP180数据的结构
typedef struct __BMP180
{
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD;
	long UT;
	long UP;
	long X1;
	long X2;
	long X3;
	long B3;
	unsigned long B4;
	long B5;
	long B6;
	long B7;
	long p;
	long Temp;
	float altitude;
}_bmp180;

_bmp180 bmp180;

void BMP_ReadCalibrationData(void);
long BMP_Read_UT(void);
long BMP_Read_UP(void);
void BMP_UncompemstatedToTrue(void);

#define ACCURACY 100
static double func1(double t,int n)
{
    double answer=1;
    for(int i=0;i<n;i++)
	{
        answer=answer*t;
    }
    return answer;
}
static double func2(double b,int n)
{
    double answer=1;
    for(int i=1;i<=n;i++)
	{
        answer=answer*(b-i+1)/i;
    }
    return answer;
}

double pow2(double a,double b)
{
    if(a==0&&b>0)
	{
        return 0;
    }
    else if(a==0&&b<=0)
	{
        return 1/0;
    }
    else if(a<0&&!(b-(int)b<0.0001||(b-(int)b>0.999)))
	{
        return 1/0;
    }

    if(a<=2&&a>=0)
	{
        double t=a-1;
        double answer=1;
        for(int i=1;i<ACCURACY;i++)
		{
            answer=answer+func1(t,i)*func2(b,i);
        }
        return answer;
    }
    else if(a>2)
	{
        int time=0;
        
        while(a>2)
		{
            a=a/2;
            time++;
        }
        
        return pow2(a,b)*pow2(2,b*time);
    }
    else
	{
        if((int)b%2==0)
		{
            return pow2(-a,b);
        }
        else 
		{
            return -pow2(-a,b);
        }
    }
}

//从BMP180的获取计算参数
void BMP_ReadCalibrationData()
{
	unsigned char temp[3];
	memset(temp,0,3);
	unsigned char reg;

	reg = 0xAA;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC1 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xAC;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC2 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xAE;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC3 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xB0;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC4 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xB2;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC5 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xB4;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.AC6 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xB6;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.B1 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xB8;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.B2 = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xBA;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.MB = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xBC;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.MC = (temp[0]<<8)|temp[1];
	
	memset(temp,0,3);
	reg = 0xBE;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, temp, 2);
	bmp180.MD = (temp[0]<<8)|temp[1];
	
}

//从BMP180读取未修正的温度
long BMP_Read_UT(void)
{
	unsigned char buffer[3];
	memset(buffer,0,3);
	unsigned char reg;

	buffer[0] = 0xF4;
	buffer[1] = 0x2E;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77,buffer,2);	
	vTaskDelay(1);
	memset(buffer,0,3);
	reg = 0xF6;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, buffer, 2);
	return (buffer[0]<<8)|buffer[1];
}
//从BMP180读取未修正的大气压
long BMP_Read_UP(void)
{
	long pressure = 0;
	unsigned char buffer[3];
	memset(buffer,0,3);
	unsigned char reg;
	
	buffer[0] = 0xF4;
	buffer[1] = 0x34;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77,buffer,2);	
	vTaskDelay(1);
	memset(buffer,0,3);
	reg = 0xF6;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, buffer, 2);
	
	return (buffer[0]<<8)|buffer[1];
}

//用获取的参数对温度和大气压进行修正，并计算海拔
void BMP_UncompemstatedToTrue(void)
{
	bmp180.UT = BMP_Read_UT();//第一次读取错误
	bmp180.UT = BMP_Read_UT();//进行第二次读取修正参数
	bmp180.UP = BMP_Read_UP();
	
	bmp180.X1 = ((bmp180.UT - bmp180.AC6) * bmp180.AC5) >> 15;
	bmp180.X2 = (((long)bmp180.MC) << 11) / (bmp180.X1 + bmp180.MD);
	bmp180.B5 = bmp180.X1 + bmp180.X2;
	bmp180.Temp  = (bmp180.B5 + 8) >> 4;
	
	bmp180.B6 = bmp180.B5 - 4000;
	bmp180.X1 = ((long)bmp180.B2 * (bmp180.B6 * bmp180.B6 >> 12)) >> 11;
	bmp180.X2 = ((long)bmp180.AC2) * bmp180.B6 >> 11;
	bmp180.X3 = bmp180.X1 + bmp180.X2;
	
	bmp180.B3 = ((((long)bmp180.AC1) * 4 + bmp180.X3) + 2) /4;
	bmp180.X1 = ((long)bmp180.AC3) * bmp180.B6 >> 13;
	bmp180.X2 = (((long)bmp180.B1) *(bmp180.B6*bmp180.B6 >> 12)) >>16;
	bmp180.X3 = ((bmp180.X1 + bmp180.X2) + 2) >> 2;
	bmp180.B4 = ((long)bmp180.AC4) * (unsigned long)(bmp180.X3 + 32768) >> 15;
	bmp180.B7 = ((unsigned long)bmp180.UP - bmp180.B3) * 50000;
	
	if(bmp180.B7 < 0x80000000)
	{
		bmp180.p = (bmp180.B7 * 2) / bmp180.B4;		
	}
	else
	{
		bmp180.p = (bmp180.B7 / bmp180.B4) * 2;
	}
	
	bmp180.X1 = (bmp180.p >> 8) * (bmp180.p >>8);
	bmp180.X1 = (((long)bmp180.X1) * 3038) >> 16;
	bmp180.X2 = (-7357 * bmp180.p) >> 16;
	
	bmp180.p = bmp180.p + ((bmp180.X1 + bmp180.X2 + 3791) >> 4);
	
	bmp180.altitude = 44330 * (1-pow2(((bmp180.p) / 101325.0),(1.0/5.255)));  
}
int BMP180_get_id()
{
	unsigned char id[3];
	unsigned char reg;
	
	if(0 != onemo_i2c_init(ONEMO_I2C_MASTER_2, ONEMO_I2C_BPS_100K))
	{
		return -1;
	}
	memset(id,0,2);
	reg = 0xD0;
	onemo_i2c_write(ONEMO_I2C_MASTER_2, 0x77, &reg, 1);
	onemo_i2c_read(ONEMO_I2C_MASTER_2, 0x77, id, 1);
	if(id[0] != 0)
	{
		onemo_printf("[I2C]ID:%d\n",id[0]);
		return 0;
	}
	else
	{
		return -1;
	}
}
void onemo_test_i2c_bmp180(unsigned char **cmd,int len)
{
	int i;
	if(BMP180_get_id())
	{
		onemo_printf("[I2C]no bmp180 found!\n");
	}
	BMP_ReadCalibrationData();
	for(i=0;i<10;i++)
	{
		BMP_UncompemstatedToTrue();
		onemo_printf("[I2C]temp = %d.%dC\t   Pressure = %ldPa\t   Altitude = %.5fm\r\n",bmp180.Temp/10,bmp180.Temp%10,bmp180.p,bmp180.altitude);
		vTaskDelay(100);
	}
	onemo_i2c_deinit(ONEMO_I2C_MASTER_2);
}
