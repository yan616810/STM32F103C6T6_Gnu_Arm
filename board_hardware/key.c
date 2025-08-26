#include "stm32f10x.h"                  // Device header
uint8_t key_value=0;

void key_init()
{
	RCC->APB2ENR |= 0X08;//使能GPIOB
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;//按键B13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12;//按键B12
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void get_key()
{
	static uint8_t status=0,mid,num=0,double_delay=0;
	static uint16_t long_delay=0;
	switch(status)
	{
		case 0:
			if((GPIOB->IDR & 0x2000)==0)status=1;//B13按键接GND
			if((GPIOB->IDR & 0x1000)==0)status=1;//B12按键接GND
			break;
		case 1:
			if((GPIOB->IDR & 0X2000)==0){mid=1;status=2;}
			else if((GPIOB->IDR & 0X1000)==0){mid=2;status=2;}
			else status=0;
			break;
		case 2:
			if((GPIOB->IDR & 0X2000)==0){
				if(long_delay<200)long_delay++;
				if(long_delay==200)
				{
					long_delay=201;
					key_value='L';
				}
			}
			else if((GPIOB->IDR & 0X1000)==0){}
			else{
				if(long_delay==201){}
				else if(mid==1)num++;
				else key_value=mid;
				status=0;long_delay=0;
			}
			break;
		default:break;
	}
	
	if(num==1)
	{
		if(double_delay<25)double_delay++;
		else{
			num=0;
			double_delay=0;
			key_value=1;
		}
	}
	else if(num==2)
	{
		num=0;
		double_delay=0;
		key_value='D';
	}
}
