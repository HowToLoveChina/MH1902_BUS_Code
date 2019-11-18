//#include "Packet.h"
//#include "iso14443a.h"
//#include "iso14443_4.h"
//#include "mh523.h"
//#include "mifare.h"
//#include "uart.h"
//#include "stdio.h"

//unsigned char UID[5];//���ţ���BCC��
//unsigned char snrSize;//��Ƭ����
//unsigned char setCID;
//unsigned char setPCB ;

///**********************************************************************
//�������ƣ�M1_AnalyzeReceivePacket
//�������ܣ�M1���������ݰ�����
//���������
//		receivePacket:�������ݰ����׵�ַ
//		sendPacket:�����÷��Ͱ��׵�ַ
//�������:
//		��
//***********************************************************************/
//void M1_AnalyzeReceivePacket(Packet *receivePacket, Packet *sendPacket)
//{
//    signed char status;
//    unsigned char i,Section,Block,VerifiMode;
//    unsigned char key[12];
//    unsigned int n;
//    transceive_buffer  *pi;
//    pi = &mf_com_data;

//    switch(receivePacket->instruction)
//    {
//    case COM_FINDCARD:	//M1Ѱ��
//        sendPacket->length = 0x03;
//        sendPacket->instruction = COM_FINDCARD;
//        n = 3;
//        do
//        {
//            n--;
//            status = pcd_request(PICC_REQALL, MH523.CardTypebuf);//Ѱ��
//        } while((status == MI_NOTAGERR) && (n > 0));

//        if (status == MI_OK)
//        {
//            sendPacket->p	= 'Y';//Ѱ���ɹ�
//        }
//        else if(status == MI_NOTAGERR)
//        {
//            sendPacket->p	= 'E';//�޿�
//        }
//        else
//        {
//            sendPacket->p	= 'N';//Ѱ��ʧ��
//        }
//        break;
//    case COM_GETUID:	//M1��ȡ����
//        sendPacket->instruction = COM_GETUID;
//        n = 3;
//        do
//        {
//            n--;
//            status = pcd_request(PICC_REQALL, MH523.CardTypebuf);
//        } while((status == MI_NOTAGERR) && (n > 0));

//        if(status == MI_NOTAGERR)
//        {
//            sendPacket->length	= 0x03;
//            sendPacket->p		= 'E';
//            break;
//        }
//        if(status == MI_OK)
//        {
//            //����ͻ
//            status=pcd_cascaded_anticoll(PICC_ANTICOLL1, MH523.coll_position, MH523.UIDbuf);
//            sendPacket->length	= 0x07;
//            if(status == MI_OK)
//            {
//                sendPacket->p		= 'Y';
//                memcpy(sendPacket->cardUID,MH523.UIDbuf,4);//���������Ŀ��ŷ�����λ��
//                pcd_cascaded_select(PICC_ANTICOLL1, MH523.UIDbuf,&MH523.SAK);
//            }
//            else
//            {
//                sendPacket->p		= 'N';
//                memset(sendPacket->cardUID,0,4);//�����Ŵ��� �ϴ�ȫ0
//            }
//            break;
//        }
//        else
//        {
//            sendPacket->p		= 'N';
//            memset(sendPacket->cardUID,0,4);//��UID���� UID��0
//            break;
//        }
//    case COM_VERIFYKEYA:
//    case COM_VERIFYKEYB:	//������֤
//        sendPacket->length	= 0x04;
//        sendPacket->sectorNumber = receivePacket->sectorNumber;
//        if(receivePacket->instruction == COM_VERIFYKEYA)
//        {
//            sendPacket->instruction = COM_VERIFYKEYA;
//            VerifiMode = PICC_AUTHENT1A;//��֤ģʽ = KeyA
//        }
//        else if(receivePacket->instruction == COM_VERIFYKEYB)
//        {
//            sendPacket->instruction = COM_VERIFYKEYB;
//            VerifiMode = PICC_AUTHENT1B;//��֤ģʽ = KeyB
//        }
//        memcpy(key,receivePacket->key,6); //����Э����е���Կ �� key������
//        Section = receivePacket->sectorNumber * 4;// ������*4
//        //������֤  (��֤ģʽ + ���Կ�� + UID + ����)
//        status =pcd_auth_state(VerifiMode, Section, MH523.UIDbuf,key);
//        if(status == MI_OK)
//        {
//            sendPacket->p	= 'Y';
//            break;
//        }
//        else if(status == MI_AUTHERR)
//        {
//            sendPacket->p	= '3';//�������
//            break;
//        }
//        else if(status == MI_ACCESSTIMEOUT)//��ʱ
//        {
//            n = 3;
//            do
//            {
//                n--;
//                status = pcd_request(PICC_REQALL, MH523.CardTypebuf);

