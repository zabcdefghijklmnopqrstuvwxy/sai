/**
********************************************************************************
* @file    sftp.c
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   sftpЭ�鴦��Դ����
* @note    
* @attention
********************************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "sftp.h"
#include "iap.h"

/** �ð����֧�ֵ�SFTP�汾��Ϊ0x01 */
#define   SFTP_VERSION_NUM        0x01

/* IAP�����������ʵ���� */
static sftp_queue_t SftpTask;

/**
* @brief ��ȡCAN����֡������������Ϣ
* @param None
* @note None
* @return ����NULL��ʾ��������Ϊ�� ���طǿձ�ʾ������Ϣ����
*/
static p_sftp_msg_t SFTP_Get_Queue(void)
{
   if(SftpTask.nRear == SftpTask.nFront)
   {
      return NULL;
   }
	 
   return &SftpTask.msg[SftpTask.nFront];
}

/**
* @brief ɾ��CAN����֡��������
* @param None
* @note None
* @return ����-1��ʾ�����Ѿ�Ϊ�ղ���Ҫɾ�� ����0��ʾɾ�����гɹ�
*/
static int32_t SFTP_Del_Queue(void)
{
   if(SftpTask.nRear == SftpTask.nFront)
   {
      return -1;
   }
	
   SftpTask.nFront = (SftpTask.nFront + 1)%SFTP_QUEUE_MAX_NUM;

   return 0;
}

/**
* @brief ���CAN����֡�����������
* @param[in] pdata��Ϣ���� nLen��Ϣ���ݳ��� 
* @note None
* @return ����-1��ʾ�����Ƿ� ����-2��ʾ������������ ����0��ʾ��ӹ������гɹ�
*/
int32_t SFTP_Add_Queue(uint8_t *pdata,uint32_t nLen)
{
    if(NULL == pdata || nLen > 8)
    {
       return -1;

    }
       
    if(SftpTask.nFront == ((SftpTask.nRear + 1)%SFTP_QUEUE_MAX_NUM))
    {
        return -2;
    }
  	
    memcpy(SftpTask.msg[SftpTask.nRear].cSftp_data,pdata,nLen);
    SftpTask.msg[SftpTask.nRear].nLen = nLen;	
    SftpTask.nRear = (SftpTask.nRear + 1)%SFTP_QUEUE_MAX_NUM;
		
    return 0;
}

/**
* @brief CAN����֡�����������ж�
* @param None
* @note None
* @return ����0��ʾδ�� ����1��ʾ����
*/
int32_t SFTP_IsFull_Queue(void)
{
    if(SftpTask.nFront == ((SftpTask.nRear + 1)%SFTP_QUEUE_MAX_NUM))
    {
        return 1;
    }

    return 0;
}

/**
* @brief SFTP����������Ϣ����
* @param[in] pmsg ����֡��Ϣ 
* @note None
* @return None
*/
static int32_t SFTP_Control_Process(p_sftp_msg_t pmsg)
{
    uint8_t cObject = 0;
	uint8_t cSubObject = 0;
    uint32_t nLen = 0;   
    control_msg_t cmsg;
    answer_msg_t amsg;

    cObject = pmsg->cSftp_data[2];             //��ֵ������
    cSubObject = pmsg->cSftp_data[3];          //��ֵ�Ӷ���

    memset(&cmsg,0,sizeof(control_msg_t));     
    memset(&amsg,0,sizeof(answer_msg_t));

    switch(cObject)
    { 
    case SYS_CMD:
    {
    if(VERSION_INQUIRE == cSubObject)
    {
        cmsg.cFrameType = SFTP_CONTROL;
        cmsg.cNode = pmsg->cSftp_data[1];
        cmsg.cObject = SYS_CMD;
        cmsg.cSubObject = VERSION_INQUIRE;
        cmsg.cMsg[0] = SFTP_VERSION_NUM;     //�汾����Ϣ
            
        nLen = 5;
        
        CAN_Send_Msg((uint8_t*)&cmsg,nLen);
        
    }
    else
    {
         amsg.cFrameType = SFTP_ANSWER;
         amsg.cObject = SYS_CMD;
         amsg.cSubObject = VERSION_INQUIRE;     
         amsg.cAStatus = AS_ERR;
         amsg.cAErrCode = AE_UNKNOWN_SUBOBJ;
         nLen = 5;
  
         CAN_Send_Msg((uint8_t*)&amsg,nLen);
    }
    }break;
    case IAP_CMD:
    {
        IAP_Control_Process(pmsg);            
    }break;
    default:                                   //���ڲ�ʶ�����������д���Ӧ��
    {
        amsg.cFrameType = SFTP_ANSWER;
        amsg.cObject = cObject;
        amsg.cSubObject = cSubObject;
        amsg.cAStatus = AS_ERR;
        amsg.cAErrCode = AE_UNKNOWN_OBJ;

        CAN_Send_Msg((uint8_t*)&amsg,5);
    };
    }  
    
    return 0;
}

/**
* @brief SFTPЭ�����ݴ���
* @param None 
* @note None
* @return None
*/
int32_t SFTP_Process(void)
{
    uint8_t cType = 0;	 

    p_sftp_msg_t pTaskmsg = NULL;

    pTaskmsg = SFTP_Get_Queue();     //��ȡIAP������������Ϣ             

    if(NULL == pTaskmsg)
    {
        return -1;  
    }

    cType = pTaskmsg->cSftp_data[0];   //��ȡ֡����

    switch(cType)
    {
    case SFTP_ANSWER:
    {
     
    }break;
    case SFTP_BINARY:                    
    {
        IAP_Binary_Process(pTaskmsg);        //������֡���ͽ��д���
    }break;
    case SFTP_CONTROL:
    {
        SFTP_Control_Process(pTaskmsg);       //�Կ���֡���ͽ��д���
    }break;
    case SFTP_DATASTATUS:
    {

    }break;
    default:;
    }

    SFTP_Del_Queue();

    return 0;
}

