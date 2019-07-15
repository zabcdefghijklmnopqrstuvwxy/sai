/**
********************************************************************************
* @file    sftp.h
* @author  Sai
* @version UDFRM-C-IAP-01V00-C01
* @date    2019-04-12
* @brief   sftp协议处理头文件
* @note    
* @attention
********************************************************************************
*/
#ifndef _SFTP_H_
#define _SFTP_H_

#include "stm32f10x.h"

#define   SFTP_QUEUE_MAX_NUM     8      /* sftp工作队列最大缓存 */
#define   SFTP_MSG_MAX_NUM       8      /* sftp信息数据最大数量 */

/**
* @brief sftp传输协议帧类型
*/
typedef enum
{
    SFTP_ANSWER        = 0x41,       /**< sftp应答帧类型，用来对收到的命令进行回应 */
	SFTP_BINARY        = 0x42,       /**< sftp数据帧类型，传送文件内容 */
	SFTP_CONTROL       = 0x43,       /**< sftp控制帧类型，用来发送文件传输所需的命令以控制文件传输的开始、传输与结束 */
	SFTP_DATASTATUS    = 0x44        /**< sftp数据状态帧类型，文件接收方接收到数据帧后的应答帧，用以文件传输的控制 */
}sftp_frame_type_t;

/**
* @brief IAP控制帧主命令
*/
typedef enum
{
    SYS_CMD   =   0x01,          /**< SFTP协议版本号自述与查询 */
    IAP_CMD   =   0x02,          /**< IAP传输文件交互命令 */
    GET_CMD   =   0x03,          /**< 文件接收命令 */
    LOG_CMD   =   0x04	         /**< 日志文件发送请求 */
}sftp_cmd_t;

/**
* @brief 应答帧状态码
*/
typedef enum
{
     AS_OK       = 0x00,       /**< 接收到合法命令确认帧 */
	 AS_HOLD     = 0x01,       /**< 暂停命令接收，此状态下接收到的任何命令将被忽略 */
	 AS_ERR      = 0x02        /**< 错误状态 */
}astatus_t;

/**
* @brief 应答帧错误码
*/
typedef enum
{
   AE_UNKNOWN_OBJ    = 0x01,   /**< 未知的主对象 */ 
   AE_UNKNOWN_SUBOBJ = 0x02,   /**< 未知的子对象 */
   AE_PARAM_INVALID  = 0x03,   /**< 非法参数 */
   AE_DATA_LOST      = 0x04    /**< 前一个数据块数据未完全接收 */
}aerr_t;

/**
* @brief 数据帧状态码
*/
typedef enum
{
   DS_READY          = 0x00,   /**< 准备好接收下一帧 */
   DS_FRAME_LOSS     = 0x01,   /**< 地址不连续，丢帧 */
   DS_HOLD           = 0x02,   /**< 暂停文件接收 */
   DS_ADDR_INVALID   = 0x03,   /**< 地址非法 */
   DS_INTER_ERR      = 0x04	   /**< 内部错误 */
}dstatus_t;

/**
* @brief 数据帧错误码
*/
typedef enum
{
   DE_IMAGE_INVALID    = 0x01,   /**< 文件格式错误 */
   DE_FILE_EXISTS      = 0x02,   /**< 文件已存在 */
   DE_DEVTYPE_INVALID  = 0x03,   /**< 设备类型不匹配 */
   DE_NOT_SAVED        = 0x04    /**< 文件无法保存 */
}dinter_err_t;

/**
* @brief 应答帧消息
*/
typedef struct
{ 
    uint8_t cFrameType;           /**< 应答帧类型 */
    uint8_t cObject;              /**< 应答帧对象 */
    uint8_t cSubObject;           /**< 应答帧子对象 */
    uint8_t cAStatus;             /**< 应答帧状态码 */
    uint8_t cAErrCode;            /**< 应答帧错误码 */
}answer_msg_t;

/**
* @brief 数据帧信息
*/
typedef struct
{
    uint8_t cFrameType;        /**< 数据帧类型 */
    uint8_t cReserved;         /**< 保留位 */
    uint8_t cOffsetLSB;        /**< 偏移低位 */
    uint8_t cOffestMSB;        /**< 偏移高位 */
    uint8_t cDataBuf[4];       /**< 数据帧信息 */
}binary_msg_t,*p_binary_msg_t;

/**
* @brief 控制帧消息
*/
typedef struct
{
    uint8_t cFrameType;          /**< 控制帧类型 */
    uint8_t cNode;               /**< 目标节点标示 */
    uint8_t cObject;             /**< 主对象，通过和子对象共同确定一个具体的命令 */
    uint8_t cSubObject;          /**< 子对象 */
    uint8_t cMsg[4];             /**< 具体的控制信息参数 */
}control_msg_t;

/**
* @brief 数据状态帧消息
*/
typedef struct
{
    uint8_t cFrameType;          /**< 数据状态帧类型 */
    uint8_t cDStatus;            /**< 数据状态码 */	
    union
    {
        struct
        {
            uint8_t cOffestLsb;        /**< 下一帧偏移低字节 */
            uint8_t cOffestMsb;        /**< 下一帧偏移高字节 */	
        }data_offest_t;		
        struct
        {
            uint8_t cDErr;             /**< 数据状态帧错误码 */
        }data_err_t;
    }data_info;	
}datastate_msg_t;

/**
* @brief sftp信息数据
*/
typedef struct
{
    uint8_t   cSftp_data[SFTP_MSG_MAX_NUM];  /**< SFTP接收的数据信息 */
    uint32_t  nLen;                          /**< SFTP接收数据长度 */
}sftp_msg_t,*p_sftp_msg_t;

/**
* @brief sftp工作队列
*/
typedef struct
{
    int32_t nFront;                           /**< 队列头 */
    int32_t nRear;                            /**< 队列尾 */
    sftp_msg_t msg[SFTP_QUEUE_MAX_NUM];       /**< 队列信息*/
}sftp_queue_t,*p_sftp_queue_t;

/**
* @brief 添加CAN数据帧工作任务队列
* @param[in] pdata信息数据 nLen信息数据长度 
* @note None
* @return 返回-1表示参数非法 返回-2表示工作队列已满 返回0表示添加工作队列成功
*/
int32_t SFTP_Add_Queue(uint8_t *pdata,uint32_t nLen);

/**
* @brief CAN数据帧工作队列满判断
* @param None
* @note None
* @return 返回0表示未满 返回1表示已满
*/
int32_t SFTP_IsFull_Queue(void);

/**
* @brief SFTP协议数据处理
* @param None 
* @note None
* @return None
*/
int32_t SFTP_Process(void);

#endif

