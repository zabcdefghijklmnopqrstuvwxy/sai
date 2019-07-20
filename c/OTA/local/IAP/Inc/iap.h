/**
********************************************************************************
* @file    iap.h
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   升级文件交互与保存头文件
* @note    
* @attention
********************************************************************************
*/
#ifndef _IAP_H_
#define _IAP_H_

#include "stm32f10x.h"
#include "can.h"
#include "sftp.h"

#define   IAP_MEMORY_SIZE       256       /**< IAP的memory缓存大小，主要用于保存接收到的文件数据 */

#define   VERSION_INQUIRE       0x01      /**< SFTP协议版本号自述与查询命令 */
#define   FILE_PUT_REQUEST      0x01      /**< 升级文件请求命令 */
#define   FILE_CHECKSUM         0x02      /**< 发送文件校验码命令 */
#define   FILE_SAVE_SUCCESS     0x03      /**< 文件保存成功指示命令 */

#define   INTERVAL_TIME      31        /**< 从机发送的间隔时间单位ms */

#define   UPGRADE_FILE_FLASH_ERR             0x01  /**< 升级文件flash错误 */
#define   UPGRADE_FILE_ORDER_SEQUENCE_ERR    0x02  /**< 升级文件的命令序列错误 */


/**
* @brief 升级文件就绪指示状态码
*/
typedef enum
{
    UPGRADE_FILE_READY      = 0x0,   /**< 升级文件就绪 */
    CHECKSUM_WRONG          = 0x01,  /**< CRC校验错误 */
    ABNORMAL_TERMINATE      = 0x02   /**< 异常终止 */
}upgrade_file_indicate_t;

/**
* @brief IAP缓存buf管理
*/
typedef struct
{
	 unsigned int  nWritePos;                  /**< 写位置 */
	 unsigned int  nReadPos;                   /**< 读位置 */
   unsigned char membuf[IAP_MEMORY_SIZE + 4];  /**< buf缓存 */
   unsigned int  nCnt;                         /**< 记录写下并且还未被读取数目 */
}iap_memory_t;

/**
* @brief 表头信息
*/
typedef struct
{
   unsigned short sTableHead;             /**< 表头魔数信息 */
   unsigned short sTableVersion;          /**< 表头版本号 */
   unsigned char  cTag0Offest;            /**< Tag0的相对偏移量 */
   unsigned char  cReserve1[7];           /**< 保留区 */
   unsigned short Tag0Length;             /**< 表头长度 */
   unsigned char  cDevType[3];            /**< 升级文件设备类型 */
   unsigned char  cReserve2;              /**< 保留区强制为0 */
   unsigned short sImageVersion;          /**< 升级文件版本号 */
   unsigned short sImageCrc;              /**< 升级文件校验码 */
   unsigned short sImageLength;           /**< 升级文件长度 */
   unsigned short sTableCrc;              /**< 表头校验码 */
}table_head_t,*p_table_head_t;

/**
* @brief IAP初始化操作
* @param None 
* @note 计算出AppBak区地址内容，初始化AppBakHead头信息
* @return None
*/
void IAP_Init(void);

/**
* @brief IAP过滤非IAP帧消息
* @param[in] pdata 需要过滤的信息数据 nLen 过滤数据长度
* @note None
* @return None
*/
int IAP_Filter_Task(uint8_t *pdata,uint32_t nLen);

/**
* @brief IAP任务处理
* @param None 
* @note None
* @return None
*/
void IAP_Task_Process(void);

/**
* @brief IAP控制命令消息处理
* @param[in] pmsg 控制帧信息 
* @note None
* @return None
*/
int32_t IAP_Control_Process(p_sftp_msg_t pmsg);

/**
* @brief IAP任务处理
* @param[in] pmsg 数据帧信息 
* @note None
* @retval None
*/
int32_t IAP_Binary_Process(p_sftp_msg_t pmsg);


#if 0

/**
* @brief IAP过滤非IAP帧消息
* @param[in] pdata 需要过滤的信息数据 nLen 过滤数据长度
* @note None
* @retval None
*/
int IAP_HostTask_Test(void);

/**
* @brief IAP事件处理
* @param None 
* @note None
* @retval None
*/
int IAP_HostEventTest(void);

#endif

#endif
