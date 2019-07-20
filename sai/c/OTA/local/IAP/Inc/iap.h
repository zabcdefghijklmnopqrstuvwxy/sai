/**
********************************************************************************
* @file    iap.h
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   �����ļ������뱣��ͷ�ļ�
* @note    
* @attention
********************************************************************************
*/
#ifndef _IAP_H_
#define _IAP_H_

#include "stm32f10x.h"
#include "can.h"
#include "sftp.h"

#define   IAP_MEMORY_SIZE       256       /**< IAP��memory�����С����Ҫ���ڱ�����յ����ļ����� */

#define   VERSION_INQUIRE       0x01      /**< SFTPЭ��汾���������ѯ���� */
#define   FILE_PUT_REQUEST      0x01      /**< �����ļ��������� */
#define   FILE_CHECKSUM         0x02      /**< �����ļ�У�������� */
#define   FILE_SAVE_SUCCESS     0x03      /**< �ļ�����ɹ�ָʾ���� */

#define   INTERVAL_TIME      31        /**< �ӻ����͵ļ��ʱ�䵥λms */

#define   UPGRADE_FILE_FLASH_ERR             0x01  /**< �����ļ�flash���� */
#define   UPGRADE_FILE_ORDER_SEQUENCE_ERR    0x02  /**< �����ļ����������д��� */


/**
* @brief �����ļ�����ָʾ״̬��
*/
typedef enum
{
    UPGRADE_FILE_READY      = 0x0,   /**< �����ļ����� */
    CHECKSUM_WRONG          = 0x01,  /**< CRCУ����� */
    ABNORMAL_TERMINATE      = 0x02   /**< �쳣��ֹ */
}upgrade_file_indicate_t;

/**
* @brief IAP����buf����
*/
typedef struct
{
	 unsigned int  nWritePos;                  /**< дλ�� */
	 unsigned int  nReadPos;                   /**< ��λ�� */
   unsigned char membuf[IAP_MEMORY_SIZE + 4];  /**< buf���� */
   unsigned int  nCnt;                         /**< ��¼д�²��һ�δ����ȡ��Ŀ */
}iap_memory_t;

/**
* @brief ��ͷ��Ϣ
*/
typedef struct
{
   unsigned short sTableHead;             /**< ��ͷħ����Ϣ */
   unsigned short sTableVersion;          /**< ��ͷ�汾�� */
   unsigned char  cTag0Offest;            /**< Tag0�����ƫ���� */
   unsigned char  cReserve1[7];           /**< ������ */
   unsigned short Tag0Length;             /**< ��ͷ���� */
   unsigned char  cDevType[3];            /**< �����ļ��豸���� */
   unsigned char  cReserve2;              /**< ������ǿ��Ϊ0 */
   unsigned short sImageVersion;          /**< �����ļ��汾�� */
   unsigned short sImageCrc;              /**< �����ļ�У���� */
   unsigned short sImageLength;           /**< �����ļ����� */
   unsigned short sTableCrc;              /**< ��ͷУ���� */
}table_head_t,*p_table_head_t;

/**
* @brief IAP��ʼ������
* @param None 
* @note �����AppBak����ַ���ݣ���ʼ��AppBakHeadͷ��Ϣ
* @return None
*/
void IAP_Init(void);

/**
* @brief IAP���˷�IAP֡��Ϣ
* @param[in] pdata ��Ҫ���˵���Ϣ���� nLen �������ݳ���
* @note None
* @return None
*/
int IAP_Filter_Task(uint8_t *pdata,uint32_t nLen);

/**
* @brief IAP������
* @param None 
* @note None
* @return None
*/
void IAP_Task_Process(void);

/**
* @brief IAP����������Ϣ����
* @param[in] pmsg ����֡��Ϣ 
* @note None
* @return None
*/
int32_t IAP_Control_Process(p_sftp_msg_t pmsg);

/**
* @brief IAP������
* @param[in] pmsg ����֡��Ϣ 
* @note None
* @retval None
*/
int32_t IAP_Binary_Process(p_sftp_msg_t pmsg);


#if 0

/**
* @brief IAP���˷�IAP֡��Ϣ
* @param[in] pdata ��Ҫ���˵���Ϣ���� nLen �������ݳ���
* @note None
* @retval None
*/
int IAP_HostTask_Test(void);

/**
* @brief IAP�¼�����
* @param None 
* @note None
* @retval None
*/
int IAP_HostEventTest(void);

#endif

#endif
