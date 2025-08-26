#ifndef __smg_h
#define __smg_h
/*
	因为数码管使用灵活，请将具体的显示需求按照变量单独处理：
		1.对外部文件暴漏 编码数组smg_code[] 和 显存数组smg_buff[]；
		2.其他.c文件可直接使用smg_code[]和smg_data[]，将想要显示的变量转换到显存数组中；
		
*/
extern volatile unsigned char smg_buff[3];
extern const unsigned char smg_code[];
void smg_init(void);
//void smg_scanf(unsigned char *smg_buff);
void smg_scanf(void);// 此非阻塞函数可10ms在中断中调用一次就代表一位数码管亮10ms后就再切换其他的数码管亮

#endif
