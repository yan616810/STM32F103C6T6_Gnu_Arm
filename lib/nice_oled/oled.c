#include "oled.h"
#include "olediic.h"
#include "word.h"

u8 OLED_GRAM[144][8];

/***************************************************
��������:OLED�ĳ�ʼ��
***************************************************/
void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_1;	  //PA1->IIC_SCL  PA2->IIC_SDA
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	                //��ʼ��PA1,PA2
 	GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);              //PA1,PA2�ø�

	OLED_WR_Byte(0xAE,0);//---�ر�OLED���
	OLED_WR_Byte(0x00,0);//---�����е͵�ַ
	OLED_WR_Byte(0x10,0);//---�����иߵ�ַ
	OLED_WR_Byte(0x40,0);//---������ʼ�е�ַ����ӳ��RAM��ʾ��ʼ�У�0x00~0x3F��
	OLED_WR_Byte(0x81,0);//---���öԱȿ��ƼĴ���
	OLED_WR_Byte(0xCF,0);//---��������
	OLED_WR_Byte(0xA1,0);//---���÷ֶ�/��ӳ��     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,0);//---����COM��/��ɨ�跽��   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,0);//---����������ʾ
	OLED_WR_Byte(0xA8,0);//---���ø��ñȣ�1��64��
	OLED_WR_Byte(0x3f,0);//---1/64����
	OLED_WR_Byte(0xD3,0);//---������ʾƫ��ӳ��RAM��������0x00~0x3F��
	OLED_WR_Byte(0x00,0);//---������
	OLED_WR_Byte(0xd5,0);//---������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WR_Byte(0x80,0);//---s���÷�Ƶ�ȣ���ʱ������Ϊ100֡/��
	OLED_WR_Byte(0xD9,0);//---����Ԥ���ʱ��
	OLED_WR_Byte(0xF1,0);//---��Ԥ�������Ϊ15��ʱ�ӣ��ŵ�����Ϊ1��ʱ��
	OLED_WR_Byte(0xDA,0);//---����com����Ӳ������
	OLED_WR_Byte(0x12,0);//---
	OLED_WR_Byte(0xDB,0);//---���� vcomh
	OLED_WR_Byte(0x30,0);//---����VCOMȡ��ѡ�񼶱�
	OLED_WR_Byte(0x20,0);//---����ҳ��Ѱַģʽ  (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,0);//---
	OLED_WR_Byte(0x8D,0);//---���õ�ɱ�����/����
	OLED_WR_Byte(0x14,0);//---���ã�0x10������
	OLED_Clear();
	OLED_WR_Byte(0xAF,0);
}


/**************************************************
��������:����һ���ֽ�
��ڲ���:mode:����/�����־ 
        0��ʾ����
        1��ʾ����
**************************************************/
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}


/**************************************************
��������:��ת��Ļ��ʾ
��ڲ���:i=0:������ʾ;
         i=1:��ת��ʾ-----����ڣ��ڱ���                      
**************************************************/
/*void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,0);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,0);//��ɫ��ʾ
		}
}*/


/**************************************************
��������:��Ļ��ת180��
��ڲ���:i=0:������ʾ
         i=1:��ת��ʾ
**************************************************/
/*void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,0);//������ʾ
			OLED_WR_Byte(0xA1,0);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,0);//��ת��ʾ
			OLED_WR_Byte(0xA0,0);
		}
}*/


/**************************************************
��������:����OLED��ʾ
**************************************************/
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,0);//��ɱ�ʹ��
	OLED_WR_Byte(0x14,0);//������ɱ�
	OLED_WR_Byte(0xAF,0);//������Ļ
}

/**************************************************
��������:�ر�OLED��ʾ
**************************************************/
/*void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,0);//��ɱ�ʹ��
	OLED_WR_Byte(0x10,0);//�رյ�ɱ�
	OLED_WR_Byte(0xAE,0);//�ر���Ļ
}*/


/**************************************************
��������:�����Դ浽OLED(��ˢ�¸�д�������,ˢ����Ļ)
**************************************************/
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,0); //��������ʼ��ַ
		OLED_WR_Byte(0x00,0);   //���õ�����ʼ��ַ
		OLED_WR_Byte(0x10,0);   //���ø�����ʼ��ַ
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}
/***************************************************
��������:���OLED��Ļ
***************************************************/
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}


/***************************************************
��������:��ָ����(x,y)���괦��һ����
��ڲ���:
        x:0-127
        y:0-63
        t:1:���;  0:���
***************************************************/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}


/****************************************************
��������:��(x1,y1)��(x2,y2)��һ��ֱ��
��ڲ���:
        x1,x2:0-127
        y1,y2:0-63
        mode:1:���  0:���
*****************************************************/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/****************************************************
��������:��(x,y)ΪԲ��,rΪ�뾶��һ��Բ
��ڲ���:
        x:0-127
        y:0-63
*****************************************************/
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ��� 
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}


