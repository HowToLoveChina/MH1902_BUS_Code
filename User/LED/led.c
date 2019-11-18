#include "led.h"

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,BGLED_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN,BGLED_PIN};


void BSP_LED_Init()
{
  GPIO_InitTypeDef  GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Pin = LED1_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED2_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
  GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED3_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
  GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);
	
  //LED PA4  ======>�����
	GPIO_InitStruct.GPIO_Pin = BGLED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD_PU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BGLED_GPIO_PORT, &GPIO_InitStruct);
	BGLED_GPIO_PORT->PUE &= ~BGLED_PIN;		//ʧ���������� ���0
	
}	
	
void LED_Control(Led_TypeDef LED,unsigned char mode)
{
   
  if(LED != BGLED)//LED1~LED3
	{
		 if(mode == 1)//��LED
		 {
		   GPIO_ResetBits(GPIO_PORT[LED],GPIO_PIN[LED]);
		 }
		 else
		 {
		   GPIO_SetBits(GPIO_PORT[LED],GPIO_PIN[LED]);
		 }
	}
	else//�����LED��Ҫ�������
	{
		 
	   if(mode == 0)//�ر�LED
		 {
      	 GPIO_PORT[LED]->PUE &= ~GPIO_PIN[LED];		//Pue disnable		 
		 }
		 else
		 {
			   SYSCTRL->PHER_CTRL &= ~BIT(20);
			 
		     //GPIO_PORT[LED]->PUE |= GPIO_PIN[LED];		//Pue enable	
			    GPIOA->PUE |=GPIO_Pin_4;//PA4 ����
			   
			    SYSCTRL->PHER_CTRL |= BIT(20);
			 
		 }
		  
	}
	
}
void LED_TEST()
{
  unsigned char i; 
  BSP_LED_Init();
	for(i=0;i<3;i++)
	{
	  LED_Control(LED1,1);
	  delayms(500);
	  LED_Control(LED1,0);
		
		LED_Control(LED2,1);
	  delayms(500);
	  LED_Control(LED2,0);
		
		LED_Control(LED3,1);
	  delayms(500);
	  LED_Control(LED3,0);
			
	  LED_Control(BGLED,1);
	  delayms(500);
	  LED_Control(BGLED,0);

	}
}	



