#include <stdio.h>
#include <stdlib.h>
#include <string.h>//C���Ա�׼��

#include "mhscpu.h"
#include "mhscpu_flash.h"
#include "flash.h"

#include "math.h"

u32tou8 u32data;//����һ��������

//==================================================================================
// ��ȡĳ����ַ���ڵ�����
// addr:FLASH��ַ
// ���أ� �õ�ַ���ڵ�����
//==================================================================================
uint16_t FLASH_GetFlashSector(unsigned int addr)
{
    if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
    else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
    else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
    else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
    else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
    else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
    else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
    else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
    else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
    else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
    else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_11;
    else if(addr<ADDR_FLASH_SECTOR_12)return FLASH_Sector_12;
    else if(addr<ADDR_FLASH_SECTOR_13)return FLASH_Sector_13;
    else if(addr<ADDR_FLASH_SECTOR_14)return FLASH_Sector_14;
    else return FLASH_Sector_15;
}


//==================================================================================
// ��FLASH�ж�ȡ һ���֣�32λ��
// addr:��ȡ��ַ
// ���أ� ������������
//��ע�� ��ַΪ4�ֽڶ���
//==================================================================================
unsigned int FLSAH_ReadWord(unsigned int addr)
{
    return (*(unsigned int *)addr);
}

//==================================================================================
// ��FLASHָ����ַ д���������
// WriteAddr:д���׵�ַ
// pBuffer:�����׵�ַ
// NumToWrite:��Ҫд�����ݵĴ�С
// ���أ� 4=�ɹ�  1,2,3,5=ʧ��
//��ע�� д����������ͱ�����32λ  д���ַΪ4�ֽڶ���
//==================================================================================
FLASH_Status  FLASH_Write(unsigned int	WriteAddr,unsigned int *pBuffer,unsigned int NumToWrite)
{

    FLASH_Status status = FLASH_COMPLETE;
    unsigned int startaddr,endaddr=0;
    startaddr = WriteAddr;
    endaddr = startaddr +4*NumToWrite;//������ַ

    FLASH_Unlock();
    FCU->RO = 0;//ȥ����������д����
    //==================================================================================
    // �ж�д���ַ�Ƿ�Ƿ�
    //==================================================================================
    if((startaddr < USER_FLASH_START_ADDR) || (endaddr >=ADDR_FLASH_SECTOR_15)) return FLASH_BUSY;

    //==================================================================================
    // �ж�Ҫд��������Ƿ�Ҫ����
    //==================================================================================
    while(WriteAddr < endaddr)
    {
        if(*(unsigned int *)WriteAddr != 0xFFFFFFFF)
        {
            status = FLASH_ErasePage(WriteAddr);
            if(status != FLASH_COMPLETE) return status;
        }
        WriteAddr +=4;
    }
    //==================================================================================
    // д���ڲ�FLASH
    //==================================================================================
    while(startaddr < endaddr)
    {
        if(FLASH_ProgramWord(startaddr, *pBuffer) != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return status;
        }
        startaddr +=4;
        pBuffer++;
    }
    FLASH_Lock();
}

//==================================================================================
//��FLASHָ����ַ ��ȡ����
//��ע�� ��ȡ��������Ϊ32λ  ��ȡ��ַΪ4�ֽڶ���
//==================================================================================
void  FLASH_Read(unsigned int	ReadAddr,unsigned int *pBuffer,unsigned int NumToRead)
{
    unsigned int i;
    for(i=0; i<NumToRead; i++)
    {
        pBuffer[i++] =FLSAH_ReadWord(ReadAddr);
        ReadAddr+=4;
    }
}


void FLASH_WriteMoreData(unsigned int WriteAddr,unsigned char *pBuffer,unsigned int NumToWrite)
{
    unsigned int i;
    unsigned count,remin = 0;
    count = NumToWrite/4;
    remin = NumToWrite%4;
    for(i = 0; i< count; i++)
    {
        u32data.buf[0] = *pBuffer;
        u32data.buf[1] = *(pBuffer+1);
        u32data.buf[2] = *(pBuffer+2);
        u32data.buf[3] = *(pBuffer+3);
        FLASH_Write(WriteAddr,&u32data.data,1);
        pBuffer+=4;//��ַƫ��4
        WriteAddr+=4;
    }
    if(NumToWrite%4)//����д��ʣ���ֽ�
    {
        for(i=0; i<remin; i++)
        {
            u32data.buf[i]=*(pBuffer+i);
        }
        FLASH_Write(WriteAddr,&u32data.data,1);
    }
}


void FLASH_ReadMoreData(unsigned int ReadAddr,unsigned char *pBuffer,unsigned int NumToWrite)
{
    unsigned int i;
    unsigned count,remin = 0;
    count = NumToWrite/4;
    remin = NumToWrite%4;
    for(i = 0; i< count; i++)
    {
        FLASH_Read(ReadAddr,&u32data.data,1);
        *pBuffer = u32data.buf[0];
        *(pBuffer+1)=u32data.buf[1];
        *(pBuffer+2)=u32data.buf[2];
        *(pBuffer+3)=u32data.buf[3];
        pBuffer+=4;//��ַƫ��4
        ReadAddr+=4;
    }
    if(remin)
    {

        FLASH_Read(ReadAddr,&u32data.data,1);
        for(i=0; i<remin; i++)
        {
            *(pBuffer+i) = u32data.buf[i];
        }
    }








}