/****************************************************
��������:��(x,y)����ʾһ���ַ�,���������ַ�
��ڲ���:
        x:0-127
        y:0-63
        size:ѡ������:6*8  6*12  8*16  12*24
        mode:0:��ɫ��ʾ  1:������ʾ
*****************************************************/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';  //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		/*if(size1==8)
			  {temp=asc2_0806[chr1][i];} //����0806����
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����*/
		if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
		/*else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
		else return;*/
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


/****************************************************
��������:��(x,y)Ϊ���,��ʾһ���ַ���
��ڲ���:
        x:0-127
        y:0-63
        size:ѡ������:6*8  6*12  8*16  12*24
        *chr:�ַ�����ʼ��ַ
        mode:0:��ɫ��ʾ  1:������ʾ
*****************************************************/
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}


/****************************************************
��������:����m^n,��m*m*m*m*m*m*m*m*m*m*m*m*m....(n��)
��ڲ���:
        m:����
        n:ָ��
*****************************************************/
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}


/****************************************************
��������:��(x.y)Ϊ��ʼ������ʾ����
��ڲ���:
        x:0-127
        y:0-63
        num :Ҫ��ʾ������
        len :���ֵ�λ��
        size:�����С
        mode:0,��ɫ��ʾ;1,������ʾ
*****************************************************/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}


/****************************************************
��������:��(x.y)Ϊ��ʼ������ʾ����
��ڲ���:
        x:0-127
        y:0-63
        num :���ֶ�Ӧ�����
        size:�����С
        mode:0,��ɫ��ʾ;1,������ʾ
*****************************************************/
/*void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//����16*16����
		else if(size1==24)
				{temp=Hzk2[num][i];}//����24*24����
		else if(size1==32)       
				{temp=Hzk3[num][i];}//����32*32����
		else if(size1==64)
				{temp=Hzk4[num][i];}//����64*64����
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}*/


/****************************************************
��������:��ʾ���ֵĸ���(������ʾ)
��ڲ���:
        num ��ʾ���ֵĸ���
        space ÿһ����ʾ�ļ��
        mode:0,��ɫ��ʾ;1,������ʾ
*****************************************************/
/*void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //д��һ�����ֱ�����OLED_GRAM[][]������
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //��ʾ���
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //ʵ������
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}*/


/****************************************************
��������:��(x,y)Ϊ���������ʾͼƬ (ͼƬȡ��ģ������bmp.h)
��ڲ���:
         x,y���������
         sizex:ͼƬ�ĳ���
         sizey:ͼƬ�Ŀ��
         BMP[]��Ҫд���ͼƬ����
         mode:0,��ɫ��ʾ;1,������ʾ
*****************************************************/
/*void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}*/

void OLED_ShowFloat(u8 x,u8 y,float num,u8 len,u8 size,u8 mode)
{
	float Abs_num;
	int unit,tens,hundreds,one_decimal,two_decimal;
	Abs_num = My_abs(num);
	hundreds = (int)Abs_num/100;    					//���λ
	tens = ((int)Abs_num/10) % 10;  					//��ʮλ
  unit = Abs_num-hundreds*100	-tens*10;   	//���λ
	one_decimal=((Abs_num-(int)Abs_num))*10;
	two_decimal=(int)((Abs_num-(int)Abs_num)*100)%10;
	if(num>=0)
	{
	OLED_ShowChar(x,y,'+',size,mode);
	OLED_ShowNum(x+8,y,hundreds,1,size,mode);
	OLED_ShowNum(x+16,y,tens,1,size,mode);
	OLED_ShowNum(x+24,y,unit,1,size,mode);
	OLED_ShowChar(x+32,y,'.',size,mode);
	OLED_ShowNum(x+40,y,one_decimal,1,size,mode);
	OLED_ShowNum(x+48,y,two_decimal,1,size,mode);
	OLED_Refresh();
	}
	else if(num<0)
	{
	OLED_ShowChar(x,y,'-',size,mode);
	OLED_ShowNum(x+8,y,hundreds,1,size,mode);
	OLED_ShowNum(x+16,y,tens,1,size,mode);
	OLED_ShowNum(x+24,y,unit,1,size,mode);
	OLED_ShowChar(x+32,y,'.',size,mode);
	OLED_ShowNum(x+40,y,one_decimal,1,size,mode);
	OLED_ShowNum(x+48,y,two_decimal,1,size,mode);
	OLED_Refresh();
	}
}


