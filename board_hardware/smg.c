#include "stm32f10x.h"                  // Device header
#include "smg.h"

volatile unsigned char smg_buff[3]={0x3f,0x3f,0x3f};//显存；volatile无视编译器优化，可直接访问显存地址中的数据，是为了防止在中断中修改显存时，编译器会优化访问显存程序；

//不规则端口驱动数码管
#define smg_PA6(x) GPIO_WriteBit(GPIOA,GPIO_Pin_6,(BitAction)x)//位定义，共阴极公共端
#define smg_PA5(x) GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)x)
#define smg_PA4(x) GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)x)
																									
#define smg_PA0(x) GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)x)//段
//#define smg_PB11(x) GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)x)
#define smg_PA1(x) GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)x)
#define smg_PA2(x) GPIO_WriteBit(GPIOA,GPIO_Pin_2,(BitAction)x)
#define smg_PA3(x) GPIO_WriteBit(GPIOA,GPIO_Pin_3,(BitAction)x)
#define smg_PA7(x) GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)x)
#define smg_PB0(x) GPIO_WriteBit(GPIOB,GPIO_Pin_0,(BitAction)x)
#define smg_PB1(x) GPIO_WriteBit(GPIOB,GPIO_Pin_1,(BitAction)x)
#define smg_PB10(x) GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)x)
const unsigned char smg_code[]=//阴极
{
	0x3f,//0
	0x06,//1
	0x5b,//2
	0x4f,//3
	0x66,//4
	0x6d,//5
	0x7d,//6
	0x07,//7
	0x7f,//8
	0x6f,//9
	0x77,//A
	0x7c,//b
	0x39,//C
	0x5e,//d
	0x79,//E
	0x71,//F
	0x76,//H
	0x38,//L
	0x73,//P
	0x3e,//U
	0x6e,//y
	0x31,//r
	0x40,//-
	0x80,//.
	0xff,//8.
	0x00,//灭
};

void smg_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=
														GPIO_Pin_0|
														GPIO_Pin_1|
														GPIO_Pin_2|
														GPIO_Pin_3|
														GPIO_Pin_4|
														GPIO_Pin_5|
														GPIO_Pin_6|
														GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=
														//GPIO_Pin_11|
														GPIO_Pin_0|
														GPIO_Pin_1|
														GPIO_Pin_10;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
}
static void date_port(unsigned char date)
{
	uint8_t num=0x01;
	smg_PA0((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1; //显示几个中断时间后关闭该位
	//smg_PB11((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PA1((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PA2((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PA3((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PA7((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PB0((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PB1((((date&num)==0)?Bit_RESET:Bit_SET));num<<=1;
	smg_PB10((((date&num)==0)?Bit_RESET:Bit_SET));
}


void smg_scanf(void)
{
	static u8 i=0;
	switch(i)
	{
		case 0:
		{
			smg_PA4(1);//关上一位
			date_port(*smg_buff);
			smg_PA6(0);//开下一位
		}break;
		case 1:
		{
			smg_PA6(1);//关上一位
			date_port(*(smg_buff+1));
			smg_PA5(0);//开下一位
		}break;
		case 2:
		{
			smg_PA5(1);//关上一位
			date_port(*(smg_buff+2));
			smg_PA4(0);//开下一位
		}break;
		default:break;
	}
	i++;
	if(i==3)i=0;
}

//void smg_scanf(unsigned char *smg_date)
//{
//	static u8 i=0;
//	switch(i)
//	{
//		case 0:
//		{
//			smg_PA4(1);//关上一位
//			date_port(*smg_date);
//			smg_PA6(0);//开下一位
//		}break;
//		case 1:
//		{
//			smg_PA6(1);//关上一位
//			date_port(*(smg_date+1));
//			smg_PA5(0);//开下一位
//		}break;
//		case 2:
//		{
//			smg_PA5(1);//关上一位
//			date_port(*(smg_date+2));
//			smg_PA4(0);//开下一位
//		}break;
//		default:break;
//	}
//	i++;
//	if(i==3)i=0;
//}
