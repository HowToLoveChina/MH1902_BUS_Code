#include "mh523.h"
#include "uart.h"
#include "stdio.h"

#include "debug.h"//���Խӿ� ͷ�ļ�

#include "jrreader.h"//���ڶ�����Э��ͷ�ļ�

#include "gpio.h"//Beep led����

#include "24cxx.h"//E2PROM 
 
#include "iso14443_4.h"
#include "iso14443a.h"
#include "iso14443b.h"//��Ƶ�����ͷ�ļ�
#include "ContactlessAPI.h"//�ǽӴ�ʽ��APDU�ӿ�ͷ�ļ�

#include "7816.h"
#include "mhscpu_sci.h"//
#include "iso7816_3.h"//�Ӵ�ʽ��ͷ�ļ�
#include "emv_errno.h"
#include "emv_hard.h"

#include "Packet.h"
#include "define.h" //��λ��ͨѶЭ�����

#include "psam.h" //PSAM ͷ�ļ� 
#include "psamAPI.h"//PSAM APDU�ӿ�ͷ�ļ�

#include "crc.h"//���CRC�㷨

#include "rtc.h"//rtcʱ��
#include "bpk.h"//���ݼĴ���
#include "Digital.h"//�����
#include "led.h" //LED�� 
#include "key.h"//����

extern Packet receivePacket;//��λ��ͨ�Ž��սṹ��
extern Packet sendPacket;//��λ��ͨ�ŷ��ͽṹ��
extern MH523TypeDef MH523;//����оƬͨ����ؽṹ��
extern Errorflag ProtocolFlag;//Э����ձ�־λ

unsigned char PCardSN[4]= {0}; //4λ����
unsigned char CardIN = 0;
unsigned char KeyVoice = 1;//������


void UploadCardSn()
{
    unsigned char checkNum = 0;//CRC������
    unsigned char status;
    unsigned char s[200];
    unsigned char Ctype;
    status = pro_GetCardID(PCardSN);
    if(status==0)
    {
        //	printf("�ӿ��ϱ�--->\n");
        //CardIN = 1;//�п�Ƭ���� 
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x33;
        s[7]= 0x00;
        s[8]= 0x06;
        s[9]= RC_DATA;

        Ctype=CaptureCardType(); //���ؿ���
        if(Ctype==M1_CARD)           s[10]=0x01;    //M1��
        else if(Ctype==CPUAPP)       s[10]=0x02;    //����ҵ��
        else if(Ctype==UNION_PAY)    s[10]=0x03;    //�����п�
        else if(Ctype==UNIONPAYCPU)  s[10]=0x04;    //�������п�
        else
        {
            s[10]=0x0F;
        }		
        memcpy(s+11,PCardSN,4);
        checkNum = crc16(s+1,14);
        s[15]= checkNum;
        s[16]= 0x03;
        UART0_SendBuf(s,17);
    }
    else
    {
			 //CardIN = 0;
       //printf("no card \r\n");
    }
}
void SendKeyValue()
{
    unsigned char checkNum = 0;//CRC������
    unsigned char KeyValue = 0;
    unsigned char s[50];
    
    KeyValue = GetKey();
	  if(KeyValue !=0)//�м�����
		{
		    s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x70;
        s[7]= 0x00;
        s[8]= 0x02;
        s[9]= 0x00;		    
			  switch(KeyValue)
				{
				  case 1:s[10]=0x35;break;//touch
				  case 2:s[10]=0x31;break;//down
				  case 3:s[10]=0x30;break;//up
				  case 4:s[10]=0x32;break;//ok
					case 5:s[10]=0x33;break;//fun
					case 6:s[10]=0x34;break;//dowm+fun
				}
		    checkNum = crc16(s+1,10);
		    s[11]= checkNum;
        s[12]= 0x03;
        UART0_SendBuf(s,13);
				if(KeyVoice)beep();//��������ѡ
				
		}
}	

