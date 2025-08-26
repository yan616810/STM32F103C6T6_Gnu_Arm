#ifndef __SPI_SW_H
#define __SPI_SW_H
#include <stdint.h>

/**
 * @brief 修改软件SPI模式
 */
#define SPI_MODE_0//选择SPI模式

/**
 * @brief 想修改软件SPI协议的4个引脚，只需改变下面的宏定义
 * 1.改变开启引脚使用到的时钟
 * 2.改变每个具体的引脚
 */
#define RCC_APB2Periph_GPIOx        RCC_APB2Periph_GPIOA     //若占用多个时钟，RCC_APB2Periph_GPIOA||RCC_APB2Periph_GPIOB
#define SPI_CS_USE_PORT             GPIOA                    //CS端口
#define SPI_CS_PIN                  GPIO_Pin_4               //该端口下的具体引脚
#define SPI_SCK_USE_PORT            GPIOA                    //SCK端口
#define SPI_SCK_PIN                 GPIO_Pin_5               //该端口下的具体引脚
#define SPI_MISO_USE_PORT           GPIOA                    //MISO端口
#define SPI_MISO_PIN                GPIO_Pin_6               //该端口下的具体引脚
#define SPI_MOSI_USE_PORT           GPIOA                    //MOSI端口
#define SPI_MOSI_PIN                GPIO_Pin_7               //该端口下的具体引脚
/*************************************************************************************************************/
void MySPI_Init(void);
void MySPI_DeInit(void);

void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_Swap_byte(uint8_t tx_data);

uint8_t spi_write_read(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len);
#endif // !__SPI_SW_H
