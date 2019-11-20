#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "mhscpu.h"
#include "mhscpu_uart.h"
#include "type.h"

uart_param g_uart_param;
//| Stop Bit 1 | Data Bit 8 |
#define LCR_DATA8_STOP1 (0x03)

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

    uart.UART_BaudRate = baudrate;
    uart.UART_Parity = parity;
    uart.UART_StopBits = UART_StopBits_1;
    uart.UART_WordLength = UART_WordLength_8b;
    UART_Init(UART0, &uart);
    memset(&g_uart_param, 0, sizeof(g_uart_param));

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

    //Set Enable FIFO Receive Threshold 1/2 Send Threshold 2 characters in the FIFO
    //UART0->OFFSET_8.FCR = (2 << 6) | (1 << 4) | BIT2 | BIT1 | BIT0;
    UART_ITConfig(UART0, RECV_INT, ENABLE);
    NVIC_EnableIRQ(UART0_IRQn);
}

/**
 ****************************************************************
 * @brief uart_IsSendFinish(void)
 *
* ���ܣ��жϴ����Ƿ��ͽ���?
 *
 * @param:NONE
 * @return: NONE
 *
 ****************************************************************/
int32_t uart_IsSendFinish(void)
{
    return ((UART0->LSR & BIT6) && g_uart_param.send.bytes < 1);
}

/**
 ****************************************************************
 * @brief uart_RecvFlush(void)
 *
* ���ܣ���ս��ջ���
 *
 * @param:NONE
 * @return: NONE
 *
 ****************************************************************/
void uart_RecvFlush(void)
{
    //disable recv interrupt.
    UART_ITConfig(UART0, RECV_INT, DISABLE);
    //Reset recv FIFO
    UART0->OFFSET_8.FCR |= BIT1;
    memset(&g_uart_param.recv, 0, sizeof(g_uart_param.recv));
    UART_ITConfig(UART0, RECV_INT, ENABLE);
}

/**
 ****************************************************************
 * @brief uart_SendChar(uint8_t ch)
 *
* ���ܣ����ڷ���һ���ַ�
 *
* @param:ch = Ҫ���͵��ַ�
 * @return: NONE
 *
 ****************************************************************/
int32_t uart_SendChar(uint8_t ch)
{
    while (g_uart_param.send.bytes == UART_SEND_BUF_SIZE)
    {
    }

    g_uart_param.send.buf[g_uart_param.send.write_index++] = ch;
    if (g_uart_param.send.write_index >= UART_SEND_BUF_SIZE)
    {
        g_uart_param.send.write_index = 0;
    }
    UART_ITConfig(UART0, SEND_INT, DISABLE);
    g_uart_param.send.bytes++;
    UART_ITConfig(UART0, SEND_INT, ENABLE);

    return 0;
}


int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len)
{
    while (len--)
    {
        uart_SendChar(*pBuf++);
    }
    return 0;
}

int32_t uart_RecvChar(void)
{
    unsigned char c;

    if (g_uart_param.recv.bytes)
    {
        c = g_uart_param.recv.buf[g_uart_param.recv.read_index];
        UART_ITConfig(UART0, RECV_INT, DISABLE);
        if (++g_uart_param.recv.read_index >= UART_RECV_BUF_SIZE)
        {
            g_uart_param.recv.read_index = 0;
        }
        g_uart_param.recv.bytes--;
        UART_ITConfig(UART0, RECV_INT, ENABLE);
        return c;
    }
    return -1;
}

int32_t uart_getbytes(void)
{
    return g_uart_param.recv.bytes;
}

typedef enum
{
    //Clear to send or data set ready or ring indicator or data carrier detect.
    //Note that if auto flow control mode is enabled, a change in CTS(that is, DCTS set)does not cause an interrupt.
    MODEM_STATUS    = 0x00,
    //None(No interrupt occur)
    NONE            = 0x01,
    //Transmitter holding register empty(Prog.
    //THRE Mode disabled)or XMIT FIFO at or below threshold(Prog. THRE Mode enable)
    TRANS_EMPTY     = 0x02,
    //Receive data available or FIFO trigger level reached
    RECV_DATA       = 0x04,
    //Overrun/parity/framing errors or break interrupt
    RECV_STATUS     = 0x06,
    //UART_16550_COMPATIBLE = NO and master has tried to write to the
    //Line Control Register while the DW_apb_uart is busy(USR[0] is set to1)
    BUSY_DETECT     = 0x07,
    //No char in or out of RCVR FIFO during the last 4 character times
    //and there is at least 1 character in it during this time
    CHAR_TIMEOUT    = 0x0C
} INT_FLAG;

Errorflag ProtocolFlag;
extern Packet receivePacket;

