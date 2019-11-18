#include "mhscpu.h"
#include "delay.h"
uint32_t fac_us=96;

void delayus(uint32_t usec)
{		
	uint32_t temp;	
	if(usec == 0)return;
	SysTick->LOAD=usec*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=(SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_ENABLE_Msk) ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}




//__ASM void delayus(uint32_t usec)
//{
//    ALIGN
//    MOV r1,#24
//    MUL r0,r1
//    NOP
//    NOP
//    NOP
//loop
//    SUBS r0,#1
//    NOP
//    BNE loop
//    NOP
//    BX lr
//}

void delayms(uint32_t ms)
{
    if(ms==0)return;
    for(; ms>0; ms--)
    {
        delayus(1000);
    }
}








































