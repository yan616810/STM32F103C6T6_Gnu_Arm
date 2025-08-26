#include "mpu6050_dmp.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <stdio.h>
#include <math.h>

#define MPU6050
#define log_i 	printf	//打印信息
#define log_e  	printf	//打印信息
#define Q30     1073741824.0f

//陀螺仪方向设置
static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1};

//方向转换
unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}
										   
//陀螺仪方向控制
unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar; 
    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

//自检  0:pass
static inline int run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

#if defined (MPU6500) || defined (MPU9250)
    result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
    result = mpu_run_self_test(gyro, accel);
#endif
    if (result == 0x03) {//0x01|0x02
		log_i("Passed!\n");
        log_i("accel: %7.4f %7.4f %7.4f\n",
                    accel[0]/65536.f,
                    accel[1]/65536.f,
                    accel[2]/65536.f);
        log_i("gyro: %7.4f %7.4f %7.4f\n",
                    gyro[0]/65536.f,
                    gyro[1]/65536.f,
                    gyro[2]/65536.f);
        /* Test passed. We can trust the gyro data here, so now we need to update calibrated data*/

#ifdef USE_CAL_HW_REGISTERS
        /*
         * This portion of the code uses the HW offset registers that are in the MPUxxxx devices
         * instead of pushing the cal data to the MPL software library
         */
        unsigned char i = 0;

        for(i = 0; i<3; i++) {
        	gyro[i] = (long)(gyro[i] * 32.8f); //convert to +-1000dps
        	accel[i] *=  2048.f; //convert to +-16G (bug fix from +-8G)
        	accel[i] = accel[i] >> 16;
        	gyro[i] = (long)(gyro[i] >> 16);
        }

        mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
        mpu_set_accel_bias_6500_reg(accel);
#elif defined (MPU6050) || defined (MPU9150)
        mpu_set_accel_bias_6050_reg(accel);
#endif
#else
        /* Push the calibrated data to the MPL library.
         *
         * MPL expects biases in hardware units << 16, but self test returns
		 * biases in g's << 16.
		 */
    	unsigned short accel_sens;
    	float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		dmp_set_accel_bias(accel);
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long) (gyro[0] * gyro_sens);
		gyro[1] = (long) (gyro[1] * gyro_sens);
		gyro[2] = (long) (gyro[2] * gyro_sens);
		dmp_set_gyro_bias(gyro);
#endif
        return 0;//self-test pass
    }
    else {
            if (!(result & 0x1))
                log_e("Gyro failed.\n");
            if (!(result & 0x2))
                log_e("Accel failed.\n");
            if (!(result & 0x4))
                log_e("Compass failed.\n");
            return -1;
     }

}

//0:Init Success   1~10:error mask
unsigned char MPU6050_Init_UseDmp(void)
{
    int result;

    result = mpu_init();
    if(result != 0)return 1;

    result = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);//设置传感器
    if(result !=0)return 2;
    /* Push both gyro and accel data into the FIFO. */
    result = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);//设置FIFO
    if(result !=0)return 3;
    result = mpu_set_sample_rate(DEFAULT_MPU_HZ);//设置采样率
    if(result !=0)return 4;

    result = dmp_load_motion_driver_firmware();//加载dmp固件
    if(result !=0)return 5;
    result = dmp_set_orientation(
        inv_orientation_matrix_to_scalar(gyro_orientation));
    if(result !=0)return 6;
    /*
     * Known Bug -
     * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
     * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
     * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
     * there will be a 25Hz interrupt from the MPU device.
     *
     * There is a known issue in which if you do not enable DMP_FEATURE_TAP
     * then the interrupts will be at 200Hz even if fifo rate
     * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
     *
     * DMP sensor fusion works only with gyro at +-2000dps and accel +-2G
     */
    result = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL);//设置dmp功能
    if(result !=0)return 7;
    result = dmp_set_fifo_rate(DEFAULT_MPU_HZ);//设置DMP输出速率(最大不超过200Hz)
    if(result !=0)return 8;
    // result = inv_set_quat_sample_rate(1000000L / DEFAULT_MPU_HZ);
    // if(result !=0)return -4;

    result =  run_self_test();//自检
    if(result !=0)return 9;

    result = mpu_set_dmp_state(1);//使能DMP
    if(result !=0)return 10;

    return 0;
}

//读fifo解算后的四元数，转为欧拉角  0:success
unsigned char mpu_dmp_get_data(float *pitch,float *roll,float *yaw)
{
    float q0,q1,q2,q3;
    unsigned long sensor_timestamp;
    short gyro[3], accel[3], sensors;
    unsigned char more;
    long quat[4];
    if(dmp_read_fifo(gyro,accel,quat,&sensor_timestamp,&sensors,&more))return 1;
    if(sensors & INV_WXYZ_QUAT)
    {
        q0 = quat[0] / Q30;
        q1 = quat[1] / Q30;
        q2 = quat[2] / Q30;
        q3 = quat[3] / Q30;
        // 计算得到俯仰角/横滚角/航向角
		*pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;	// pitch
		*roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
		*yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
    }else return 2;
return 0;
}