void protocolflag_init(void)
{
 memset(&ProtocolFlag,0,sizeof(ProtocolFlag));
}	

  uint8_t Res;
void UART0_IRQHandler(void)
{
  
	  static unsigned char bits = 0;
	  if(0x04==(UART0->OFFSET_8.IIR & 0x0F))//���ڽ��յ���Ч����
	  {
			  
		    Res =UART_ReceiveData(UART0);	//��ȡ���յ�������	
        //printf("%02x",Res);			
			  if(ProtocolFlag.RecSucc !=0) return;//ProtocolFlag.RecSucc==0ʱ �ſ�ʼ��������		
				if(ProtocolFlag.Head)
				{
				   if(ProtocolFlag.DataLen)//���ݰ����� 2�ֽ�
					 {
					   if(ProtocolFlag.Type)//������ 1�ֽ�
						 {
						     if(ProtocolFlag.Instruction)//ָ�� 1�ֽ�
						     {
						         if(ProtocolFlag.Num ==receivePacket.length-2)//���� N�ֽ�
										 {
										   if(ProtocolFlag.Tail)//������ 1�ֽ�
											 {
															if(ProtocolFlag.EndFlg)//BCC 1�ֽ�
															{
																	if(Res == ProtocolFlag.BCC)
																	{		  
																			 ProtocolFlag.RecSucc = 1;
																		   complete_Packet(&receivePacket);//�����յ������ݰ���� �ṹ��
																			 return;							
																	}
																	else
																	{
																			 memset(&ProtocolFlag,0,sizeof(ProtocolFlag));
																			 return;
																	}
															}						   
											 }
											 else
											 {
											   if(Res == 0x03)
												 {
												   ProtocolFlag.Tail = 1;
													 ProtocolFlag.BCC ^= 0x03;//BCC
													 receivePacket.tail = 0x03;	//���ݽ�β
                           receivePacket.bcc = ProtocolFlag.BCC;
                           ProtocolFlag.EndFlg = 1; 													 
												 }
                         else
                         {
												    memset(&ProtocolFlag,0,sizeof(ProtocolFlag));	   
												 }													 
											 }								 
										 }
                     else
                     {					  
											 receivePacket.data[ProtocolFlag.Num++]= Res;
										   ProtocolFlag.BCC^= Res; //BCC
										 }											 			 			 
						     }
						     else
						     {
						         ProtocolFlag.Instruction = 1;
							       receivePacket.instruction = Res;//ָ��	
                     ProtocolFlag.BCC^= Res; //BCC									 
						     } 
						 }
						 else
						 {
						    ProtocolFlag.Type = 1;
							  receivePacket.type = Res;//������
                ProtocolFlag.BCC^= Res; 							 
						 }
						 				 					 
					 }
					 else
					 {
					   bits++;
						 if(bits == 2)
						 {
						   bits = 0;
               ProtocolFlag.DataLen = 1;//���ݰ����Ƚ�����ɱ�־λ ��1  							 
							 receivePacket.length |=Res;
							 ProtocolFlag.BCC^= Res; //BCC 
						 }
						 else
						 {
						   receivePacket.length = Res;
							 receivePacket.length <<= 8;
               ProtocolFlag.BCC^= Res;//BCC							 
						 }   
					 
					 }		 
				}
				else
				{
				   if(Res == 0x02)//���յ�����ͷ
				   {
						 ProtocolFlag.Head = 1;//����ͷ��־λ��1 
						 ProtocolFlag.BCC ^=0x02;
						 receivePacket.head = 0x02;
					 }
					 else
					 {
					   memset(&ProtocolFlag,0,sizeof(ProtocolFlag));//����ͷ���� ��0���б�־λ
					 } 		
				}	
        		
		} 
}

