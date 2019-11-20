#ifndef __24CXX_H
#define __24CXX_H
  
#include "rtc.h" 	

typedef struct
{
	unsigned char ProgramUpState;//������ʶ	1
	unsigned char SoftVersion[7];//����汾
	unsigned char HardVersion[7];//Ӳ���汾
	unsigned char HardError;//Ӳ�������־
	unsigned int  baudrate; //������
	
}STTERMINALPARAM;

extern  STTERMINALPARAM  stTerminalParam;//�ն˲���
 	   		   
//IO��������
#define SDA_IN()  {GPIOC->OEN |=1<<15;}	//PC15����ģʽ
#define SDA_OUT() {GPIOC->OEN &=~(1<<15);} //PC15���ģʽ
//IO��������	 
#define READ_SDA   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)  //����SDA 


#define IIC_SCL_H  GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define IIC_SCL_L  GPIO_ResetBits(GPIOC, GPIO_Pin_14)

#define IIC_SDA_H  GPIO_SetBits(GPIOC,GPIO_Pin_15)
#define IIC_SDA_L  GPIO_ResetBits(GPIOC, GPIO_Pin_15)

#define nop    __NOP 

//IIC���в�������
void IIC_Init(void);    //��ʼ��IIC��IO��				 
void IIC_Start(void);		//����IIC��ʼ�ź�
void IIC_Stop(void);	  //����IICֹͣ�ź�
void IIC_Send_Byte(unsigned char txd);	//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(void); 			//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź� 

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  
#define AT24C512	65535    //170228���

//ʹ�õ���24C512�����Զ���EE_TYPEΪAT24C512
#define EE_TYPE AT24C512

#define fNVR_Addr_Start			0x100 // �����洢��ʼ��ַ
#define fPosCheck_off			fNVR_Addr_Start
#define fPosCheck_len			8

// �����ʶ 
#define fPosProgram_off			fPosCheck_off+fPosCheck_len
#define fPosProgram_len			1

//����汾
#define fSoftVersion_off		fPosProgram_off+fPosProgram_len	
#define fSoftVersion_len		6

//Ӳ���汾
#define fHardVersion_off				fSoftVersion_off+fSoftVersion_len	
#define fHardVersion_len				6

//Ӳ�������־λ
#define fHard_Erroroff				fHardVersion_off+fHardVersion_len
#define fHard_Error_len				1


//Ԥ��
#define fFixValue_off		fIsLock_off+fIsLock_len
#define fFixValue_len		4

//Ԥ��
#define fTerminalId_off		fFixValue_off+fFixValue_len	
#define fTerminalId_len		8

//Ԥ��
#define fMerchantId_off		fTerminalId_off+fTerminalId_len	
#define fMerchantId_len		15


//Ԥ��
#define fTraceNo_off				fMerchantId_off+fMerchantId_len
#define fTraceNo_len				6  //2a��? ��??��?����?D�䨨????��


#define fNVR_Addr_End   fTraceNo_off+fReversalData_len

		  
unsigned char AT24CXX_ReadOneByte(unsigned short ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24CXX_WriteLenByte(unsigned short WriteAddr,unsigned int DataToWrite,unsigned char Len);//ָ����ַ��ʼд��ָ�����ȵ�����
unsigned int AT24CXX_ReadLenByte(unsigned short ReadAddr,unsigned char Len);					//ָ����ַ��ʼ��ȡָ����������
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

unsigned char AT24CXX_Check(void);  //�������
void AT24CXX_Init(void); //��ʼ��IIC


///////////////////////////////////////////////////
void SaveConfigDataNew( unsigned int StartAddr, unsigned char *s, unsigned int number );
void ReadConfigDataNew( unsigned int StartAddr, unsigned char *s, unsigned int number );
void SaveConfigData( unsigned int StartAddr, unsigned char *s, unsigned int number );
void ReadConfigData( unsigned int StartAddr, unsigned char *s, unsigned int number );
unsigned char IIC_Test(void);



unsigned char GetSoftVersionStat(unsigned char *softversion);// ��ȡ����汾��ʶ


unsigned char SetSoftVersionStat(unsigned char *softversion);// ��������汾��ʶ


unsigned char GetHardVersionStat(unsigned char *hardversion);//��ȡӲ���汾��ʶ 


unsigned char SetHardVersionStat(unsigned char *hardversion);//����Ӳ���汾��ʶ 


unsigned char GetHardErrorStat(unsigned char *harderror);//��ȡӲ�������ʶ 


unsigned char SetHardErrorStat(unsigned char *harderror);//����Ӳ�������ʶ 


void ReadJRreaderTerminalParam(void);// ��ȡ����

void parameter_init(void);//������ʼ��

#endif



















