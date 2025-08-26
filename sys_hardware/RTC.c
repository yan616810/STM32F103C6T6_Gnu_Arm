#include "RTC.h"
#include "stm32f10x.h"
#include <time.h>
/*
注意：RTC使用LSE外部低速晶振时,RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI)函数注释说明要求一旦选择了RTCCLK时钟源，除非重置备用域，否则复位也将无法更改RTCCLK时钟源。

*/
// Type_Struct_Timezone_and_UTCxTime RTC_config_struct={
// 	.timezone=8,//默认UTC+8时区
// 	.UTCxTime={
// 		.tm_year=2025-1900,
// 		.tm_mon=4-1,
// 		.tm_mday=27,
// 		.tm_hour=20,
// 		.tm_min=25,
// 		.tm_sec=4,
// 		.tm_isdst=0,
// 		// .tm_wday=,
// 		// .tm_yday=,
// 	},
// };

/**
 * @brief 更方便的向RTC数据结构中填充年月日时分秒和夏令时;
 *
 * @param Timezone 时区：范围-12~+14    支持小数点后两位如：UTC+8.45则Timezone=8.45
 * @param year     年：1900~2106(对于无符号32位设备)    1900/1/1 00:00:00  ->   ​​2106/2/7 06:28:15​
 * @param mon      月：1~12
 * @param mday     日：1~31
 * @param hour     小时：0~23
 * @param min      分钟：0~59
 * @param sec      秒：0~60(考虑闰秒)
 * @param dst      夏令时：正数=有效;0=无效;负数=该信息不可用
 */
void RTC_Filling_DataStruct(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC,float Timezone,int year,int mon,int mday,int hour,int min,int sec,int dst)
{
    pStruct_RTC->timezone = Timezone;
    pStruct_RTC->UTCxTime.tm_year = year - 1900;//years since 1900
    pStruct_RTC->UTCxTime.tm_mon = mon - 1;//months since January, 0 to 11
    pStruct_RTC->UTCxTime.tm_mday = mday;
    pStruct_RTC->UTCxTime.tm_hour = hour;
    pStruct_RTC->UTCxTime.tm_min = min;
    pStruct_RTC->UTCxTime.tm_sec = sec;
    pStruct_RTC->UTCxTime.tm_isdst = dst;
}

/**
 * @brief 将包含时区信息的RTC_DatsStruct数据结构变量，转换成协调世界时UTC时间戳
 * 
 * @param pStruct_RTC 传入想要解析的RTC_Struct变量的指针
 * @return time_t 返回该RTC数据结构对应的'世界统一'的协调世界时UTC时间戳
 */
time_t RTC_DataStruct_To_Timestamp(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC)
{
    time_t Timestamp;
    Timestamp = mktime(&(pStruct_RTC->UTCxTime));//始终将传入的任何时区的日期强制看作格林威治日期，然后转成UTC时间戳
    Timestamp -=(int8_t)pStruct_RTC->timezone*3600 + (int16_t)(pStruct_RTC->timezone*100)%100*60;//计算此刻真正的UTC时间戳
    return Timestamp;
}

/**
 * @brief 0.启用LSE用于RTCCLK;
 *        1.向RTC_CNT中写入初始时间2025-4-27 16:20:04无夏令时转换后的世界统一UTC时间戳;(mktime函数强制将你传入的UTCxTime看作格林尼治时间UTC0Time时区为0,所以要在得到的时间戳上再加或减时区偏移,详细见笔记本)
 *        2.初始完成RTC后规定向BKP_DR10的最低位写'1',保证系统下次复位时不会再次配置LSE,配置RTC寄存器等初始化操作;
 * 
 * 省略在该函数内初始化RTC_CNT更好直接，RTC_CNT=0x0000表示1900-1-1 00:00:00
 * 
 * @param pStruct_RTC 指向配置RTC的数据结构: 1.时区; 例子：时区是UTC+8:45 => timezone=8.45;其中8代表小时，0.45代表45分钟;
 *                                         2.该时区下的细分日期; 例子：比如你时区设置为8.45,则细分日期是UTC+8:45下的细分日期;
 */
