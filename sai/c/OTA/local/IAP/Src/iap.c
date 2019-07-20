/**
********************************************************************************
* @file    iap.c
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   �����ļ������뱣��Դ����
* @note    
* @attention
********************************************************************************
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "iap.h"
#include "drv_flash.h"

/* App���� */
typedef enum{
    APP_TYPE = 0x01,            
    APPBAK_TYPE = 0x02
}app_type_t;

/* IAP�¼��� */
typedef enum
{     
	IAP_QUEUE_FULL_EVENT            = 0x00,         /* �������������¼����� */
	IAP_MEMORY_FULL_EVENT           = 0x01,         /* iap������ */	 	
    IAP_BLOCK_INCOMPLETE            = 0x02,         /* ���ݿ�����¼� */
	IAP_FLASH_ERR_EVENT             = 0x03,         /* flash����ʧ�� */ 	 
	IAP_IMAGE_SAVE_SUCCESS_EVENT    = 0x04,         /* ӳ���ļ������¼��ɹ� */
          
 // IAPHOST_UPDATE                  = 0x08,         /* ��ʼ���������ļ������¼������ڲ��� */
 //	IAPHOST_BINARY                  = 0x09,         /* ���Է��������¼������ڲ��� */
 // IAPHOST_COMPLETE                = 0x0a          /* ���ڲ��� */
}iap_event_t;

/* IAP״̬�� */
typedef enum
{
    IAP_START_FILE_TRANSPORT_STATE  = 0x00,         /* ��ʼ�ļ������¼�״̬��־ */
    IAP_BLOCK_COMPLETE_STATE        = 0x01,         /* ���͵�һ�����ݿ���� */
    IAP_RECVFILE_COMPLETE_STATE     = 0x02,         /* ���յ���������һ���ļ�*/
    IAP_IMAGE_UPDATE_STATE          = 0x03,         /* �豸������ƥ�� */
    IAP_ERASE_FLASH_STATE           = 0x04,         /* ��ʼflash����״̬ */
    IAP_DEVTYPE_UNMATCH_STATE       = 0x05,         /* �豸���Ͳ�ƥ�� */
    IAP_DATASUSPEND_STATE           = 0x06,         /* IAP�������ݹ����¼� */ 
    IAP_CMDSUSPEND_STATE            = 0x07          /* IAP������������¼� */
}iap_state_t;

/** App�����豸���͵�ַ */
static  uint8_t *pcDevName = (uint8_t *)0x08004820;    //App���豸���ͺ�
/** AppBak���İ汾�ŵĵ�ַ */
static  uint8_t *pAppBakVer = (uint8_t *)0x08042824;  //AppBak�İ汾��

/** bootloader�����flash��С����λΪK */
#define   BOOTLOADER_SIZE         16    
/** ��ͷ�Ĵ�С��λΪK */
#define   TABLE_SIZE         2    
/** ���ͷħ����Ϣ */
#define   TABLE_MAGIC_NUM  0x4655                  

#define   ERASE_PAGE_NUM          1   /* �����������Ŀ����ȴ�С */
#define   FLASHSIZE_REG  0x1FFFF7E0   /* flash��С�ļĴ�����ַ��ֻ���ST�����103ϵ�� */  
#define   IAP_EVENT_MAX_NUM       16  /* IAP�¼�������󳤶� */
#define   IAP_STATE_MAX_NUM       16  /* IAP״̬������󳤶� */
#define   IAP_PROGRAMMER_NUM      16  /* ����һ������д������λΪByet */

static iap_event_t cIapEvent[IAP_EVENT_MAX_NUM];  /* iap�¼�����ʵ���� */
static iap_state_t cIapState[IAP_STATE_MAX_NUM];  /* iap״̬����ʵ���� */ 
static uint8_t cEraseRecord[128];                 /* ҳ������¼ */

static iap_memory_t IapBuf;        /* iap�������ļ����ݻ���ʵ���� */   
static uint16_t sImageCrc;         /* �����ļ���CRCУ���� */ 
static uint16_t sImageLength;      /* �����ļ��ĳ��ȵ�λΪ�� */   
static uint32_t nBaseAddr;         /* �����ļ������ݿ��flash��ַ */
static uint16_t sDataBlock_Len;    /* �������ݿ鳤�ȣ�����Ϊ��λ */
static uint16_t sDataOffest;       /* ��һ�ε������ļ�ƫ�������յ������ļ�����ʱ������ */ 
static uint32_t  nFlashSize;       /* ����flash��С��λΪK */  
static uint32_t  nAppBakHead_Addr; /* AppBakHead�ĵ�ַ */ 
static uint32_t  unDevType;        /* �豸���ͱ��� */ 
static uint16_t sImageVersion;     /* �����ļ��汾�� */
static uint8_t  cNode;             /* ���淢�����������Ľڵ����� */                        
static uint32_t unSaveOffest = 0;  /* �������ݵ�ƫ�� */ 
static uint32_t unFlag = 0;        /**< �յ�����֡���ݱ�־*/



