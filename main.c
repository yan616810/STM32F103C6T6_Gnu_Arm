// /**
//  * @file main.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2025-08-18
//  * 
//  * @copyright Copyright (c) 2025
//  * 
//  */
// #include "stm32f10x.h"
// #include <stdlib.h>

// int main(void)
// {
//     uint32_t *ptr = (uint32_t *)malloc(100*sizeof(ptr));
//     ptr[0] = 1000000; // 设置ptr指向的内存区域的第一个元素为1000000
//     ptr[1] = 0; // 设置ptr指向的内存区域的第二个元素为0
//     // memset(ptr, 6, 100);
    
//     // 配置GPIO
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//     {

//         GPIO_InitTypeDef GPIO_InitStructure;
//         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
//         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//         GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//         GPIO_Init(GPIOC, &GPIO_InitStructure);
//     }
//     // GPIOC->ODR ^= GPIO_Pin_13;
//     // for(;;);
//     for(;;)
//     {
//           // 切换LED状态
//         GPIOC->ODR ^= GPIO_Pin_13;
//         for (uint32_t delay = 0; delay < ptr[0]; delay++);
//         ptr[1]+=1;
//     }
// }


#include "stm32f10x.h"                  // Device header

/*4位数码管*/
// #include "smg.h"

/*tick延时*/
#include "Delay.h"

/*自写按键*/
#include "key.h"

/*内部备份RAM + 引脚侵入检测*/
// #include "BKP.h"

/*内部实时时钟*/
// #include "RTC.h"

/*printf*/
#include "USART.h"
#include <stdio.h>

/*IIC 搜索挂载软件IIC上的设备*/
#include "iic.h"

// /*单色屏幕*/
// #include "u8g2.h"
// #include "u8g2_monochrome_display.h"
// #include <string.h>//memset
// // #include "OLED.h"

// /*旋转编码器*/
// #include "Encoder.h"

// /*mpu6050*/
// #include "mpu6050_dmp.h"
// // #include "inv_mpu.h"
// // #include "mpu6050.h"

// /*flash W25q128*/
// #include "w25q128.h"
// #include "driver_w25qxx_basic.h"

// /*环形缓冲区*/
// #include "ringbuff.h"
// #include "lwrb.h"

// /*DMA + USART1 + LWRB + LWGPS*/
// #include "DMA.h"
// #include "lwgps.h"

// //LCD
// #include "LCD.h"


// //DMP库，fifo容量有限，尝试最佳时间，避免溢出或读取过快
// #define dmp_get_fifo  10//10ms读一次mpu6050 fifo

// volatile uint16_t second_cnt=1000;
volatile uint8_t key_cnt=10;
// volatile uint8_t ms_cnt=100;
// volatile uint8_t ms_25_cnt=dmp_get_fifo;

// /*RTC*/
// Type_Struct_Timezone_and_UTCxTime RTC_Init_And_Adjustment,RTC_read_RTCStruct;
// //u8g2
// u8g2_t u8g2;
// char u8g2_buf[18];
// //秒计数器
// uint16_t second;
// //smg
// uint8_t smg_ui_root;
// //编码器
// int16_t encoder_value;
// //mpu6050 DMP库模式传输至匿名上位机
// int dmp_result;
// float pitch,roll,yaw;
// char niming[13]={0xaa,0xff,0x03,7,};
// int16_t roll_temp;
// int16_t pitch_temp;
// int16_t yaw_temp;
// uint8_t sumcheck; 
// uint8_t addcheck; 
// //不使用DMP库
// short gx,gy,gz,ax,ay,az;
// //flash
// uint8_t flash_buf[1024];
// //自写ringbuff.c创建一个环形缓冲区句柄rb_1
// RING_BUF_DEF(rb_1,10);
// //LWRB
// lwrb_t buff_1;
// uint8_t buffdata_1[1024];//大于等于DMA缓冲区4倍
// //LWGPS
// lwgps_t lwgps_handle;
// // lwgps_float_t latitude, longitude, altitude;

