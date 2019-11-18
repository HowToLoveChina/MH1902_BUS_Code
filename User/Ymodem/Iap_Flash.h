#ifndef Iap_Flash_H
#define Iap_Flash_H
#define FLASH_BASE_ADDR 0x01000000      //Flash����ַ
#define IAP_ADDR        (FLASH_BASE_ADDR+0xD400+100*1024)     //���������ַ


//0x01026400    0x0100F000
#define IAP_PARAM_SIZE  1
#define IAP_PARAM_ADDR  (FLASH_BASE_ADDR + FLASH_SIZE - PAGE_SIZE) //Flash�ռ����1ҳ��ַ��ʼ����Ų���


#define PAGE_SIZE          (0x1000)    //ҳ�Ĵ�С4K
//#define FLASH_SIZE         (0x80000)  //Flash�ռ�512K

void IAP_DisableFlashWPR(void);
signed char IAP_UpdataParam(unsigned int *param);
void IAP_FlashEease(unsigned int size);
signed char IAP_UpdataProgram(unsigned int addr, unsigned int size);
#endif





