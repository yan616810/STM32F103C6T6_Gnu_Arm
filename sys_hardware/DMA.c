/**
 * @file DMA.c
 * @author your name (you@domain.com)
 * @brief DMA双缓冲机制实现无缝接收串口数据并拷贝到LWRB环形缓冲区中 DMA+USART1+LWRB
 * 1.先在外部，如主程序中初始化一个LWRB环形缓冲区;
 * 2.使用DMA_link_lwrb_t()链接外部环形缓冲区;
 * 3.调用DMA_usart1_to_arrybuffer_init() 1.初始化DMA功能及其中断; 2.初始化usart1的空闲中断(前提是usart1时钟等已提前初始化); 3.并立即开始DMA转运;
 * 
 * 注意：应该设置串口空闲中断优先级低于DMA1通道5的中断优先级,这是保险措施，但不是致命的必要条件。
 * @version 0.1
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "DMA.h"
#include "lwrb.h"

/*定义双缓冲区*/
#define DMA_BUF_SIZE (uint16_t)128  // 适当增大缓冲区，最好大于单帧报文的长度

uint8_t  dma_buf1[DMA_BUF_SIZE], dma_buf2[DMA_BUF_SIZE];  //双缓冲，大小考量看笔记项目定位器
volatile uint8_t dma_buf_index = 0;                       // 区分 0: buf1, 1: buf2

static lwrb_t *static_rb_buff;  //链接外部定义的环形缓冲区

 /**
  * @brief 链接外部定义的环形缓冲区
  * 
  * @param rb_buff lwrb_t型结构体指针，指明保存在dma双缓冲区中的数据，转运到哪个外部环形缓冲区
  */
void DMA_link_lwrb_t(lwrb_t *rb_buff)
{
    static_rb_buff = rb_buff;
}

/**
 * @brief 配置usart1 + 使用DMA1的通道5，通过双缓冲机制
 * 
 */
void DMA_usart1_to_arrybuffer_init(void)
{
    /*1.开启DMA1时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /*2.开启USART1时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /*3.开启GPIOA时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*4.配置GPIOA9和GPIOA10为复用推挽输出和输入*/
    GPIO_InitTypeDef GPIO_InitStruct = {
        .GPIO_Pin   = GPIO_Pin_9,            // PA9
        .GPIO_Mode  = GPIO_Mode_AF_PP,            // 推挽输出
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;            // PA10
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    /*5.配置USART1波特率9600,8位数据位,无校验位,1位停止位*/
    USART_InitTypeDef USART_InitStruct = {
        .USART_BaudRate = 9600,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    };
    USART_Init(USART1, &USART_InitStruct);
    /*6.开启USART1接收中断*/
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 接收数据寄存器空闲
    /*7.配置USART1空闲中断优先级*/
    // 注意：USART1的中断优先级必须低于DMA1通道5的中断优先级，这是保险措施
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel                   = USART1_IRQn;  // USART1空闲中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);
    /*8.开启DMA1通道5接收USART1数据*/
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);/*1.每个通道连接着多个外设，通过逻辑或输入到DMA1控制器，这意味着同时只能有一个请求有效.
                                                  2.外设的DMA请求，可以通过设置相应外设寄存器中的控制位，被独立地开启或关闭。*/
/********************************************************分界线下为配置DMA****************************************************************** */    
    /*8.配置DMA1通道5为接收USART1数据到双缓冲区*/
    DMA_InitTypeDef DMA_InitStruct={
        .DMA_BufferSize         = DMA_BUF_SIZE,                  //每次传输的数据量
        .DMA_M2M                = DMA_M2M_Disable,               //禁止内存到内存传输
        .DMA_Mode               = DMA_Mode_Normal,               //工作模式：非循环模式
        .DMA_Priority           = DMA_Priority_VeryHigh,         // DMA优先级：非常高
        .DMA_DIR                = DMA_DIR_PeripheralSRC,         //数据传输方向：外设到内存
        .DMA_MemoryBaseAddr     = (uint32_t)dma_buf1,            //注意传入的应该是缓冲区的地址
        .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,       //内存数据大小：字节
        .DMA_MemoryInc          = DMA_MemoryInc_Enable,          //内存地址递增
        .DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR,         //注意传入的应该是DR寄存器的地址
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,   //外设数据大小：字节
        .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,     //外设地址不递增
    };
    DMA_Init(DMA1_Channel5,&DMA_InitStruct);
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel                   = DMA1_Channel5_IRQn;  // DMA1通道5中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;                   // DMA1通道5中断优先级必须高于USART1的空闲中断
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);

    DMA_Cmd(DMA1_Channel5,ENABLE);
    /*开启USART1*/
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief 1.DMA1通道5中断处理函数，串口1接收双缓冲满中断; 
 *        2.改变缓冲区; 
 *        3.将双缓冲数据拷贝到环形缓冲区;
 */