// /************************tamp var************************ */
// uint8_t earth_flag=1;//全球缩略图标志位

// void timer1_init(void)
// {
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
// 	TIM_InternalClockConfig(TIM1);
	
// 	{
// 		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		
// 		TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
		
// 		TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
// 		TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
// 		TIM_TimeBaseInitStruct.TIM_Period=65536-1;//0us - 65536*8us=524.288ms,两帧
// 		TIM_TimeBaseInitStruct.TIM_Prescaler=576-1;//8us
// 		TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
// 	}
	
// 	TIM_Cmd(TIM1,ENABLE);
// }

void timer2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		
		TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
		
		TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period=1000-1;
		TIM_TimeBaseInitStruct.TIM_Prescaler=72-1;
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	}
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	{
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=15;//最低优先级
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
		NVIC_Init(&NVIC_InitStruct);
	}
	TIM_Cmd(TIM2,ENABLE);
	
}

void TIM2_IRQHandler(void)//1ms
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		// if(second_cnt<1000)second_cnt++;
		// if(ms_cnt<100)ms_cnt++;
		if(key_cnt<10)key_cnt++;
		// if(ms_25_cnt<dmp_get_fifo)ms_25_cnt++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
/**
 * @brief 按键处理任务
 * 
 */
void key_task()
{
	if(key_value=='L')//长按B13
	{
		key_value=0;
		GPIOC->ODR ^= GPIO_Pin_13;
		Delay_ms(500);
		GPIOC->ODR ^= GPIO_Pin_13;
	}
	else if(key_value=='D')//双击B13
	{
		key_value=0;
	//pc13 led
		GPIOC->ODR ^= GPIO_Pin_13;
	}
	else if (key_value==1)//单击B13
	{
		key_value=0;
	//自己写的软件SPI驱动W2Q128
		// memset(flash_buf,0x00,sizeof(flash_buf));
		// W25Q128_ReadData(0x001100,flash_buf,256);//读第一块的第二扇区的第二页
		// usart2_send_Hex(flash_buf,256);
		// //Sector erase
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,0*7,4*10,128,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,0*7,4*10,"Sector erase ing···");
		// u8g2_SendBuffer(&u8g2);
		// if(W25Q128_SectorErase(0x001100)==0)
		// {
		// 	printf("Sector erase %s!\r\n","ok");
		// 	u8g2_SetDrawColor(&u8g2,0);
		// 	u8g2_DrawBox(&u8g2,0*7,4*10,128,10);
		// 	u8g2_SetDrawColor(&u8g2,1);
		// 	u8g2_DrawStr(&u8g2,0*7,4*10,"Sector Erase Finish!");
		// }
		// else{
		// 	printf("Sector erase %s!\r\n","error");
		// 	u8g2_SetDrawColor(&u8g2,0);
		// 	u8g2_DrawBox(&u8g2,0*7,4*10,128,10);
		// 	u8g2_SetDrawColor(&u8g2,1);
		// 	u8g2_DrawStr(&u8g2,0*7,4*10,"Sector Erase fail!");
		// }
		// u8g2_SendBuffer(&u8g2);

		// memset(flash_buf,0x00,sizeof(flash_buf));
		// W25Q128_ReadData(0x001100,flash_buf,256);//读第一块的第二扇区的第二页
		// usart2_send_Hex(flash_buf,256);
	//ringbuff 写
		// static uint8_t data='b';
		// rb_write(&rb_1,data++);
	//2025/6/25 gps earth QGIS
		// earth_flag=(earth_flag==0)?1:0;
		// if(earth_flag == 1)
		// {
		// 	u8g2_oled_draw_earth(&u8g2);
		// 	u8g2_SendBuffer(&u8g2);
		// }
	}
	else if (key_value==2)//单击B12
	{
		key_value=0;
	//ringbuff 读
		// uint8_t data='A';
		// rb_read(&rb_1,&data);
		// printf("%c\r\n",data);
	}
}

// void gps_task(void)
// {
// 	uint8_t temp_buf[128];//建议temp_buf大小与DMA缓冲区一致
//     size_t len;

