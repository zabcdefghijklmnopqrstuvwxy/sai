/**
********************************************************************************
* @file    drv_flash.h
* @author  Sai
* @version UDFRM-B-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash驱动头文件
* @note    
* @attention
********************************************************************************
*/
#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"

/**
* @brief 页类型
*/
typedef enum{
    PAGE1K_TYPE = 0x01,
    PAGE2K_TYPE = 0x02
}page_type_t;

/**
* @brief flash数据擦除
* @param[in] nAddr 待擦除地址  nCountToErase 擦除的页数量
* @note flash擦除只针对页擦除
* @return 返回-1表示传入参数非法 返回-2表示擦除数据失败 返回0表示擦写操作正常
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t nCountToErase);

/**
* @brief flash数据写入
* @param nAddr 写入地址 pdata写入数据 nLen 写入数据长度
* @note None
* @retval 返回-1表示传入参数写数据无效非法 返回-2表示传入写地址非法 返回 0 表示写操作正常
*/
int32_t DRV_Write_Flash(uint32_t nAddr,uint8_t *pdata,uint32_t nLen);

#endif

