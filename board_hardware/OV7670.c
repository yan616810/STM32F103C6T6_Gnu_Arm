#include "OV7670.h"
#include "stm32f10x.h"
#include "iic.h"
#include "Delay.h"

#define OV7670_SCCB_ADDR 0x21  // OV7670的SCCB地址

// #define OV_SIO_C_CLR()   GPIO_ResetBits(GPIOB, GPIO_Pin_12)         // SIO_C引脚清零
// #define OV_SIO_C_SET()   GPIO_SetBits(GPIOB, GPIO_Pin_12)           // SIO_C引脚置位
// #define OV_SIO_D_CLR()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)         // SIO_D引脚清零
// #define OV_SIO_D_SET()   GPIO_SetBits(GPIOB, GPIO_Pin_13)           // SIO_D引脚置位
#define OV_SIO_C_OUT(x)  GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(x))     // SIO_C引脚输出
#define OV_SIO_D_OUT(x)  GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(x))     // SIO_D引脚输出
#define OV_SIO_D_IN()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)             // SIO_D引脚输入
#define OV_RESET_OUT(x)      GPIO_WriteBit(GPIOB, GPIO_Pin_15,(BitAction)(x))  // RESET引脚清零
#define OV_PWDN_OUT(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_15,(BitAction)(x))  // PWDN引脚置位
#define OV_HREF_IN()     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)             // HREF引脚输入
#define OV_VSYNC_IN()   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)               // VSYNC引脚输入
#define OV_PCLK_IN()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)               // PCLK引脚输入

/**
 * @brief 向OV7670寄存器写入一个字节数据
 * 
 * @param reg 
 * @param data 
 */
void OV7670_SCCB_Write_byte(uint8_t reg, uint8_t data)
{
    IIC_Write_Byte(OV7670_SCCB_ADDR, reg, data);  // 使用IIC协议写入数据到OV7670寄存器
}

/**
 * @brief 从OV7670寄存器读取一个字节数据
 * 
 * @param reg 
 * @param data 
 */
void OV7670_SCCB_Read_byte(uint8_t reg, uint8_t *data)
{
    *data = IIC_Read_Byte(OV7670_SCCB_ADDR, reg);  // 使用IIC协议读取OV7670寄存器数据
}

/**
 * @brief 初始化OV7670的引脚IO口
 * 
 */
void OV7670_Pins_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 初始化GPIOB的引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // // 配置SIO_C引脚
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  // SIO_C
    // GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_Out_OD;  // 开漏输出模式
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // // 配置SIO_D引脚
    // GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_Out_OD;  // 开漏输出模式
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // SIO_D
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置RESET引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出模式，低电平复位
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  // RESET
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置PWDN引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出模式，高电平节电模式，低电平正常工作
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  // PWDN
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置HREF引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;// 输入下拉模式,上升沿表示新行同步信号
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   // HREF
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置VSYNC引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 输入上拉模式,下降沿表示新帧同步信号
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   // VSYNC
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置PCLK引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 输入上拉模式,下降沿表示像素时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  // PCLK
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    // OV_SIO_C_OUT(1);// 初始化SIO_C引脚为高电平
    // OV_SIO_D_OUT(1);// 初始化SIO_D引脚为高电平
    OV_PWDN_OUT(0);   // 初始化PWDN引脚为低电平，正常工作
    OV_RESET_OUT(0);  // 初始化RESET引脚为低电平，硬件复位
    Delay_ms(3); // 等待3ms，确保OV7670复位完成
    OV_RESET_OUT(1);  // 初始化RESET引脚为高电平，硬件复位完成
    Delay_ms(3); // 等待3ms后写初始化代码
}