//     // 尝试从环形缓冲区读取数据
//     while ((len = lwrb_read(&buff_1, temp_buf, sizeof(temp_buf))) > 0) {
//         // 送入LWGPS解析
//         lwgps_process(&lwgps_handle, temp_buf, len);//lwgps_process()本身是流式解析，处理速度很快。
//     }
// }

void task_proc(void)
{
	// /*LWGPS解析DMA.c接收到缓冲区中的数据*/
	// gps_task();

	// if(second_cnt == 1000)//1s
	// {
	// 	second_cnt=0;
	// 	if (lwgps_handle.is_valid) 
	// 	{
	// 		if(earth_flag == 0)//earth_flag==1时代表以全球缩略图的形式显示实时坐标
	// 		{
	// 			// printf("[--YLAD--]\r\n");

	// 			// 可选：解析结果输出
    // 			// if (lwgps_handle.is_valid) {
    // 			    // printf("Lat: %.6f, Lon: %.6f, Alt: %.4f\r\n",
    // 			    //     latitude, longitude, altitude);
    // 			// }
	// 				u8g2_ClearBuffer(&u8g2);
	// 			memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 			sprintf(u8g2_buf,"[>Lat:%03.7f<]",lwgps_handle.latitude);
	// 			u8g2_SetDrawColor(&u8g2,0);
	// 			u8g2_DrawBox(&u8g2,0*7,2*10,18*7,10);
	// 			u8g2_SetDrawColor(&u8g2,1);
	// 			u8g2_DrawStr(&u8g2,0*7,2*10,u8g2_buf);

	// 			memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 			sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.longitude);
	// 			u8g2_SetDrawColor(&u8g2,0);
	// 			u8g2_DrawBox(&u8g2,0*7,3*10,18*7,10);
	// 			u8g2_SetDrawColor(&u8g2,1);
	// 			u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);

	// 			memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 			sprintf(u8g2_buf,"[>Alt:%03.5f<]", lwgps_handle.altitude);
	// 			u8g2_SetDrawColor(&u8g2,0);
	// 			u8g2_DrawBox(&u8g2,0*7,4*10,18*7,10);
	// 			u8g2_SetDrawColor(&u8g2,1);
	// 			u8g2_DrawStr(&u8g2,0*7,4*10,u8g2_buf);

	// 			u8g2_SendBuffer(&u8g2);
	// 		}
	// 		else{
	// 			//全球缩略图
	// 			u8g2_ClearBuffer(&u8g2);
	// 			u8g2_oled_draw_earth(&u8g2);//在全幅缓冲区内绘制全球缩略图
	// 			u8g2_oled_draw_earth_pixel_VHxvLine(&u8g2,lwgps_handle.latitude,lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
	// 			u8g2_SendBuffer(&u8g2);
	// 		}
				
	// 	}
	// 	else
	// 	{
	// 		// u8g2_ClearBuffer(&u8g2);
	// 		memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 		sprintf(u8g2_buf,"[>GPS No Data<]");
	// 		u8g2_SetDrawColor(&u8g2,1);
	// 		u8g2_DrawBox(&u8g2,3*7,27,13*7,15);
	// 		u8g2_SetDrawColor(&u8g2,0);
	// 		u8g2_DrawStr(&u8g2,3*7,3*10,u8g2_buf);
	// 		u8g2_SendBuffer(&u8g2);
	// 	}
	// }
	if(key_cnt==10)//10ms
	{
		key_cnt=0;
		get_key();
		if(key_value != 0)
		{
			key_task();
		}
	}
	// if(ms_cnt==100)//100ms
	// {
	// 	ms_cnt=0;
//mpu6050
		// // 将上一次画布缓冲区角速度旧值区域填充空Box
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,2*7,0*10,6*7,10);
		// u8g2_DrawBox(&u8g2,2*7,1*10,6*7,10);
		// u8g2_DrawBox(&u8g2,2*7,2*10,6*7,10);
		// // 在缓冲区空Box处写入角速度数据
		// u8g2_SetDrawColor(&u8g2,1);
		// memset(u8g2_buf, 0, sizeof(u8g2_buf));  // 将数组所有字节设为0
		// sprintf(u8g2_buf,"%+6d",gx);
		// u8g2_DrawStr(&u8g2,2*7,0*10,u8g2_buf);
		// memset(u8g2_buf,0,sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%+6d",gy);
		// u8g2_DrawStr(&u8g2,2*7,1*10,u8g2_buf);
		// memset(u8g2_buf,0,sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%+6d",gz);
		// u8g2_DrawStr(&u8g2,2*7,2*10,u8g2_buf);
		// // 将上一次画布缓冲区重力加速度旧值区域填充空Box
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,11*7,0*10,6*7,10);
		// u8g2_DrawBox(&u8g2,11*7,1*10,6*7,10);
		// u8g2_DrawBox(&u8g2,11*7,2*10,6*7,10);
		// // 在缓冲区空Box处写入重力加速度数据
		// u8g2_SetDrawColor(&u8g2,1);
		// memset(u8g2_buf, 0, sizeof(u8g2_buf));  // 将数组所有字节设为0
		// sprintf(u8g2_buf,"%+6d",ax);
		// u8g2_DrawStr(&u8g2,11*7,0*10,u8g2_buf);
		// memset(u8g2_buf,0,sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%+6d",ay);
		// u8g2_DrawStr(&u8g2,11*7,1*10,u8g2_buf);
		// memset(u8g2_buf,0,sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%+6d",az);
		// u8g2_DrawStr(&u8g2,11*7,2*10,u8g2_buf);
