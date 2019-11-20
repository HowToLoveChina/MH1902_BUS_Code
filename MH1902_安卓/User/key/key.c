#include "key.h"

#include "stdio.h"

void key_init()//����IO�ڳ�ʼ��
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//��������
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Remap=GPIO_Remap_1;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}


//PC2~PC5 �ӵ��Ƕ�������
static unsigned char KEY0_ReadPin(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1);//��PC1  ��������
}

static unsigned char KEY1_ReadPin(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2);//��PC2
}

static unsigned char KEY2_ReadPin(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3);//��PC3
}

static unsigned char KEY3_ReadPin(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4);//��PC4;
}

static unsigned char KEY4_ReadPin(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5);//��PC5;
}

static unsigned char KEY5_ReadPin(void)//����ϼ�  0:������ͬʱ����   1�����������ͷ� ��Ϊ�ߵ�ƽ
{
    unsigned char KeyValue=0;
    KeyValue = 	KEY1_ReadPin()+KEY4_ReadPin();
    if(KeyValue == 0) return 0;
    //else if(KeyValue == 1)return 0;
	  else return 1;
}

KEY_COMPONENTS Key_Buf[KEY_NUM] =
{
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY0_ReadPin},
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY1_ReadPin},
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY2_ReadPin},
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY3_ReadPin},
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY4_ReadPin},
    {1,0,0,0,KEY_NULL,KEY_NULL,KEY5_ReadPin},
};//���ĸ�����KEY_DOWN_LEVEL��ʾ��������ʱIO�ڵĵ�ƽ  0���͵�ƽ  1���ߵ�ƽ  �ҵĵ�·���������ǵ͵�ƽ ������������Ϊ0

//��ѯÿ�������ĵ�ƽ ����ֵ����Ӧ�������ƽ
static void Get_Key_Level(void)
{
    unsigned char i;

    for(i = 0; i < KEY_NUM; i++)
    {
        if(Key_Buf[i].KEY_SHIELD == 0)
            continue;
        if(Key_Buf[i].READ_PIN() == Key_Buf[i].KEY_DOWN_LEVEL)
        {
            Key_Buf[i].KEY_LEVEL = 1;//�����ƽ 1������ 0��̧��
        }
        else
        {
            Key_Buf[i].KEY_LEVEL = 0;
        }
    }
}

void ReadKeyStatus(void)
{
    unsigned char i;
    Get_Key_Level();
    for(i = 0; i < KEY_NUM; i++)
    {
        switch(Key_Buf[i].KEY_STATUS)
        {
            //״̬0��û�а�������
        case KEY_NULL:
            if(Key_Buf[i].KEY_LEVEL == 1)//�а�������
            {
                Key_Buf[i].KEY_STATUS = KEY_SURE;//ת��״̬1
                Key_Buf[i].KEY_EVENT = KEY_NULL;//���¼�
            }
            else
            {
                Key_Buf[i].KEY_EVENT = KEY_NULL;//���¼�
            }
            break;
            //״̬1����������ȷ��
        case KEY_SURE:
            if(Key_Buf[i].KEY_LEVEL == 1)//ȷ�Ϻ��ϴ���ͬ
            {
                Key_Buf[i].KEY_STATUS = KEY_DOWN;//ת��״̬2
                Key_Buf[i].KEY_EVENT = KEY_NULL;//�����¼�
                Key_Buf[i].KEY_COUNT = 0;//����������
            }
            else
            {
                Key_Buf[i].KEY_STATUS = KEY_NULL;//ת��״̬0
                Key_Buf[i].KEY_EVENT = KEY_NULL;//���¼�
            }
            break;
            //״̬2����������
        case KEY_DOWN:
            if(Key_Buf[i].KEY_LEVEL == 0)//�����ͷţ��˿ڸߵ�ƽ
            {
                Key_Buf[i].KEY_STATUS = KEY_NULL;//ת��״̬0
                Key_Buf[i].KEY_EVENT = KEY_NULL;//���¼�					  
            }
            else
            {
                Key_Buf[i].KEY_STATUS = KEY_LONG;//ת��״̬3
                Key_Buf[i].KEY_EVENT = KEY_DOWN;//�����¼�						
            }
            break;
            //״̬3��������������
        case KEY_LONG:
            if(Key_Buf[i].KEY_LEVEL != 1)//�����ͷţ��˿ڸߵ�ƽ
            {
                Key_Buf[i].KEY_STATUS = KEY_NULL;//ת��״̬0
                Key_Buf[i].KEY_EVENT = KEY_UP;//�ɿ��¼�          
            }
            else 
						if((Key_Buf[i].KEY_LEVEL == 1)
             && (++Key_Buf[i].KEY_COUNT >= KEY_LONG_DOWN_DELAY)&&(i==5)) //����KEY_LONG_DOWN_DELAYû���ͷ�
            {
                Key_Buf[i].KEY_EVENT = KEY_LONG;//�����¼�
                Key_Buf[i].KEY_COUNT = 0;//����������
            }
            else
            {
                Key_Buf[i].KEY_EVENT = KEY_NULL;//���¼�
            }
            break;
        }
    }
    if(Key_Buf[5].KEY_LEVEL == 1 || Key_Buf[5].KEY_EVENT == KEY_LONG)
    {
        Key_Buf[1].KEY_EVENT = KEY_NULL;
        Key_Buf[1].KEY_STATUS= KEY_NULL;
        Key_Buf[4].KEY_EVENT = KEY_NULL;
        Key_Buf[4].KEY_STATUS= KEY_NULL;
    }
}

