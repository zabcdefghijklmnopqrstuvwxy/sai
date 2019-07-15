/**
********************************************************************************
* @file    drv_flash.c
* @author  Sai
* @version UDFRM-B-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash����Դ�ļ�
* @note    
* @attention
********************************************************************************
*/

#include "drv_flash.h"


#define SECTOR_SIZE  2048                 /**< flash ҳ��С */
#define FLASH_SIZE   256                  /**< flash ��С */

extern void    FLASH_PageErase(uint32_t PageAddress);

/**
* @brief flash���ݲ���
* @param[in] nAddr ��������ַ  nCountToErase ������ҳ����
* @note flash����ֻ���ҳ����
* @return ����-1��ʾ��������Ƿ� ����-2��ʾ��������ʧ�� ����0��ʾ��д��������
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t nCountToErase)
{
    uint32_t unErr = 0;
    uint32_t i = 0;
    uint32_t unSize = 0;
    FLASH_EraseInitTypeDef EraseCfg;
       
    uint32_t nOffsetAddress = nAddr - FLASH_BASE;                             //����ȥ��0X08000000���ʵ��ƫ�Ƶ�ַ
    uint32_t nSectorPosition = nOffsetAddress/SECTOR_SIZE;                    //����������ַ
    uint32_t nSectorStartAddress = nSectorPosition*SECTOR_SIZE+FLASH_BASE;    //��Ӧ�������׵�ַ

    unSize = nCountToErase * SECTOR_SIZE; 
    
    if(nAddr<FLASH_BASE||((nAddr+nCountToErase*SECTOR_SIZE)>=(FLASH_BASE + SECTOR_SIZE * FLASH_SIZE)))
    {
       return -1;
    }
        
    EraseCfg.Banks = FLASH_BANK_1;
    EraseCfg.NbPages = nCountToErase;
    EraseCfg.PageAddress = nSectorStartAddress;
    EraseCfg.TypeErase = FLASH_TYPEERASE_PAGES;
  
    HAL_FLASH_Unlock();         //����д����
    
    HAL_FLASHEx_Erase(&EraseCfg,&unErr);
      
    HAL_FLASH_Lock();
    
    for(i = 0; i < unSize; i = i + 4)
    {
       if(0xffffffff != *(uint32_t*)(nSectorStartAddress + i)) 
       {
            break;   
       }              
    }
    
    if(i < unSize)
    {
       return -2;    
    }
       
    return 0;
}

/**
* @brief flash����д��
* @param nAddrд���ַ pdataд������ nLenд�����ݳ���
* @note None
* @return ����-1��ʾ�������д������Ч�Ƿ� ����-2��ʾ����д��ַ�Ƿ� ����0��ʾд������
*/
int32_t DRV_Write_Flash(uint32_t nAddr,uint8_t *pdata,uint32_t nLen)
{
    int32_t i = 0;
    int32_t nErr = 0;
    uint32_t lData = 0;
    
    if(NULL == pdata)
    {
       return -1;
    }
    
    if(nAddr<FLASH_BASE || ((nAddr+nLen)>=(FLASH_BASE + SECTOR_SIZE * FLASH_SIZE)))
    {
       return -2;
    }
    
    if(nLen > 2)
    {        
        for(i = 0;i < nLen;i = i + 4)
        {
           lData = ((uint32_t)pdata[i + 3] << 24) | ((uint32_t)pdata[i + 2] << 16) | ((uint32_t)pdata[i + 1] << 8) | ((uint32_t)pdata[i]);
               
           HAL_FLASH_Unlock();         //����д���� 
           nErr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,nAddr + i,lData);
           HAL_FLASH_Lock();         //����д����
        }
    }
    else
    {
           lData = ( ((uint32_t)pdata[i + 1] << 8) | ((uint32_t)pdata[i]));
               
           HAL_FLASH_Unlock();         //����д���� 
           nErr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,nAddr + i,lData);
           HAL_FLASH_Lock();         //����д����
    }
        
    return nErr;
}