//            } while((status == MI_NOTAGERR) && (n > 0));

//            if(status == MI_NOTAGERR)
//            {
//                sendPacket->p	= 'E';
//                break;
//            }
//            else
//            {
//                sendPacket->p	= 'N';//��0x4E�� Ѱ�����ɹ�
//                break;
//            }
//        }
//        else
//        {
//            sendPacket->p	= 'N';
//            break;
//        }
//    case COM_READBLOCK:
//    case COM_WRITEBLOCK:	//��д��
//        sendPacket->sectorNumber= receivePacket->sectorNumber;
//        sendPacket->blockNumber	= receivePacket->blockNumber;

//        if(receivePacket->sectorNumber <= 31)
//            Block = receivePacket->sectorNumber*4 + receivePacket->blockNumber;//���Կ��
//        else
//            Block = receivePacket->sectorNumber*16 + 128 + receivePacket->blockNumber;

//        if(receivePacket->instruction == COM_READBLOCK)//�յ�����ָ��
//        {
//            sendPacket->instruction	= COM_READBLOCK;
//            status = pcd_read(Block,MH523.Block);//��ȡһ��
//        }
//        else if(receivePacket->instruction == COM_WRITEBLOCK)//�յ�д��ָ��
//        {
//            sendPacket->instruction	= COM_WRITEBLOCK;
//            status = pcd_write(Block,receivePacket->blockData);//д��һ��
//        }

//        if(status == MI_OK)
//        {
//            sendPacket->length	= 0x15;
//            sendPacket->p		= 'Y';
//            if(receivePacket->instruction == COM_READBLOCK)//��ȡһ��ɹ�
//            {
//                memcpy(sendPacket->blockData,MH523.Block,16);// �������������� ���͵���λ��
//            }
//            else if(receivePacket->instruction == COM_WRITEBLOCK)//д��һ��ɹ�
//            {
//                memcpy(sendPacket->blockData,MH523.Block,16);
//            }
//            break;
//        }
//        else if(status == MI_ACCESSTIMEOUT)//��д����ʱ
//        {
//            sendPacket->length	= 0x05;

//            n = 3;
//            do
//            {
//                n--;
//                status = pcd_request(PICC_REQALL, MH523.CardTypebuf);

//            } while((status == MI_NOTAGERR) && (n > 0));

//            if(status == MI_NOTAGERR)
//            {
//                sendPacket->p	= 'E';//��0x45�� �������޿�
//                break;
//            }
//            else
//            {
//                sendPacket->p	= '4';//��д���ݴ�
//                break;
//            }
//        }
//        else
//        {
//            sendPacket->p	= 'N';//Ѱ��ʧ��
//            break;
//        }
//    case COM_MODIFYKEY:	//�޸�����
//        sendPacket->length		= 0x04;
//        sendPacket->instruction	= COM_MODIFYKEY;
//        sendPacket->sectorNumber= receivePacket->sectorNumber;

//        if(receivePacket->sectorNumber <= 31)
//            mf_com_data.mf_data[1] = receivePacket->sectorNumber*4 + 3;
//        else
//            mf_com_data.mf_data[1] = receivePacket->sectorNumber*16 + 15;

//        for(i=0; i<6; i++)
//        {
//            mf_com_data.mf_data[i+2] = receivePacket->key[i];
//        }
//        mf_com_data.mf_data[8]	= 0xFF;
//        mf_com_data.mf_data[9]	= 0x07;
//        mf_com_data.mf_data[10]	= 0x80;
//        mf_com_data.mf_data[11]	= 0x69;// 8~11 Ϊ �����ֽ� ������ı�

//        for(i=0; i<6; i++)
//        {
//            mf_com_data.mf_data[i+12] = 0xFF;// KEYB �̶�Ϊ6��0xFF
//        }
//        status 	= pcd_write(mf_com_data.mf_data[1],&mf_com_data.mf_data[2]);
//        if(status == MI_OK)
//        {
//            sendPacket->p		= 'Y';
//            break;
//        }
//        else if(status == MI_ACCESSTIMEOUT)
//        {
//            n = 3;
//            do
//            {
//                n--;
//                status=pcd_request(PICC_REQALL, MH523.CardTypebuf);

//            } while((status == MI_NOTAGERR) && (n > 0));

//            if(status == MI_NOTAGERR)
//            {
//                sendPacket->p	= 'E';
//                break;
//            }
//            else
//            {
//                sendPacket->p	= 'N';
//                break;
//            }
//        }
//        else
//        {
//            sendPacket->p	= 'N';
//            break;
//        }
//    case COM_INCREMENT:
//    case COM_DECREMENT:	//����ֵ
//        sendPacket->length		= 0x05;
//        sendPacket->sectorNumber= receivePacket->sectorNumber;
//        sendPacket->blockNumber	= receivePacket->blockNumber;