//RTC
		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%4d",RTC_read_RTCStruct.UTCxTime.tm_year);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,0*10,4*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,0*10,u8g2_buf);

		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%2d",RTC_read_RTCStruct.UTCxTime.tm_mon);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,1*10,2*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,1*10,u8g2_buf);

		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%2d",RTC_read_RTCStruct.UTCxTime.tm_mday);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,2*10,2*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,2*10,u8g2_buf);		

		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%02d",RTC_read_RTCStruct.UTCxTime.tm_hour);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,3*10,2*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,3*10,u8g2_buf);

		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%02d",RTC_read_RTCStruct.UTCxTime.tm_min);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,4*10,2*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,4*10,u8g2_buf);

		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%02d",RTC_read_RTCStruct.UTCxTime.tm_sec);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,5*7,5*10,2*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,5*7,5*10,u8g2_buf);
		// //时间戳
		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%10lu",RTC_read_Timestamp());
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,8*7,5*10,10*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,8*7,5*10,u8g2_buf);
//编码器值
		// memset(u8g2_buf, 0, sizeof(u8g2_buf));
		// sprintf(u8g2_buf,"%+06d",encoder_value);
		// u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawBox(&u8g2,8*7,4*10,6*7,10);
		// u8g2_SetDrawColor(&u8g2,1);
		// u8g2_DrawStr(&u8g2,8*7,4*10,u8g2_buf);

		// u8g2_SendBuffer(&u8g2);
	// }
	// if(ms_25_cnt==dmp_get_fifo)
	// {
	// 	ms_25_cnt=0;
	//MPU6050中DMP数据
		// if(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0){/*printf("error\r\n");*/}//返回值:0,DMP成功解出欧拉角   
    	// // else printf("pitch->%f\troll->%f\tyaw->%f\r\n",pitch,roll,yaw);
		// else{
		// 	roll_temp = roll*100;
		// 	pitch_temp = pitch*100;
		// 	yaw_temp = yaw*100;
		// 	sumcheck = 0; 
		// 	addcheck = 0; 
		// 	niming[4]=(uint8_t)(roll_temp);
		// 	niming[5]=(uint8_t)(roll_temp>>8);
		// 	niming[6]=(uint8_t)(pitch_temp);
		// 	niming[7]=(uint8_t)(pitch_temp>>8);
		// 	niming[8]=(uint8_t)(yaw_temp);
		// 	niming[9]=(uint8_t)(yaw_temp>>8);
		// 	niming[10]=0x01;
		// 	for(uint8_t i=0; i < (niming[3]+4); i++) 
		// 	{ 
		// 		sumcheck += niming[i];
		// 		addcheck += sumcheck;  
		// 	}
		// 	niming[11]=sumcheck;
		// 	niming[12]=addcheck;
		// 	for(uint8_t i=0;i<13;i++)
		// 	{
		// 		usart2_send_Char(niming[i]);
		// 	}
		// }
	//RTC实时时钟
		// RTC_get_DataStruct(&RTC_read_RTCStruct,&RTC_Init_And_Adjustment);
	//编码器
		// encoder_value = Encoder_get_value();
	// }
}