float My_abs(float a)
{
	if(a>=0) return a;
	else return(-a);
}
/*void drawcircle(u8 x,u8 y,u8 r)//׼��
{
	u8 num;
	u8 a=0;
	u8 b=r;
	while(2*b*b>=r*r)
	{
		OLED_DrawPoint(x+b,y+a,1);
		OLED_DrawPoint(x+b,y-a,1);
		OLED_DrawPoint(x-b,y-a,1);
		OLED_DrawPoint(x-b,y+a,1);
		OLED_DrawPoint(x+a,y+b,1);
		OLED_DrawPoint(x+a,y-b,1);
		OLED_DrawPoint(x-a,y-b,1);
		OLED_DrawPoint(x-a,y+b,1);
		a++;
		num=a*a+b*b-r*r;
		if(num>0){a=0;
		b--;}
	}
}
void drawcircle1(u8 x,u8 y,u8 r)//��Բ
{
	s16 num;
	u8 a=0;
	u8 b=r;
	while(2*b*b>=r*r)
	{

		a++;
		num=a*a+b*b-r*r;
		if(num>0){a-=1;
		OLED_DrawPoint(x+b,y+a,1);
		OLED_DrawPoint(x+b,y-a,1);
		OLED_DrawPoint(x-b,y-a,1);
		OLED_DrawPoint(x-b,y+a,1);
		OLED_DrawPoint(x+a,y+b,1);
		OLED_DrawPoint(x+a,y-b,1);
		OLED_DrawPoint(x-a,y-b,1);
		OLED_DrawPoint(x-a,y+b,1);
			a=0;
		b--;}
	}
}
void drawcircle2(u8 x,u8 y,u8 r)//��Ǯ
{
	s16 num;
	u8 a=0;
	u8 b=r;
	while(2*b*b>=r*r)
	{
		OLED_DrawPoint(x+b,y+a,1);
		OLED_DrawPoint(x+b,y-a,1);
		OLED_DrawPoint(x-b,y-a,1);
		OLED_DrawPoint(x-b,y+a,1);
		OLED_DrawPoint(x+a,y+b,1);
		OLED_DrawPoint(x+a,y-b,1);
		OLED_DrawPoint(x-a,y-b,1);
		OLED_DrawPoint(x-a,y+b,1);
		a++;
		num=a*a+b*b-r*r;
		if(num>0){a=0;
		b--;}
	}
}*/
void drawcircle3(u8 x,u8 y,u16 r)//Բ
{
	s16 num;
	s16 a=0;
	s16 b=r-1;
	while((2*(b+1)*(b+1))>=r*r)
	{
		OLED_DrawPoint(x+b,y+a,1);
		OLED_DrawPoint(x+b,y-a,1);
		OLED_DrawPoint(x-b,y-a,1);
		OLED_DrawPoint(x-b,y+a,1);
		OLED_DrawPoint(x+a,y+b,1);
		OLED_DrawPoint(x+a,y-b,1);
		OLED_DrawPoint(x-a,y-b,1);
		OLED_DrawPoint(x-a,y+b,1);
		a++;
		num=a*a+b*b-r*r;
		if(num>0){
		
			a--;
		b--;}
	}
	
}
/***************************************************
��������:��ָ����(x,y)���괦��һ����
��ڲ���:
        x:0-127
        y:0-63
        t:1:���;  0:���
***************************************************/
void draw_point(u8 x,s8 y)
{
	s8 i;
	i=y%8;
	y/=8;
	OLED_GRAM[x][y]|=(0x01<<i);
}
/***************************************************
��������:��һ��a,b��Բ
��ڲ���:
        a:0-127
        b:0-63
        t:1:���;  0:����
***************************************************/
void oled_draw_elliptic(u8 x,u8 y,u8 a,u8 b)
{
	u32 w=a*a*b*b;
	u8 c=a;u8 d=0;
	while(c<=128)
	{
		draw_point(x+c,y+d);
		draw_point(x-c,y+d);
		draw_point(x-c,y-d);
		draw_point(x+c,y-d);
		d++;
		u32 t=d*d*a*a+c*c*b*b;
		if(t>w)
		{
			d--;
			c--;
		}	

	}
		
}
/***************************************************
��������:��һ��a,b��Բ
��ڲ���:
        a:0-127
        b:0-63
        t:1:���;  0:����
***************************************************/
void oled_draw_elliptic_half(u8 x,u8 y,u8 a,u8 b,u8 tt)//tt=0ʱ�Ұ���Բ��tt=1ʱ�����Բ��
{
	u32 w=a*a*b*b;
	u8 c=a;u8 d=0;
	while(c<=128)
	{
		if(tt==1)
		{
			draw_point(x-c,y+d);
			draw_point(x-c,y-d);
		}
		else if(tt==0)
		{
			draw_point(x+c,y+d);
			draw_point(x+c,y-d);
		}
		d++;
		u32 t=d*d*a*a+c*c*b*b;
		if(t>w)
		{
			d--;
			c--;
		}	

	}
		
}
/***************************************************
��������:��һ��a,bֱ��
��ڲ���:


***************************************************/
//void oled_drawline(u8 a1,u8 a2,u8 b1,u8 b2)
//{
//	
//}


