////AT24C512�ڲ���512ҳ��ÿҳΪ128�ֽڣ����ⵥԪ�ĵ�ַΪ16λ
////��ַ��ΧΪ0000H-FFFFH���ɰ�ҳ���ֽ���ʾ

#include "24cxx.h"
#include "delay.h"
#include "delay.h"//��������ͷ�ļ�
#include "mhscpu.h"
#include "psamAPI.h"
#include "stdio.h"


STTERMINALPARAM  stTerminalParam;//�ն˲���


#if 0
void iic_dir_set(GPIO_TypeDef* GPIOx,unsigned short GPIO_Pin, unsigned char in_or_out)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    //===================================================
    // iic SCL��SDA����ѡ��  SCL==P14       SDA==PC15
    //===================================================
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    //===================================================
    //   ��������ѡ��
    //===================================================
    if (in_or_out == 0)//���
    {
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
    else//����
    {
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}
#endif



//��ʼ��24C512�ӿ�
void IIC_Init(void)
{
    //========================================
    // SCL = PC14   SDA=PC15
    //========================================
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    IIC_SCL_H;
    IIC_SDA_H;

}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();//sda�����
    IIC_SDA_H;
    IIC_SCL_H;
    nop;
    IIC_SDA_L;//START:when CLK is high,DATA change form high to low
    nop;
    IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ��������
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL_L;
    IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
    nop;
    IIC_SCL_H;
    nop;
    IIC_SDA_H;//����I2C���߽����ź�
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
unsigned char IIC_Wait_Ack(void)
{
    unsigned char ucErrTime=0;
    SDA_IN();      //SDA����Ϊ����
    IIC_SDA_H;
    nop;
    IIC_SCL_H;
    nop;
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_L;//ʱ�����0
    return 0;
}

//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_L;
    nop;
    IIC_SCL_H;
    nop;
    IIC_SCL_L;
}