int main(void)
{
//配置PC13-led
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	{
	    GPIO_InitTypeDef GPIO_InitStructure;
	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	    GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//点亮LED
/*编码器*/
	// Encoder_init();
/*按键初始化*/
	key_init();
/*串口初始化*/
	usart2_init();
/*IIC协议端口初始化 && 以极低的协议速度搜索iic设备并通过串口打印地址信息*/
	// IIC_InitPins_or_ChangePins(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_8,RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_9);
	// IIC_Set_speed(10);//防止iic协议速度过快，搜索不到低速设备
	// IIC_Search_all_devices_printf_example();
	// IIC_Set_speed(1);
/*oled初始化*/
	// u8g2_oled_init(&u8g2);
	// //u8g2_oled_play_Animation(&u8g2);
	// u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);//w=7  h=10
	// u8g2_SetFontPosTop(&u8g2);
	// u8g2_SetFontMode(&u8g2,0);//显示字体的背景，不透明
	// u8g2_SetDrawColor(&u8g2,1);
	// u8g2_ClearDisplay(&u8g2);

	// // u8g2_DrawStr(&u8g2,0,0*10,"X=");
	// // u8g2_DrawStr(&u8g2,0,1*10,"Y=");
	// // u8g2_DrawStr(&u8g2,0,2*10,"Z=");
	// // u8g2_DrawStr(&u8g2,0,3*10,"Gyro");
	// // u8g2_DrawStr(&u8g2,9*7,0*10,"X=");
	// // u8g2_DrawStr(&u8g2,9*7,1*10,"Y=");
	// // u8g2_DrawStr(&u8g2,9*7,2*10,"Z=");
	// // u8g2_DrawStr(&u8g2,9*7,3*10,"Acce");

	// // u8g2_SendBuffer(&u8g2);
//LCD
	// timer1_init();//测帧率时间

	// LCD_Init_All();
	// LCD_Clear(BLACK);
	// LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花
	// LCD_FillRect(0,50,85,150,0xc88c);
	// LCD_FillRect(50,100,120,200,0x57f6);
	// LCD_DrawLine(0,0,200,200,0x0000);//画一条黑色斜线
	// while(1)
	// {
	// 	uint32_t ms=0;//清零帧率计数器

	// 	LCD_Clear(BLACK);
	// 	LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花

	// 	TIM1->CNT=0;//清零计数器
	// 	LCD_FillRect(0,0,239,319,BLUE);
	// 	// LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花
	// 	ms += (TIM1->CNT * 8)/1000.0;//计算帧率

	// 	TIM1->CNT=0;//清零计数器
	// 	LCD_FillRect(0,0,239,319,BRED);
	// 	// LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花
	// 	ms += (TIM1->CNT * 8)/1000.0;//计算帧率

	// 	LCD_FillRect(0,0,239,319,GRED);
	// 	LCD_FillRect(0,0,239,319,RED);
	// 	LCD_FillRect(0,0,239,319,MAGENTA);
		
	// 	TIM1->CNT=0;//清零计数器
	// 	LCD_FillRect(0,0,239,319,GREEN);
	// 	// LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花
	// 	ms += (TIM1->CNT * 8)/1000.0;//计算帧率

	// 	LCD_FillRect(0,0,239,319,CYAN);

	// 	TIM1->CNT=0;//清零计数器
	// 	LCD_FillRect(0,0,239,319,YELLOW);
	// 	// LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);//显示雪花
	// 	ms += (TIM1->CNT * 8)/1000.0;//计算帧率

	// 	LCD_FillRect(0,0,239,319,BROWN);
	// 	LCD_FillRect(0,0,239,319,BRRED);
	// 	LCD_FillRect(0,0,239,319,GRAY);
	// 	LCD_FillRect(0,0,239,319,GRAY25);

	// 	memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 	sprintf(u8g2_buf,"FPS:%dms/picture",ms>>2);//刷新4个图片取平均
	// 	u8g2_SetDrawColor(&u8g2,0);
	// 	u8g2_DrawBox(&u8g2,0,0,128,10);//清除上次的FPS显示
	// 	u8g2_SetDrawColor(&u8g2,1);
	// 	u8g2_DrawStr(&u8g2,0,0,u8g2_buf);//显示FPS

	// 	memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// 	sprintf(u8g2_buf,"%4.1fFPS",1000.0/(ms>>2));//刷新4个图片取平均
	// 	u8g2_SetDrawColor(&u8g2,0);
	// 	u8g2_DrawBox(&u8g2,0,20,128,20);//清除上次的FPS显示
	// 	u8g2_SetDrawColor(&u8g2,1);
	// 	u8g2_DrawStrX2(&u8g2,0,20,u8g2_buf);//显示FPS

	// 	u8g2_SendBuffer(&u8g2);//发送缓冲区内容到屏幕
	// }
/*片上RTC时钟初始化*/
	// u8g2_DrawStr(&u8g2,0,0,"RTC initing...");
	// RTC_Filling_DataStruct(&RTC_Init_And_Adjustment,8,2025,5,20,11,25,0,-1);
	// RTC_init(&RTC_Init_And_Adjustment);
	// u8g2_ClearBuffer(&u8g2);
	// u8g2_DrawStr(&u8g2,0,0,"year->");
	// u8g2_DrawStr(&u8g2,0,1*10,"mon ->");
	// u8g2_DrawStr(&u8g2,0,2*10,"day ->");
	// u8g2_DrawStr(&u8g2,0,3*10,"hour->");
	// u8g2_DrawStr(&u8g2,0,4*10,"min ->");
	// u8g2_DrawStr(&u8g2,0,5*10,"sec ->");
	// u8g2_SendBuffer(&u8g2);
/*mpu6050初始化*/
	// dmp_result = MPU6050_Init_UseDmp();
	// if(dmp_result != 0)
	// {//有错误
	// 	printf("DMP Init error->%d\r\n",dmp_result);
	// 	Delay_ms(500);//在获取fifo数据时，若获取失败，不要延时，避免fifo溢出
	// }else{
	// 	printf("DMP Init Success!\r\n");
	// }
//OV7670

//W25Q128 Flash
	// W25Q128_Init();
	// uint8_t MID;uint16_t DID;
	// W25Q128_Read_id(&MID,&DID);
	// //MID
	// memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// sprintf(u8g2_buf,"Flash MID:0x%x",MID);
	// u8g2_DrawStr(&u8g2,0*7,2*10,u8g2_buf);
	// //DID
	// memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// sprintf(u8g2_buf,"Flash DID:0x%x",DID);
	// u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);
	// u8g2_SendBuffer(&u8g2);
	// //Chip Erase
	// u8g2_DrawStr(&u8g2,0,4*10,"Chip erase ing···");
	// u8g2_SendBuffer(&u8g2);

	// printf("Chip erase %s!\r\n",W25Q128_ChipErase()==0?"ok":"error");
	// u8g2_SetDrawColor(&u8g2,0);
	// u8g2_DrawBox(&u8g2,0*7,4*10,128,10);
	// u8g2_SetDrawColor(&u8g2,1);
	// u8g2_DrawStr(&u8g2,0*7,4*10,"Chip erase finish!");
	// u8g2_SendBuffer(&u8g2);
	// // W/R Data
	// memset(flash_buf,0x00,sizeof(flash_buf));
	// W25Q128_ReadData(0x001100,flash_buf,256);//读第一块的第二扇区的第二页
	// usart2_send_Hex(flash_buf,256);

	// memset(flash_buf,0x00,sizeof(flash_buf));
	// for(uint16_t i=0;i<256;i++)
	// {
	// 	flash_buf[i]=i;
	// }
	// W25Q128_PageProgram(0x001100,flash_buf,256);//写第一块的第二扇区的第二页

	// memset(flash_buf,0x00,sizeof(flash_buf));
	// W25Q128_ReadData(0x001100,flash_buf,256);//读第一块的第二扇区的第二页
	// usart2_send_Hex(flash_buf,256);

//Libdriver W25Qxxx
	// w25qxx_basic_init(W25Q128,W25QXX_INTERFACE_SPI,W25QXX_BOOL_FALSE);
	// uint8_t MID;uint16_t DID;
	// w25qxx_basic_get_jedec_id(&MID,&DID);
	// //MID
	// memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// sprintf(u8g2_buf,"Flash MID:0x%x",MID);
	// u8g2_DrawStr(&u8g2,0*7,0*10,u8g2_buf);
	// //DID
	// memset(u8g2_buf, 0, sizeof(u8g2_buf));
	// sprintf(u8g2_buf,"Flash DID:0x%x",DID);
	// u8g2_DrawStr(&u8g2,0*7,1*10,u8g2_buf);
	// u8g2_SendBuffer(&u8g2);
//LWGPS
	// lwrb_init(&buff_1, buffdata_1,sizeof(buffdata_1));  //初始化DMA双缓冲使用到的环形缓冲区
	// DMA_link_lwrb_t(&buff_1);                            //链接外部定义的环形缓冲区,供DMA.c使用
	// DMA_usart1_to_arrybuffer_init();                     //配置usart1 + 使用DMA1的通道5，通过双缓冲机制
	// /*执行到这里是DMA已经可以自动从usart1接收数据并自动拷贝到LWRB的环形缓冲区中*/
	// printf("DMA usart1 to lwrb init success!\r\n");

//ringbuff 写
	// rb_write(&rb_1,'a');
//LWRB
	// lwrb_init(&buff_1, buffdata_1, sizeof(buffdata_1)); /* Initialize buffer */
	// lwrb_write(&buff_1, "123456789", 5);
	// printf("Bytes in buffer: %d\r\n", (int)lwrb_get_full(&buff_1));
	// /* Now let's read */
	// uint8_t data[8];
	// size_t len;
	// len = lwrb_read(&buff_1, data, 3);
	// printf("Number of bytes read: %d\r\n", (int)len);
	// usart2_send_Hex(data,len);
	// printf("\r\n");

	// memset(data,0,sizeof(data));
	// len = lwrb_read(&buff_1, data, 8);
	// printf("Number of bytes read: %d\r\n", (int)len);
	// usart2_send_Hex(data,len);
	// printf("\r\n");
/*任务滴答*/
	timer2_init();
	printf("system init success!!!\r\n");
	for(;;)
	{
		task_proc();
		rx_data_proc();
	}
}


//用函数指针替代
void rx_data_procCallback(uint16_t cmd_num)//Starting from 0
{
	switch(cmd_num)
	{
		case 0://cmd1
			printf("rx_buff=cmd1 -> Ctrl_1\r\n");
			break;
		case 1://cmd2
			printf("rx_buff=cmd2 -> Ctrl_2\r\n");
			break;
		case 2://cmd3
			printf("rx_buff=cmd3 -> Ctrl_3\r\n");
			break;
		case 3://help
			printf("rx_buff=help -> I will help you!\r\n");
			break;
		case 4://YLAD
			printf("\
[===============YLAD=============]\r\n\
[================================]\r\n\
[*****欢迎来到无人机租赁公司*******]\r\n\
[1.关于公司。。。                 ]\r\n\
[2.立刻注册免费租一架试玩10min!    ]\r\n\
[3.更多功能敬请期待！             ]\r\n\
");
			break;
		default:break;
	}
}



