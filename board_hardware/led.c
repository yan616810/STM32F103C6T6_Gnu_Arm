#include "stm32f10x.h"                  // Device header
#include "led.h"
void led_pin_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&GPIO_InitStruct);
	}
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);//Ãð
}

void led_Reversal(void)//led·´×ª
{
	/*RCC->APB2ENR=0x00000010;
	GPIOC->CRH=0x00300000;
	GPIOC->ODR=0x00000000; */

	GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13)==Bit_RESET));

}
