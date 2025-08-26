/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

#include <stdio.h>
// #include "BKP.h"
// #include "DMA.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// 声明全局标志变量（用于主循环处理）
//volatile uint8_t clock_failure_flag = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
//	// 1. 检查是否CSS中断触发
//    if(RCC_GetITStatus(RCC_IT_CSS) == SET)
//    {
//        // 2. 强制清除CSS中断挂起标志！！！（关键步骤）
//        RCC_ClearITPendingBit(RCC_IT_CSS);

//        // 3. 记录时钟故障标志（供主程序处理）
//        clock_failure_flag = 1;

//        // 4. 立即关闭可能受影响的危险外设（如电机驱动PWM）
//        TIM_Cmd(TIM1, DISABLE);
//        TIM_Cmd(TIM8, DISABLE);

//        // 5. 手动切换系统时钟到HSI（如果硬件未自动完成）
//        if(RCC_GetSYSCLKSource() != RCC_SYSCLKSource_HSI)
//        {
//            RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); // 切换时钟源
//            while(RCC_GetSYSCLKSource() != 0x04);   // 等待切换完成
//        }

//        // 6. 关闭HSE和关联的PLL（防止重复故障）
//        RCC_HSEConfig(RCC_HSE_OFF);
//        RCC_PLLCmd(DISABLE);
//    }
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  // printf("LR: 0x%08X\n", __get_LR());
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @brief 设备外壳被打开后需要立即执行的操作
 * @brief 一般被拆除后的操作，要根据具体情况来写，是直接报废,还是发送报警信息后继续正常运行，
 *        这些都是要根据具体的需求才能继续编写的!!!
 *
 */
// void TAMPER_IRQHandler(void)
// {
//   if(BKP_GetITStatus()==SET)
//   {
// /*****一般被拆除后的操作，要根据具体情况来写，是直接报废还是发送报警信息后继续正常运行，这些都是要根据具体的需求才能继续编写的******/
//     //app_start


//     //app_end

//     /* Clear Tamper pin interrupt pending bit */
//     BKP_ClearITPendingBit();
//     /* Clear Tamper pin Event(TE) pending flag */
//     BKP_ClearFlag();
//     //触发系统复位
//     // NVIC_SystemReset();
//   }
// }

// void USART1_IRQHandler(void)
// {
//   // printf("USART1_IRQHandler\r\n");//调试时使用
//   USART1_IRQHandler_IDLE_callback(); // 处理USART1空闲中断
// }

// void DMA1_Channel5_IRQHandler(void)
// {
//   // printf("DMA1_Channel5_IRQHandler\r\n");//调试时使用
//   DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(); // 处理DMA1通道5接收满中断
// }

/**
  * @}
  */ 