//        if(receivePacket->sectorNumber <= 31)
//        {
//            // set.MfData[1] = receivePacket->sectorNumber*4 + receivePacket->blockNumber;
//        }
//        else
//        {
//            //  set.MfData[1] = receivePacket->sectorNumber*16 + 128 + receivePacket->blockNumber;
//        }
//        for(i=0; i<4; i++)
//        {
//            //   set.MfData[i+2] = receivePacket->blockValue[i];
//        }
//        if(receivePacket->instruction == COM_INCREMENT)
//        {
//            sendPacket->instruction = COM_INCREMENT;
//            // status = Increment(&set);//���ֵ
//            // status=PcdValue(PICC_INCREMENT,set.MfData[1],&set.MfData[2]);//��ֵ
//        }
//        else if(receivePacket->instruction == COM_DECREMENT)
//        {
//            sendPacket->instruction = COM_DECREMENT;
//            // status = Decrement(&set);//���ֵ
//            // status=PcdValue(PICC_DECREMENT,set.MfData[1],&set.MfData[2]);//�ۿ�
//        }

//        if(status == MI_OK)
//        {
//            sendPacket->p		= 'Y';
//            break;
//        }
//        else if(status == MI_ACCESSTIMEOUT)
//        {
//            n = 5;
//            do
//            {
//                n--;
//                status=pcd_request(PICC_REQALL, MH523.CardTypebuf);

//            } while((status == MI_NOTAGERR) && (n > 0));

//            if(status == MI_NOTAGERR)
//            {
//                sendPacket->p	= 'E';
//                break;
//            }
//            else
//            {
//                sendPacket->p	= 'N';
//                break;
//            }
//        }
//        else
//        {
//            sendPacket->p	='4';//���ʽ���� ����ֵ��ʽ
//            break;
//        }

//    }
//}

//void CPU_Contact_AnalyzeReceivePacket(Packet *receivePacket, Packet *sendPacket)
//{
//    char status;
//    unsigned int i;
//    unsigned int tx_len,rx_len;
//    Block sendBlock, receiveBlock;

//    switch(receivePacket->instruction)
//    {
//    case CPU_APDUSEND_T0:
//    case CPU_APDUSEND_T1:
//        if(receivePacket->instruction == CPU_APDUSEND_T0)
//            sendPacket->instruction = CPU_APDUSEND_T0;
//        else if(receivePacket->instruction == CPU_APDUSEND_T1)
//            sendPacket->instruction = CPU_APDUSEND_T1;
//        tx_len = receivePacket->APDULength;//��Ҫ����APDU����
//        status = ISO14443_4_HalfDuplexExchange(&g_pcd_module_info, receivePacket->APDU, tx_len, receivePacket->APDU, &rx_len);
//        if(status == MI_OK)
//        {
//            sendPacket->length		= rx_len+5;
//            sendPacket->APDULength	= rx_len;
//            for(i=0; i<sendPacket->APDULength; i++)
//            {
//                sendPacket->APDU[i]	= receivePacket->APDU[i];
//            }
//            sendPacket->p	= 'Y';
//            break;
//        }
//        else
//        {
//            sendPacket->length	= 0x03;

//            if(status == ISO14443_4_ERR_PROTOCOL)
//            {
//                sendPacket->p	= 'E';
//                break;
//            }
//            else
//            {
//                sendPacket->p	= 'N';
//                break;
//            }
//        }
//    case CPU_DESELECT:
//        sendPacket->instruction = CPU_DESELECT;
//        sendPacket->length		= 3;

//        //Deselect(&sendBlock);
//        //status = SendReceiveBlock(&sendBlock, &receiveBlock);

//        if(status == MI_OK)
//            sendPacket->p		= 'Y';
//        else if(status == MI_COM_ERR)
//        {
//            sendPacket->p	= 'E';
//            break;
//        }
//        else
//        {
//            sendPacket->p	= 'N';
//            break;
//        }

//        break;
//    }
//}

//void CPU_Proximity_AnalyzeReceivePacket(Packet *receivePacket, Packet *sendPacket)
//{
//    signed char status;
//    unsigned char i;
//    // Transceive set;
//    transceive_buffer  *pi;
//    pi = &mf_com_data;

