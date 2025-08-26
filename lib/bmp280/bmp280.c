#include "bmp280.h"
#include "iic.h"
#include "string.h"
/*
1.连接BMP280时，如果连接的是模块&从设备地址是0x76，SDO地址位可不用接地(其模块内部已经接地)。
2.CSB接3.3V选择IIC接口协议。

*/
struct bmp280 p_bmp280;

u8 BMP280_Chack(void)
{	
	static u8 chip_id=0;
	static u8 time;
	while(time<50){
	chip_id=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CHIPID_REG);
	if(chip_id==0x58)
		{
			p_bmp280.chip_id=chip_id;
			return 0;
		}
	time++;
	}
	return 1;
}
u8 BMP280_CalibParam(void)
{
	u8 a_data_u8[24],res=0;
	//memset(a_data_u8,0,24*sizeof(u8));
	res=IIC_Read_Len(BMP280_SlaveAddr,BMP280_DIG_T1_LSB_REG,24,a_data_u8);
	p_bmp280.calib_param.dig_T1=(u16)((((u16)((u8)a_data_u8[1]))<<8)|a_data_u8[0]);
	p_bmp280.calib_param.dig_T2=(s16)((((s16)((s8)a_data_u8[3]))<<8)|a_data_u8[2]);
	p_bmp280.calib_param.dig_T3=(s16)((((s16)((s8)a_data_u8[5]))<<8)|a_data_u8[4]);
	p_bmp280.calib_param.dig_P1=(u16)((((u16)((u8)a_data_u8[7]))<<8)|a_data_u8[6]);
	p_bmp280.calib_param.dig_P2=(s16)((((s16)((s8)a_data_u8[9]))<<8)|a_data_u8[8]);
	p_bmp280.calib_param.dig_P3=(s16)((((s16)((s8)a_data_u8[11]))<<8)|a_data_u8[10]);
	p_bmp280.calib_param.dig_P4=(s16)((((s16)((s8)a_data_u8[13]))<<8)|a_data_u8[12]);
	p_bmp280.calib_param.dig_P5=(s16)((((s16)((s8)a_data_u8[15]))<<8)|a_data_u8[14]);
	p_bmp280.calib_param.dig_P6=(s16)((((s16)((s8)a_data_u8[17]))<<8)|a_data_u8[16]);
	p_bmp280.calib_param.dig_P7=(s16)((((s16)((s8)a_data_u8[19]))<<8)|a_data_u8[18]);
	p_bmp280.calib_param.dig_P8=(s16)((((s16)((s8)a_data_u8[21]))<<8)|a_data_u8[20]);
	p_bmp280.calib_param.dig_P9=(s16)((((s16)((s8)a_data_u8[23]))<<8)|a_data_u8[22]);
	return res;
}
u8 BMP280_SetPowerMode(u8 mode)
{
	u8 v_mode_u8=0,res=0;
	if (mode<=BMP280_NORMAL_MODE) 
	{
		v_mode_u8=(p_bmp280.oversamp_temperature<<5)+(p_bmp280.oversamp_pressure<<2)+mode;
		res=IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,v_mode_u8);
		}
	else res=2;
		return res;
}
u8 BMP280_SetWorkMode(WORKING_MODE mode)
{
	u8 res=0,v_data_u8=0;
	if (mode<=0x04) 
	{
		v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG);//读取出控制寄存器的值
		switch(mode)
		{
			case BMP280_ULTRA_LOW_POWER_MODE:
				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
			p_bmp280.oversamp_pressure=BMP280_P_MODE_x1;
			break;
			
			case BMP280_LOW_POWER_MODE:
				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
			p_bmp280.oversamp_pressure=BMP280_P_MODE_x2;
			break;
			
			case BMP280_STANDARD_RESOLUTION_MODE:
				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
			p_bmp280.oversamp_pressure=BMP280_P_MODE_x4;				
			break;
			
			case BMP280_HIGH_RESOLUTION_MODE:
				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
			p_bmp280.oversamp_pressure=BMP280_P_MODE_x8;
			break;
			
			case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
				p_bmp280.oversamp_temperature=BMP280_P_MODE_x2;
			p_bmp280.oversamp_pressure=BMP280_P_MODE_x16;
			break;
		}
		v_data_u8=((v_data_u8&~0xE0)|((p_bmp280.oversamp_temperature<<5)&0xE0));
		v_data_u8=((v_data_u8&~0x1C)|((p_bmp280.oversamp_pressure<<2)&0x1C));
		res=IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,v_data_u8);
	} 
	else res=1;
	return res;
}
u8 BMP280_SetStandbyDurn(BMP280_T_SB standby_durn)
{
	u8 v_data_u8=0;
	v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG);                             //读取出寄存器的值
	v_data_u8=((v_data_u8&~0xE0)|((standby_durn<<5)&0xE0));                    //高3位
	return IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG,v_data_u8);
}
u8 bmp_filter(BMP280_FILTER_COEFFICIENT filter)
{
	u8 v_data_u8=0;
	v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG);
	v_data_u8=((v_data_u8&~0x1C)|((filter<<3)&0x1C)); //此处应为<<2
	return IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG,v_data_u8);
}
u8 BMP280_SetSoftReset(void)
{
	return (IIC_Write_Byte(BMP280_SlaveAddr,BMP280_RESET_REG,BMP280_RESET_VALUE));

}
u8 qingling(void)
{

	return (IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,0)||IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG ,0));

}
u8 BMP280_Init(void)
{
	BMP280_Pins_init();//引脚配置
	if(BMP280_SetSoftReset())
			return 1; 
	if(BMP280_Chack())
		return 2;                                                              //BMP280检测芯片
	else
	{
		                                                         //软件复位使用不了，使用之后用不了
		if(BMP280_CalibParam())
			return 3;
		if(BMP280_SetSoftReset())
			return 4; 
		if(qingling())
			return 5;
		if(BMP280_SetPowerMode(BMP280_NORMAL_MODE))
			return 6;
		if(BMP280_SetWorkMode(BMP280_ULTRA_HIGH_RESOLUTION_MODE))
			return 7;
		if(BMP280_SetStandbyDurn(BMP280_T_SB_0_5MS))
			return 8;
		if(bmp_filter(BMP280_FILTER_MODE_4))
			return 9;
	}
	return 0;
}
/********************************/
u8 BMP280_ReadUncompPressureTemperature(s32 *un_press,s32 *un_temp)
{
	u8 a_data_u8[6]={0,0,0,0,0,0},res = 0;
	res=IIC_Read_Len(BMP280_SlaveAddr,BMP280_PRESSURE_MSB_REG,6,a_data_u8);
	*un_press=(s32)((((u32)(a_data_u8[0]))<<12)|(((u32)(a_data_u8[1]))<<4)|((u32)a_data_u8[2]>>4));/*气压*/
	*un_temp=(s32)((((u32)(a_data_u8[3]))<<12)| (((u32)(a_data_u8[4]))<<4)|((u32)a_data_u8[5]>>4));/* 温度 */
	return res;
}
/*s32 BMP280_CompensateTemperatureInt32(s32 un_temp)
{
	s32 v_x1_u32r=0;
	s32 v_x2_u32r=0;
	s32 temperature=0;
	v_x1_u32r=((((un_temp>>3)-((s32)p_bmp280.calib_param.dig_T1<<1)))*((s32)p_bmp280.calib_param.dig_T2))>>11;
	v_x2_u32r=(((((un_temp>>4)-((s32)p_bmp280.calib_param.dig_T1))*((un_temp>>4)-((s32)p_bmp280.calib_param.dig_T1)))>>12)*((s32)p_bmp280.calib_param.dig_T3))>>14;
	p_bmp280.calib_param.t_fine=v_x1_u32r+v_x2_u32r;
	temperature=(p_bmp280.calib_param.t_fine*5+128)>> 8;
	return temperature;
}*/
s32 BMP280_CompensateTemperatureInt32(s32 un_temp,
                                     struct bmp280 *dev)
{
    s32 var1, var2;
    s32 temperature;