void SerJRReaderHandle(Packet *recPacket)
{

    unsigned char checkNum = 0;//CRC������
    unsigned char Status=0;
    unsigned short ren=0;
    unsigned char  UARTRevbuf[300]= {0};
    unsigned char s[200]= {0};
		unsigned char i = 0;
		unsigned int cost = 0;
		//unsigned char UARTRevbuf[500],okflag=0;	
//    checkNum = crc16(recPacket->packet_buf,recPacket->packrt_buf_len);
//    if(checkNum != recPacket->crc16)
//    {
//        printf("crc error! rec:%02X now:%02X\r\n",recPacket->crc16,checkNum);
//        return;
//    }
    switch(recPacket->command)
    {
    case 0x08://ECHO
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"��ͨ",4);
        s[6]=0x08;
        s[7]= recPacket->length>>8;
        s[8]= recPacket->length & 0xFF;
        memcpy(s+10,recPacket->data,recPacket->length);
        checkNum = crc16(s+1,9+recPacket->length);//CRCУ��
        s[10+recPacket->length]= checkNum;
        s[11+recPacket->length]= 0x03;
        UART0_SendBuf(s,12+recPacket->length);
        break;
    case 0x30:
        ;
        break;
    case 0x34:
        if(recPacket->data[0] == 0x04)//�ǽӿ�
        {
					  //ouputRes("APDU:",recPacket->data,recPacket->length);
            Status = pro_APDU(&recPacket->data[1],recPacket->length-1,UARTRevbuf,&ren);
            if(Status==0)s[9]= RC_DATA;
            else s[9]=RC_FAILURE;
            s[0]=STX;
            s[1]=0x01;
            memcpy(s+2,"��ͨ",4);
            s[6]=0x34;
            s[7]= (ren+1)>>8;
            s[8]= ren+1;
            memcpy(s+10,UARTRevbuf,ren);
            checkNum = crc16(s+1,9+ren);//CRCУ��
            s[10+ren]= checkNum;
            s[11+ren]= 0x03;
            //Uart1Send(s,12+ren);
            UART0_SendBuf(s,12+ren);
        }
        else
        {
            if(recPacket->data[0]==1)//����1 
            {
                selPsam=1;
            }
            else if(recPacket->data[0]==2)//����2
            {
                selPsam=2;
            }
            else if(recPacket->data[0]==3)//����3 Ӳ��PSAM���ӿ�
            {
                selPsam=3;
            }
            else
            {
                printf("error psam num!\r\n");
            }
										
						//cost = get_tick();
						Status=pro_APDU_PSAM(&recPacket->data[1],recPacket->length-1,UARTRevbuf,&ren);		
						//printf("cost1=%dms\r\n",get_tick()-cost);
            if(Status==0)s[9]= RC_DATA;
            else s[9]=RC_FAILURE;

            s[0]=STX;
            s[1]=0x01;
            memcpy(s+2,"\x0E\x01\x0b\x01",4);
            s[6]=0x34;
            s[7]= (ren+1)>>8;
            s[8]= ren+1;
            memcpy(s+10,UARTRevbuf,ren);
            checkNum = crc16(s+1,9+ren);
            s[10+ren]= checkNum;
            s[11+ren]= 0x03;
            UART0_SendBuf(s,12+ren);
        }
        break;
				
    case 0x35://�Ӵ�ʽ�� �ϵ�
        if(recPacket->data[0]==1)//����1
        {
            selPsam=1;
        }
        else if(recPacket->data[0]==2)//����2
        {
            selPsam=2;
        }
        else if(recPacket->data[0]==3)//����3
        {
            selPsam=3;
        }
        else
        {
            printf("no psam num!\r\n");
        }
				if(selPsam ==1 || selPsam ==2 )
				{
					if(psam_reset(selPsam))
					{
							s[9]=RC_DATA;
					}
					else s[9]=RC_FAILURE;
			  }
				else if(selPsam == 3)//Ӳ��PSAM���ӿ�
				{
				  if(tst_SCIWarmReset(0)==0)
					{
					   memcpy(psam_atr_tab,&atr_buf[1],atr_buf[0]);//atr
						 psam_atr_len = atr_buf[0];//atr ����
					   s[9]=RC_DATA;
					}
					else
					{
					   s[9]=RC_FAILURE;			
					}
				}
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x35;
        s[7]= (psam_atr_len+1)>>8;
        s[8]= psam_atr_len+1;

        memcpy(s+10,psam_atr_tab,psam_atr_len);

        checkNum = crc16(s+1,9+psam_atr_len); //�䨪?��??

        s[10+psam_atr_len]=checkNum;
        s[11+psam_atr_len]= 0x03;
        UART0_SendBuf(s,12+psam_atr_len);
        break;
				
    case 0x36: // ����/�رն����� ������Ƶ���ߣ�
        if(recPacket->data[0]==0x01)
        {
            pcd_antenna_on();//��������
            beep();
            // delay_ms(30);
        }
        else if(recPacket->data[0]==0x02)
        {
            pcd_antenna_off();//�ر�����
            beep();
        }
        else
        {
            printf("36--->\n");
        }
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x36;
        s[7]= 0x00;
        s[8]= 0x01;
        s[9]= RC_DATA;
        checkNum = crc16(s+1,9);
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
        break;
				
		case 0x37:// ����������		
				for(i = 0;i<recPacket->data[0];i++)
				{
				  beep();
				}	
        break;
		case 0x40:// �������ʾ����
				if(recPacket->data[0]==0x30)//�����
        {
            dig_Display(recPacket->data[1]<<8|recPacket->data[2]);//��ʾ��ֵ ��8λ��ǰ
        }
        else
        {
            printf("dig error--->\n");
        }
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x40;
        s[7]= 0x00;
        s[8]= 0x01;
        s[9]= RC_SUCCESS;
        checkNum = crc16(s+1,9);
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
        break;			
		case 0x43://��ȡʱ��
		    s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"��ͨ",4);
        s[6]=0x45;
        s[7]= 0x00;
        s[8]= 0x08;//�������ݳ���
		    s[9] = RC_SUCCESS;
		    s[10]= calendar.w_year >> 8;
        s[11]= calendar.w_year & 0xFF;	
        s[12]= calendar.w_month;
		    s[13]= calendar.w_date;
		    s[14]= calendar.hour;
        s[15]= calendar.min;	
		    s[16]= calendar.sec;
		    checkNum = crc16(s+1,16);
        s[17]=checkNum;
        s[18]= 0x03;
		     UART0_SendBuf(s,19);		
		break;			
		case 0x44://����ʱ��
			  writeBPK(1,0x80808080);
			  RTC_Set(recPacket->data[0]<<8|recPacket->data[1],recPacket->data[2],recPacket->data[3],\
		    recPacket->data[4],recPacket->data[5],recPacket->data[6]);
		    s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"��ͨ",4);
        s[6]=0x44;
        s[7]= 0x00;
        s[8]= 0x01;//�������ݳ���
			  s[9]= RC_SUCCESS;	
        checkNum = crc16(s+1,9);
				s[10]=checkNum;
				s[11]= 0x03;
				UART0_SendBuf(s,12);
		break;
		
		case 0x45://��ȡ����
        ReadJRreaderTerminalParam();//��ȡ����
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"��ͨ",4);
        s[6]=0x45;
        s[7]= 0x00;
        s[8]= 0x17;//�������ݳ���
        s[9]= RC_DATA;
		    s[10] =0x00;
		    s[11]= 0x01;
		    s[12]= 0x06;	
		    memcpy(&s[13],stTerminalParam.SoftVersion,fSoftVersion_len);//����汾
		    s[19]= 0x00;
		    s[20]= 0x02;
		    s[21]= 0x06;
		    memcpy(&s[22],stTerminalParam.HardVersion,fHardVersion_len);//Ӳ���汾
		    s[28]= 0x00;
		    s[29]= 0x03;
		    s[30]= 0x01;
		    s[31]=stTerminalParam.HardError;
		    checkNum = crc16(s+1,31);
        s[32]=checkNum;
        s[33]= 0x03;
        UART0_SendBuf(s,34);	
		 break;		
		 
		case 0x46:	//��������汾
		    s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"��ͨ",4);
        s[6]=0x46;
		    s[7]= 0x00;
			  s[8]= 0x01;//�������ݳ���
		   if(((recPacket->data[0]<<8)+recPacket->data[1])==1)//ֻ��1������
			 {
			    if(recPacket->data[2]==0x00 && recPacket->data[3] == 0x01)//��������汾
					{
						SetSoftVersionStat(&recPacket->data[5]);//���ò���	
						ReadJRreaderTerminalParam();//��ȡ����
						if(0!=(Mystrcmp(stTerminalParam.SoftVersion,&recPacket->data[5],recPacket->data[4])))
						{
                s[9]= RC_FAILURE;	
						}
						else
						{
						    s[9]= RC_SUCCESS;
						}	
					}
					else if(recPacket->data[2]==0x00 && recPacket->data[3] ==0x02)
					{
						SetHardVersionStat(&recPacket->data[5]);	
						ReadJRreaderTerminalParam();//��ȡ����
						if(0!=(Mystrcmp(stTerminalParam.HardVersion,&recPacket->data[5],recPacket->data[4])))
						{
                s[9]= RC_FAILURE;	
						}
						else
						{
						    s[9]= RC_SUCCESS;
						}	
					}
			 }
       else if(((recPacket->data[0]<<8)+recPacket->data[1])==2)//2������
			 {   
			    if(recPacket->data[2]==0x00 && recPacket->data[3] ==0x01)//��������汾
					{
						SetSoftVersionStat(&recPacket->data[5]);	
					}
					if(recPacket->data[11]==0x00 && recPacket->data[12] ==0x02)
					{
						SetHardVersionStat(&recPacket->data[14]);	
					}	
          ReadJRreaderTerminalParam();//��ȡ����
					if(0!=(Mystrcmp(stTerminalParam.SoftVersion,&recPacket->data[5],recPacket->data[4])))
				  {
                s[9]= RC_FAILURE;	
					}
					else
				  {
						if(0!=(Mystrcmp(stTerminalParam.HardVersion,&recPacket->data[14],recPacket->data[13])))
						{
                s[9]= RC_FAILURE;	
						}
						else
						{
						    s[9]= RC_SUCCESS;
						}		
					}		
			 }
			 checkNum = crc16(s+1,9);
			 s[10]=checkNum;
			 s[11]= 0x03;
			 UART0_SendBuf(s,12);	 
		break;
		case 0x52://��ȡ���ڲ�����
		    stTerminalParam.baudrate =115200;
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x52;
		    if(stTerminalParam.baudrate == 115200)
				{
				  s[10]=0x00;
				}
			  else if(stTerminalParam.baudrate == 57600)
				{
				  s[10]=0x01;
				}
		    else if(stTerminalParam.baudrate == 38400)
				{
				  s[10]=0x02;
				}
			  else if(stTerminalParam.baudrate == 28800)
				{
				  s[10]=0x03;
				}		
			  else if(stTerminalParam.baudrate == 19200)
				{
				  s[10]=0x04;
				}	
        else
        {
				  s[7]= 0x00;
					s[8]= 0x01;//�������ݳ���
					s[9]= RC_FAILURE;	
          checkNum = crc16(s+1,9);
					s[10]=checkNum;
					s[11]= 0x03;
					UART0_SendBuf(s,12);	
          break;					
				}					
				s[7]= 0x00;
				s[8]= 0x02;//�������ݳ���
				s[9]= RC_SUCCESS;	
        checkNum = crc16(s+1,10);
				s[11]=checkNum;
				s[12]= 0x03;
				UART0_SendBuf(s,13);
		    break; 	
		case 0x53:
			  ReadJRreaderTerminalParam(); 
		    s[0]=STX;
				s[1]=0x01;
				memcpy(s+2,"��ͨ",4);
				s[6]=0x53;
        if(recPacket->data[0] == 0x00)
				{
				  stTerminalParam.baudrate = 115200;
				}
			  else if(recPacket->data[0] == 0x01)
				{
				  stTerminalParam.baudrate = 57600;
				}
		    else if(recPacket->data[0] == 0x02)
				{
				  stTerminalParam.baudrate = 38400;
				}
			  else if(recPacket->data[0] == 0x03)
				{
				  stTerminalParam.baudrate = 28800;
				}		
			  else if(recPacket->data[0] == 0x04)
				{
				  stTerminalParam.baudrate = 19200;
				}	
        else
        {
				  s[7]= 0x00;
					s[8]= 0x01;//�������ݳ���
					s[9]= RC_FAILURE;	
          checkNum = crc16(s+1,9);
					s[10]=checkNum;
					s[11]= 0x03;
					UART0_SendBuf(s,12);	
          break;					
				}	
				//SetJRreaderTerminalParam(&stTerminalParam);
				s[7]= 0x00;
			  s[8]= 0x01;
			  checkNum = crc16(s+1,9);
				s[9]= RC_SUCCESS;	
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
				uart_Config(stTerminalParam.baudrate, UART_Parity_No);//�������� bps
        break;
								
