#include "mhscpu.h"
#include <stdio.h>
#include <stdlib.h>//ʹ����atoi����
#include "delay.h"
#include "uart.h"
#include "Ymodem.h"
#include "Iap_Flash.h"


unsigned char file_name[FILE_NAME_LENGTH];


/*************************************************************
  Function   : IAP_SerialSendByte 
  Description: ���ڷ����ֽ�
  Input      : c-Ҫ���͵��ֽ�        
  return     : none    
*************************************************************/
void IAP_SerialSendByte(unsigned char c)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0,c);
}

/*************************************************************
  Function   : IAP_SerialSendStr 
  Description: ���ڷ����ַ���
  Input      : none        
  return     : none    
*************************************************************/
void IAP_SerialSendStr(unsigned char *s)
{
	while(*s != '\0')
	{
		IAP_SerialSendByte(*s);
		s++;
	}
}
/*************************************************************
  Function   : IAP_SerialGetByte 
  Description: ����һ���ֽ�����
  Input      : none        
  return     : ���ؽ��ֵ��0-û�н��յ����ݣ�1-���յ�����    
*************************************************************/
unsigned char IAP_SerialGetByte(unsigned char *c)
{
  if(UART_IsRXReceived(UART0))
	{
	  *c = UART_ReceiveData(UART0);
    return 1;		
	}
  return 0; 		
}
/*************************************************************
  Function   : YModem_RecvByte 
  Description: ymodem����һ���ֽ�
  Input      : c-��Ž��յ����ֽ� timeout-��ʱʱ��        
  return     : none    
*************************************************************/
signed char YModem_RecvByte(unsigned char *c, unsigned int timeout)
{
	while(timeout-- > 0)
	{
		if(IAP_SerialGetByte(c) == 1)
		{		
			return 0;
		}
 	
	}
	return -1;
}

/*************************************************************
  Function   : YModem_SendByte 
  Description: ����һ���ֽ�
  Input      : c-Ҫ���͵��ֽ�        
  return     : none    
*************************************************************/
void YModem_SendByte(unsigned char c)
{
	IAP_SerialSendByte(c);
}

/*************************************************************
  Function   : IAP_GetKey 
  Description: ��ȡ����ֵ
  Input      : none        
  return     : ���ؼ�ֵ    
*************************************************************/
unsigned char IAP_GetKey(void)
{
	unsigned char data;
	while(!IAP_SerialGetByte(&data)){ }
	return data;
}

/*************************************************************
  Function   : UpdateCRC16
  Description: ����һ���ֽڵ�CRC16У����(CRC16-CCITTŷ�ޱ�׼)
  Input      : crcIn-��һ�ε�CRC��
               byte-һ���ֽ�
  return     : ����crc��
*************************************************************/
unsigned int UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte|0x100;
    do
    {
        crc <<= 1;
        in <<= 1;
        if (in&0x100)
            ++crc;		//crc |= 0x01
        if (crc&0x10000)
            crc ^= 0x1021;
    }
    while (!(in&0x10000));
    return crc&0xffffu;
}
/*************************************************************
  Function   : Cal_CRC16
  Description: �������ݵ�CRC��
  Input      : data-Ҫ���������
               size-���ݵĴ�С
  return     : ���ؼ������CRC��
*************************************************************/
unsigned int Cal_CRC16(const uint8_t* data, uint32_t size)
{
    uint32_t crc = 0;
    const uint8_t* dataEnd = data+size;
    while (data<dataEnd)
        crc = UpdateCRC16(crc,*data++);
    crc = UpdateCRC16(crc,0);
    crc = UpdateCRC16(crc,0);
    return crc&0xffffu;
}
/*************************************************************
  Function   : Int2Str
  Description: ����ת�����ַ���
  Input      : str-�ַ���ָ�� intnum-ת��ֵ
  return     : none
*************************************************************/
void YModem_Int2Str(unsigned char* str, int intnum)
{
    unsigned int i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + '0';//����ת�����ַ�

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))//������ǰ���'0'
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}
/*************************************************************
  Function   : YModem_RecvPacket
  Description: ����һ�����ݰ�
  Input      : data-��Ž��յ�������
               length-���ݰ��ĳ���
			   timeout-��ʱʱ��
  return     :  0 -��������  -1 -���մ���
*************************************************************/
signed char YModem_RecvPacket(unsigned char *data, int *length, unsigned int timeout)
{
    unsigned int i, packet_size;
    unsigned char c;
    unsigned int CRC1=0;
    *length = 0;
	  
    if(YModem_RecvByte(&c, timeout) != 0)//�������ݰ��ĵ�һ���ֽ�
    {	  
        return -1;
    }
	
    switch(c)
    {
    case SOH:				//128�ֽ����ݰ�
        packet_size = PACKET_SIZE;	//��¼���ݰ��ĳ���
        break;
    case STX:				//1024�ֽ����ݰ�
        packet_size = PACKET_1K_SIZE;	//��¼���ݰ��ĳ���
        break;
    case EOT:				//���ݽ��ս����ַ�
        return 0;   //���ս���
    case CA:				//������ֹ��־
        if((YModem_RecvByte(&c, timeout) == 0) && (c == CA))//�ȴ�������ֹ�ַ�
        {
            *length = -1;		//���յ���ֹ�ַ�
            return 0;
        }
        else				//���ճ�ʱ
        {
            return -1;
        }
    case ABORT1:				//�û���ֹ���û�����'A'
    case ABORT2:				//�û���ֹ���û�����'a'
        return 1;                       //������ֹ
    default:
        return -1;                      //���մ���
    }
    *data = c;	                        //�����һ���ֽ�
    for(i = 1; i < (packet_size + PACKET_OVERHEAD); i++)//��������
    {
        if(YModem_RecvByte(data + i, timeout) != 0)//���ս�����1024+5���ֽ�
        {
            return -1;
        }
    }
    if(data[PACKET_SEQNO_INDEX] ==(~data[PACKET_SEQNO_COMP_INDEX]))//�������ŵķ�
    {
        return -1;                   //���մ���
    }
    if(data[0]==SOH)
    {
        if(Cal_CRC16(&data[3],128)!=(unsigned int)((unsigned int)data[131]*256+data[132]))//CRCУ��
        {
            return -1;
        }
    }
    if(data[0]==STX)
    {
        if(Cal_CRC16(&data[3],1024)!=(unsigned int)((unsigned int)data[1027]*256+data[1028]))//CRCУ��
        {
            return -1;
        }
    }

    *length = packet_size;               //������յ������ݳ���
    return 0;                            //��������
}

