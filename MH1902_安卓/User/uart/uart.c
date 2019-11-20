#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "mhscpu.h"
#include "mhscpu_uart.h"
#include "type.h"
#include "delay.h"

uart_param g_uart_param;
Errorflag ProtocolFlag;

unsigned short uart_len=0;
unsigned char uart_buf[512]={0};

extern Packet receivePacket;


//֧�� printf ����
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    if (ch == '\n')
    {
        fputc('\r', f);
    }
    while(!UART_IsTXEmpty(UART0));
    UART_SendData(UART0, (uint8_t) ch);
    return ch;
}

/**
 ****************************************************************
 * @brief uart_Config(uint32_t baudrate, uint32_t parity)
 *
 * ���ܣ����ڳ�ʼ��
 *
 * @param:baudrate = ������
 * @param:parity:��żУ��λ
 * @return: NONE
 *
 ****************************************************************/
void uart_Config(uint32_t baudrate, uint32_t parity)
{
    UART_InitTypeDef uart;
    NVIC_InitTypeDef nvic;
	
    uart.UART_BaudRate = baudrate;
    uart.UART_Parity = parity;
    uart.UART_StopBits = UART_StopBits_1;
    uart.UART_WordLength = UART_WordLength_8b;
    UART_Init(UART0, &uart);
    memset(&g_uart_param, 0, sizeof(g_uart_param));
	 
	  nvic.NVIC_IRQChannel = UART0_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);
	  

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

    //Set Enable FIFO Receive Threshold 1/2 Send Threshold 2 characters in the FIFO
    //UART0->OFFSET_8.FCR = (2 << 6) | (1 << 4) | BIT2 | BIT1 | BIT0;
		
    UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);
		
    NVIC_EnableIRQ(UART0_IRQn);
}


/**
 ****************************************************************
 * @brief protocolflag_init(void)
 *
 * ���ܣ����������Э����ر�־λ 
 *
 * @return: NONE
 *
 ****************************************************************/
void protocolflag_init(void)
{
 memset(&ProtocolFlag,0,sizeof(ProtocolFlag));
 memset(&receivePacket,0,sizeof(receivePacket));
}	


void uart_cmd_Receive(unsigned char *buf,unsigned short *len)
{
	unsigned short rxlen=uart_len;
	unsigned short i=0;
	*len=0;				//Ĭ��Ϊ0
	
	delayus(100);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս��� 
	
	if(rxlen==uart_len&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=uart_buf[i];	
		}		
		*len=uart_len;	//��¼�������ݳ���
		uart_len=0;		  //����
	}
}


void UART0_IRQHandler(void)
{
    uint8_t Res;
    static unsigned char bits = 0;
    if(0x04==(UART0->OFFSET_8.IIR & 0x0F))//���ڽ��յ���Ч����
    {
        Res =UART_ReceiveData(UART0);	//��ȡ���յ�������
			  if(uart_len < 512) uart_buf[uart_len++] = Res;//���ڽ��յ��������ݻ���	 
        if(ProtocolFlag.receive_ok !=0) return;//ProtocolFlag.receive_ok==0ʱ �ſ�ʼ��������
        if(ProtocolFlag.Head_flag)//����ͷ
        {
            if(ProtocolFlag.sn_flag)//���кű�־λ
            {
                if(ProtocolFlag.unuse_flag)//Ԥ���ֽڱ�־λ
                {
                    if(ProtocolFlag.command_flag)//���� 1�ֽ�
                    {
                        if(ProtocolFlag.datalen_flag)//���ݳ��ȱ�־λ 2�ֽ�
                        {
                            if(ProtocolFlag.datalen == receivePacket.length)//���յ���������������
                            {
                                if(ProtocolFlag.crc16_flag)//crc 1�ֽ�
                                {
																		if(Res == 0x03)
																		{
																				receivePacket.tail = 0x03;	//���ݽ�β
																			  ProtocolFlag.receive_ok = 1;
																		}
																		else
																		{
																				protocolflag_init();//���ݽ�β���� ��0���б�־λ			
																		} 
                                }
                                else
                                {
                                    receivePacket.crc16 = Res;
																	  //CRCУ����ȷ 
																	  if(1)
																	 // if(receivePacket.crc16 == crc16(receivePacket.packet_buf,receivePacket.packrt_buf_len))
																		{
																		    ProtocolFlag.crc16_flag = 1;
																		
																		}
																		else
																		{
																		   protocolflag_init();//CRCУ����� ��0���б�־λ		
																		}
                                }

                            }
                            else//������������ 
                            {
                                receivePacket.data[ProtocolFlag.datalen++]= Res;
															  receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
                            }
                        }
                        else// �������ݳ��� 2�ֽ�
                        {
                            bits++;
                            if(bits == 2)
                            {
                                bits = 0;
                                ProtocolFlag.datalen_flag = 1;//���ݰ����Ƚ�����ɱ�־λ ��1
                                receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
															  receivePacket.length |=Res;
															  if(receivePacket.length >400) protocolflag_init();//���ȴ��� ������������
															
															
                            }
                            else
                            {
                                receivePacket.length = Res*256;
															  receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
                            }
                        }
                    }
                    else//����ָ���ֽ� 1�ֽ�
                    {
                        ProtocolFlag.command_flag = 1;
                        receivePacket.command = Res;//ָ��
											  receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
                    }
                }
                else//����Ԥ���ֽ� 4�ֽ�
                {
										receivePacket.unuse[bits++]=Res;
										receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
										if(bits == 4)
										{
											bits = 0;
											ProtocolFlag.unuse_flag = 1;//���ݰ����Ƚ�����ɱ�־λ ��1
										}
                }

            }
            else//���յ����к� 1�ֽ�
            {
                ProtocolFlag.sn_flag = 1;
                receivePacket.sn = Res;
                receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res; 
            }
        }
        else//���յ�����ͷ
        {
            if(Res == 0x02)
            {
                ProtocolFlag.Head_flag = 1;//����ͷ��־λ��1
                receivePacket.head = 0x02;
							  receivePacket.packet_buf[receivePacket.packrt_buf_len++]=Res;
            }
            else
            {
                protocolflag_init();//����ͷ���� ��0���б�־λ						  
            }
        }
    }

}

//���ڽ���һ���ֽ� ������ʽ
unsigned char UART0_GetChar()
{
    while(!UART_IsRXReceived(UART0));
    return UART_ReceiveData(UART0);
}
//���ڷ���һ���ֽ� ������ʽ
void UART0_SendByte(unsigned char DAT)
{
    while(!UART_IsTXEmpty(UART0));
    UART_SendData(UART0, (uint8_t) DAT);
}

//���ڷ��ʹ����ֽ� ������ʽ
void UART0_SendBuf(unsigned char *pbuf,unsigned short len)
{
	while(len--)
  {
	  UART0_SendByte(*(pbuf++)); 
	}
}