int32_t uart_SendCharPoll(uint8_t ch)
{
    while(0 == UART_IsTXEmpty(UART0));
    UART0->OFFSET_0.THR = ch;
    //while(0 == UART_IsTXEmpty(UART0));

    return 0;
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

//����һ���ַ�����������λ��ͨ�Ÿ�ʽ
uint8_t UART0_GetString(Packet *receivePacket)
{
    uint16_t unFrameLength;
    uint8_t unTemp, i;//, unBcc=0;

    receivePacket->head = UART0_GetChar();

    if(receivePacket->head != 0x02)//ͨѶ��ͷ�ж�
        return FALSE;

//	UART0_SendByte(receivePacket->head);

    receivePacket->bcc		=	0;
    receivePacket->bcc		^=	receivePacket->head;
    receivePacket->length	=	UART0_GetChar();

//	UART0_SendByte(receivePacket->length);

    receivePacket->bcc		^=	receivePacket->length;

    receivePacket->length	<<= 8;
    unTemp					=	UART0_GetChar();

//	UART0_SendByte(unTemp);

    receivePacket->bcc		^=	unTemp;
    receivePacket->length	|=	unTemp;

    unFrameLength			=	receivePacket->length;

    receivePacket->type		=	UART0_GetChar();
    receivePacket->instruction = UART0_GetChar();

//	UART0_SendByte(receivePacket->type);
//	UART0_SendByte(receivePacket->instruction);

    receivePacket->bcc		^=	receivePacket->type;
    receivePacket->bcc		^=	receivePacket->instruction;

    for(i=0; i<unFrameLength-2; i++)
    {
        receivePacket->data[i] =	UART0_GetChar();

//		UART0_SendByte(receivePacket->data[i]);

        receivePacket->bcc	 ^= receivePacket->data[i];
    }
    receivePacket->tail	 =  UART0_GetChar();

//	UART0_SendByte(receivePacket->tail);

    if(receivePacket->tail != 0x03)
        return 0;

    receivePacket->bcc  ^= receivePacket->tail;
    unTemp = UART0_GetChar();

//	UART0_SendByte(unTemp);

    if(unTemp != receivePacket->bcc)
        return 0;

    if(receivePacket->type == TYPE_M1)
    {
        switch(receivePacket->instruction)
        {
        case COM_FINDCARD:
        case COM_GETUID:
            break;
        case COM_VERIFYKEYA:
        case COM_VERIFYKEYB:
        case COM_MODIFYKEY:
            receivePacket->sectorNumber = receivePacket->data[0];
            for(i=0; i<6; i++)
                receivePacket->key[i] = receivePacket->data[i+1];
            break;
        case COM_READBLOCK:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            break;
        case COM_WRITEBLOCK:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            for(i=0; i<16; i++)
                receivePacket->blockData[i] = receivePacket->data[i+2];
            break;
        case COM_INCREMENT:
        case COM_DECREMENT:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            for(i=0; i<4; i++)
                receivePacket->blockValue[i] = receivePacket->data[i+2];
            break;
        }
    }
    else if(receivePacket->type == TYPE_CPU_CONTACT)
    {
        switch(receivePacket->instruction)
        {
        case CPU_APDUSEND_T0:
        case CPU_APDUSEND_T1:
            receivePacket->APDULength = receivePacket->data[0];
            receivePacket->APDULength <<= 8;
            receivePacket->APDULength |= receivePacket->data[1];
            for(i=0; i<receivePacket->APDULength; i++)
                receivePacket->APDU[i] = receivePacket->data[i+2];
            break;
        case CPU_DESELECT:
            break;
        }
    }
    else if(receivePacket->type == TYPE_CPU_PROXI)
    {
        switch(receivePacket->instruction)
        {
        case PROXIMITY_CPU_SENDRATS:
            receivePacket->RATS = receivePacket->data[0];
            break;
        }
    }
    return TRUE;
}

void complete_Packet(Packet *receivePacket)
{
	 unsigned int i ;
  if(receivePacket->type == TYPE_M1)
    {
        switch(receivePacket->instruction)
        {
        case COM_FINDCARD:
        case COM_GETUID:
            break;
        case COM_VERIFYKEYA:
        case COM_VERIFYKEYB:
        case COM_MODIFYKEY:
            receivePacket->sectorNumber = receivePacket->data[0];
            for(i=0; i<6; i++)
                receivePacket->key[i] = receivePacket->data[i+1];
            break;
        case COM_READBLOCK:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            break;
        case COM_WRITEBLOCK:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            for(i=0; i<16; i++)
                receivePacket->blockData[i] = receivePacket->data[i+2];
            break;
        case COM_INCREMENT:
        case COM_DECREMENT:
            receivePacket->sectorNumber = receivePacket->data[0];
            receivePacket->blockNumber	= receivePacket->data[1];
            for(i=0; i<4; i++)
                receivePacket->blockValue[i] = receivePacket->data[i+2];
            break;
        }
    }
    else if(receivePacket->type == TYPE_CPU_CONTACT)//�Ӵ�ʽCPU��
    {
        switch(receivePacket->instruction)
        {
        case CPU_APDUSEND_T0:
        case CPU_APDUSEND_T1:
            receivePacket->APDULength = receivePacket->data[0];
            receivePacket->APDULength <<= 8;
            receivePacket->APDULength |= receivePacket->data[1];
            for(i=0; i<receivePacket->APDULength; i++)
                receivePacket->APDU[i] = receivePacket->data[i+2];
            break;
        case CPU_DESELECT:
            break;
        }
    }
    else if(receivePacket->type == TYPE_CPU_PROXI)//�ǽӴ�CPU��
    {
        switch(receivePacket->instruction)
        {
        case PROXIMITY_CPU_SENDRATS:
            receivePacket->RATS = receivePacket->data[0];
            break;
        }
    }
}	