#include "mhscpu.h"
#include "delay.h"
#include "uart.h"
#include "Ymodem.h"
#include "Iap_Flash.h"
#include "Download.h"

#include "core_cmFunc.h" //���ö�ջָ�뺯��

extern unsigned char file_name[FILE_NAME_LENGTH];
unsigned char tab_1024[1024] = {0};
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
/*************************************************************
  Function   : IAP_JumpToApplication
  Description: ��ת����������
  Input      : none
  return     : none
*************************************************************/
void IAP_JumpToApplication(void)
{
    unsigned int JumpAddress;//��ת��ַ

	   //printf("addr= %ld\r\n",(*(__IO unsigned int *)IAP_ADDR));
    if(((*(__IO unsigned int *)IAP_ADDR) & 0x2FFE0000) == 0x20000000)//���������룬IAP_ADDR��ַ����Ӧָ������ջ������0x20000000
    {  
        JumpAddress = *(__IO unsigned int *)(IAP_ADDR + 4);//��ȡ��λ��ַ
        Jump_To_Application = (pFunction)JumpAddress;//����ָ��ָ��λ��ַ
        __set_MSP(*(__IO unsigned int*)IAP_ADDR);//��������ջָ��MSPָ����������IAP_ADDR
        Jump_To_Application();//��ת���������봦
    }
}
/*************************************************************
  Function   : DownloadFirmware
  Description: ���������̼�
  Input      : none
  return     : none
*************************************************************/
void DownloadFirmware(void)
{
    unsigned char number[10]= "          ";        //�ļ��Ĵ�С�ַ���
    int size = 0;
	  IAP_SerialSendStr("\r\nWaiting for the file to be send...(press 'a' or 'A' to abort)\r\n");
    size = YModem_Receive(&tab_1024[0]);//��ʼ������������
    delayms(1000);//��ʱ1s����secureCRT���㹻ʱ��ر�ymodem�Ի�������Ӱ���������Ϣ��ӡ
    if(size > 0)
    {
        IAP_SerialSendStr("+-----------------------------------+\r\n");
        IAP_SerialSendStr("Proramming completed successfully!\r\nName: ");
        IAP_SerialSendStr(file_name);//��ʾ�ļ���
        YModem_Int2Str(number, size);
        IAP_SerialSendStr("\r\nSize:");
        IAP_SerialSendStr(number);//��ʾ�ļ���С
        IAP_SerialSendStr("Bytes\r\n");
        IAP_SerialSendStr("+-----------------------------------+\r\n");
        IAP_JumpToApplication();
    }
    else if(size == -1)//�̼��Ĵ�С������������flash�ռ�
    {
        IAP_SerialSendStr("The image size is higher than the allowed space memory!\r\n");
    }
    else if(size == -2)//������д���ɹ�
    {
        IAP_SerialSendStr("Verification failed!\r\n");
    }
    else if(size == -3)//�û���ֹ
    {
        IAP_SerialSendStr("Aborted by user!\r\n");
    }
    else //��������
    {
        IAP_SerialSendStr("Failed to receive the file!\r\n");
    }
}
