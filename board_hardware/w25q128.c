#include "w25q128.h"
#include "spi.h"
#include "w25q128_reg.h"
#include "Delay.h"

/**
 * @brief 初始化连接到flash芯片的软件SPI的引脚
 * 
 */
void W25Q128_Init(void)
{
    MySPI_Init();
}

/**
 * @brief 执行写入，擦除操作前应该调用此函数将状态寄存器1的WEL位置1
 * 
 */
void W25Q128_WriteEnable(void)
{
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_WRITE_ENABLE);
    MySPI_Stop();
}

//
/**
 * @brief 阻塞式等待状态寄存器1中的Busy位从1(忙)变到0(空闲)
 * 
 * @param us 阻塞式等待多少微妙
 * @return uint8_t 0:等待Busy位变为0，未超时    1:等待Busy位变为0，超时
 */
uint8_t W25Q128_Read_StatusReg1_WaitBusy(uint32_t us)
{
    uint32_t timeout;
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_READ_STATUS_REG1);
    while(MySPI_Swap_byte(0x00) & 0x01)
    {
        timeout++;Delay_us(1);
        if(timeout==us)
        {
            MySPI_Stop();
            return 1;
        }
    }
    MySPI_Stop();
    return 0;
}

/**
 * @brief 擦除指定地址所在的整个扇区，全变为0xFF
 * 注意：1.使用前先调用写使能W25Q128_WriteEnable开启WEL位为1;
 *      2./CS上升沿开始擦除，擦除期间不响应其他指令;
 *      3.擦除完成后，busy自动变为0，WEL也自动变为0;
 * @param address 指定欲擦除的地址
 * @return uint8_t 0:擦除成功，未超时    1:擦除失败，超时
 */
uint8_t W25Q128_SectorErase(uint32_t address)
{
    W25Q128_WriteEnable();
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_SECTOR_ERASE_4K);
    MySPI_Swap_byte(address>>16);
    MySPI_Swap_byte(address>>8);
    MySPI_Swap_byte(address);
    MySPI_Stop();//上升沿开始擦除···

    if(W25Q128_Read_StatusReg1_WaitBusy(400000))return 1;//max 400ms
    return 0;
}

/**
 * @brief 整片擦除
 * 注意：1.使用前先调用写使能W25Q128_WriteEnable开启WEL位为1;
 *      2./CS上升沿开始擦除，擦除期间不响应其他指令;
 *      3.擦除完成后，busy自动变为0，WEL也自动变为0;
 * @return uint8_t 0:擦除成功，未超时    1:擦除失败，超时
 */
uint8_t W25Q128_ChipErase(void)
{
    W25Q128_WriteEnable();
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_CHIP_ERASE);
    MySPI_Stop();//上升沿开始擦除···

    if(W25Q128_Read_StatusReg1_WaitBusy(200000000))return 1;//max 200s
    return 0;
}

/**
 * @brief 页编程
 * 注意：1.写入时要先擦除该区域W25Q128_SectorErase
 *      2.先调用写使能W25Q128_WriteEnable开启WEL位为1;
 *      3.一次最多可写256个字节，超出的数据，依次覆盖掉原来开头起始的数据;地址低8位一般为0x00,这样可以一下子写256个字节，如0x001100->表示第一个块的第二个扇区的第二页起始位置
 * @param address 在该地址处写入第一个数据
 * @param Arry 将要写入Flash中的数据数组
 * @param len 向Flash中写入几字节
 * @return uint8_t 0:写入成功，未超时    1:写入失败，超时
 */
uint8_t W25Q128_PageProgram(uint32_t address,uint8_t *Arry,uint16_t len)
{
    W25Q128_WriteEnable();
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_PAGE_PROGRAM);
    MySPI_Swap_byte(address>>16);
    MySPI_Swap_byte(address>>8);
    MySPI_Swap_byte(address);
    for(uint16_t i=0;i<len;i++)
    {
        MySPI_Swap_byte(*(Arry+i));
    }
    MySPI_Stop();//上升沿后256Byte页缓存数据开始真正的转移到Flash中···

    if(W25Q128_Read_StatusReg1_WaitBusy(3000))return 1;//max 3ms
    return 0;
}

//busy=1时调用该函数无效

/**
 * @brief 读取JEDEC_ID
 * 
 * @param MID 存放读取到的Flash的厂商ID
 * @param DID 存放读取到的Flash的设备ID
 */
void W25Q128_Read_id(uint8_t *MID,uint16_t *DID)
{
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_JEDEC_ID);
    *MID = MySPI_Swap_byte(0x00);
    *DID = MySPI_Swap_byte(0x00);
    *DID <<=8;
    *DID |= MySPI_Swap_byte(0x00);
    MySPI_Stop();
}

//busy=1时调用该函数无效

/**
 * @brief 从Flash任意地址开始读取任意长度的数据
 * 
 * @param address 从该Flash地址开始读第一个数据
 * @param Arry 读取Flash数据存放处
 * @param len 读多少个数据
 */
void W25Q128_ReadData(uint32_t address,uint8_t *Arry,uint16_t len)
{
    MySPI_Start();
    MySPI_Swap_byte(W25QXX_COMMAND_READ_DATA);
    MySPI_Swap_byte(address>>16);
    MySPI_Swap_byte(address>>8);
    MySPI_Swap_byte(address);
    for(uint16_t i=0;i<len;i++)
    {
        *(Arry+i) = MySPI_Swap_byte(0x00);
    }
    MySPI_Stop();
}



