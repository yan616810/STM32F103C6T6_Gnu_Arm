/**
 * @file mpu6050.c
 * @author your name (you@domain.com)
 * @brief 大坑：
 * 1.和oled一样，mpu6050上电或软件复位需要延时20ms~100ms，才能读写寄存器，手册P14??"START-UP TIME FOR REGISTER READ/WRITE: 20~100ms"??
 * 2.等上电复位完成后，若想写所有的可写寄存器，必须先向MPU_PWR_MGMT1_REG写数据，进行唤醒;默认睡眠模式，不可写入所有可写寄存器；
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "mpu6050.h"
#include "iic.h"   												  	  
#include "Delay.h"

/**********************************************
函数名称：MPU_Init
函数功能：初始化MPU6050
函数参数：无
函数返回值：0,初始化成功  1,初始化失败
**********************************************/
uint8_t MPU_Init(void)
{ 
	uint8_t res;
	Delay_ms(100);//等待上电复位初始化完成才能读写寄存器
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
	Delay_ms(100);//等待软件复位初始化完成才能读写寄存器，上电立即手动软件复位也没屌用，不等100ms根本就写不进去寄存器
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if(res==MPU_ADDR)								//器件ID正确,即res = MPU_ADDR = 0x68
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//唤醒MPU6050 && 使用x轴时钟,必须先唤醒，否则无法写某些寄存器
		MPU_Set_Gyro_Fsr(0);					//陀螺仪传感器,±250dps
		MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
		MPU_Set_Rate(200);						//设置采样率200Hz，据奈奎斯特，实际能够使用的信号的频谱最大频率为100HZ，再高频的震荡看作噪声由DLPF滤去
		MPU_Write_Byte(MPU_INT_EN_REG,0X00);		//关闭所有中断
		MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
		MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);		//关闭FIFO
		MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
		
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);		//加速度与陀螺仪都工作 
		Delay_ms(30);//陀螺仪从启动到稳定工作所需的时间
 	}
	else return 1;    //地址设置错误,返回1
	return 0;		   //地址设置正确,返回0

	// MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
//   Delay_ms(100);
	// MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	// MPU_Set_Gyro_Fsr(0);										//陀螺仪传感器,±2000dps
	// MPU_Set_Accel_Fsr(0);										//加速度传感器,±2g
	// MPU_Set_Rate(50);												//设置采样率50Hz
	// MPU_Write_Byte(MPU_INT_EN_REG,0X00);		//关闭所有中断
	// MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	// MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);		//关闭FIFO
	// MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	
	// res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	// if(res==MPU_ADDR)												//器件ID正确,即res = MPU_ADDR = 0x68
	// {
		// MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);		//设置CLKSEL,PLL X轴为参考
		// MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);		//加速度与陀螺仪都工作
		// MPU_Set_Rate(50);													//设置采样率为50Hz
 	// }else return 1;    //地址设置错误,返回1
	// return 0;					 //地址设置正确,返回0

}

/**********************************************
函数名称：MPU_Set_Gyro_Fsr
函数功能：设置MPU6050陀螺仪传感器满量程范围
函数参数：fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3); //设置陀螺仪满量程范围
}

/**********************************************
函数名称：MPU_Set_Accel_Fsr
函数功能：设置MPU6050加速度传感器满量程范围
函数参数：fsr:0,±2g;1,±4g;2,±8g;3,±16g
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3); //设置加速度传感器满量程范围  
}

/**********************************************
函数名称：MPU_Set_LPF
函数功能：设置MPU6050的数字低通滤波器
函数参数：lpf:数字低通滤波频率(Hz)
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;//截止频率为188HZ的DLPF
	else if(lpf>=98)data=2;//98HZ~188HZ，//截止频率为98HZ的DLPF
	else if(lpf>=42)data=3;//42HZ~98HZ，//截止频率为42HZ的DLPF
	else if(lpf>=20)data=4;//20HZ~42HZ区间的用户姿态信号，都使用截止频率为20HZ的DLPF
	else if(lpf>=10)data=5;//截止频率为10HZ的DLPF
	else data=6;//截止频率为5HZ的DLPF
	return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}

/**********************************************
函数名称：MPU_Set_Rate
函数功能：设置MPU6050的采样率(假定Fs=1KHz)
函数参数：rate:4~1000(Hz)  初始化中rate取50
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;//1kHZ/256分频=3.91HZ
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);						//自动设置LPF为采样率的一半，奈奎斯特
}

/**********************************************
函数名称：MPU_Get_Temperature
函数功能：得到温度传感器值
函数参数：无
函数返回值：温度值(扩大了100倍)
**********************************************/
short MPU_Get_Temperature(void)
{
   uint8_t buf[2]; 
   short raw;
	 float temp;
	 MPU_Read_Len(MPU_TEMP_OUTH_REG,2,buf); 
   raw=((uint16_t)buf[0]<<8)|buf[1];
   temp=36.53+((double)raw)/340;//35+521/340=36.5324
   return temp*100;
}

/**********************************************
函数名称：MPU_Get_Gyroscope
函数功能：得到陀螺仪值(原始值)
函数参数：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
  uint8_t buf[6],res;
	res=MPU_Read_Len(MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];
		*gy=((uint16_t)buf[2]<<8)|buf[3];
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
  return res;
}

/**********************************************
函数名称：MPU_Get_Accelerometer
函数功能：得到加速度值(原始值)
函数参数：ax,ay,az:加速度传感器x,y,z轴的原始读数(带符号)
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;
}

/**********************************************
函数名称：MPU_Write_Len
函数功能：IIC连续写(写器件地址、寄存器地址、数据)
函数参数：reg:寄存器地址
				 len:写入数据的长度  buf:数据区
函数返回值：0,写入成功  其他,写入失败
**********************************************/
uint8_t MPU_Write_Len(uint8_t reg,uint8_t len,uint8_t *buf)
{
	return IIC_Write_Len(MPU_ADDR,reg,len,buf);
}

/**********************************************
函数名称：MPU_Read_Len
函数功能：IIC连续读(写入器件地址后,读寄存器地址、数据)
函数参数：reg:要读的寄存器地址
		len:要读取的数据长度  buf:读取到的数据存储区
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Read_Len(uint8_t reg,uint8_t len,uint8_t *buf)
{
	return IIC_Read_Len(MPU_ADDR,reg,len,buf);
}

/**********************************************
函数名称：MPU_Write_Byte
函数功能：IIC写一个字节
函数参数：data:写入的数据    reg:要写的寄存器地址
函数返回值：0,写入成功  其他,写入失败
**********************************************/
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data)
{
	return IIC_Write_Byte(MPU_ADDR,reg,data);
}

/**********************************************
函数名称：MPU_Read_Byte
函数功能：IIC读一个字节
函数参数：reg:要读的寄存器地址
函数返回值：res:读取到的数据
**********************************************/
uint8_t MPU_Read_Byte(uint8_t reg)
{
	return IIC_Read_Byte(MPU_ADDR,reg);
}
