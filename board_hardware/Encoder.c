#include "Encoder.h"
#include "stm32f10x.h"

void Encoder_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1,ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct={
        .GPIO_Mode=GPIO_Mode_IPU,
        .GPIO_Pin=GPIO_Pin_8,
        .GPIO_Speed=GPIO_Speed_2MHz
    };
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={
    .TIM_ClockDivision=TIM_CKD_DIV1,
    .TIM_CounterMode=TIM_CounterMode_Up,
    .TIM_Period=0xffff,
    .TIM_Prescaler=0,
    .TIM_RepetitionCounter=0
    };
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
    
    TIM_ICInitTypeDef TIM_ICInitStruct={
        .TIM_Channel=TIM_Channel_1,
        .TIM_ICFilter=0xf,
    };
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInit(TIM1,&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInit(TIM1,&TIM_ICInitStruct);

    TIM_EncoderInterfaceConfig(TIM1,TIM_EncoderMode_TI2,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);

    TIM_Cmd(TIM1,ENABLE);
}


int16_t Encoder_get_value(void)
{
    return (int16_t)TIM_GetCounter(TIM1);
}