//11 09 00 00 03 00 00a1 A00000000301201212310101C696034213D7D8546984579D1D0F0EA519CFF8DE0
//����(1)+����֯ID(4)+����(1)+����(2)
		case 0x61:		
       if(recPacket->data[0] == 0x11)//����CA
			 {
			   if(recPacket->data[3] == 0x00 && recPacket->data[4]==0x03)
				 {
				   SaveConfigData(0x8000+recPacket->data[5]*0x200,&recPacket->data[6],recPacket->data[6]*256+recPacket->data[7]+2);
		       ReadConfigData(0x8000+recPacket->data[5]*0x200,&UARTRevbuf[0],recPacket->data[6]*256+recPacket->data[7]+2);	 
//					for(i=0;i<UARTRevbuf[0]*256+UARTRevbuf[1]+2;i++)
//					{
//					   printf("%.2X ",UARTRevbuf[i+3]);
//					}
					if(Mystrcmp(UARTRevbuf,&recPacket->data[6],recPacket->data[6]*256+recPacket->data[7]+2)==0)
					{			
					   // printf("CA-SUC\n");
						 s[9] = RC_SUCCESS;
					}
					else
					{
					   s[9] = RC_FAILURE;
					}
				 }
         else
				 {
				     s[9] = RC_FAILURE;
				 }					 
			 }
			 else
			 {
			   s[9] = RC_FAILURE;
			 }
			  s[0]=STX;
				s[1]=0x01;
				memcpy(s+2,"��ͨ",4);
				s[6]=0x61;
				s[7]= 0x00;
			  s[8]= 0x01;
			  checkNum = crc16(s+1,9);
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
       break;
		case 0x68:
			
