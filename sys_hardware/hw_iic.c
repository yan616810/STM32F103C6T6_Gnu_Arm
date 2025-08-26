#include "hw_iic.h"
#include "Delay.h"

//hw_iic端口外设&引脚初始化
void hw_iic_init(void)
{
//时钟开启一定要在最前边，否则会产生Bug
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
//IO端口模式初始化
    GPIO_InitTypeDef GPIO_InitStructure={
    .GPIO_Mode=GPIO_Mode_AF_OD,
    .GPIO_Speed=GPIO_Speed_50MHz,
    .GPIO_Pin =GPIO_Pin_6
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
//iic外设初始化
    I2C_InitTypeDef I2C_InitStruct={
        .I2C_Ack=I2C_Ack_Enable,
        .I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit,
        .I2C_OwnAddress1=0x00,
        .I2C_ClockSpeed=400000,
        .I2C_DutyCycle=I2C_DutyCycle_2,
        .I2C_Mode=I2C_Mode_I2C,
    };
    I2C_Init(I2C1,&I2C_InitStruct);
    I2C_Cmd(I2C1,ENABLE);
}

uint8_t hw_iic_CheckEvent_timeout(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint8_t timeout;
    while(I2C_CheckEvent(I2Cx,I2C_EVENT) != SUCCESS)//DR和数据移位寄存器都是空
    {
        timeout++;Delay_us(1);
        if(timeout==100)
        {
            I2C_GenerateSTOP(I2Cx,ENABLE);
            return 1;
        }
    }
    return 0;
}

uint8_t hw_iic_write_byte(uint8_t addr,uint8_t reg,uint8_t data)
{
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;

    I2C_SendData(I2C1,reg);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING))return 1;

    //程序运行到这里时，DR为空，移位寄存器刚要发送数据，如果下面程序紧接着向DR再传个数据
    //这里是个窗口期，等待从机应答前如果向DR发数据继续发送；否则就表示主机不想发数据了
    I2C_SendData(I2C1,data);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;//DR和数据移位寄存器都是空

    I2C_GenerateSTOP(I2C1,ENABLE);
	return 0;
}

uint8_t hw_iic_write_byte_len(uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf)
{
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;
	
    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;

    I2C_SendData(I2C1,reg);
    while(len)
    {
        len--;
        if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING))return 1;//DR和数据移位寄存器都是非空，就会一直在这里循环，直到移位寄存器发完一个字节，DR转移到移位寄存器中，接收到从机应答后，退出循环
        I2C_SendData(I2C1,*data_buf++);
    }
    
    //程序运行到这里时，DR为空，移位寄存器刚要发送数据，如果下面程序紧接着向DR再传个数据
    //这里是个窗口期，等待从机应答前如果向DR发数据继续发送；否则就表示主机不想发数据了
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;//DR和数据移位寄存器都是空
    I2C_GenerateSTOP(I2C1,ENABLE);
	return 0;
}

uint8_t hw_iic_read_byte(uint8_t addr,uint8_t reg)
{
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;

    I2C_SendData(I2C1,reg);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;
//接收
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Receiver);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))return 1;

    I2C_AcknowledgeConfig(I2C1,DISABLE);//设置读完最后一个字节后主机不回应，提前设置不回应，才能在下面接受完成后立马发送主不回应
    I2C_GenerateSTOP(I2C1,ENABLE);//提前设置停止位，才能在下面接受完成后立马停止
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))return 1;

    uint8_t dat = I2C_ReceiveData(I2C1);
    I2C_AcknowledgeConfig(I2C1,ENABLE);//只有这里不需要主机回应，其他的命令都需要主机回应
    return dat;
}

uint8_t hw_iic_read_byte_len(uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf)
{
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;

    I2C_SendData(I2C1,reg);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;
//读数据
    I2C_GenerateSTART(I2C1,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2C1,(addr<<1),I2C_Direction_Receiver);
    if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))return 1;

    while (len)
    {
        len--;
        if(!len)
        {
            I2C_AcknowledgeConfig(I2C1,DISABLE);//设置读完最后一个字节后主机不回应，提前设置不回应，才能在下面接受完成后立马发送主不回应
            I2C_GenerateSTOP(I2C1,ENABLE);//提前设置停止位，才能在下面接受完成后立马停止
        }
        if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))return 1;
        *data_buf++ = I2C_ReceiveData(I2C1);
    }
    I2C_AcknowledgeConfig(I2C1,ENABLE);//恢复默认的需要需要主机回应从机
    return 0;
}

/***************************其他函数*****************/

// //返回0，表示已搜索到该7位地址的设备
// uint8_t hw_iic_search_devices(uint8_t addr_7bit)
// {
//     hw_iic_init();
//     I2C_GenerateSTART(I2C1,ENABLE);
//     if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

//     I2C_Send7bitAddress(I2C1,(addr_7bit<<1),I2C_Direction_Transmitter);
//     if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
//     return 0;
// }


