/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "stm32f10x.h"
#include <stdlib.h>

int main(void)
{
    uint32_t *ptr = (uint32_t *)malloc(100*sizeof(ptr));
    ptr[0] = 1000000; // 设置ptr指向的内存区域的第一个元素为1000000
    ptr[1] = 0; // 设置ptr指向的内存区域的第二个元素为0
    // memset(ptr, 6, 100);
    
    // 配置GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    {

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    // GPIOC->ODR ^= GPIO_Pin_13;
    // for(;;);
    for(;;)
    {
          // 切换LED状态
        GPIOC->ODR ^= GPIO_Pin_13;
        for (uint32_t delay = 0; delay < ptr[0]; delay++);
        ptr[1]+=1;
    }
}