//������ACKӦ��
void IIC_NAck(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_H;
    nop;
    IIC_SCL_H;
    nop;
    IIC_SCL_L;
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(unsigned char txd)
{
    unsigned char t;
    SDA_OUT();
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
        if((txd&0x80)>>7)IIC_SDA_H;
        else IIC_SDA_L;
        txd<<=1;
        nop;   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_H;
        nop;
        IIC_SCL_L;
        nop;
    }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
    for(i=0; i<8; i++ )
    {
        IIC_SCL_L;
        nop;
        IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;
        nop;
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
    IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
unsigned char AT24CXX_ReadOneByte(unsigned short ReadAddr)
{
    unsigned char temp=0;
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);	   //����д����
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
    } else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1);           //�������ģʽ
    IIC_Wait_Ack();
    temp=IIC_Read_Byte(0);
    IIC_Stop();//����һ��ֹͣ����
    return temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);	    //����д����
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
    }
    else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);     //�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
    //delay_us(100);//��os����,���������delay us ����
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(unsigned short WriteAddr,unsigned int DataToWrite,unsigned char Len)
{
    unsigned char t;
    for(t=0; t<Len; t++)
    {
        AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
unsigned int AT24CXX_ReadLenByte(unsigned short ReadAddr,unsigned char Len)
{
    unsigned char t;
    unsigned int temp=0;
    for(t=0; t<Len; t++)
    {
        temp<<=8;
        temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
    }
    return temp;
}

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
unsigned char AT24CXX_Check(void)
{
    unsigned char temp;
    temp=AT24CXX_ReadOneByte(65535);//����ÿ�ο�����дAT24CXX
    if(temp==0X55)return 0;
    else//�ų���һ�γ�ʼ�������
    {
        AT24CXX_WriteOneByte(65535,0X55);
        temp=AT24CXX_ReadOneByte(65535);
        if(temp==0X55)return 0;
    }
    return 1;
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
    unsigned char temp=0;
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);	   //����д����
        if(IIC_Wait_Ack())  return(0);
        IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
    }
    else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д����
    if(IIC_Wait_Ack())  return(0);
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ

    if(IIC_Wait_Ack())  return(0);
    IIC_Start();
    IIC_Send_Byte(0XA1);           //�������ģʽ
    if(IIC_Wait_Ack())  return(0);
    while(NumToRead)
    {
        if(NumToRead==1) *pBuffer=IIC_Read_Byte(0);//������,����nACK
        else *pBuffer=IIC_Read_Byte(1);		//������,����ACK
        NumToRead--;
        pBuffer++;
    }

    IIC_Stop();//����һ��ֹͣ����
    return 1;
}
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
    unsigned char i;
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);	    //����д����
        if(IIC_Wait_Ack())  return(0);
        IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
    }

    else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д����

    if(IIC_Wait_Ack())  return(0);
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
    if(IIC_Wait_Ack())  return(0);
    for(i=0; i<NumToWrite; i++)
    {
        IIC_Send_Byte(*pBuffer);     //�����ֽ�
        if(IIC_Wait_Ack())  return(0);
        pBuffer++;
    }
    IIC_Stop();//����һ��ֹͣ����
    return(1);
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
void SaveConfigDataNew( unsigned int StartAddr, unsigned char *s, unsigned int number )
{
    unsigned char a,b,i;

    if( (StartAddr%128) && ( ((StartAddr%128) + number)>128) )//��д���� �������趨��ַ��ʼ��һҳ��Χ13-03-18
    {
        b=128-(StartAddr%128);

        AT24CXX_Write(StartAddr,s,b);

        delayms(10);//////////////////////////////  3-4  5

        StartAddr+=b;//���µ�ַ
        s+=b;        //����buf��ַ

        number=number-b;
        a=number/128;
        b=number%128;
    }
    else
    {
        a=number/128;
        b=number%128;
    }
    for(i=0; i<a; i++)
    {
        AT24CXX_Write(StartAddr+i*128,s+i*128,128);
        //delay(1);
        delayms(10);  //3-5
    }
    if(b)
    {
        AT24CXX_Write(StartAddr+a*128,s+a*128,b);
    }
    delayms(10); //3-5
    //delay(2);
    //delay_us(10000);//��os����,���������delay us ����
}

////////////////////////////////////////////////////////////////////////////
void ReadConfigDataNew( unsigned int StartAddr, unsigned char *s, unsigned int number )
{
    unsigned char a,b,i;
    if( (StartAddr%128) && ( ((StartAddr%128) + number)>128) )//��д���� �������趨��ַ��ʼ��һҳ��Χ13-03-18
    {
        b=128-(StartAddr%128);
        AT24CXX_Read(StartAddr,s,b);

        delayms(10);

        StartAddr+=b; //���µ�ַ
        s+=b;         //����buf��ַ

        number=number-b;
        a=number/128;
        b=number%128;
    }
    else
    {
        a=number/128;
        b=number%128;
    }

    for(i=0; i<a; i++)
    {
        AT24CXX_Read(StartAddr+i*128,s+i*128,128);
        //I2C_ReadNByte (CATaddr,TWO_BYTE_SUBA, StartAddr+i*128, s+i*128,128);
        delayms(10);
    }
    if(b)
    {
        AT24CXX_Read(StartAddr+a*128,s+a*128,b);
        //I2C_ReadNByte (CATaddr,TWO_BYTE_SUBA, StartAddr+a*128, s+a*128,b);
    }
    //delay_ms(1);

}

void SaveConfigData( unsigned int StartAddr, unsigned char *s, unsigned int number )
{
    delayms(20);
    SaveConfigDataNew( StartAddr, s, number );
}
//
void ReadConfigData( unsigned int StartAddr, unsigned char *s, unsigned int number )
{
    delayms(20);
    ReadConfigDataNew( StartAddr, s, number );
}

