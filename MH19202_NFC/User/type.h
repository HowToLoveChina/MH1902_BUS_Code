/*****************************************************************************
 *   type.h:  Type definition Header file for NXP LPC17xx Family
 *   Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.25  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include <stdint.h>

#ifndef __TYPE_H__
#define __TYPE_H__

//��λ��ͨѶ��������
#define TYPE_M1				0x35	//M1��
#define TYPE_CPU_CONTACT	0x37	//�Ӵ�ʽCPU��
#define TYPE_CPU_PROXI		0x38	//�ǽӴ�ʽCPU��

//��λ��ͨѶ����
#define COM_FINDCARD	0x30	//Ѱ��
#define COM_GETUID		0x31	//��ȡ����
#define COM_VERIFYKEYA	0x32	//��֤����A
#define COM_VERIFYKEYB	0x39	//��֤����B
#define COM_READBLOCK	0x33	//����
#define COM_WRITEBLOCK	0x34	//д��
#define COM_MODIFYKEY	0x35	//�޸�����
#define COM_INCREMENT	0x37	//��ֵ
#define COM_DECREMENT	0x38	//��ֵ

#define CONTACT_CPU_RESETHOT	0x2F
#define CONTACT_CPU_RESETCOLD	0x30

#define CPU_APDUSEND_T0			0x31
#define CPU_APDUSEND_T1			0x32
#define CPU_DESELECT			0x33

#define PROXIMITY_CPU_SENDRATS	0x30
#define PROXIMITY_CPU_SENDPPS	0x31

typedef struct
{
    unsigned char length;
    unsigned char value[64];
} Array;

typedef struct
{
    unsigned char command;			//PCD����
    unsigned char length;			//������ISO14443A����������ȣ������
    unsigned char sr_data[64];		//������ISO14443A���������
//	unsigned char receive_length;	//����ISO14443A���ز���Ӧ�г���
} Transceive;

/*typedef struct
{
	unsigned char CLA;
	unsigned char INS;
	unsigned char P1;
	unsigned char P2;
	unsigned char main_part[40];
}APDU_CMD;

typedef struct
{
	unsigned char data_seg[42];
	unsigned char SW1;
	unsigned char SW2;
}APDU_RSD;*/

typedef struct	//��ṹ������
{
    unsigned char PCB;
    unsigned char CID;
    unsigned char NAD;
    unsigned char INF_length;
    unsigned char INF[64];
    unsigned char EDC[2];
} Block;

typedef struct	//ͨѶ���ṹ��
{
    unsigned char head;
    unsigned int length;
    unsigned char type;
    unsigned char instruction;
    unsigned char data[256];
    unsigned char tail;
    unsigned char bcc;

    unsigned char p;
    unsigned char sectorNumber;
    unsigned char blockNumber;
    unsigned char cardUID[4];
    unsigned char key[6];
    unsigned char blockData[16];
    unsigned char blockValue[4];

    unsigned int APDULength;
    unsigned char APDU[254];

    unsigned char RATS;
    unsigned char ATS[256];
    unsigned int ATSLength;
} Packet;


typedef struct	//ͨѶ���ṹ��
{
    unsigned int length;
    unsigned char APDULength;
    unsigned char APDU[254];
} APDUPacket;





#endif  /* __TYPE_H__ */
