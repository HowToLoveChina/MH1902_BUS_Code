#include "mhscpu.h"
#include "mhscpu_flash.h"
#include <stdio.h>
#include "Iap_Flash.h"
#include "uart.h"
/*************************************************************
  Function   : IAP_DisableFlashWPR 
  Description: �ر�flash��д����
  Input      : none        
  return     : none    
*************************************************************/
void IAP_DisableFlashWPR(void)
{
//	u32 blockNum = 0, UserMemoryMask = 0;

//    blockNum = (IAP_ADDR - FLASH_BASE_ADDR) >> 12;   //����flash��
//	 UserMemoryMask = ((u32)(~((1 << blockNum) - 1)));//��������

//	if((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)//�鿴�����������Ƿ�д����
//	{
//		FLASH_EraseOptionBytes ();//����ѡ��λ
//	}
	
	 //�ж��Ƿ����
    if (1 == FLASH_IsProtect(IAP_ADDR))
    {
        printf("addr %08X in block is protect,please unprotect it.\n",IAP_ADDR);
    }
    //����
    FLASH_Unlock();
    FLASH_UnProtect(IAP_ADDR);
    printf("Flash Unprotect.\n");
	
	
}


/*************************************************************
  Function   : 
  Description: ��FLASH�����һҳд�����
  Input      : size-�����Ĵ�С         
  return     : none    
*************************************************************/
signed char IAP_UpdataParam(unsigned int *param)
{
	unsigned int i;
	unsigned int flashptr = IAP_PARAM_ADDR;

	FLASH_Unlock();//flash����
	FLASH_UnProtect(IAP_PARAM_ADDR);
	for(i = 0; i < IAP_PARAM_SIZE; i++)
	{	
		//FLASH_ProgramWord(flashptr + 4 * i, *param);
		FLASH_ProgramWord(IAP_PARAM_ADDR+ 4 * i, *param);	
		if(*(unsigned int *)(flashptr + 4 * i) != *param)//�����ϻض�
		{
			return -1;
		}	
		param++;
	}
	FLASH_Lock();//flash����
	return 0;
}

/*************************************************************
  Function   : IAP_FlashEease 
  Description: ����Flash
  Input      : size-�����Ĵ�С        
  return     : none    
*************************************************************/
void IAP_FlashEease(unsigned int size)
{
	unsigned int eraseCounter = 0;
	unsigned int nbrOfPage = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;	  

	if(size % PAGE_SIZE != 0)//������Ҫ��д��ҳ��
	{										  
		nbrOfPage = size / PAGE_SIZE + 1; 
	}
	else
	{
		nbrOfPage = size / PAGE_SIZE;
	}
	
	FLASH_Unlock();//���flash��д����
	
	FCU->RO = 0;//�ر������� ��д����
	for(eraseCounter = 0; (eraseCounter < nbrOfPage) && ((FLASHStatus == FLASH_COMPLETE)); eraseCounter++)//��ʼ����
	{
		FLASHStatus = FLASH_ErasePage(IAP_ADDR + (eraseCounter * PAGE_SIZE));//����
		//IAP_SerialSendStr(".");//��ӡ'.'����ʾ����
	}
	FLASH_ErasePage(IAP_PARAM_ADDR);//�����������ڵ�flashҳ
	FLASH_Lock();//flash��д����
}
/*************************************************************
  Function   : IAP_UpdataProgram 
  Description: ��������
  Input      : addr-��д�ĵ�ַ size-��С        
  return     : 0-OK 1-error    
*************************************************************/
signed char IAP_UpdataProgram(unsigned int addr, unsigned int size)
{
	unsigned int i;
	static unsigned int flashptr = IAP_ADDR;

	FLASH_Unlock();//flash����
	for(i = 0; i < size; i += 4)
	{	
		FLASH_ProgramWord(flashptr, *(unsigned int *)addr);//��д1����
		if(*(unsigned int *)flashptr != *(unsigned int *)addr)//�ж��Ƿ���д�ɹ�
		{
			return -1;
		}
		flashptr += 4;
		addr += 4;
	}
	FLASH_Lock();//flash����
	return 0;
}
