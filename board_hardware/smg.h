#ifndef __smg_h
#define __smg_h
/*
	��Ϊ�����ʹ�����뽫�������ʾ�����ձ�����������
		1.���ⲿ�ļ���© ��������smg_code[] �� �Դ�����smg_buff[]��
		2.����.c�ļ���ֱ��ʹ��smg_code[]��smg_data[]������Ҫ��ʾ�ı���ת�����Դ������У�
		
*/
extern volatile unsigned char smg_buff[3];
extern const unsigned char smg_code[];
void smg_init(void);
//void smg_scanf(unsigned char *smg_buff);
void smg_scanf(void);// �˷�����������10ms���ж��е���һ�ξʹ���һλ�������10ms������л��������������

#endif