void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(void)
{
    /*每一个通道都有一组配置寄存器，此处串口1接收功能使用DMA1的通道5*/
    if(DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;                     //关闭DMA才可向寄存器写配置

        uint8_t *full_buf;
        if(dma_buf_index)//buf2 满中断
        {
            //切换缓冲区到buf1
            DMA1_Channel5->CMAR = (uint32_t)dma_buf1;
            dma_buf_index = 0;
            //此时dma_buf2中存有新数据
            full_buf = dma_buf2;
        }
        else//buf1 满中断
        {
            //切换缓冲区到buf2
            DMA1_Channel5->CMAR = (uint32_t)dma_buf2;
            dma_buf_index = 1;
            //此时dma_buf1中存有新数据
            full_buf = dma_buf1;
        }
        DMA1_Channel5->CNDTR = (uint16_t)DMA_BUF_SIZE;          // 重新设置传输数据量
        DMA1_Channel5->CCR |= DMA_CCR5_EN;                     //开启DMA传输

        DMA_ClearITPendingBit(DMA1_IT_TC5);

        //将full_buf指向的填满数据的dma_bufx拷贝到LWRB环形缓冲区
        lwrb_write(static_rb_buff,full_buf,DMA_BUF_SIZE);
    }
}

/**
 * @brief USART1空闲中断处理函数（DMA方式，处理未满缓冲区数据）
 * 1.USART1空闲中断回调函数，处理未满缓冲区数据，保证GPS全帧报文都能被转移到LWRB环形缓冲区中
 * @note 空闲中断发生时，DMA缓冲区可能未满，应将已接收数据从缓冲区头部写入环形缓冲区，并及时重启DMA。
 */
void USART1_IRQHandler_IDLE_callback(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        volatile uint32_t tmp;
        tmp = USART1->SR;  // 先读SR
        tmp = USART1->DR;  // 再读DR，清除空闲中断标志（必须，防止中断重复进入）
        (void)tmp;

        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;  // 关闭DMA

        uint16_t received = DMA_BUF_SIZE - DMA1_Channel5->CNDTR;  // 计算实际接收到的数据量
        uint8_t  *cur_buf ;                                       // 当前缓冲区指针
        uint8_t  *next_buf;                                       // 下一个缓冲区指针

        if(dma_buf_index == 0)
        {
            cur_buf       = dma_buf1;  // buf1未满
            next_buf      = dma_buf2;  // buf2为下一个缓冲区
            dma_buf_index = 1;         // 切换到buf2
        }
        else
        {
            cur_buf       = dma_buf2;  // buf2未满
            next_buf      = dma_buf1;  // buf1为下一个缓冲区
            dma_buf_index = 0;         // 切换到buf1
        }
        
        DMA1_Channel5->CMAR   = (uint32_t)next_buf;      // 立即切换到下一个缓冲区
        DMA1_Channel5->CNDTR  = (uint16_t)DMA_BUF_SIZE;  // 重新设置传输数据量
        DMA1_Channel5->CCR   |= DMA_CCR5_EN;             // 重新使能DMA

        // 将实际接收到的数据从当前缓冲区写入LWRB环形缓冲区
        if (received > 0 && received <= DMA_BUF_SIZE)
        {
            // 从缓冲区头部起始写入实际接收到的数据
            lwrb_write(static_rb_buff, cur_buf, received);
        }
    }
}

