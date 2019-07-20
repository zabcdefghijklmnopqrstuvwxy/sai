/**
********************************************************************************
* @file    drv_flash.h
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash����ͷ�ļ�
* @note    
* @attention
********************************************************************************
*/

#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include "zd510_driver.h"

#define SECTOR_SIZE  2048                 /**< flash ҳ��С */

#if 0
/**
* @brief ҳ����
*/
typedef enum{
    PAGE1K_TYPE = 0x01,
    PAGE2K_TYPE = 0x02
}page_type_t;
#endif

/**
* @brief flash���ݲ���
* @param nAddr ������ַ 
* @note flash����ֻ���ҳ����
* @return ������
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t countToWrite);

/**
* @brief flash����д��
* @param nAddr д���ַ pdataд������ nLen д�����ݳ���
* @note None
* @return None
*/
int32_t DRV_Write_Flash(uint32_t nAddr,uint8_t *pdata,uint32_t nLen);



#if 0
/**
* @brief  ��Ԫ����flash�Ķ�д
* @param None
* @note None
* @return ����0 ��ʾ��ȷ ���ط�0��ʾ��д�쳣
*/
int unittest_flash(void);
#endif

#endif
