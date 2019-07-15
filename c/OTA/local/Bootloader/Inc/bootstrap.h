/**
********************************************************************************
* @file    bootstrap.h
* @author  Sai
* @version UDFRM-B-S01-01V00-C01
* @date    2019-04-12
* @brief   检查是否需要升级并引导App头文件
* @note    
* @attention
********************************************************************************
*/

#ifndef _BOOTSTARP_H_
#define _BOOTSTARP_H_

#include "stm32f1xx_hal.h"


#define   BOOTLOADER_SIZE  16              /**< bootloader的flash分配空间大小，单位为K */
#define   TABLE_MAGIC_NUM  0x4655          /**< 表头魔数 */
#define   TABLE_SIZE       2048            /**< 表的长度大小，单位为Byte */



#define    ERR_OK                              0              /**< 正常 */
#define    ERR_APP_FAILED                     -1              /**< 引导失败，即App和AppBak区皆非法无效 */
#define    ERR_MAGICNUM_WRONG                 -2              /**< 表头魔数错误 */
#define    ERR_CHECKSUM_WRONG                 -3              /**< 校验失败 */
#define    ERR_IMAGEVERSION_INVALID           -4              /**< 映像文件版本号无效 */
#define    ERR_STACKPOINT_ILLEGA              -5              /**< 栈顶指针非法 */
#define    ERR_ERASEFLASH_FAILED              -6              /**< flash擦除失败 */
#define    ERR_PROGRAMFLASH_FAILED            -7              /**< flash 写入失败 */

#pragma pack (1) 

/**
* @brief 表信息
*/
typedef struct
{
   uint16_t  usTableHead;             /**< 表头魔术信息 */
   uint16_t  usTableVersion;          /**< 表头版本号 */
   uint8_t   ucTag0Offest;            /**< Tag0的相对偏移量 */
   uint8_t   ucReserve1[7];           /**< 保留区 */
   uint16_t  usTag0Length;            /**< 表长度 */
   uint8_t   ucDevType[3];            /**< 升级文件设备类型 */
   uint8_t   ucReserve2;              /**< 保留区强制为0 */
   uint16_t  usImageVersion;          /**< 升级文件版本号 */
   uint16_t  usImageCrc;              /**< 升级文件校验码 */
   uint16_t  usImageLength;           /**< 升级文件长度 */
   uint16_t  usTableCrc;              /**< 表头校验码 */
}table_head_t,*p_table_head_t;

#pragma pack () 


/**
* @brief 检查是否需要升级
* @param None
* @note 通过获取检查App和AppBak的TableHead得到的结果，根据其结果进行相应的处理。
* @return None
*/
void BOOT_Check_Image(void);

#if 0
void UnitTest_Write_Head(app_type_t itype);


/**
* @brief 单元测试写入表头
* @param itype app的类型 
*     @arg APP_TYPE 将App的头数据进行写入
*     @arg APPBAK_TYPE 将AppBak头数据写入
*     @arg 0 将App和AppBak头数据进行擦除
* @note None
* @retval None
*/
void UnitTest_CRC(void);
#endif

#endif