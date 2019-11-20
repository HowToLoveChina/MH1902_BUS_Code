#include "Digital.h"

/********************************************************
CLK ===========> PA13
STB ===========> PA15
DIN ===========> PA6
*********************************************************/
#define DAT_IN()  {GPIOA->OEN |=1<<6;}	//PA6����ģʽ
#define DAT_OUT() {GPIOA->OEN &=~(1<<6);} //PA6���ģʽ

#define READ_DAT   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) 

#define DIN_H    GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define DIN_L    GPIO_ResetBits(GPIOA, GPIO_Pin_6)

#define CLK_H  GPIO_SetBits(GPIOA,GPIO_Pin_13)
#define CLK_L  GPIO_ResetBits(GPIOA, GPIO_Pin_13)

#define STB_H  GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define STB_L  GPIO_ResetBits(GPIOA, GPIO_Pin_15)

const unsigned char Seg[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,/*0~9*/
0x77,0x7C,0x39,0x5E,0x79,0x71/*A~F*/}; //����� 0~F


//����� IO�ڳ�ʼ��
void dig_bsp_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
	  unsigned char i;
    /// CLK (PA13)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
   
    /// STB (PA15)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  // DIN  PA6
	  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOA, &GPIO_InitStruct); 
	 
		 CLK_H;
		 DIN_H;
		 STB_H;
		 dig_write_dat(0x02);//��ʾģʽ���ã�����Ϊ6��GRID��8��SEG 
		 STB_H;
		 dig_write_dat(0x40);//��ַ����ģʽ
		 STB_H;
		 dig_write_dat(0xC0);//��ַ��C0��ʼ
		 for(i = 0;i<12;i++)
		 {
			 dig_write_dat(0x3f);//��ջ��� ��ֹ���������ʾ
		 }
		 STB_H;	
		 dig_write_dat(0x89); 
}

void dig_write_dat(unsigned char  dat)
{
		unsigned char i = 0;    
	  STB_L;
    for(i=0;i<8;i++)
    {
		  CLK_L;		
		  if(dat & 0x01) DIN_H;//��λ�ȷ�
			else DIN_L;
			dat =dat>>1;
			CLK_H;//������ ��������
		}
}	

void dig_test(unsigned char dat)
{
	 unsigned char i;
	 dig_bsp_init();
	 CLK_H;
	 DIN_H;
	 STB_H;
	 dig_write_dat(0x02);//��ʾģʽ���ã�����Ϊ6��GRID��8��SEG 
	 STB_H;
	 dig_write_dat(0x40);//��ַ����ģʽ
	 STB_H;
	 dig_write_dat(0xC0);//��ַ��C0��ʼ
	 for(i = 0;i<12;i++)
	 {
	   dig_write_dat(0x3f);//��ַ��C0��ʼ ����ʾ0
	 }
   STB_H;	
	 dig_write_dat(0x89); 
	 
	 dig_Display(12959);
	 
}


void dig_Display(unsigned int dat)
{
	 unsigned char i;
	 unsigned char Buffer[4];
  if(dat > 9999)
	{  
		STB_H;
		dig_write_dat(0xC0);//��ַ��C0��ʼ 
		for(i = 0;i<4;i++)
		{	
		 dig_write_dat(Seg[15]);
		 dig_write_dat(Seg[15]);
		}
	}
	else
	{
		Buffer[0] = dat /1000;
		Buffer[1] = dat /100%10;
		Buffer[2] = dat %100/10;
		Buffer[3] = dat %10;
		STB_H;
		dig_write_dat(0xC0);//��ַ��C0��ʼ 
		for(i = 0;i<4;i++)
		{	
		 dig_write_dat(Seg[Buffer[i]]);
		 dig_write_dat(Seg[Buffer[i]]);
		}
  }
	STB_H;	
	dig_write_dat(0x89); 
	 
}	




