/*********************************************************************************************************
** Function name:       IIC_Test
** Descriptions:        ���ڲ��Ժ���
** ע�⣺�ڶ�24C256����д����ʱ��ÿд��һҳʱҪ��ʱһ���ٽ�����һҳд
**       ��д�����ݣ��������̶����ݣ���ʱһ�²ſ��Զ�����������������ǲ��Ե�
*********************************************************************************************************/
unsigned char IIC_Test(void)
{

    int i;
    unsigned char temp[20]= {"01234567890123456789"};

    unsigned char testbuf[180]= {"\x00\xB2\xA0\x00\x00\x03\x33\x08\x20\x20\x12\x31\x01\x01\xB6\x16\x45\xED\xFD\x54\x98\xFB\x24\x64\x44\x03\x7A\x0F\xA1\x8C\x0F\x10\x1E\xBD\x8E\xFA\x54\x57\x3C\xE6\xE6\xA7\xFB\xF6\x3E\xD2\x1D\x66\x34\x08\x52\xB0\x21\x1C\xF5\xEE\xF6\xA1\xCD\x98\x9F\x66\xAF\x21\xA8\xEB\x19\xDB\xD8\xDB\xC3\x70\x6D\x13\x53\x63\xA0\xD6\x83\xD0\x46\x30\x4F\x5A\x83\x6B\xC1\xBC\x63\x28\x21\xAF\xE7\xA2\xF7\x5D\xA3\xC5\x0A\xC7\x4C\x54\x5A\x75\x45\x62\x20\x41\x37\x16\x96\x63\xCF\xCC\x0B\x06\xE6\x7E\x21\x09\xEB\xA4\x1B\xC6\x7F\xF2\x0C\xC8\xAC\x80\xD7\xB6\xEE\x1A\x95\x46\x5B\x3B\x26\x57\x53\x3E\xA5\x6D\x92\xD5\x39\xE5\x06\x43\x60\xEA\x48\x50\xFE\xD2\xD1\xBF\x03\xEE\x23\xB6\x16\xC9\x5C\x02\x65\x2A\xD1\x88\x60\xE4\x87\x87\xC0\x79\xE8\xE8\x5A\x72"} ;
    unsigned char temp1[300];
		
    SaveConfigDataNew(41984,testbuf,180);

		printf("AT24C512test!\n");	 
		
    ReadConfigDataNew(41984,testbuf,180);

		for(i=0;i<180;i++)printf("%.2x ",testbuf[i]); printf("\r\n");

    //if(temp1[9]==0x39) printf("IIC-Test-Well\n");

}


unsigned char E2PROM_Check()
{
   
   unsigned char temp[20]= {"01234567890123456789"};
	 unsigned char readbuf[20]={0}; 
   SaveConfigDataNew(0xF000,temp,20);
   ReadConfigDataNew(0xF000,readbuf,20);
	 if(Mystrcmp(temp,readbuf,20)!= 0)
	 {
	    
	   
	 
	 
	 }
	 

}

//eeprom
int getvar(unsigned char *obuf,int off,int len)
{
	
	ReadConfigData(off, obuf, len); 
	
	return 0;
}

int savevar(const char *ibuf,int off,int len)
{
	int i;
	
  SaveConfigData( off,(unsigned char *)ibuf, len ); 
	
	return 0;
}

/**
* @fn CalcLocalDesPubSaveVar
* @brief ������Ϣ�����籣������
*			�����ڲ��������api���ж�apiִ�еĳɹ����
*			��apiִ�в��ɹ����ٴ�ִ�У���ִ��20�ζ����ɹ���
*			���ش���
* @param in const char *psSaveBuf 		�豣�����ݵ�ָ��
* @param in const int nStart 			��ʼλ��
* @param in const int nLen 				����ĳ���
* @return li FILE_SUCC �ɹ�
*		li FILE_FAIL   ʧ��
*/

