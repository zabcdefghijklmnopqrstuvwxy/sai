/**
********************************************************************************
* @file    sftp.h
* @author  Sai
* @version UDFRM-C-IAP-01V00-C01
* @date    2019-04-12
* @brief   sftpЭ�鴦��ͷ�ļ�
* @note    
* @attention
********************************************************************************
*/
#ifndef _SFTP_H_
#define _SFTP_H_

#include "stm32f10x.h"

#define   SFTP_QUEUE_MAX_NUM     8      /* sftp����������󻺴� */
#define   SFTP_MSG_MAX_NUM       8      /* sftp��Ϣ����������� */

/**
* @brief sftp����Э��֡����
*/
typedef enum
{
    SFTP_ANSWER        = 0x41,       /**< sftpӦ��֡���ͣ��������յ���������л�Ӧ */
	SFTP_BINARY        = 0x42,       /**< sftp����֡���ͣ������ļ����� */
	SFTP_CONTROL       = 0x43,       /**< sftp����֡���ͣ����������ļ���������������Կ����ļ�����Ŀ�ʼ����������� */
	SFTP_DATASTATUS    = 0x44        /**< sftp����״̬֡���ͣ��ļ����շ����յ�����֡���Ӧ��֡�������ļ�����Ŀ��� */
}sftp_frame_type_t;

/**
* @brief IAP����֡������
*/
typedef enum
{
    SYS_CMD   =   0x01,          /**< SFTPЭ��汾���������ѯ */
    IAP_CMD   =   0x02,          /**< IAP�����ļ��������� */
    GET_CMD   =   0x03,          /**< �ļ��������� */
    LOG_CMD   =   0x04	         /**< ��־�ļ��������� */
}sftp_cmd_t;

/**
* @brief Ӧ��֡״̬��
*/
typedef enum
{
     AS_OK       = 0x00,       /**< ���յ��Ϸ�����ȷ��֡ */
	 AS_HOLD     = 0x01,       /**< ��ͣ������գ���״̬�½��յ����κ���������� */
	 AS_ERR      = 0x02        /**< ����״̬ */
}astatus_t;

/**
* @brief Ӧ��֡������
*/
typedef enum
{
   AE_UNKNOWN_OBJ    = 0x01,   /**< δ֪�������� */ 
   AE_UNKNOWN_SUBOBJ = 0x02,   /**< δ֪���Ӷ��� */
   AE_PARAM_INVALID  = 0x03,   /**< �Ƿ����� */
   AE_DATA_LOST      = 0x04    /**< ǰһ�����ݿ�����δ��ȫ���� */
}aerr_t;

/**
* @brief ����֡״̬��
*/
typedef enum
{
   DS_READY          = 0x00,   /**< ׼���ý�����һ֡ */
   DS_FRAME_LOSS     = 0x01,   /**< ��ַ����������֡ */
   DS_HOLD           = 0x02,   /**< ��ͣ�ļ����� */
   DS_ADDR_INVALID   = 0x03,   /**< ��ַ�Ƿ� */
   DS_INTER_ERR      = 0x04	   /**< �ڲ����� */
}dstatus_t;

/**
* @brief ����֡������
*/
typedef enum
{
   DE_IMAGE_INVALID    = 0x01,   /**< �ļ���ʽ���� */
   DE_FILE_EXISTS      = 0x02,   /**< �ļ��Ѵ��� */
   DE_DEVTYPE_INVALID  = 0x03,   /**< �豸���Ͳ�ƥ�� */
   DE_NOT_SAVED        = 0x04    /**< �ļ��޷����� */
}dinter_err_t;

/**
* @brief Ӧ��֡��Ϣ
*/
typedef struct
{ 
    uint8_t cFrameType;           /**< Ӧ��֡���� */
    uint8_t cObject;              /**< Ӧ��֡���� */
    uint8_t cSubObject;           /**< Ӧ��֡�Ӷ��� */
    uint8_t cAStatus;             /**< Ӧ��֡״̬�� */
    uint8_t cAErrCode;            /**< Ӧ��֡������ */
}answer_msg_t;

/**
* @brief ����֡��Ϣ
*/
typedef struct
{
    uint8_t cFrameType;        /**< ����֡���� */
    uint8_t cReserved;         /**< ����λ */
    uint8_t cOffsetLSB;        /**< ƫ�Ƶ�λ */
    uint8_t cOffestMSB;        /**< ƫ�Ƹ�λ */
    uint8_t cDataBuf[4];       /**< ����֡��Ϣ */
}binary_msg_t,*p_binary_msg_t;

/**
* @brief ����֡��Ϣ
*/
typedef struct
{
    uint8_t cFrameType;          /**< ����֡���� */
    uint8_t cNode;               /**< Ŀ��ڵ��ʾ */
    uint8_t cObject;             /**< ������ͨ�����Ӷ���ͬȷ��һ����������� */
    uint8_t cSubObject;          /**< �Ӷ��� */
    uint8_t cMsg[4];             /**< ����Ŀ�����Ϣ���� */
}control_msg_t;

/**
* @brief ����״̬֡��Ϣ
*/
typedef struct
{
    uint8_t cFrameType;          /**< ����״̬֡���� */
    uint8_t cDStatus;            /**< ����״̬�� */	
    union
    {
        struct
        {
            uint8_t cOffestLsb;        /**< ��һ֡ƫ�Ƶ��ֽ� */
            uint8_t cOffestMsb;        /**< ��һ֡ƫ�Ƹ��ֽ� */	
        }data_offest_t;		
        struct
        {
            uint8_t cDErr;             /**< ����״̬֡������ */
        }data_err_t;
    }data_info;	
}datastate_msg_t;

/**
* @brief sftp��Ϣ����
*/
typedef struct
{
    uint8_t   cSftp_data[SFTP_MSG_MAX_NUM];  /**< SFTP���յ�������Ϣ */
    uint32_t  nLen;                          /**< SFTP�������ݳ��� */
}sftp_msg_t,*p_sftp_msg_t;

/**
* @brief sftp��������
*/
typedef struct
{
    int32_t nFront;                           /**< ����ͷ */
    int32_t nRear;                            /**< ����β */
    sftp_msg_t msg[SFTP_QUEUE_MAX_NUM];       /**< ������Ϣ*/
}sftp_queue_t,*p_sftp_queue_t;

/**
* @brief ���CAN����֡�����������
* @param[in] pdata��Ϣ���� nLen��Ϣ���ݳ��� 
* @note None
* @return ����-1��ʾ�����Ƿ� ����-2��ʾ������������ ����0��ʾ��ӹ������гɹ�
*/
int32_t SFTP_Add_Queue(uint8_t *pdata,uint32_t nLen);

/**
* @brief CAN����֡�����������ж�
* @param None
* @note None
* @return ����0��ʾδ�� ����1��ʾ����
*/
int32_t SFTP_IsFull_Queue(void);

/**
* @brief SFTPЭ�����ݴ���
* @param None 
* @note None
* @return None
*/
int32_t SFTP_Process(void);

#endif

