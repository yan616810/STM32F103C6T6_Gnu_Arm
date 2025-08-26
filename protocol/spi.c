#include "spi.h"
#include "stm32f10x.h"
#include "Delay.h"

#define SPI_CS_PIN_OUT(x)     GPIO_WriteBit(SPI_CS_USE_PORT,SPI_CS_PIN,(BitAction)(x))
#define SPI_SCK_PIN_OUT(x)    GPIO_WriteBit(SPI_SCK_USE_PORT,SPI_SCK_PIN,(BitAction)(x))
#define SPI_MISO_PIN_IN       GPIO_ReadInputDataBit(SPI_MISO_USE_PORT,SPI_MISO_PIN)
#define SPI_MOSI_PIN_OUT(x)   GPIO_WriteBit(SPI_MOSI_USE_PORT,SPI_MOSI_PIN,(BitAction)(x))

void MySPI_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure={
    .GPIO_Mode=GPIO_Mode_IN_FLOATING,
    .GPIO_Speed=GPIO_Speed_50MHz,
    };
    GPIO_InitStructure.GPIO_Pin=SPI_MISO_PIN;
    GPIO_Init(SPI_MISO_USE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=SPI_CS_PIN;
    GPIO_Init(SPI_CS_USE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=SPI_SCK_PIN;
    GPIO_Init(SPI_SCK_USE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=SPI_MOSI_PIN;
    GPIO_Init(SPI_MOSI_USE_PORT, &GPIO_InitStructure);

    SPI_CS_PIN_OUT(1);//CS=0代表选中该器件
#if defined(SPI_MODE_0) || defined(SPI_MODE_1)
    SPI_SCK_PIN_OUT(0);//mode 0/1 起始一定是低电平
#elif defined(SPI_MODE_2) || defined(SPI_MODE_3)
    SPI_SCK_PIN_OUT(1);//mode 2/3 起始一定是高电平
#endif // DEBUG
}

void MySPI_DeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={
    .GPIO_Mode=GPIO_Mode_IN_FLOATING,
    .GPIO_Speed=GPIO_Speed_50MHz,
    };
    GPIO_InitStructure.GPIO_Pin=SPI_MISO_PIN;
    GPIO_Init(SPI_MISO_USE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=SPI_CS_PIN;
    GPIO_Init(SPI_CS_USE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=SPI_SCK_PIN;
    GPIO_Init(SPI_SCK_USE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=SPI_MOSI_PIN;
    GPIO_Init(SPI_MOSI_USE_PORT, &GPIO_InitStructure);

    //时钟就不失能了，避免复杂化
}

void MySPI_Start(void)
{
    SPI_CS_PIN_OUT(0);
}

void MySPI_Stop(void)
{
    SPI_CS_PIN_OUT(1);
}

uint8_t MySPI_Swap_byte(uint8_t tx_data)
{
//自写
    // for(uint8_t i=0;i<8;i++)
    // {
    //     if( (tx_data & 0x80)==0)SPI_MOSI_PIN_OUT(0);
    //     else SPI_MOSI_PIN_OUT(1);
    //     tx_data<<=1;
    //     SPI_SCK_PIN_OUT(1);//采样
    //     rx_data<<=1;
    //     if(SPI_MISO_PIN_IN)rx_data|=0x01;
    //     SPI_SCK_PIN_OUT(0);//移出数据
    // }
//江协1
    // for(uint8_t i=0;i<8;i++)
    // {
    //     SPI_MOSI_PIN_OUT(tx_data & 0x80);
    //     tx_data<<=1;
    //     SPI_SCK_PIN_OUT(1);//采样
    //     rx_data<<=1;
    //     if(SPI_MISO_PIN_IN)rx_data|=0x01;
    //     SPI_SCK_PIN_OUT(0);//移出数据
    // }
//江协2
    // for(uint8_t i=0;i<8;i++)
    // {
    //     SPI_MOSI_PIN_OUT(tx_data & 0x80);
    //     tx_data<<=1;
    //     SPI_SCK_PIN_OUT(1);//采样
    //     if(SPI_MISO_PIN_IN)tx_data|=0x01;
    //     SPI_SCK_PIN_OUT(0);//移出数据
    // }
    // return tx_data;

    for(uint8_t i=0;i<8;i++)
    {
        #if defined(SPI_MODE_0)
            SPI_MOSI_PIN_OUT(tx_data & 0x80);
            tx_data<<=1;
            SPI_SCK_PIN_OUT(1);//采样
            if(SPI_MISO_PIN_IN)tx_data|=0x01;
            SPI_SCK_PIN_OUT(0);//移出数据
        #elif defined(SPI_MODE_1)
            SPI_SCK_PIN_OUT(1);//移出数据
            SPI_MOSI_PIN_OUT(tx_data & 0x80);
            tx_data<<=1;
            SPI_SCK_PIN_OUT(0);//采样
            if(SPI_MISO_PIN_IN)tx_data|=0x01;
        #elif defined(SPI_MODE_2)
            SPI_MOSI_PIN_OUT(tx_data & 0x80);
            tx_data<<=1;
            SPI_SCK_PIN_OUT(0);//采样
            if(SPI_MISO_PIN_IN)tx_data|=0x01;
            SPI_SCK_PIN_OUT(1);//移出数据
        #elif defined(SPI_MODE_3)
            SPI_SCK_PIN_OUT(0);//移出数据
            SPI_MOSI_PIN_OUT(tx_data & 0x80);
            tx_data<<=1;
            SPI_SCK_PIN_OUT(1);//采样
            if(SPI_MISO_PIN_IN)tx_data|=0x01;
        #endif
    }
    return tx_data;
}

/**Libdriver使用到的接口函数->软件
 * @brief      spi bus write read
 * @param[in]  *in_buf pointer to an input buffer  欲发送到flash的数据
 * @param[in]  in_len input length
 * @param[out] *out_buf pointer to an output buffer
 * @param[in]  out_len output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t spi_write_read(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len)
{    
    /* set cs low */
    MySPI_Start();

    /* if in_len > 0 */
    if (in_len > 0)
    {
        for(uint16_t i=0;i<in_len;i++)
        {
            MySPI_Swap_byte(*(in_buf+i));
        }
    }

    /* if out_len > 0 */
    if (out_len > 0)
    {
        for(uint16_t i=0;i<out_len;i++)
        {
            *(out_buf+i) = MySPI_Swap_byte(0x00);
        }
    }

    MySPI_Stop();
    return 0;
}

