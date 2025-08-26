#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x.h"
#include "lwrb.h"
void DMA_link_lwrb_t(lwrb_t *rb_buff);//链接外部定义的环形缓冲区
void DMA_usart1_to_arrybuffer_init(void);//配置usart1接收 + 配置DMA1的通道5 + 双缓冲机制

void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(void);//DMA1通道5接收满中断处理函数，处理USART1接收数据到双缓冲区
void USART1_IRQHandler_IDLE_callback(void);//USART1空闲中断处理函数（DMA方式，处理未满缓冲区数据）

#endif // !__DMA_H