// savevar--> ������� �ϵ籣��
int PubSaveVar (const  char *psSaveBuf, const int nStart, const int nLen)
{
    int nSaveTimes = 0;

    for (;;)
    {
        if (savevar(psSaveBuf, nStart, nLen) == 0)// eeprom
        {
            break;
        }
        else
        {
            nSaveTimes ++;
            if(20 == nSaveTimes)
            {
                return 1;
            }
        }
    }
    return 0;
}

/**
* @fn PubGetVar
* @brief  �ӵ��籣������ȡ��Ϣ��
*			�����ڲ��������api���ж�apiִ�еĳɹ����
*			��apiִ�в��ɹ����ٴ�ִ�У���ִ��20�ζ����ɹ���
*			���ش���
* @param in char *psBuf	 			������Ϣ��ָ��
* @param in const int nStart 			��ʼλ��
* @param in const int nLen 				�����ĳ���
* @return li		FILE_SUCC �ɹ�
*		li FILE_FAIL   ʧ��
*/
int PubGetVar (unsigned char *psBuf, const int nStart, const int nLen)
{
    int nSaveTimes = 0;

    for (;;)
    {
        if (getvar(psBuf, nStart, nLen) == 0)
        {
            break;
        }
        else
        {
            nSaveTimes ++;
            if(20 == nSaveTimes)
            {
                return 1;
            }
        }
    }
    return 0;
}



// ��ȡ������ʶ
unsigned char GetProgramUpStat(unsigned char *ProgramUpStat)
{
	 *ProgramUpStat=stTerminalParam.ProgramUpState;
}

//����������ʶ 
unsigned char SetProgramUpStat(unsigned char ProgramUpStat)
{
	stTerminalParam.ProgramUpState=ProgramUpStat;
	PubSaveVar(&ProgramUpStat,fPosProgram_off,fPosProgram_len); 
}

// ��ȡ����汾��ʶ
unsigned char GetSoftVersionStat(unsigned char *softversion)
{
	 *softversion=(unsigned char)stTerminalParam.SoftVersion;
}

// ��������汾��ʶ
unsigned char SetSoftVersionStat(unsigned char *softversion)
{
	PubSaveVar((const char *)softversion,fSoftVersion_off,fSoftVersion_len);  
}


//��ȡӲ���汾��ʶ 
unsigned char GetHardVersionStat(unsigned char *hardversion)
{
	
	*hardversion=(unsigned char)stTerminalParam.SoftVersion;
}

//����Ӳ���汾��ʶ 
unsigned char SetHardVersionStat(unsigned char *hardversion)
{
	PubSaveVar((const char *)hardversion,fHardVersion_off,fHardVersion_len);  
}

//��ȡӲ�������ʶ 
unsigned char GetHardErrorStat(unsigned char *harderror)
{
	*harderror=(unsigned char)stTerminalParam.HardError;
}

//����Ӳ�������ʶ 
unsigned char SetHardErrorStat(unsigned char *harderror)
{
	PubSaveVar((const char *)harderror,fHard_Erroroff,fHard_Error_len);  
}

// ��ȡ����
void ReadJRreaderTerminalParam(void)
{
	PubGetVar(&stTerminalParam.ProgramUpState,fPosProgram_off,fPosProgram_len);//����
	PubGetVar(stTerminalParam.SoftVersion,fSoftVersion_off,fSoftVersion_len);//����汾
	PubGetVar(stTerminalParam.HardVersion,fHardVersion_off,fHardVersion_len);//Ӳ���汾
	PubGetVar(&stTerminalParam.HardError,fHard_Erroroff,fHard_Error_len);//Ӳ�������־
}

void parameter_init(void)//������ʼ��
{
  ReadJRreaderTerminalParam();//��ȡ����
  if(NULL==(strstr((const char*)stTerminalParam.SoftVersion,"jrs")))
  {
	   SetSoftVersionStat((unsigned char*)"jrs1.0");  
	}
  if(NULL==(strstr((const char*)stTerminalParam.HardVersion,"jrh")))
  {
	   SetHardVersionStat((unsigned char*)"jrh1.0");  
	}
}	











