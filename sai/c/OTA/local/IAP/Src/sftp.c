/**
********************************************************************************
* @file    sftp.c
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   sftp协议处理源代码
* @note    
* @attention
********************************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "sftp.h"
#include "iap.h"

/** 该版程序支持的SFTP版本号为0x01 */
#define   SFTP_VERSION_NUM        0x01

/* IAP工作任务队列实例化 */
static sftp_queue_t SftpTask;

/**
* @brief 获取CAN数据帧任务工作队列信息
* @param None
* @note None
* @return 返回NULL表示工作队列为空 返回非空表示正常信息数据
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
* @brief 删除CAN数据帧工作队列
* @param None
* @note None
* @return 返回-1表示队列已经为空不需要删除 返回0表示删除队列成功
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
* @brief 添加CAN数据帧工作任务队列
* @param[in] pdata信息数据 nLen信息数据长度 
* @note None
* @return 返回-1表示参数非法 返回-2表示工作队列已满 返回0表示添加工作队列成功
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
* @brief CAN数据帧工作队列满判断
* @param None
* @note None
* @return 返回0表示未满 返回1表示已满
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
* @brief SFTP控制命令消息处理
* @param[in] pmsg 控制帧信息 
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

    cObject = pmsg->cSftp_data[2];             //赋值主对象
    cSubObject = pmsg->cSftp_data[3];          //赋值子对象

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
        cmsg.cMsg[0] = SFTP_VERSION_NUM;     //版本号信息
            
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
    default:                                   //对于不识别的主对象进行错误应答
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
* @brief SFTP协议数据处理
* @param None 
* @note None
* @return None
*/
int32_t SFTP_Process(void)
{
    uint8_t cType = 0;	 

    p_sftp_msg_t pTaskmsg = NULL;

    pTaskmsg = SFTP_Get_Queue();     //获取IAP任务工作队列消息             

    if(NULL == pTaskmsg)
    {
        return -1;  
    }

    cType = pTaskmsg->cSftp_data[0];   //获取帧类型

    switch(cType)
    {
    case SFTP_ANSWER:
    {
     
    }break;
    case SFTP_BINARY:                    
    {
        IAP_Binary_Process(pTaskmsg);        //对数据帧类型进行处理
    }break;
    case SFTP_CONTROL:
    {
        SFTP_Control_Process(pTaskmsg);       //对控制帧类型进行处理
    }break;
    case SFTP_DATASTATUS:
    {

    }break;
    default:;
    }

    SFTP_Del_Queue();

    return 0;
}

