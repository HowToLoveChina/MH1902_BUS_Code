#include "mh523.h"
#include "delay.h"
#include "uart.h"
#include "rtc.h"
#include "stdio.h"
#include "bpk.h"

_calendar_obj calendar;//ʱ�ӽṹ��

static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//����0:����
//����:�������
u8 RTC_Init(void)
{
	  SENSOR_EXTCmd(DISABLE);
		SENSOR_EXTPortCmd(SENSOR_Port_All, DISABLE);
		SENSOR_ANACmd(SENSOR_ANA_MESH, DISABLE);
		SENSOR_ClearITPendingBit();//�����ⲿ�������ر�
	   //printf("BKP=%04X\r\n",readBPK(5));
    if (readBPK(1)!=0x80808080)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
    {
			  RTC_Set(2019,8,23,10,50,55);  //����ʱ��	
			  writeBPK(1,0x80808080);
			  //printf("BKP=%08X\r\n",readBPK(1));
    }
    else//ϵͳ������ʱ
    {

    }
		RTC_ITConfig(ENABLE);
    RTC_NVIC_Config();
    RTC_Get();//����ʱ��
    return 0; //ok
}
//RTCʱ���ж�
//ÿ�봥��һ��
//extern u16 tcnt;
void RTC_IRQHandler(void)
{  
    RTC_Get();//����ʱ��
    RTC_ClearITPendingBit();//������жϱ�־λ
	  RTC_SetAlarm(RTC_GetCounter()+1);
}
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{
    if(year%4==0) //�����ܱ�4����
    {
        if(year%100==0)
        {
            if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400����
            else return 0;
        } else return 1;
    } else return 0;
}
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�
u8 const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//ƽ����·����ڱ�
const u8 mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<1970||syear>2099)return 1;
    for(t=1970; t<syear; t++)	//��������ݵ��������
    {
        if(Is_Leap_Year(t))seccount+=31622400;//�����������
        else seccount+=31536000;			  //ƽ���������
    }
    smon-=1;
    for(t=0; t<smon; t++)	 //��ǰ���·ݵ����������
    {
        seccount+=(u32)mon_table[t]*86400;//�·����������
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������
    }
    seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ����������
    seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
    seccount+=sec;//�������Ӽ���ȥ

    RTC_SetRefRegister(seccount);	//����RTC��������ֵ
    RTC_ResetCounter();
		RTC_SetAlarm(1);
    RTC_Get();
    return 0;
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
    static u16 daycnt=0;
    u32 timecount=0;
    u32 temp=0;
    u16 temp1=0;
    //timecount=RTC_GetCounter();
	  timecount += (RTC->RTC_REF+RTC_GetCounter());
	  //RTC_SetRefRegister(timecount);	//����RTC��������ֵ
    temp=timecount/86400;   //�õ�����(��������Ӧ��)
    if(daycnt!=temp)//����һ����
    {
        daycnt=temp;
        temp1=1970;	//��1970�꿪ʼ
        while(temp>=365)
        {
            if(Is_Leap_Year(temp1))//������
            {
                if(temp>=366)temp-=366;//�����������
                else {
                    temp1++;
                    break;
                }
            }
            else temp-=365;	  //ƽ��
            temp1++;
        }
        calendar.w_year=temp1;//�õ����
        temp1=0;
        while(temp>=28)//������һ����
        {
            if(Is_Leap_Year(calendar.w_year)&&temp1==1)//�����ǲ�������/2�·�
            {
                if(temp>=29)temp-=29;//�����������
                else break;
            }
            else
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
                else break;
            }
            temp1++;
        }
        calendar.w_month=temp1+1;	//�õ��·�
        calendar.w_date=temp+1;  	//�õ�����
    }
    temp=timecount%86400;     		//�õ�������
    calendar.hour=temp/3600;     	//Сʱ
    calendar.min=(temp%3600)/60; 	//����
    calendar.sec=(temp%3600)%60; 	//����
    calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����
    return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
    u16 temp2;
    u8 yearH,yearL;

    yearH=year/100;
    yearL=year%100;
    // ���Ϊ21����,�������100
    if (yearH>19)yearL+=100;
    // ����������ֻ��1900��֮���
    temp2=yearL+yearL/4;
    temp2=temp2%7;
    temp2=temp2+day+table_week[month-1];
    if (yearL%4==0&&month<3)temp2--;
    return(temp2%7);
}




void GetTimes(unsigned char *timess)
{
	unsigned char data[8];
	//RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm);
	//RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week);

	//RTC_Get_Date(&data[0],&data[1],&data[2],&data[3]);
	//RTC_Get_Time(&data[4],&data[5],&data[6],&data[7]);

	timess[0]  = (unsigned char)(calendar.w_year-2000);//ֻȡ ��ݵ�ʮλ�͸�λ 

	timess[1]  = calendar.w_month;								// ??? ??

	timess[2]  = calendar.w_date;								// ??? ??

	//timess[6]  = data[3];		          // ??? ????

	timess[3]  = calendar.hour;								// ??? ��3?

	timess[4]  = calendar.min;	       		    // ??? ????

	timess[5]  = calendar.sec;			    			// ??? ????

//	printf("-->get data %02d-%02d-%02d %02d:%02d:%02d\r\n",timess[0],timess[1],timess[2],timess[3],timess[4],timess[5]);
}
unsigned char SetTime(unsigned char *timess)
{
	unsigned char ret=0;

	unsigned char data[8];
	
	printf("-->set data %02d-%02d-%02d %02d:%02d:%02d\r\n",timess[0],timess[1],timess[2],timess[3],timess[4],timess[5]);

	//year month day hour min sec
	RTC_Set(timess[0]+2000,timess[1],timess[2],timess[3],timess[4],timess[5]);
	
  delayms(50);
	
	printf("-->set time after-%04d-%02d-%02d %02d-%02d-%02d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);


	GetTimes(data);

	return 0;
}












