#ifndef __RTC_H
#define __RTC_H

//ʱ��ṹ��
typedef struct
{
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    //������������
    unsigned int w_year;
    unsigned char  w_month;
    unsigned char  w_date;
    unsigned char  week;
} _calendar_obj;

extern _calendar_obj calendar;	//�����ṹ��

extern unsigned char const mon_table[12];	//�·��������ݱ�
unsigned char RTC_Init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
unsigned char Is_Leap_Year(unsigned short year);//ƽ��,�����ж�
unsigned char RTC_Get(void);         //����ʱ��
unsigned char RTC_Get_Week(unsigned short year,unsigned char month,unsigned char day);
unsigned char RTC_Set(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);//����ʱ��


void GetTimes(unsigned char *timess);
unsigned char SetTime(unsigned char *timess);

#endif


