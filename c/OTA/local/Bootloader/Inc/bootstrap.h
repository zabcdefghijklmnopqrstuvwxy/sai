/**
********************************************************************************
* @file    bootstrap.h
* @author  Sai
* @version UDFRM-B-S01-01V00-C01
* @date    2019-04-12
* @brief   ����Ƿ���Ҫ����������Appͷ�ļ�
* @note    
* @attention
********************************************************************************
*/

#ifndef _BOOTSTARP_H_
#define _BOOTSTARP_H_

#include "stm32f1xx_hal.h"


#define   BOOTLOADER_SIZE  16              /**< bootloader��flash����ռ��С����λΪK */
#define   TABLE_MAGIC_NUM  0x4655          /**< ��ͷħ�� */
#define   TABLE_SIZE       2048            /**< ��ĳ��ȴ�С����λΪByte */



#define    ERR_OK                              0              /**< ���� */
#define    ERR_APP_FAILED                     -1              /**< ����ʧ�ܣ���App��AppBak���ԷǷ���Ч */
#define    ERR_MAGICNUM_WRONG                 -2              /**< ��ͷħ������ */
#define    ERR_CHECKSUM_WRONG                 -3              /**< У��ʧ�� */
#define    ERR_IMAGEVERSION_INVALID           -4              /**< ӳ���ļ��汾����Ч */
#define    ERR_STACKPOINT_ILLEGA              -5              /**< ջ��ָ��Ƿ� */
#define    ERR_ERASEFLASH_FAILED              -6              /**< flash����ʧ�� */
#define    ERR_PROGRAMFLASH_FAILED            -7              /**< flash д��ʧ�� */

#pragma pack (1) 

/**
* @brief ����Ϣ
*/
typedef struct
{
   uint16_t  usTableHead;             /**< ��ͷħ����Ϣ */
   uint16_t  usTableVersion;          /**< ��ͷ�汾�� */
   uint8_t   ucTag0Offest;            /**< Tag0�����ƫ���� */
   uint8_t   ucReserve1[7];           /**< ������ */
   uint16_t  usTag0Length;            /**< ���� */
   uint8_t   ucDevType[3];            /**< �����ļ��豸���� */
   uint8_t   ucReserve2;              /**< ������ǿ��Ϊ0 */
   uint16_t  usImageVersion;          /**< �����ļ��汾�� */
   uint16_t  usImageCrc;              /**< �����ļ�У���� */
   uint16_t  usImageLength;           /**< �����ļ����� */
   uint16_t  usTableCrc;              /**< ��ͷУ���� */
}table_head_t,*p_table_head_t;

#pragma pack () 


/**
* @brief ����Ƿ���Ҫ����
* @param None
* @note ͨ����ȡ���App��AppBak��TableHead�õ��Ľ������������������Ӧ�Ĵ���
* @return None
*/
void BOOT_Check_Image(void);

#if 0
void UnitTest_Write_Head(app_type_t itype);


/**
* @brief ��Ԫ����д���ͷ
* @param itype app������ 
*     @arg APP_TYPE ��App��ͷ���ݽ���д��
*     @arg APPBAK_TYPE ��AppBakͷ����д��
*     @arg 0 ��App��AppBakͷ���ݽ��в���
* @note None
* @retval None
*/
void UnitTest_CRC(void);
#endif

#endif