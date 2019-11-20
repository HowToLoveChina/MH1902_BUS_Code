#include "stdio.h"
#include "mhscpu.h"

#include "delay.h"


#define DMA_BLOCK_SIZE	15

uint8_t src_Buf[128] = {0};
uint8_t dst_Buf[128] = {0};
 
void UART_Configuration(void);
void NVIC_Configuration(void);
void TimerInit(void);
void LED_Init(void);

int main()
{	  
    UART_Configuration();//���ڳ�ʼ��
    NVIC_Configuration();//NVIC���� 
	  LED_Init();
    while(1)
    {   	
      printf("Hello Mh1902\r\n");	
      GPIOC->IODR ^=0x20; 			
			delayms(500);
    }
}


//Retarget Printf
int fputc(int ch, FILE *f)
{
	  while(!UART_IsTXEmpty(UART0)){};//�ȴ����ͼĴ���Ϊ��
    UART_SendData(UART0, (uint8_t) ch);
    return ch;   
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

TIM_InitTypeDef TIM_InitStructure;
//��ʱ������ʱ����PCLK�ṩ������ʱ��ʱ��Ƶ�ʵ���PCLK����ʱ��Ƶ�� 48Mhz
void TimerInit()
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0 ,ENABLE);//����TIMʱ��
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);//ʹ��TIM����	
//	//ControlReg
//	//bit      3                     2                     1                      0 
//	//     0-pwm�ر�             0-�жϴ�             0-����ģʽ            0-�򿪶�ʱ��                                            
//  //	   1-pwm��             1-�жϹر�             1-�û�����ģʽ        1-�رն�ʱ��                                                  
//	TIM_TypeStructure.TIM[0].ControlReg=0x03;
//	TIM_InitStructure.TIMx=TIM_0;
	TIM_InitStructure.TIM_Period=0x2DC6C00;//TIM_Period=��ʱʱ��(΢��)*48   48Mhz�����59.65s 
	TIM_Init(TIMM0,&TIM_InitStructure);//�û��Զ���ģʽ�����ö�ʱ����
	TIM_ITConfig(TIMM0, TIM_0,ENABLE);//�����ж�
	//TIM_Cmd(TIMM0, TIM_0, ENABLE);//ʹ��ʱ�� 	
}



void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    //�жϿ�����0 0λ��ռ���ȼ� 4λ��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;//UART�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	  NVIC_InitStructure.NVIC_IRQChannel = TRNG_IRQn;//TRNG�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure); 
	   
//	  NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;//TIM0�ж�
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	  NVIC_Init(&NVIC_InitStructure); 
}

void LED_Init()
{
   GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	 GPIO_InitStructure.GPIO_Remap=GPIO_Remap_1;
	 GPIO_Init(GPIOC,&GPIO_InitStructure);
	 GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}	

void TRNG_Init()
{
   SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TRNG ,ENABLE);//����TRNGʱ��
	 SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TRNG, ENABLE);//ʹ��TRNG����
	 TRNG_ITConfig(ENABLE);//ʹ���ж�
   TRNG_Start(TRNG0);//�������������
}
	
void UART_Configuration(void)
{
    UART_InitTypeDef UART_InitStructure;
    UART_FIFOInitTypeDef UART_FIFOInitStruct;

    UART_StructInit(&UART_InitStructure);
    UART_FIFOStructInit(&UART_FIFOInitStruct);
	  SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);//����UART0 GPIO ʱ��
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);//ʹ��UART0
	  GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);//PA0 PA1 ����ΪUART���� 

    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;

    UART_FIFOInitStruct.FIFO_Enable = ENABLE;
    UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_1;
    UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_2_Full;
    UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_RX_Trigger_1_2_Full;
    UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = ENABLE;

    UART_Init(UART0, &UART_InitStructure);
    //UART_FIFOInit(UART0, &UART_FIFOInitStruct);
  	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);	
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStruct;

	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_Peripheral = (uint32_t)UART0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(UART0->OFFSET_0.RBR);
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&dst_Buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = DMA_BLOCK_SIZE;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;

	DMA_Init(DMA_Channel_3, &DMA_InitStruct);
}
#endif
