/*使用方法：
    1.先自己定义一个RTC数据结构型变量，此变量不仅用于初始化，还可用于步骤4的调整时间函数;
    2.调用RTC_Filling_DataStruct()函数向上一条变量里填充初始化细分时间。
    3.调用RTC_init()把设置的初始时间写到RTC_CNT。

    ---上面步骤1，2，3是使用RTC模块是必要的操作---
-------------------分--界--线----------------------------------------------
    ---下面用于后续用户若想调整设备的时间---

    4.调用RTC_Filling_DataStruct()，直接修改步骤1中的初始化时间变量;
    5.调用RTC_Adjustment()并传入步骤4修改后的变量即可;
*/
#ifndef __RTC_H
#define __RTC_H
#include <stdint.h>
#include <time.h>

typedef struct{
    //UTC+8:45 -> timezone=8.45
    float timezone;
    //x时区下的UTC+x细分日期
    struct tm UTCxTime;
}Type_Struct_Timezone_and_UTCxTime;

void RTC_Filling_DataStruct(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC,float Timezone,int year,int mon,int mday,int hour,int min,int sec,int dst);
void RTC_init(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC);
void RTC_Adjustment(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC);

void RTC_get_DataStruct(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC,Type_Struct_Timezone_and_UTCxTime *pStruct_RTC_Init_And_Adjustment);

uint32_t RTC_read_Timestamp(void);

time_t RTC_DataStruct_To_Timestamp(Type_Struct_Timezone_and_UTCxTime *pStruct_RTC);


#endif // !__RTC_H
