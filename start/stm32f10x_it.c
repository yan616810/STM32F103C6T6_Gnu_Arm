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
// ����ȫ�ֱ�־������������ѭ������
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
//	// 1. ����Ƿ�CSS�жϴ���
//    if(RCC_GetITStatus(RCC_IT_CSS) == SET)
//    {
//        // 2. ǿ�����CSS�жϹ����־���������ؼ����裩
//        RCC_ClearITPendingBit(RCC_IT_CSS);

//        // 3. ��¼ʱ�ӹ��ϱ�־������������
//        clock_failure_flag = 1;

//        // 4. �����رտ�����Ӱ���Σ�����裨��������PWM��
//        TIM_Cmd(TIM1, DISABLE);
//        TIM_Cmd(TIM8, DISABLE);

//        // 5. �ֶ��л�ϵͳʱ�ӵ�HSI�����Ӳ��δ�Զ���ɣ�
//        if(RCC_GetSYSCLKSource() != RCC_SYSCLKSource_HSI)
//        {
//            RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); // �л�ʱ��Դ
//            while(RCC_GetSYSCLKSource() != 0x04);   // �ȴ��л����
//        }

//        // 6. �ر�HSE�͹�����PLL����ֹ�ظ����ϣ�
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
 * @brief �豸��Ǳ��򿪺���Ҫ����ִ�еĲ���
 * @brief һ�㱻�����Ĳ�����Ҫ���ݾ��������д����ֱ�ӱ���,���Ƿ��ͱ�����Ϣ������������У�
 *        ��Щ����Ҫ���ݾ����������ܼ�����д��!!!
 *
 */
// void TAMPER_IRQHandler(void)
// {
//   if(BKP_GetITStatus()==SET)
//   {
// /*****һ�㱻�����Ĳ�����Ҫ���ݾ��������д����ֱ�ӱ��ϻ��Ƿ��ͱ�����Ϣ������������У���Щ����Ҫ���ݾ����������ܼ�����д��******/
//     //app_start


//     //app_end

//     /* Clear Tamper pin interrupt pending bit */
//     BKP_ClearITPendingBit();
//     /* Clear Tamper pin Event(TE) pending flag */
//     BKP_ClearFlag();
//     //����ϵͳ��λ
//     // NVIC_SystemReset();
//   }
// }

// void USART1_IRQHandler(void)
// {
//   // printf("USART1_IRQHandler\r\n");//����ʱʹ��
//   USART1_IRQHandler_IDLE_callback(); // ����USART1�����ж�
// }

// void DMA1_Channel5_IRQHandler(void)
// {
//   // printf("DMA1_Channel5_IRQHandler\r\n");//����ʱʹ��
//   DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(); // ����DMA1ͨ��5�������ж�
// }

/**
  * @}
  */ 