/*************************************************************
  Function   : YModem_Receive
  Description: ymodem����
  Input      : buf-��Ž��յ�������
  return     :  0 -���Ͷ˴�����ֹ
               -1 -�̼�����
			         -2 -flash��д����
			         -3 -�û���ֹ
*************************************************************/
int YModem_Receive(unsigned char *buf)
{  
    unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH];     	
	  unsigned char session_done, file_done, session_begin, packets_received, errors;
    unsigned char *file_ptr, *buf_ptr;
    int packet_length = 0;
	  unsigned int  size = 0;
    unsigned int i = 0,RamSource = 0;
    for (session_done = 0, errors = 0, session_begin = 0; ;)//��ѭ����һ��ymodem����
    {	    
        for (packets_received = 0, file_done = 0, buf_ptr = buf; ; )//��ѭ�������Ͻ�������
        {			 
            switch(YModem_RecvPacket(packet_data, &packet_length, NAK_TIMEOUT))//����һ�����ݰ�
            {
            case 0:
                errors = 0;//����0 ��ʾһ������
                switch(packet_length)
                {
                case -1: //���Ͷ���ֹ����
                    YModem_SendByte(ACK);//�ظ�ACK
                    return 0;
                case 0:	//���ս�������մ���
                    YModem_SendByte(ACK);
                    file_done = 1;//�������
                    break;
                default: //����������
                    if((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))//���ݰ���Ų���
                    {
                        YModem_SendByte(NAK);//���մ�������ݣ��ظ�NAK
                    }
                    else//���յ���ȷ������
                    {
                        if(packets_received == 0)//���յ�һ֡����
                        {
                            if(packet_data[PACKET_HEADER] != 0)//�����ļ���Ϣ���ļ������ļ����ȵ�
                            {
                                for(i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH); )
                                {//�ļ��������ݵ���λ��ʼ��\0����
                                    file_name[i++] = *file_ptr++;//�����ļ���
                                }
                                file_name[i++] = '\0';//�ļ�����'\0'����
																
                                for(i = 0, file_ptr++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH); )
                                {
                                    file_size[i++] = *file_ptr++;//�����ļ���С
                                }
                                file_size[i++] = '\0';//�ļ���С��'\0'����
                                size = atoi((const char *)file_size);//���ļ���С�ַ���ת��������
                                if(size > (0x80000-1))//�����̼����� FLASH_SIZE=0x80000  512k
                                {
                                    YModem_SendByte(CA);
                                    YModem_SendByte(CA);//��������2����ֹ��CA
                                    return -1;//����
                                }
                                IAP_FlashEease(size);//������Ӧ��flash�ռ�
                                IAP_UpdataParam(&size);//��size��С��д��Flash��Parameter��
                                YModem_SendByte(ACK);//�ظ�ACk
                                YModem_SendByte(CRC16);//����'C',ѯ������
                            }
                            else//�ļ������ݰ�Ϊ�գ���������
                            {
                                YModem_SendByte(ACK);//�ظ�ACK
                                file_done = 1;//ֹͣ����
                                session_done = 1;//�����Ի�
                                break;
                            }
                        }
                        else //�յ����ݰ�
                        {
                            memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);//��������
                            RamSource = (unsigned int)buf;//8λǿ��ת����32Ϊ����
                            if(IAP_UpdataProgram(RamSource, packet_length) != 0)        //��д��������
                            {
                                YModem_SendByte(CA);
                                YModem_SendByte(CA);//flash��д������������2����ֹ��CA
                                return -2;//��д����
                            }
                            YModem_SendByte(ACK);//flash��д�ɹ����ظ�ACK
                        }
                        packets_received++;//�յ����ݰ��ĸ���
                        session_begin = 1;//���ý����б�־
                    }
                }
                break;
            case 1:		                //�û���ֹ
                YModem_SendByte(CA);
                YModem_SendByte(CA);    //��������2����ֹ��CA
                return -3;		//��д��ֹ
            default:
                if(session_begin > 0)   //��������з�������
                {
                    errors++;
                }
                if(errors > MAX_ERRORS) //���󳬹�����
                {
                    YModem_SendByte(CA);
                    YModem_SendByte(CA);//��������2����ֹ��CA
                    return 0;	//������̷����������
                }
                YModem_SendByte(CRC16); //����'C',��������
                break;
            }
            if(file_done != 0)//�ļ�������ϣ��˳�ѭ��
            {
                break;
            }
        }
        if(session_done != 0)//�Ի�����������ѭ��
        {
            break;
        }
    }
    return (int)size;//���ؽ��յ��ļ��Ĵ�С
}