//    switch(receivePacket->instruction)
//    {
//    case PROXIMITY_CPU_SENDRATS: //0x30 RATS
//        sendPacket->instruction = PROXIMITY_CPU_SENDRATS;//0x30
//        status = pcd_rats_a(0, MH523.ATS,&MH523.ATSLength);	//����RATS
//        if(status == MI_OK)
//        {
//            for(i=0; i<MH523.ATSLength; i++)
//            {
//                sendPacket->ATS[i]	= MH523.ATS[i];
//            }
//            sendPacket->length		= MH523.ATSLength+3;
//            sendPacket->ATSLength	= MH523.ATSLength-2;
//            sendPacket->p			= 'Y';
//            break;
//        }

//        sendPacket->length	= 0x03;

//        if(status == MI_NOTAGERR)
//        {
//            sendPacket->p	= 'E';
//            break;
//        }
//        else
//        {
//            sendPacket->p	= 'N';
//            break;
//        }
//    case PROXIMITY_CPU_SENDPPS:
//        //SendPPS();//0x31
//        break;
//    }
//}

///********************************************
//�������ƣ�SendPacket
//�������ܣ�����λ���������ݰ�
//���������
//		sendPacket:�������ݰ��׵�ַ
//�������:
//		��
//*********************************************/
//void SendPacket(Packet *sendPacket)
//{
//    unsigned char temp, bcc=0;
//    unsigned int i;

//    if(sendPacket->type == TYPE_M1)
//    {
//        switch(sendPacket->instruction)
//        {
//        case COM_FINDCARD:
//            sendPacket->data[0] = sendPacket->p;
//            break;
//        case COM_GETUID:
//            sendPacket->data[0] = sendPacket->p;
//            for(i=0; i<4; i++)
//            {
//                sendPacket->data[i+1] = sendPacket->cardUID[i];
//            }
//            break;
//        case COM_VERIFYKEYA:
//        case COM_VERIFYKEYB:
//        case COM_MODIFYKEY:
//            sendPacket->data[0] = sendPacket->sectorNumber;
//            sendPacket->data[1] = sendPacket->p;
//            break;
//        case COM_READBLOCK:
//        case COM_WRITEBLOCK:
//            sendPacket->data[0] = sendPacket->sectorNumber;
//            sendPacket->data[1] = sendPacket->blockNumber;
//            sendPacket->data[2] = sendPacket->p;
//            if(sendPacket->p == 'Y')
//            {
//                for(i=0; i<16; i++)
//                    sendPacket->data[i+3] = sendPacket->blockData[i];
//            }
//            break;
//        case COM_INCREMENT:
//        case COM_DECREMENT:
//            sendPacket->data[0] = sendPacket->sectorNumber;
//            sendPacket->data[1] = sendPacket->blockNumber;
//            sendPacket->data[2] = sendPacket->p;
//            break;
//        default:
//            break;
//        }
//    }
//    else if(sendPacket->type == TYPE_CPU_CONTACT)
//    {
//        switch(sendPacket->instruction)
//        {
//        case CONTACT_CPU_RESETCOLD:
//        case CONTACT_CPU_RESETHOT:
//        case CPU_APDUSEND_T0:
//        case CPU_APDUSEND_T1:
//            sendPacket->data[0] = sendPacket->p;
//            if(sendPacket->p == 'Y')
//            {
//                sendPacket->data[1] = sendPacket->APDULength/256;
//                sendPacket->data[2] = sendPacket->APDULength%256;
//                for(i=0; i<sendPacket->APDULength; i++)
//                    sendPacket->data[i+3] = sendPacket->APDU[i];
//            }
//            break;
//        case CPU_DESELECT:
//            sendPacket->data[0] = sendPacket->p;
//            break;
//        }
//    }
//    else if(sendPacket->type == TYPE_CPU_PROXI)
//    {
//        switch(sendPacket->instruction)
//        {
//        case PROXIMITY_CPU_SENDRATS:
//            sendPacket->data[0] = sendPacket->p;
//            if(sendPacket->p == 'Y')
//            {
//                for(i=0; i<sendPacket->ATSLength+2; i++)
//                    sendPacket->data[i+1] = sendPacket->ATS[i];
//            }
//            break;
//        }
//    }

//    bcc ^= 0x02;
//    UART0_SendByte(0x02);//02

//    temp = sendPacket->length>>8;

//    bcc ^= temp;
//    UART0_SendByte(temp);//00

//    temp = sendPacket->length;
//    bcc ^= temp;
//    UART0_SendByte(temp);//length

//    bcc ^= sendPacket->type;
//    bcc ^= sendPacket->instruction;

//    UART0_SendByte(sendPacket->type);//type
//    UART0_SendByte(sendPacket->instruction);//command

//    for(i=0; i<(sendPacket->length-2); i++)
//    {
//        bcc ^= sendPacket->data[i];
//        UART0_SendByte(sendPacket->data[i]);
//    }
//    bcc ^= 0x03;
//    UART0_SendByte(0x03);
//    UART0_SendByte(bcc);
//}