//�������Ժ���
void Task_KEY_Scan(void)
{
    //ReadKeyStatus();

    if(Key_Buf[KEY0].KEY_EVENT == KEY_UP)
    {
        printf("Touch\n");
			  Key_Buf[KEY0].KEY_EVENT = KEY_NULL;//���¼�
    }
    if(Key_Buf[KEY1].KEY_EVENT == KEY_UP)
    {
        printf("DOWN\n");
			  Key_Buf[KEY1].KEY_EVENT = KEY_NULL;//���¼�
    }
    if(Key_Buf[KEY2].KEY_EVENT == KEY_UP)
    {
        printf("UP\n");
		  	Key_Buf[KEY2].KEY_EVENT = KEY_NULL;//���¼�
    }
    if(Key_Buf[KEY3].KEY_EVENT == KEY_UP)
    {
        printf("OK\n");
			  Key_Buf[KEY3].KEY_EVENT = KEY_NULL;//���¼�
    }
    if(Key_Buf[KEY4].KEY_EVENT == KEY_UP)
    {
        printf("FNC\n");
			  Key_Buf[KEY4].KEY_EVENT = KEY_NULL;//���¼�
    }
    if(Key_Buf[KEY5].KEY_EVENT == KEY_LONG)
    {
        printf("DOWN+FUN\n");
			  Key_Buf[KEY5].KEY_EVENT = KEY_NULL;//���¼�
    }

}

unsigned char GetKey()
{  
    if(Key_Buf[KEY0].KEY_EVENT == KEY_UP)
    {
			  Key_Buf[KEY0].KEY_EVENT = KEY_NULL;//���¼�
        return 1;
    }
    else if(Key_Buf[KEY1].KEY_EVENT == KEY_UP)
    {
			  Key_Buf[KEY1].KEY_EVENT = KEY_NULL;//���¼�
        return 2;
		 }
    else if(Key_Buf[KEY2].KEY_EVENT == KEY_UP)
    {
			  Key_Buf[KEY2].KEY_EVENT = KEY_NULL;//���¼�
        return 3;
    }
    else if(Key_Buf[KEY3].KEY_EVENT == KEY_UP)
    {
			  Key_Buf[KEY3].KEY_EVENT = KEY_NULL;//���¼�
        return 4;
    }
    else if(Key_Buf[KEY4].KEY_EVENT == KEY_UP)
    {
			  Key_Buf[KEY4].KEY_EVENT = KEY_NULL;//���¼�
        return 5;
    }
    else if(Key_Buf[KEY5].KEY_EVENT == KEY_LONG)
    {
			  Key_Buf[KEY5].KEY_EVENT = KEY_NULL;//���¼�
        return 6;
    }
    else
    {
        return 0;
    }

}



















