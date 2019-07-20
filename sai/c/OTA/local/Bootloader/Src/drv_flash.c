/**
********************************************************************************
* @file    drv_flash.c
* @author  Sai
* @version UDFRM-B-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash驱动源文件
* @note    
* @attention
********************************************************************************
*/

#include "drv_flash.h"


#define SECTOR_SIZE  2048                 /**< flash 页大小 */
#define FLASH_SIZE   256                  /**< flash 大小 */

extern void    FLASH_PageErase(uint32_t PageAddress);

/**
* @brief flash数据擦除
* @param[in] nAddr 待擦除地址  nCountToErase 擦除的页数量
* @note flash擦除只针对页擦除
* @return 返回-1表示传入参数非法 返回-2表示擦除数据失败 返回0表示擦写操作正常
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t nCountToErase)
{
    uint32_t unErr = 0;
    uint32_t i = 0;
    uint32_t unSize = 0;
    FLASH_EraseInitTypeDef EraseCfg;
       
    uint32_t nOffsetAddress = nAddr - FLASH_BASE;                             //计算去掉0X08000000后的实际偏移地址
    uint32_t nSectorPosition = nOffsetAddress/SECTOR_SIZE;                    //计算扇区地址
    uint32_t nSectorStartAddress = nSectorPosition*SECTOR_SIZE+FLASH_BASE;    //对应扇区的首地址

    unSize = nCountToErase * SECTOR_SIZE; 
    
    if(nAddr<FLASH_BASE||((nAddr+nCountToErase*SECTOR_SIZE)>=(FLASH_BASE + SECTOR_SIZE * FLASH_SIZE)))
    {
       return -1;
    }
        
    EraseCfg.Banks = FLASH_BANK_1;
    EraseCfg.NbPages = nCountToErase;
    EraseCfg.PageAddress = nSectorStartAddress;
    EraseCfg.TypeErase = FLASH_TYPEERASE_PAGES;
  
    HAL_FLASH_Unlock();         //解锁写保护
    
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
* @brief flash数据写入
* @param nAddr写入地址 pdata写入数据 nLen写入数据长度
* @note None
* @return 返回-1表示传入参数写数据无效非法 返回-2表示传入写地址非法 返回0表示写入正常
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
               
           HAL_FLASH_Unlock();         //解锁写保护 
           nErr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,nAddr + i,lData);
           HAL_FLASH_Lock();         //上锁写保护
        }
    }
    else
    {
           lData = ( ((uint32_t)pdata[i + 1] << 8) | ((uint32_t)pdata[i]));
               
           HAL_FLASH_Unlock();         //解锁写保护 
           nErr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,nAddr + i,lData);
           HAL_FLASH_Lock();         //上锁写保护
    }
        
    return nErr;
}

