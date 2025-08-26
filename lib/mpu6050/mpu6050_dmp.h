#ifndef __MPU6050_DMP_H
#define __MPU6050_DMP_H

unsigned char MPU6050_Init_UseDmp(void);
unsigned char mpu_dmp_get_data(float *pitch,float *roll,float *yaw);

#endif // !__MPU6050_DMP_H