    var1 =
        ((((un_temp>>3) - ((s32) dev->calib_param.dig_T1<<1))) *
         ((s32) dev->calib_param.dig_T2))>>11;
    var2 =
        (((((un_temp>>4) - ((s32) dev->calib_param.dig_T1)) *
           ((un_temp>>4) - ((s32) dev->calib_param.dig_T1)))>>12) *
         ((s32) dev->calib_param.dig_T3))>>14;

    dev->calib_param.t_fine = var1 + var2;

    temperature =(dev->calib_param.t_fine * 5 + 128)>>8;
    return temperature;
}

/*u32 BMP280_CompensatePressureInt32(s32 un_press)
{
	s32 v_x1_u32r=0;
	s32 v_x2_u32r=0;
	u32 v_pressure_u32=0;
	v_x1_u32r=(((s32)p_bmp280.calib_param.t_fine)>>1)-(s32)64000;
	v_x2_u32r=(((v_x1_u32r>>2)* (v_x1_u32r>>2))>>11)*((s32)p_bmp280.calib_param.dig_P6);
	v_x2_u32r=v_x2_u32r+((v_x1_u32r *((s32)p_bmp280.calib_param.dig_P5))<< 1);
	v_x2_u32r=(v_x2_u32r>>2)+(((s32)p_bmp280.calib_param.dig_P4)<<16);
	v_x1_u32r=(((p_bmp280.calib_param.dig_P3*(((v_x1_u32r>>2)*(v_x1_u32r>>2))>>13))>>3)+((((s32)p_bmp280.calib_param.dig_P2)* v_x1_u32r)>>1))>>18;
	v_x1_u32r=((((32768 + v_x1_u32r))* ((s32)p_bmp280.calib_param.dig_P1))>>15);
	v_pressure_u32=(((u32)(((s32)1048576)-un_press)-(v_x2_u32r>>12)))* 3125;
	if(v_pressure_u32<0x80000000)
		if(v_x1_u32r!=0)
			v_pressure_u32=(v_pressure_u32<<1)/((u32)v_x1_u32r);
		else return 0;
	else if (v_x1_u32r!=0)
		v_pressure_u32=(v_pressure_u32/(u32)v_x1_u32r)*2;
	else return 0;
	v_x1_u32r=(((s32)p_bmp280.calib_param.dig_P9)*((s32)(((v_pressure_u32>>3)*(v_pressure_u32>>3))>>3)))>>12;
	v_x2_u32r=(((s32)(v_pressure_u32>>2))*((s32)p_bmp280.calib_param.dig_P8))>>13;
	v_pressure_u32=(u32)((s32)v_pressure_u32+((v_x1_u32r+v_x2_u32r+ p_bmp280.calib_param.dig_P7)>>4));
	return v_pressure_u32;
}*/
u32 BMP280_CompensatePressureInt32(s32 un_press,
                                  const struct bmp280 *dev)
{
    s32 var1, var2;
    u32 pressure = 0;

    var1 = (((s32) dev->calib_param.t_fine) / 2) - (s32) 64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((s32) dev->calib_param.dig_P6);
    var2 = var2 + ((var1 * ((s32) dev->calib_param.dig_P5)) * 2);
    var2 = (var2 / 4) + (((s32) dev->calib_param.dig_P4) * 65536);
    var1 =
        (((dev->calib_param.dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
         ((((s32) dev->calib_param.dig_P2) * var1) / 2)) / 262144;
    var1 = ((((32768 + var1)) * ((s32) dev->calib_param.dig_P1)) / 32768);

    /* Avoid exception caused by division with zero */
    if (var1 != 0)
    {
        pressure = (((u32)(((s32)1048576) - un_press) - (var2 / 4096))) * 3125;

        /* Check for overflows against UINT32_MAX/2; if pressure is left-shifted by 1 */
        if (pressure < 0x80000000)
        {
            pressure = (pressure * 2) / ((u32) var1);
        }
        else
        {
            pressure = (pressure / (u32) var1) * 2;
        }

        var1 = (((s32) dev->calib_param.dig_P9) * ((s32) (((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((s32) (pressure / 4)) * ((s32) dev->calib_param.dig_P8)) / 8192;
        pressure = (u32) ((s32) pressure + ((var1 + var2 + dev->calib_param.dig_P7) / 16));
    }

    return pressure;
}
u8 BMP280_ReadPressureTemperature(u32 *press,s32 *temp)
{
	s32 un_press=0;
	s32 un_temp=0;
	u8 res=0;
	res=BMP280_ReadUncompPressureTemperature(&un_press,&un_temp);
	
	/* 读取真实的温度值和气压值*/
	*temp=BMP280_CompensateTemperatureInt32(un_temp,&p_bmp280);
	*press=BMP280_CompensatePressureInt32(un_press,&p_bmp280);
	return res;
}