/** CRC��ѯ�������㷨 CRC16 0xA001 (x^16 + x^15 + x^2 + 1) */
const uint16_t crc_16_table[256] = {
0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

/**
* @brief CRCУ��������
* @param[in] pdata��У������ nLenУ������ݳ��� nIvУ��ĳ�ʼֵ  
* @note ����crc16����У�飬�������ɶ���ʽΪG(x) = x^16 + x^15 + x^2 + 1�����÷����㷨����ʼֵΪ0x0
* @return ����CRC16У����
*/
static uint16_t IAP_CaculateCrc(uint8_t *pdata,uint32_t nLen,uint16_t nIv)
{
    uint16_t usAccum = nIv;
    uint32_t i = 0;
    
    for(i = 0; i < nLen; i++)
    {
       usAccum = ( usAccum >> 8 ) ^ crc_16_table[( usAccum ^ *pdata) & 0xFF];
       pdata++; 
    }
    
    return usAccum;
}

/**
* @brief ��ȡƬ��flash��С
* @param None
* @note ͨ����ȡ��ȡflash��С�Ĵ�����ȡflash��С
* @return ����flash��С��λΪK
*/
static uint32_t IAP_Get_FlashSize(void)
{
    nFlashSize = *(uint32_t*)FLASHSIZE_REG; 

    nFlashSize = nFlashSize & 0xffff;  
    
    return nFlashSize;
}

/**
* @brief ��ȡAppBakHead�ĵ�ַ
* @param[in] itype ӳ���ļ������
* @param[out] nAddr ��ȡ���ĵ�ַ 
* @note None
* @return None
*/
static void IAP_Caculate_TableHeadAddr(uint32_t *nAddr,app_type_t itype)
{
   uint32_t nFlashSize = 0;
   uint32_t nOffest = 0;
        
   if(APP_TYPE == itype)  
   {
       nOffest = BOOTLOADER_SIZE*1024;                                          
       *nAddr = nOffest +  FLASH_BASE;                                            
   }
   else if(APPBAK_TYPE == itype)
   {
       nFlashSize = IAP_Get_FlashSize();                                         
       nOffest = ((nFlashSize - BOOTLOADER_SIZE)/2 + BOOTLOADER_SIZE)*1024;       
       
       *nAddr = nOffest +  FLASH_BASE; 
   }
   else
   {
       printf("[BOOT_Caculate_AppBak]apptyye is undefine\r\n");
   }	
}

/**
* @brief �����¼�
* @param event ��Ҫ���õ��¼� 
* @note None
* @return None
*/
static void IAP_SetEvent(iap_event_t event)
{
   cIapEvent[event] = 1;
}

/**
* @brief ���IAP�¼�
* @param event ������¼� 
* @note None
* @return None
*/
static void IAP_ClrEvent(iap_event_t event)
{
    cIapEvent[event] = 0;
}

/**
* @brief ����¼��Ƿ񴥷�
* @param event �������¼� 
* @note None
* @return ����1��ʾ�¼����� ����0��ʾ�¼�δ����
*/
static uint8_t IAP_CheckEvent(iap_event_t  event)
{
   return cIapEvent[event];
}

/**
* @brief ����״̬
* @param state ��Ҫ���õ�״̬
* @note None
* @return None
*/
static void IAP_SetState(iap_state_t state)
{
   cIapState[state] = 1;
}

/**
* @brief ���IAP״̬
* @param state �����״̬
* @note None
* @return None
*/
static void IAP_ClrState(iap_state_t state)
{
    cIapState[state] = 0;
}

/**
* @brief �����Ӧ״̬�Ƿ���
* @param state ������״̬
* @note None
* @return ����1��ʾ״̬���� ����0��ʾ��Ӧ״̬δ����
*/
static uint8_t IAP_CheckState(iap_state_t state)
{
   return cIapState[state];
}



/**
* @brief IAPѭ������buf���ݽṹ����
* @param None
* @note None
* @return None
*/
static void IAP_Reset_Buf(void)
{
     IapBuf.nCnt = 0;
	 IapBuf.nReadPos = 0;
	 IapBuf.nWritePos = 0;
}

/**
* @brief �������ļ������ж�ȡ���յ����ļ�����
* @param pdata ��Ϣ���� nLen ��Ϣ���ݳ��� 
* @note None
* @return ����-1��ʾ�������ݲ�����д������0��ʾ������ȡ������1��ʾ���ݿ���Ϣ�Ѷ�ȡ���
*/
static int32_t IAP_Read_Buf(uint8_t *pdata,uint32_t *nLen)
{
    if(IapBuf.nCnt >= IAP_PROGRAMMER_NUM)                 //�жϻ����е����ݳ��ȴ�С�Ƿ�ɽ�����д��
    {			                                                       
         *nLen = IAP_PROGRAMMER_NUM;
         memcpy(pdata,&IapBuf.membuf[IapBuf.nReadPos],IAP_PROGRAMMER_NUM);
         IapBuf.nReadPos = (IapBuf.nReadPos + IAP_PROGRAMMER_NUM)%IAP_MEMORY_SIZE;
         IapBuf.nCnt = 	IapBuf.nCnt - IAP_PROGRAMMER_NUM;	      
    }
    else                                                  
    {
         if((IAP_CheckState(IAP_BLOCK_COMPLETE_STATE)))    //�ж��Ƿ��յ����ļ�����¼�������յ��򽫻�����ʣ������ݶ�����
         {
                 if(IapBuf.nCnt > 0)
                 {
                     memcpy(pdata,&IapBuf.membuf[IapBuf.nReadPos],IapBuf.nCnt);
                    *nLen = IapBuf.nCnt;
                     IapBuf.nReadPos = (IapBuf.nReadPos + IapBuf.nCnt)%IAP_MEMORY_SIZE;
                     IapBuf.nCnt = 0;                                     
                 }
                 
                 IAP_ClrState(IAP_BLOCK_COMPLETE_STATE);
                 IAP_ClrState(IAP_START_FILE_TRANSPORT_STATE);

                 return 1;				 
         }
         else
         {
             return -1;    //�����������û�дﵽ����д�����ݳ��Ȳ���û���յ��ļ���ɵ��¼��򷵻�-1��ʾ��������д����
         }
     }
	     
    return 0;
}

/**
* @brief �����յ��������ļ�����д�뵽������
* @param[in] pdata ��Ϣ���� nLen ��Ϣ���ݳ��� 
* @note None
* @return ����-1��ʾ��������������0��ʾ����д��
*/
static int32_t IAP_Write_Buf(uint8_t *pdata,uint32_t nLen)
{
    if((IapBuf.nCnt + nLen) >= IAP_MEMORY_SIZE) 
    {
			 IAP_SetEvent(IAP_MEMORY_FULL_EVENT);
             return -1;
    }
	
	memcpy(&IapBuf.membuf[IapBuf.nWritePos],pdata,nLen);
		
    IapBuf.nWritePos = (IapBuf.nWritePos + nLen)%IAP_MEMORY_SIZE;
    IapBuf.nCnt = IapBuf.nCnt + nLen;
		
	return 0;
}

/**
* @brief �ж������ļ������Ƿ����ж�
* @param None
* @note None
* @return ����1��ʾ��������0��ʾδ��
*/
static int32_t IAP_IsFull_Buf(void)
{
    if((IapBuf.nCnt + 4) >= IAP_MEMORY_SIZE) 
    {			 
             return 1;
    }

    return 0;
}



/**
* @brief IAP��ʼ������
* @param None 
* @note �����AppBakHead��ַ
* @return None
*/
void IAP_Init(void)
{
   IAP_Caculate_TableHeadAddr(&nAppBakHead_Addr,APPBAK_TYPE);    //�����AppBakHead����ַ	
	
 //  IAP_SetEvent(IAPHOST_UPDATE);       //����
    
  // DRV_Erase_Flash(nAppBakHead_Addr,123);
}

/**
* @brief IAPӦ��֡
* @param[in] nMainCmd ������ nSubCmd ������ nStatus ״̬�� cErr ������
* @note None
* @return None
*/
static void IAP_Send_AnswerProcess(sftp_cmd_t nMainCmd,uint32_t nSubCmd,uint32_t nStatus,uint8_t cErr)
{
	 uint32_t nLen = 0;
     answer_msg_t amsg;
	
     memset(&amsg,0,sizeof(answer_msg_t)); 
	 
     if(SYS_CMD == nMainCmd)
     {
         amsg.cFrameType = SFTP_ANSWER;
         amsg.cObject = SYS_CMD;
         amsg.cSubObject = VERSION_INQUIRE;
         
         if(AS_ERR == nStatus)
         {
            amsg.cAStatus = AS_ERR;
            amsg.cAErrCode = cErr;
            nLen = 5;
         }
         else
         {
            amsg.cAStatus = nStatus; 
            nLen = 4;
         }	
     }    
     else if(IAP_CMD == nMainCmd)
     {
	 switch(nSubCmd)
     {
         case FILE_PUT_REQUEST:
         {
               amsg.cFrameType = SFTP_ANSWER;
               amsg.cObject = IAP_CMD;
               amsg.cSubObject = FILE_PUT_REQUEST;
              
                if(AS_ERR == nStatus)
                {
                     amsg.cAStatus = AS_ERR;
                     amsg.cAErrCode = cErr;
                     nLen = 5;
                }
                else
                {
                    amsg.cAStatus = nStatus; 
                    nLen = 4;
                }							 				 
          }break;
		   case FILE_CHECKSUM:
		   {
                amsg.cFrameType = SFTP_ANSWER;
                amsg.cObject = IAP_CMD;
                amsg.cSubObject = FILE_CHECKSUM;
              
                 if(AS_ERR == nStatus)
                 {
                      amsg.cAStatus = AS_ERR;
                      amsg.cAErrCode = cErr;
                      nLen = 5;
                 }
                 else
                 {
                     amsg.cAStatus = nStatus; 
                     nLen = 4;
                 }							
             }break;
			 case FILE_SAVE_SUCCESS:
			 {
                 amsg.cFrameType = SFTP_ANSWER;
			     amsg.cObject = IAP_CMD;
				 amsg.cSubObject = FILE_SAVE_SUCCESS;
					  
                 if(AS_ERR == nStatus)
                 {
                    amsg.cAStatus = AS_ERR;
                    amsg.cAErrCode = cErr;
                    nLen = 5;
                 }
                 else
                 {
                        amsg.cAStatus = nStatus; 
                        nLen = 4;
                 }							
             }break;
		     default:;
         }
         }
			 
		 CAN_Send_Msg((uint8_t*)&amsg,nLen);
}

/**
* @brief IAP����״̬֡Ӧ��
* @param[in] nStatus ����Ӧ��֡״̬�� cErr ������ 
* @note None
* @return None
*/
static void IAP_Send_BinaryProcess(int32_t nStatus,uint8_t cErr)
{
	  int32_t nStatusCode;
      uint32_t nLen = 0;
	  datastate_msg_t dmsg;
	   
	  nStatusCode = nStatus;
	  memset(&dmsg,0,sizeof(datastate_msg_t));
			
		switch(nStatusCode)
		{
			 case DS_READY:                          //����׼����Ϣ����Ӧ��
			 {
					 dmsg.cFrameType = SFTP_DATASTATUS;
					 dmsg.cDStatus = DS_READY;
					 dmsg.data_info.data_offest_t.cOffestLsb = (sDataOffest + 1)%256;
					 dmsg.data_info.data_offest_t.cOffestMsb = (sDataOffest + 1)/256;
				 
					 nLen = 4;
			 }break;
			 case DS_FRAME_LOSS:
			 {
					 dmsg.cFrameType = SFTP_DATASTATUS;
					 dmsg.cDStatus = DS_FRAME_LOSS;
					 dmsg.data_info.data_offest_t.cOffestLsb = (sDataOffest + 1)%256;
					 dmsg.data_info.data_offest_t.cOffestMsb = (sDataOffest + 1)/256;
				 
					 nLen = 4;
			 }break;
			 case DS_HOLD:
			 {
					 dmsg.cFrameType = SFTP_DATASTATUS;
					 dmsg.cDStatus = DS_HOLD;
				 
					 nLen = 2;
			 }break;
			 case DS_ADDR_INVALID:
			 {
					 dmsg.cFrameType = SFTP_DATASTATUS;
					 dmsg.cDStatus = DS_ADDR_INVALID;
				 
					 nLen = 2;
			 }break;
			 case DS_INTER_ERR:
			 {
					 dmsg.cFrameType = SFTP_DATASTATUS;
					 dmsg.cDStatus = DS_FRAME_LOSS;
					 dmsg.data_info.data_err_t.cDErr = cErr;
			 }break;
			 default:;
		}
				
		CAN_Send_Msg((uint8_t*)&dmsg,nLen);
}

/**
* @brief IAP����״̬֡Ӧ��
* @param[in] nCmd ����֡��Ϣ���� cErr ����֡������
* @note None
* @return None
*/
static void IAP_Send_ControlProcess(uint32_t nCmd,uint8_t cErr)
{
	   uint32_t nLen = 0;
       control_msg_t cmsg;
	   
	   memset(&cmsg,0,sizeof(control_msg_t));
	
	   switch(nCmd)
	   {
            case FILE_SAVE_SUCCESS:
            {
               cmsg.cFrameType = SFTP_CONTROL;
               cmsg.cNode = cNode;
               cmsg.cObject = IAP_CMD;
               cmsg.cSubObject = FILE_SAVE_SUCCESS;
               cmsg.cMsg[0] = cErr;             
            
               nLen = 5;		
            }break;
         default:;
      }
 	 
	  CAN_Send_Msg((uint8_t*)&cmsg,nLen);
}

/**
* @brief IAP������
* @param[in] pmsg����֡��Ϣ 
* @note None
* @return None
*/
int32_t IAP_Binary_Process(p_sftp_msg_t pmsg)
{
	 int32_t nErr = 0;
	 uint16_t sOffest = 0;
     p_table_head_t ptable = NULL;
    
	 sOffest = pmsg->cSftp_data[3]*256 + pmsg->cSftp_data[2];           //������������ڻ���ַ��ƫ����
    
     ptable = (p_table_head_t)nAppBakHead_Addr; 
              		
	 if(0 == nBaseAddr)      
	 {
		 if(8 == sOffest)
		 {
			  unDevType = *(uint32_t *)&pmsg->cSftp_data[4];
			  unDevType &= 0xFFFFFF;	

              if((*(uint32_t *)pcDevName & 0xFFFFFF) != unDevType) //�Ƚ��Ƿ��Ǳ����豸����
			  {
                     IAP_Reset_Buf(); //����Ǳ��������ļ��Ĳ������ݴ���
                     IAP_Send_BinaryProcess(DS_INTER_ERR,DE_DEVTYPE_INVALID);
                     IAP_SetState(IAP_DEVTYPE_UNMATCH_STATE);
              }             			  
		 }
		 else if(9 == sOffest)
		 {	
                sImageVersion = *(uint16_t*)&pmsg->cSftp_data[4];	

                if(( *(uint32_t *)pcDevName & 0xFFFFFF) == unDevType &&
                    ( sImageVersion !=  *(uint32_t *)(pAppBakVer) ||   
                sImageCrc != ptable->sImageCrc) )   //�Ƚ������ļ��汾�Ƿ�������AppBak���Ƚ�
			    {
                     IAP_SetState(IAP_IMAGE_UPDATE_STATE);
                }
			    else
			    {
					 IAP_Reset_Buf(); //����Ǳ��������ļ��Ĳ������ݴ���
                     IAP_Send_BinaryProcess(DS_INTER_ERR,DE_DEVTYPE_INVALID);
                }			             
        }
   }
		  
	 if(sOffest >= (sDataBlock_Len - 1))
	 {
          IAP_SetState(IAP_BLOCK_COMPLETE_STATE);
     }
		
	 if(sOffest >= sDataBlock_Len)
	 {
          IAP_Send_BinaryProcess(DS_ADDR_INVALID,nErr);
		  return -1;
     }
	 		
	 if(((sOffest - sDataOffest) == 1) || (0 == sDataOffest))
	 {
           if(0 == sDataOffest)
           {
                unFlag++;
           }
                       
           if(unFlag < 2)
           {
               IAP_Send_BinaryProcess(DS_READY,nErr);
             
               IAP_Write_Buf(&pmsg->cSftp_data[4],pmsg->nLen - 4);
           }
           else
           {
               unFlag = 1;
               IAP_Send_BinaryProcess(DS_FRAME_LOSS,nErr);
           }
     }
	 else
	 {
           IAP_Send_BinaryProcess(DS_FRAME_LOSS,nErr);
		   return -1;
     }
     	 
     sDataOffest = sOffest;
	 
     return 0;
}

/**
* @brief AppBak��ͷд�뵽flash��
* @param None 
* @note None
* @return None
*/
static int32_t IAP_Write_AppBakHead(uint16_t sImageCrc)
{
	 uint32_t  nAppBakHeadAddr = 0;
     uint16_t sTestCrc = 0;
     uint8_t cRbuf[sizeof(table_head_t)] = {0};
     table_head_t  table;
   
     memset(&table,0,sizeof(table_head_t));
    
     IAP_Caculate_TableHeadAddr(&nAppBakHeadAddr,APPBAK_TYPE);         //

     memcpy((uint8_t*)&table,(uint8_t*)nAppBakHeadAddr,sizeof(table_head_t));
	   	
     table.sTableHead = TABLE_MAGIC_NUM;
	 table.sTableVersion = 0x0001;
	 table.cTag0Offest = 0x0c;
		   
	 memcpy(table.cDevType,(uint8_t*)&unDevType,sizeof(table.cDevType));
	 table.sImageVersion = sImageVersion;
	 table.sImageLength = sImageLength;                               
		 
	 table.Tag0Length = sizeof(table_head_t);
		  
     table.sImageCrc = sImageCrc;
	     
     table.sTableCrc = IAP_CaculateCrc((uint8_t*)&table,table.Tag0Length - 2,0x0);	      
		 
	 DRV_Erase_Flash(nAppBakHeadAddr,1);
	 DRV_Write_Flash(nAppBakHeadAddr,(uint8_t*)&table,sizeof(table_head_t));
	
     memcpy(cRbuf,(uint8_t*)nAppBakHeadAddr,sizeof(table_head_t));    
	// DRV_Read_Flash(nAppBakHeadAddr,cRbuf,sizeof(table_head_t));	

     if(0 != memcmp(cRbuf,(uint8_t*)&table,sizeof(table_head_t))) 
     {
        return -1;
     }				       	   
    
     return 0;
}

/**
* @brief IAP��������
* @param None 
* @note �������ļ�����Buf�л�ȡ�����ļ����ݣ�������д��flash��
* @return None
*/
static int32_t IAP_SaveFile_Process(void)
{
    static uint32_t nPageCnt = 0;
    
    uint32_t nWaddr = 0;
    uint8_t rbuf[IAP_PROGRAMMER_NUM] = {0};
    uint8_t cBackWordBUf[IAP_PROGRAMMER_NUM] = {0};
    uint32_t nLen = 0;
    int32_t nErr = 0;

     if(IAP_CheckState(IAP_IMAGE_UPDATE_STATE))  //�����յ��������ļ��Ƿ��Ǳ����ģ��������򲻽�����д������Ҫ������д
	 {   
         if(IAP_CheckState(IAP_ERASE_FLASH_STATE))
	     {
             uint32_t nCnt = 0;			  
             uint32_t nWaddr = 0;
                     			
			 nWaddr = nAppBakHead_Addr + nBaseAddr*4 + nPageCnt*SECTOR_SIZE;           //����AppBak����ַ����
             if((nPageCnt + ERASE_PAGE_NUM) <= ((sDataBlock_Len * 4 - 1)/SECTOR_SIZE + 1))
			 {
                 nCnt = ERASE_PAGE_NUM;
             }
			 else
			 {
                 nCnt =  ((sDataBlock_Len * 4 - 1)/SECTOR_SIZE + 1) - nPageCnt;
             }
			
             if(0 == cEraseRecord[(nWaddr - nAppBakHead_Addr)/ERASE_PAGE_NUM])  //�ж�δ����ҳ��¼
             {
                cEraseRecord[(nWaddr - nAppBakHead_Addr)/ERASE_PAGE_NUM] = 1;
                nErr = DRV_Erase_Flash(nWaddr,nCnt);
                 
                if(nErr < 0)
                {
                    IAP_SetEvent(IAP_FLASH_ERR_EVENT);
                }                    
             }
             
             IAP_Send_BinaryProcess(DS_HOLD,0);  
             IAP_SetState(IAP_DATASUSPEND_STATE);
              						 			
             if((nPageCnt + ERASE_PAGE_NUM) <= ((sDataBlock_Len * 4 -1)/SECTOR_SIZE + 1))
             {
                 nPageCnt = nPageCnt + ERASE_PAGE_NUM;
             }
             else
             {
                 IAP_Send_BinaryProcess(DS_READY,0);  

                 IAP_ClrState(IAP_ERASE_FLASH_STATE);
                 IAP_ClrState(IAP_DATASUSPEND_STATE);
             }				
         } 
       
         if(0 == IAP_CheckState(IAP_ERASE_FLASH_STATE))  
         {             
             nErr = IAP_Read_Buf(rbuf,&nLen);

             if(nErr < 0)
             {
                     return -1;
             }
                
             if(nLen > 0)
             {             
                 nWaddr = nAppBakHead_Addr + 2048 + 
                 (nBaseAddr + unSaveOffest)*4;  //�������Ҫ������д��AppBak�ĵ�ַ
                     
                 DRV_Write_Flash(nWaddr,rbuf,nLen);
                 memcpy(cBackWordBUf,(uint8_t*)nWaddr,nLen);
                 
                 if(0 != memcmp(rbuf,cBackWordBUf,nLen))
                 {
                         DRV_Write_Flash(nWaddr,rbuf,nLen);
                         memcpy(cBackWordBUf,(uint8_t*)nWaddr,nLen);
                     
                         if(0 != memcmp(rbuf,cBackWordBUf,nLen))
                         {
                                 IAP_SetEvent(IAP_FLASH_ERR_EVENT); 
                                 return -2;				 
                         }     
                 }
             }
                                     
             unSaveOffest = unSaveOffest + 4;
       }
      
   }		 
 
   return 0;
}


/**
* @brief IAP�¼�����
* @param None 
* @note None
* @return None
*/
static int32_t IAP_Event_Process(void)
{	   
	int32_t nErr = 0;
          
    if(IAP_CheckEvent(IAP_FLASH_ERR_EVENT))                         //flash�𻵷��ʹ��������������
    {
       IAP_ClrEvent(IAP_FLASH_ERR_EVENT);
       IAP_Send_BinaryProcess(DS_INTER_ERR,DE_NOT_SAVED);
       IAP_SetState(IAP_DATASUSPEND_STATE);
    }
       	
	if(IAP_CheckEvent(IAP_IMAGE_SAVE_SUCCESS_EVENT)) 
	{
			 uint16_t sChkSum = 0;
			
             sChkSum = IAP_CaculateCrc((uint8_t*)(nAppBakHead_Addr + (TABLE_SIZE*1024))
              ,sImageLength*4,0x0);  //�����ļ�CRCУ��
			
			 if(sChkSum == sImageCrc)       //�����ļ�У��ɹ������ļ�����ɹ�ָʾ                                  
			 {           
				     nErr = IAP_Write_AppBakHead(sChkSum);
				     if(0 == nErr)
					 {
                        IAP_Send_ControlProcess(FILE_SAVE_SUCCESS,0);   //У��ɹ�����AppBak��ͷ�ɹ�д�뷢��
                     }
					 else
					 {
                        IAP_Send_ControlProcess(FILE_SAVE_SUCCESS,1);
                     }
             }
             #if 1
			 else
			 {                 
				   IAP_Send_ControlProcess(FILE_SAVE_SUCCESS,1);
             } 
             #endif

             IAP_ClrEvent(IAP_IMAGE_SAVE_SUCCESS_EVENT);             
       }
    
       if(0 == IAP_CheckState(IAP_BLOCK_COMPLETE_STATE))
       {	        			 	                 
            if(IAP_CheckState(IAP_RECVFILE_COMPLETE_STATE))
            {
               IAP_SetEvent(IAP_IMAGE_SAVE_SUCCESS_EVENT); 
               IAP_ClrState(IAP_IMAGE_UPDATE_STATE);
            }
       }
        
		if(IAP_CheckEvent(IAP_QUEUE_FULL_EVENT)) 
		{                    
		    IAP_Send_AnswerProcess(IAP_CMD,FILE_PUT_REQUEST,AS_HOLD,0);              
            IAP_SetState(IAP_CMDSUSPEND_STATE);    

             if(0 == SFTP_IsFull_Queue())         //�ж�CAN����֡�����������ж�
             {   
               IAP_ClrEvent(IAP_QUEUE_FULL_EVENT);                   
               IAP_ClrState(IAP_CMDSUSPEND_STATE);  //δ����������ݲ�����״̬
               IAP_Send_AnswerProcess(IAP_CMD,FILE_PUT_REQUEST,AS_OK,0); //����������Խ���Ӧ�� 
            }            
        }		
		
        if(IAP_CheckEvent(IAP_MEMORY_FULL_EVENT))  
        {             
            IAP_Send_BinaryProcess(DS_HOLD,0);
            IAP_SetState(IAP_DATASUSPEND_STATE);
            
            if(0 == IAP_IsFull_Buf())     //�ж������ļ����ݻ����Ƿ���״̬
            {
               IAP_ClrEvent(IAP_MEMORY_FULL_EVENT);
               IAP_ClrState(IAP_DATASUSPEND_STATE);  //δ����������ݲ�����״̬
               IAP_Send_BinaryProcess(DS_READY,0);  
            }    
        }            
                       
        return 0;
}

/**
* @brief IAP����������Ϣ����
* @param[in] pmsg ����֡��Ϣ 
* @note None
* @return None
*/
int32_t IAP_Control_Process(p_sftp_msg_t pmsg)
{
    uint8_t cObject = 0;
    uint8_t cSubObject = 0;
    uint16_t sImageLen = 0;   
    control_msg_t cmsg;
    answer_msg_t amsg;

    cNode = pmsg->cSftp_data[1];	 	
    cObject = pmsg->cSftp_data[2];             //��ֵ������
    cSubObject = pmsg->cSftp_data[3];          //��ֵ�Ӷ���

    memset(&cmsg,0,sizeof(control_msg_t));     
    memset(&amsg,0,sizeof(answer_msg_t));
		
    if(FILE_PUT_REQUEST == cSubObject)
    {
        sDataBlock_Len = pmsg->cSftp_data[6] + pmsg->cSftp_data[7]*256;  //���ݿ鳤��
             
        if(0 == IAP_CheckState(IAP_START_FILE_TRANSPORT_STATE))
        {  
             unSaveOffest = 0;          //���㱣���ļ���ƫ�Ƶ�ַ      
             unFlag = 0;            
             IAP_Send_AnswerProcess(IAP_CMD,FILE_PUT_REQUEST,AS_OK,0);  
             IAP_Reset_Buf();                            
             IAP_SetState(IAP_START_FILE_TRANSPORT_STATE);    //���������ļ���ʼ�¼�
             IAP_SetState(IAP_ERASE_FLASH_STATE);             //���ÿɲ�����״̬
             IAP_ClrState(IAP_DEVTYPE_UNMATCH_STATE);         //�������ƥ��״̬
                                                                                            
             nBaseAddr = pmsg->cSftp_data[4] + pmsg->cSftp_data[5]*256;  //���ݿ����ļ��еĵ�ַ

             sImageLen = nBaseAddr + sDataBlock_Len;					 

             if(sImageLen > sImageLength)  //ͨ����ַ�����ֵ���ж��ļ��ĳ��ȴ�С
             {
                sImageLength = sImageLen;
             }
             
             sDataOffest = 0;                                                           
       }
       else
       {		
            if(0 == sDataBlock_Len)
            {
                IAP_SetState(IAP_RECVFILE_COMPLETE_STATE);
                memset(cEraseRecord,0,sizeof(cEraseRecord));  
            }
            else
            {  
                IAP_Send_AnswerProcess(IAP_CMD,FILE_PUT_REQUEST,AS_ERR,AE_DATA_LOST); //���ݿ鲻����                                                                                  
            }                                          
        }
    }
    else if(FILE_CHECKSUM == cSubObject)
    {
        sImageCrc = pmsg->cSftp_data[5]*256 + pmsg->cSftp_data[4];  //���������ļ�У����
             
        IAP_Send_AnswerProcess(IAP_CMD,FILE_CHECKSUM,AS_OK,0);	                
    }
    else
    {
        amsg.cFrameType = SFTP_ANSWER;
        amsg.cObject = cObject;
        amsg.cSubObject = cSubObject;
        amsg.cAStatus = AS_ERR;
        amsg.cAErrCode = AE_UNKNOWN_SUBOBJ;

        CAN_Send_Msg((uint8_t*)&amsg,5);              //�Ӷ������ʶ�������ӻ��������ļ�����ɹ����Ӷ���
    }

    return 0;
}

/**
* @brief IAP������
* @param None 
* @note None
* @return None
*/
void IAP_Task_Process(void)
{
    SFTP_Process();
    
    IAP_Event_Process();
    
    IAP_SaveFile_Process(); 
}

/**
* @brief ���˷�SFTPЭ��֡����
* @param[in] pdata ��Ҫ���˵���Ϣ���� nLen �������ݳ���
* @note None
* @return ����0��ʾ�������� ����-1��ʾ��ӹ����������ʧ��
*/
int32_t IAP_Filter_Task(uint8_t *pdata,uint32_t nLen)
{
    uint8_t cType = 0;
    int32_t nErr = 0;
	
    if(pdata == NULL || nLen <= 0) 
    {
        return -1;
    }			
	
    cType = pdata[0];                         
					
    if(cType >= SFTP_ANSWER && cType <= SFTP_DATASTATUS)      //���˷�SFTPЭ��֡����
    {
        if(SFTP_BINARY == cType)
        {
            if(IAP_CheckState(IAP_START_FILE_TRANSPORT_STATE) && 
                (0 == IAP_CheckState(IAP_DATASUSPEND_STATE)) && 
                (0 == IAP_CheckState(IAP_DEVTYPE_UNMATCH_STATE)))               
            {
                nErr = SFTP_Add_Queue(pdata,nLen);                      
            }
        }
        else if(SFTP_CONTROL == cType)
        {
            if(0 == IAP_CheckState(IAP_CMDSUSPEND_STATE))
            {                
               nErr = SFTP_Add_Queue(pdata,nLen);
            }
        }
        else
        {
            nErr = SFTP_Add_Queue(pdata,nLen);

        }

        if(nErr < 0)
        {
            IAP_SetEvent(IAP_QUEUE_FULL_EVENT);               //���������������������¼�
        }
    }

    return nErr;
}

#if 0
/**
* @brief ���������Ĺ��˰�����
* @param[in] pdata ��Ҫ���˵���Ϣ���� nLen �������ݳ���
* @note None
* @return None
*/
int32_t IAP_HostFilterTest(uint8_t *pdata,uint32_t nLen)
{
	  uint8_t cType = 0;
      int32_t nErr = 0;
	
    if(pdata == NULL || nLen <= 0)
		{
        return -1;
    }			
	
	  cType = pdata[0];                         
				
		if(cType >= SFTP_ANSWER || cType <= IAP_DATASTATUS)      //���˷�IAP֡����
		{
           nErr = IAP_Add_Queue(pdata,nLen);
		}

    return nErr;  
}

/**
* @brief �����������¼���������
* @param None 
* @note None
* @return None
*/
int32_t IAP_HostEventTest(void)
{
	static uint16_t sOffest = 0;
    uint32_t nAddr = 0x08042800;
    uint8_t nRbuf[4] = {0};	
	binary_msg_t bmsg;
  	control_msg_t cmsg;
	
    if(IAP_CheckEvent(IAPHOST_UPDATE))
   {
           IAP_ClrEvent(IAPHOST_UPDATE);
             memset(&cmsg,0,sizeof(control_msg_t));
                         
             cmsg.cFrameType = IAP_CONTROL;
             cmsg.cNode = 0x10;
             cmsg.cObject = IAP_CMD;
             cmsg.cSubObject = 	FILE_PUT_REQUEST;
             cmsg.cMsg[0] = 0;
             cmsg.cMsg[1] = 0;
             cmsg.cMsg[2] = 0x0;
             cmsg.cMsg[3] = 0xf8;
        
           CAN_Send_Msg((uint8_t*)&cmsg,8);
    } 	
		
	if(IAP_CheckEvent(IAPHOST_BINARY) && (0 == IAP_CheckEvent(IAPHOST_COMPLETE)))
	{		
          DRV_Read_Flash(nAddr + sOffest*4,nRbuf,4);
        
          IAP_ClrEvent(IAPHOST_BINARY);
          memset(&bmsg,0,sizeof(binary_msg_t));	
          bmsg.cFrameType = SFTP_ANSWER;
          bmsg.cReserved = 0;
          bmsg.cOffestMSB = sOffest/256;
          bmsg.cOffsetLSB = sOffest%256;
        
          bmsg.cDataBuf[0] = nRbuf[0];
          bmsg.cDataBuf[1] = nRbuf[1];
          bmsg.cDataBuf[2] = nRbuf[2];
          bmsg.cDataBuf[3] = nRbuf[3];
        
          CAN_Send_Msg((uint8_t*)&bmsg,8);
        
          sOffest++;
        
          if(sOffest >= 0xf800)
          {
             IAP_SetEvent(IAPHOST_COMPLETE);
          }						  
    }
		
	if(IAP_CheckEvent(IAPHOST_COMPLETE))
	{
         cmsg.cFrameType = IAP_CONTROL;
         cmsg.cNode = 0x10;
         cmsg.cObject = IAP_CMD;
         cmsg.cSubObject = 	FILE_PUT_REQUEST;
         cmsg.cMsg[0] = 0;
         cmsg.cMsg[1] = 0;
         cmsg.cMsg[2] = 0;
         cmsg.cMsg[3] = 0;
    
         CAN_Send_Msg((uint8_t*)&cmsg,8);
    }
				 
    return 0;
}

/**
* @brief �������������������
* @param None
* @note None
* @return None
*/
int32_t IAP_HostTask_Test(void)
{	 
	 uint8_t cType = 0;	 
	 uint8_t cAObject = 0;
	 uint8_t cASubObject = 0;
	 uint8_t cAStatus = 0;
	 uint8_t cDStatus = 0;
	
     p_iap_msg_t pTaskmsg = NULL;		 
	 pTaskmsg = IAP_Get_Queue();     //��ȡIAP������������Ϣ             

	 if(NULL == pTaskmsg)
	 {
      return -1;  
     }
	 
	 cType = pTaskmsg->cIap_data[0];   //��ȡ֡����
	 
	 switch(cType)
	 {
      case SFTP_ANSWER:
	  {
			
      }break;
	  case SFTP_ANSWER:                    
	  {
          IAP_Binary_Process(pTaskmsg);        //������֡���ͽ��д���
      }break;
	  case IAP_CONTROL:
	  {
          IAP_Control_Process(pTaskmsg);       //�Կ���֡���ͽ��д���
      }break;
	  case IAP_DATASTATUS:
	  {
          cDStatus = pTaskmsg->cIap_data[1];
           
	      if(DS_READY == cDStatus)
		   {
              IAP_SetEvent(IAPHOST_BINARY);      
           }				        				
      }break;
			default:;
   }
	 
	 IAP_Del_Queue();
		  
	 return 0;
}
#endif
