/**
********************************************************************************
* @file    drv_flash.h
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash驱动头文件
* @note    
* @attention
********************************************************************************
*/

#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include "zd510_driver.h"

#define SECTOR_SIZE  2048                 /**< flash 页大小 */

#if 0
/**
* @brief 页类型
*/
typedef enum{
    PAGE1K_TYPE = 0x01,
    PAGE2K_TYPE = 0x02
}page_type_t;
#endif

/**
* @brief flash数据擦除
* @param nAddr 擦除地址 
* @note flash擦除只针对页擦除
* @return 错误码
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t countToWrite);

/**
* @brief flash数据写入
* @param nAddr 写入地址 pdata写入数据 nLen 写入数据长度
* @note None
* @return None
*/
int32_t DRV_Write_Flash(uint32_t nAddr,uint8_t *pdata,uint32_t nLen);



#if 0
/**
* @brief  单元测试flash的读写
* @param None
* @note None
* @return 返回0 表示正确 返回非0表示读写异常
*/
int unittest_flash(void);
#endif

#endif