//AID������ 27 ��ʼ��		
//11 09 00 00 00 01 27 A0000000032010000140D84000A800D84004F8000010000000000000000000000099999F370400		
		  if(recPacket->data[0] == 0x11)//����AID
			{
					if(recPacket->data[3] == 0x00 && recPacket->data[4]==0x00)
					{
						if(recPacket->data[6]==0x27)
						{
							SaveConfigData(0x7000+recPacket->data[5]*0x40,&recPacket->data[6],40);
							ReadConfigData(0x7000+recPacket->data[5]*0x40,&UARTRevbuf[0],40);
							if(Mystrcmp(UARTRevbuf,&recPacket->data[6],40)==0)
							{
								s[9] = RC_SUCCESS;
							}
						}
						else if(recPacket->data[6]==0x28)
						{
							SaveConfigData(0x7000+recPacket->data[5]*0x40,&recPacket->data[6],41);
							ReadConfigData(0x7000+recPacket->data[5]*0x40,&UARTRevbuf[0],41);
							if(Mystrcmp(UARTRevbuf,&recPacket->data[6],41)==0)
							{
								s[9] = RC_SUCCESS;
							}
						}
						else if(recPacket->data[6]<=0x40)//14-08-04
						{
							SaveConfigData(0x7000+recPacket->data[5]*0x40,&recPacket->data[6],recPacket->data[6]);
							ReadConfigData(0x7000+recPacket->data[5]*0x40,&UARTRevbuf[0],recPacket->data[6]);
							if(Mystrcmp(UARTRevbuf,&recPacket->data[3],recPacket->data[6])==0)
							{
								s[9] = RC_SUCCESS;
							}
						}					 
				 }
         else
				 {
				     s[9] = RC_FAILURE;
				 }					 
			 }
			 else
			 {
			   s[9] = RC_FAILURE;
			 }
				s[0]=STX;
				s[1]=0x01;
				memcpy(s+2,"��ͨ",4);
				s[6]=0x68;
				s[7]= 0x00;
				s[8]= 0x01;
				checkNum = crc16(s+1,9);
				s[10]=checkNum;
				s[11]= 0x03;
				UART0_SendBuf(s,12);
				break;		

      case 0x71:// LED
				if(recPacket->data[1]==0x30)//ָʾ��
        {
					switch(recPacket->data[3])//ѡ��ĳ��LED
					{
						case 0x30: LED_Control(LED1,recPacket->data[2]-0x30);break;
					  case 0x31: LED_Control(LED2,recPacket->data[2]-0x30);break;
						case 0x32: LED_Control(LED3,recPacket->data[2]-0x30);break;
						default:break;
					}         
        }
        else if(recPacket->data[1]==0x31)//�����
        {
            LED_Control(BGLED,recPacket->data[2]-0x30);//���Ʋ����
        }
				else
				{
				  printf("led type error--->\n");			
				}
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x71;
        s[7]= 0x00;
        s[8]= 0x01;
        s[9]= RC_SUCCESS;
        checkNum = crc16(s+1,9);
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
        break;	
		case 0x72:// ���� ������������
				if(recPacket->data[1]==0x31)//����
        {
       
        }
        else if(recPacket->data[1]==0x32)//������
        {
          if(recPacket->data[2]==0x30)  KeyVoice = 1;
					else if(recPacket->data[2]==0x31) KeyVoice = 0;//�رհ�����		
        }
        s[0]=STX;
        s[1]=0x01;
        memcpy(s+2,"\x0E\x01\x0b\x01",4);
        s[6]=0x72;
        s[7]= 0x00;
        s[8]= 0x01;
        s[9]= RC_SUCCESS;
        checkNum = crc16(s+1,9);
        s[10]=checkNum;
        s[11]= 0x03;
        UART0_SendBuf(s,12);
        break;	
	 
		} 
}  	






