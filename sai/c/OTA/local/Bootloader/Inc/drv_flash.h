/**
********************************************************************************
* @file    drv_flash.h
* @author  Sai
* @version UDFRM-B-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash����ͷ�ļ�
* @note    
* @attention
********************************************************************************
*/
#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"

/**
* @brief ҳ����
*/
typedef enum{
    PAGE1K_TYPE = 0x01,
    PAGE2K_TYPE = 0x02
}page_type_t;

/**
* @brief flash���ݲ���
* @param[in] nAddr ��������ַ  nCountToErase ������ҳ����
* @note flash����ֻ���ҳ����
* @return ����-1��ʾ��������Ƿ� ����-2��ʾ��������ʧ�� ����0��ʾ��д��������
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t nCountToErase);

/**
* @brief flash����д��
* @param nAddr д���ַ pdataд������ nLen д�����ݳ���
* @note None
* @retval ����-1��ʾ�������д������Ч�Ƿ� ����-2��ʾ����д��ַ�Ƿ� ���� 0 ��ʾд��������
*/
int32_t DRV_Write_Flash(uint32_t nAddr,uint8_t *pdata,uint32_t nLen);

#endif

