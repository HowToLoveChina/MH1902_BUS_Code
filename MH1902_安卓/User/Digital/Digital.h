#ifndef _DIGITAL_H
#define _DIGITAL_H

#include "mh523.h"
#include "mhscpu.h"
#include "delay.h"

void dig_bsp_init();//����ܳ�ʼ��
void dig_write_dat(unsigned char  dat);
void dig_test(unsigned char dat);
void dig_Display(unsigned int dat);



#endif