void RTC_init(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC)
{
	time_t init_Timestamp;//存日期转换后的时间戳
    uint16_t _BKP_DR10;//存储'RTC已初始化过'标志位

    //1.使能RTC相关的驱动时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);

    //作用见简介1
    _BKP_DR10 = BKP_ReadBackupRegister(BKP_DR10);//读BKP_DR不用开启写访问
    if(!(_BKP_DR10 & 0x0001))//(_BKP_DR10 & 0x0001)=0:需要初始化  1:之前RTC已初始化过将此位设置为了1
    {
        //2.允许写后备域寄存器
        PWR_BackupAccessCmd(ENABLE);

        //3.配置RTCCLK
        RCC_LSEConfig(RCC_LSE_ON);
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//等待LSE成功提供给RTCCLK时钟信号
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        // RCC_LSICmd(ENABLE );
        // while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);//等待LSI成功提供给RTCCLK时钟信号
        // RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        RCC_RTCCLKCmd(ENABLE);

        //4.等待上次写数据完成
        RTC_WaitForSynchro();//等待同步
        RTC_WaitForLastTask();

        //5.设置预分频器32.768kHz / 32768 = 1s
        RTC_SetPrescaler(32768-1);
        //6.等待预分频器写数据完成
        RTC_WaitForLastTask();

        //7.根据传入参数时区，设置RTC_CNT初始时间戳
        init_Timestamp = RTC_DataStruct_To_Timestamp(pStruct_RTC);
        RTC_SetCounter(init_Timestamp);
        //8.等待预RTC_CNT写数据完成
        RTC_WaitForLastTask();

        //9.向BKP_DR10寄存器的最低位写入1表示RTC已初始化过
        BKP_WriteBackupRegister(BKP_DR10,_BKP_DR10|0x0001);

        //10.禁止写后备域寄存器
        PWR_BackupAccessCmd(DISABLE);
    }
}

/**
 * @brief 读取RTC_CNT内的时间戳，转换成用户手动设置时区下的细分日期存在RTC_read_RTCStruct数据结构内
 * 
 * @param pStruct_RTC 保存转换后的细分日期
 * @param pStruct_RTC_Init_And_Adjustment 只读取其中用户设置的设备时区
 */
void RTC_get_DataStruct(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC,Type_Struct_Timezone_and_UTCxTime *pStruct_RTC_Init_And_Adjustment)
{
    uint32_t init_Timestamp;
    pStruct_RTC->timezone = pStruct_RTC_Init_And_Adjustment->timezone;//获取用户设置的时区
    init_Timestamp = RTC_read_Timestamp();
    init_Timestamp += (int8_t)pStruct_RTC->timezone*3600 + (int16_t)(pStruct_RTC->timezone*100)%100*60;
    pStruct_RTC->UTCxTime=*localtime(&init_Timestamp);
    pStruct_RTC->UTCxTime.tm_mon +=1;
    pStruct_RTC->UTCxTime.tm_year +=1900;
}

 /**
  * @brief 将修改'时区年月日时分秒夏令时'的RTC数据结构，转换成全球统一的UTC时间戳
  * 
  * @param pStruct_RTC 指向已修改的RTC数据结构
  */
void RTC_Adjustment(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC)
{
    time_t init_Timestamp;//修改后的时间戳
    //1.使能RTC相关的驱动时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);
    //2.允许写后备域寄存器
    PWR_BackupAccessCmd(ENABLE);
    //3.等待上次写数据完成
    RTC_WaitForLastTask();

    //4.根据传入RTC数据结构，重新设置RTC_CNT初始时间戳
    init_Timestamp = RTC_DataStruct_To_Timestamp(pStruct_RTC);
    RTC_SetCounter(init_Timestamp);

    //5.等待预RTC_CNT写数据完成
    RTC_WaitForLastTask();
    //6.禁止写后备域寄存器
    PWR_BackupAccessCmd(DISABLE);
}

/**
 * @brief 读RTC_CNT寄存器内的时间戳
 * 
 * @return uint32_t 
 */
uint32_t RTC_read_Timestamp(void)
{
	RTC_WaitForSynchro();
    return RTC_GetCounter();
}
