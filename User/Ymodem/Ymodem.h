#ifndef Ymodem_H
#define Ymodem_H

/* /-------- Packet in IAP memory ------------------------------------------\
 * | 0      |  1    |  2     |  3   |  4      | ... | n+4     | n+5  | n+6  |
 * |------------------------------------------------------------------------|
 * | unused | start | number | !num | data[0] | ... | data[n] | crc0 | crc1 |
 * \------------------------------------------------------------------------/
 * the first byte is left unused for memory alignment reasons                 */

/*
���Ͷ�                                         ���ն�
 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    C
SOH 00 FF "foo.c" "1064'' NUL[118] CRC CRC >>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<     ACK
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<     C
STX 01 FE data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>>      
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
STX 02 FD data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
STX 03 FC data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
STX 04 FB data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
SOH 05 FA data[100]  1A[28] CRC CRC>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
EOT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    NAK
EOT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    C
SOH 00 FF NUL[128] CRC CRC >>>>>>>>>>>>>>>>>>>>>>>
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    ACK

*/


#include "mhscpu.h"

#define PACKET_SEQNO_INDEX      (1)       //���ݰ����
#define PACKET_SEQNO_COMP_INDEX (2)       //����ȡ��

#define PACKET_HEADER           (3)	  //�ײ�3λ
#define PACKET_TRAILER          (2)	  //CRC�����2λ
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)//3λ�ײ�+2λCRC
#define PACKET_SIZE             (128)     //128�ֽ�
#define PACKET_1K_SIZE          (1024)    //1024�ֽ�

#define FILE_NAME_LENGTH        (256)     //�ļ���󳤶�
#define FILE_SIZE_LENGTH        (16)      //�ļ���С

#define SOH                     (0x01)    //128�ֽ����ݰ���ʼ
#define STX                     (0x02)    //1024�ֽڵ����ݰ���ʼ
#define EOT                     (0x04)    //��������
#define ACK                     (0x06)    //��Ӧ
#define NAK                     (0x15)    //û��Ӧ
#define CA                      (0x18)    //�����������ֹת��
#define CRC16                   (0x43)    //'C'��0x43, ��Ҫ 16-bit CRC 

#define ABORT1                  (0x41)    //'A'��0x41, �û���ֹ 
#define ABORT2                  (0x61)    //'a'��0x61, �û���ֹ

#define NAK_TIMEOUT             (0x100000U)//���ʱʱ��
#define MAX_ERRORS              (5)       //��������

void IAP_SerialSendByte(unsigned char c);
void IAP_SerialSendStr(unsigned char *s);
unsigned char IAP_SerialGetByte(unsigned char *c);
signed char YModem_RecvByte(unsigned char *c, unsigned int timeout);
void YModem_SendByte(unsigned char c);
unsigned char IAP_GetKey(void);


unsigned int UpdateCRC16(uint16_t crcIn, uint8_t byte);
unsigned int Cal_CRC16(const uint8_t* data, uint32_t size);
void YModem_Int2Str(unsigned char* str, int intnum);
signed char YModem_RecvPacket(unsigned char *data, int *length, unsigned int timeout);
int YModem_Receive(unsigned char *buf);

#endif
