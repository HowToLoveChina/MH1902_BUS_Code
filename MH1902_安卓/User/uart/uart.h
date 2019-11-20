
#ifndef MHSMCU_UART
#define MHSMCU_UART
#include "type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


#define RECV_INT  (BIT0)
#define SEND_INT  (BIT1 | BIT7)


#define UART_SEND_BUF_SIZE      256
#define UART_RECV_BUF_SIZE      256

    typedef struct uart_param_s
    {
        int     tx_transmited:1;        //���ݴ��������ǣ�0��ʾ���ڴ������ݣ�1��ʾ������������Կ�ʼ������һ���ֽ�
        int     baudrate;                //���ڲ�����
        struct {
            int read_index;             //�жϷ�������
            int write_index;            //д����������
            //u8 send_bytes;            //�����ֽ���
            //u8 send_int;              //������������жϵĴ���
            //u32 cnt;
            volatile int bytes;                 //�������е��ֽ���
            uint8_t buf[UART_SEND_BUF_SIZE];    //���ͻ�����
        } send;
        struct {
            int read_index;             //������������
            int write_index;            //�ж�д����������
            volatile int bytes;                 //�������е��ֽ���
            //u32   cnt;            //�����ֽ���
            //u32   error_bytes;            //żУ������ֽ���
            uint8_t buf[UART_RECV_BUF_SIZE];    //���ջ�����
            int overflow;           //���ջ�����������
        } recv;
    } uart_param;

		
		typedef struct 
		{
			/*unsigned char  Head ;//����ͷ 
			unsigned char  Type;
      unsigned char  Instruction; 			
			unsigned char  DataLen;
			unsigned char  Tail;
			unsigned char  EndFlg;
			unsigned char  RecSucc;
			unsigned int   Num; //�Ѿ��������ݳ��� 
			unsigned char  BCC;//BCC ����*/
			
			
			unsigned char  Head_flag;//����ͷ��־λ
			unsigned char  sn_flag; //sn�� ��־λ
      unsigned char  unuse_flag;//Ԥ���ֽڱ�־λ			
      unsigned char  command_flag;//��������	
      unsigned char  datalen_flag;//���ݳ����ֽڱ�־λ			
			unsigned int   datalen;//�Ѿ��������ݳ���
			unsigned char  crc16_flag;			
			unsigned char  tail;//������ʶ��
			unsigned char  receive_ok;//�������
			
		}Errorflag;

		
    void uart_Config(uint32_t baudrate, uint32_t parity);
   
    unsigned char UART0_GetChar(void);
    void UART0_SendByte(unsigned char DAT);
		void protocolflag_init(void);
		void uart_cmd_Receive(unsigned char *buf,unsigned short *len); 
    void UART0_SendBuf(unsigned char *pbuf,unsigned short len);

#ifdef __cplusplus
}
#endif

#endif

