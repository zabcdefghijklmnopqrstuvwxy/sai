/**
********************************************************************************
* @file    bootstrap.c
* @author  Sai
* @version UDFRM-B-S01-01V00-51X
* @date    2019-04-12
* @brief   ����Ƿ���Ҫ����������AppԴ�ļ�
* @note    
* @attention 
********************************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "bootstrap.h"
#include "drv_flash.h"


typedef void (*Function_Pointer)(void);    /* ��ת����ָ������ */


//App��������
typedef enum{
    APP_TYPE = 0x01,                      // App������ 
    APPBAK_TYPE = 0x02                    //AppBak������                  
}app_type_t;


//��Ϣ���״̬���
typedef enum{
   ALL_IMAGE_INVALID = 0x01,            // App��AppBak�����Ƿ�                
   SINGLE_APP_VALID = 0x02,             // ֻ��App�����Ϸ�
   SINGLE_APPBAK_VALID = 0x03,          // ֻ��AppBak�����Ϸ� 
   ALL_IMAGE_VALID = 0x04               // App��AppBak�����Ϸ� 
}chk_result_t;


#define   FLASHSIZE_REG  0x1FFFF7E0        // flash��С�Ĵ�����ַ 
#define   PAGE_SIZE   2048                 // һҳ�Ĵ�СΪ2048Byte 

static uint8_t *pcDevName = (uint8_t *)0x08004820;  /**< App���豸���ͺͰ汾�ŵĵ�ַ�������ؽ���ͷ */
static uint32_t nFlashSize = 0;                     /**< flash��С����λΪK */
static int32_t  nChkResult = 0;                     /**< App��AppBak�ļ���� */



/** CRC��ѯ�� �����㷨 CRC16 0xA001 (x^16 + x^15 + x^2 + 1) */
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

void BOOT_Strap(void);

/**
* @brief ��ȡflash��С
* @param None
* @note ��ȡflash��С�Ĵ�����ȡflash��С
* @return ����Ƭ�ڵ�flash��С��λΪK
*/
static uint32_t BOOT_Get_FlashSize(void)
{
    nFlashSize = *(uint32_t*)FLASHSIZE_REG; 

    nFlashSize = nFlashSize & 0xffff;  
    
    return nFlashSize;
}

/**
* @brief ����AppBak����ʼ��ַ
* @param[in] itype ӳ���ļ������
* @param[out] nAddr ���������
* @note None
* @return None
*/
static void BOOT_Caculate_TableHeadAddr(uint32_t *nAddr, app_type_t itype)
{
    uint32_t nFlashSize = 0;
    uint32_t nOffest = 0;
        
   if(APP_TYPE == itype)  
   {
       nOffest = BOOTLOADER_SIZE*1024;       //App����Ե�ַƫ������bootloader֮��
       *nAddr = nOffest +  FLASH_BASE;                                            
   }
   else if(APPBAK_TYPE == itype)
   {
       nFlashSize = BOOT_Get_FlashSize();          //��ȡƬ��flash��С
       nOffest = ((nFlashSize - BOOTLOADER_SIZE)/2 
       + BOOTLOADER_SIZE)*1024;    //����App(AppBak)�Ĵ�С����flash��С����bootloader���һ��
       
       *nAddr = nOffest +  FLASH_BASE; 
   }
   else
   {
       printf("[BOOT_Caculate_AppBak]apptyye is undefine\r\n");
   }
}

/**
* @brief boot�Ĵ�����
* @param[in] nErr ������
* @note ��ǰ�汾����������߼���ѭ��
* @return None
*/
static void BOOT_Process_Err(int32_t nErr)
{  
    while(1);
}

/**
* @brief CRCУ��
* @param[in] pdata У������� nLen У������ݳ��� nIv У��ĳ�ʼֵ 
* @note ����crc16����У�飬�������ɶ���ʽΪG(x) = x^16 + x^15 + x^2 + 1�����÷����㷨����ʼֵΪ0x0
* @return ����У����
*/
static uint16_t BOOT_Caculate_Crc16(uint8_t *pdata, uint32_t nLen, uint16_t usIv)
{
    uint32_t i = 0;
    uint16_t usAccum = usIv;
    
    for(i = 0; i < nLen; i++)
    {
       usAccum = ( usAccum >> 8 ) ^ crc_16_table[( usAccum ^ *pdata) & 0xFF];
       pdata++; 
    }
    return usAccum;
}

/**
* @brief д��App��ͷ��Ϣ
* @param None
* @note None
* @return ����0��ʾ��ͷ��Ϣ����д�� ����-1��ʾ��ͷ��Ϣд��ʧ��
*/
static int32_t BOOT_Write_TableHead(void)
{
   uint32_t unAppHeadAddr = 0;
   uint8_t cRbuf[sizeof(table_head_t)] = {0};
   table_head_t table;
   
   memset(&table, 0, sizeof(table_head_t));
    
   BOOT_Caculate_TableHeadAddr(&unAppHeadAddr, APP_TYPE);         //����AppHead����ʼ��ַ

   table.usTableHead = TABLE_MAGIC_NUM;
   table.usTableVersion = 0x0001;
   table.ucTag0Offest = 0x0c;
       
   memcpy(table.ucDevType, pcDevName, sizeof(table.ucDevType));
   table.usImageVersion = *(uint16_t*)(pcDevName + 4);
   table.usImageLength = ((BOOT_Get_FlashSize() - BOOTLOADER_SIZE)/2)*256;                          //��ȡӳ���ļ�����
     
   table.usTag0Length = sizeof(table_head_t);
      
   table.usImageCrc = BOOT_Caculate_Crc16((uint8_t*)(unAppHeadAddr + TABLE_SIZE), table.usImageLength*4, 0x0);
     
   table.usTableCrc = BOOT_Caculate_Crc16((uint8_t*)&table, table.usTag0Length - 2, 0x0);	       //�Ա�ͷ����CRCУ��
     
   DRV_Erase_Flash(unAppHeadAddr, 1);
   DRV_Write_Flash(unAppHeadAddr, (uint8_t*)&table, sizeof(table_head_t));
               
   memcpy(cRbuf,(uint8_t*)unAppHeadAddr,sizeof(table_head_t));

   if(0 != memcmp(cRbuf,(uint8_t*)&table, sizeof(table_head_t))) 
   {
        return -1;
   }				       	   
        
   return 0;
}

/**
* @brief ���Table��Ч��Ϸ���
* @param itype ӳ���ļ������
* @note None
* @return ������
*/
static int32_t BOOT_Check_TableHead(app_type_t itype)
{
    int32_t nErr = 0;
    uint32_t unAddr = 0;
    uint16_t usTableCrc = 0;
    uint16_t usCacuCrc = 0;
    uint32_t unAppAddr = 0;
    p_table_head_t ptable = NULL;
           
    BOOT_Caculate_TableHeadAddr(&unAddr,itype);
           
    ptable = (p_table_head_t)(unAddr);             //����ͷ��ַ��ַ���и�ֵ
    
    usTableCrc = ptable->usTableCrc;               //��ȡ��ͷУ����
     

    if(ptable->usTableHead != TABLE_MAGIC_NUM)      //���App��ͷ��Ϣ�Ƿ���ȷ
    {
       return ERR_MAGICNUM_WRONG;
    }    
        
    usCacuCrc = BOOT_Caculate_Crc16((uint8_t*)ptable,
                            sizeof(table_head_t) - 2,
                            0);       //��AppHead�ı�ͷ����CRCУ�����
   
    if(usTableCrc != usCacuCrc)
    {
       return ERR_CHECKSUM_WRONG;
    }
  
    if(0xFFFF == ptable->usImageVersion)           //���App��imageVersion�Ƿ���Ч
    {
       return ERR_IMAGEVERSION_INVALID;
    }            
    
    unAppAddr = unAddr + TABLE_SIZE;
    
    if(((*(uint32_t*)unAppAddr) & 0x2FFE0000) != 0x20000000)   //���App��ջ��ָ���Ƿ���ȷ
    {
       return ERR_STACKPOINT_ILLEGA;
    }       
     
    return 0;
}

/**
* @brief ����������ӳ���ļ����п���
* @param None
* @note None
* @return ���ش�����
*/
static int32_t BOOT_Copy_AppBak(void)
{
    uint32_t unAppAddr = 0;             
    uint32_t unAppBakAddr = 0;
    uint32_t unPageCnt = 0;                                       //��Ҫ��д��ҳ����
    uint16_t usImageCrc = 0;
    uint16_t usCacuCrc = 0;
    uint32_t i = 0;
    uint32_t unOffest = 0;
    int32_t nErr = 0;
    uint16_t usImageVer = 0;
    uint16_t usReadBackVer = 0;
    table_head_t tablebak;
    p_table_head_t ptable = NULL;
    
    memset(&tablebak,0,sizeof(table_head_t));
    
    BOOT_Caculate_TableHeadAddr(&unAppAddr,APP_TYPE);                       //����App�ı�ͷ��ַ
    BOOT_Caculate_TableHeadAddr(&unAppBakAddr,APPBAK_TYPE);                 //����AppBak�ı�ͷ��ַ
    
    ptable = (p_table_head_t)(unAppBakAddr);                           //��ȡAppBak�ı�ͷ��Ϣ
        
    usImageCrc = ptable->usImageCrc;                                   
    usCacuCrc = BOOT_Caculate_Crc16((uint8_t*)(unAppBakAddr + TABLE_SIZE),
            ptable->usImageLength*4, 0);   //��AppBak������CRCУ�����,image�ĳ���Ϊ�ֵ�λ
    
    if(usCacuCrc != usImageCrc)
    {
       if(nChkResult != SINGLE_APP_VALID || nChkResult != ALL_IMAGE_VALID)
       {
            BOOT_Process_Err(ERR_CHECKSUM_WRONG);
       }
    }
    else
    {        
        unPageCnt = (ptable->usImageLength*4 - 1)/PAGE_SIZE + 1;    //����ӳ���ļ��ĵ�ҳ��Ŀ   
        
        nErr = DRV_Erase_Flash(unAppAddr,
        unPageCnt); //ImageSize�������ֵ�λ,���������������Ҫ��һҳ
        
        if(nErr != 0)
        {
           return ERR_ERASEFLASH_FAILED;
        }
        
        for(i = 1; i < unPageCnt; i++)
        {        
              DRV_Write_Flash(unAppAddr + i*PAGE_SIZE, (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
              PAGE_SIZE);                 //��AppBak����ȡ����ӳ���ļ�д�뵽App��
                   
              nErr = memcmp((uint8_t*)(unAppAddr + i*PAGE_SIZE), (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
              PAGE_SIZE);                                    //�Ƚ϶�ȡ�����ݺ�д��������Ƿ���ͬ
            
              if(0 != nErr)              //�Ƚϲ���ͬ���ٴ�д��
              {
                  DRV_Write_Flash(unAppAddr + i*PAGE_SIZE, (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
                  PAGE_SIZE);    //��AppBak����ȡ����ӳ���ļ�д�뵽App��
                  nErr = memcmp((uint8_t*)(unAppAddr + i*PAGE_SIZE), (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
                  PAGE_SIZE);                                    //�Ƚ϶�ȡ�����ݺ�д��������Ƿ���ͬ 
                  
                  if(0 != nErr)  //�ٴλض�������AppBak���ı�ͷ�������������ٴμ��ʱ���Ա���У����̡�
                  {                 
                     DRV_Erase_Flash(unAppBakAddr,1);              
                     nErr = ERR_PROGRAMFLASH_FAILED;
                     break;
                  }
              }
        }       
                       
        usImageVer = ptable->usImageVersion;
        ptable->usImageVersion = 0xffff;
              
        DRV_Write_Flash(unAppAddr, (uint8_t*)ptable,
        sizeof(table_head_t));                   //��AppBak����ȡ����ӳ���ļ�д�뵽App��        
            
        nErr = memcmp((uint8_t*)ptable, (uint8_t*)unAppAddr, sizeof(table_head_t)); 

        if(0 != nErr)
        {
            DRV_Write_Flash(unAppAddr, (uint8_t*)ptable,
            sizeof(table_head_t));             //��AppBak����ȡ����ӳ���ļ�д�뵽App��                  
            nErr = memcmp((uint8_t*)ptable, (uint8_t*)unAppAddr, sizeof(table_head_t)); 
            
            if(0 != nErr)          //�ٴλض�������AppBak���ı�ͷ�������������ٴμ��ʱ���Ա���У����̡�
            {                 
                     DRV_Erase_Flash(unAppBakAddr, 1);              
                     nErr = ERR_PROGRAMFLASH_FAILED;
             }
        } 

        unOffest = (uint8_t*)&ptable->usImageVersion - (uint8_t*)ptable;        
        DRV_Write_Flash(unAppAddr + unOffest, (uint8_t*)&usImageVer,
        sizeof(tablebak.usImageVersion));                 //��AppBak����ȡ����ӳ���ļ�д�뵽App��         
        usReadBackVer = *(uint16_t*)(unAppAddr + unOffest);
        
        if(usImageVer != usReadBackVer)
        {
            DRV_Erase_Flash(unAppBakAddr, 1);              
            nErr = ERR_PROGRAMFLASH_FAILED; 
        }                
    }   
    
    return nErr; 
}

#if 0
/**
* @brief ϵͳ��λ
* @param None
* @note None
* @return None
*/
__asm void BOOT_SysReset(void)
{ 
   LDR R0, =0XE000ED0C
   LDR R1, =0X05FA0001 
   STR R1, [R0]  
   B    . 
    
   #if 0
   __asm{
   LDR R0, =0XE000ED0C
   LDR R1, =0X05FA0001 
   STR R1, [R0]  
   B    .
   }
   
       
   #pragma asm
   LDR R0, =0XE000ED0C
   LDR R1, =0X05FA0001 
   STR R1, [R0]  
   B    .
   #pragma endasm   
   #endif
}
#endif


/**
* @brief App����
* @param None
* @note None
* @return None
*/
static void BOOT_JumpToApp(void)
{
   register Function_Pointer Jump_Function;                 //��ת��������
   
   uint32_t nAddr = 0;
   uint32_t nJump_address = 0;
   
   BOOT_Caculate_TableHeadAddr(&nAddr, APP_TYPE);        //��ȡAppHeadͷ��ַ
   nJump_address = nAddr + TABLE_SIZE;             //��ȡApp�ĵ�ַ
    
   Jump_Function = (Function_Pointer)*(uint32_t*)(nJump_address + 4);//��ת������ַ��ֵ
   
   __set_MSP(*(uint32_t*)nJump_address);  //�����û�ջָ��
    
   Jump_Function();                      //������ת
    
   while(1);
}

/**
* @brief App����
* @param None
* @note ��Appӳ���ļ�����crcУ�鲢�����SP��PC�ĺϷ��Բ�����App����
* @return None
*/
static void BOOT_Strap(void)
{
      uint32_t nAppAddr = 0;
      uint32_t nLen = 0;
      int32_t nErr = 0;   
      uint32_t nAddr = 0;
      uint16_t sChk = 0;
      p_table_head_t ptable = NULL;
        
      BOOT_Caculate_TableHeadAddr(&nAddr, APP_TYPE);   
      nAppAddr = nAddr + TABLE_SIZE;
      
#if 1    
      ptable = (p_table_head_t)(nAddr);
      nLen = ptable->usImageLength;
       
      sChk = BOOT_Caculate_Crc16((uint8_t*)nAppAddr, nLen*4, 0);   //Appӳ���ļ�CRCУ��
    
      if(sChk != ptable->usImageCrc)
      {
          if(nChkResult == ALL_IMAGE_VALID)
          {
              nErr = BOOT_Copy_AppBak();              
              if(0 != nErr)
              {
                    sChk = BOOT_Caculate_Crc16((uint8_t*)nAppAddr, nLen, 0);   //CRCУ��                   
                    if(sChk != ptable->usImageCrc)
                    {
                       DRV_Erase_Flash(nAddr, 1); 
                       BOOT_Process_Err(ERR_CHECKSUM_WRONG);                        
                    }                        
               }         
          }     
      }   
#endif
           
      if(((*(uint32_t*)nAppAddr) & 0x2ffe0000) != 0x20000000  || 
          (nAppAddr != 0x08004800) 
      || (*(uint32_t*)nAppAddr) % 4 != 0)   //���App��ջ��ָ���pcָ���Ƿ���ȷ
      {
         if(nChkResult == ALL_IMAGE_VALID)
         {
               nErr = BOOT_Copy_AppBak();
              
               if(((*(uint32_t*)nAppAddr) & 0x2FFE0000) != 0x20000000  || 
             ((nAppAddr & 0x08ff0000) != 0x08000000) 
               || (*(uint32_t*)nAppAddr) % 4 != 0)   //���App��ջ��ָ���pcָ���Ƿ���ȷ
               {
                  BOOT_Process_Err(ERR_STACKPOINT_ILLEGA);
               }              
         }
         else
         {
              BOOT_Process_Err(ERR_APP_FAILED);
         }
      } 

      __disable_irq();      //�ر����ж�
      
      BOOT_JumpToApp();            
}    

/**
* @brief ����Ƿ���Ҫ����
* @param None
* @note ͨ����ȡ���App��AppBak��TableHead�õ��Ľ������������������Ӧ�Ĵ���
* @return None
*/
void BOOT_Check_Image(void)
{
    int32_t nAppRet = 0;
    int32_t nAppBakRet = 0;
    int32_t nErr = 0;
    uint32_t unAppHeadAddr = 0;
    uint32_t unAppBakHeadAddr = 0;
    p_table_head_t pApphead = NULL;
    p_table_head_t pAppbakhead = NULL;
        
    BOOT_Caculate_TableHeadAddr(&unAppHeadAddr, APP_TYPE); 
    BOOT_Caculate_TableHeadAddr(&unAppBakHeadAddr, APPBAK_TYPE); 
    
    pApphead = (p_table_head_t)unAppHeadAddr;
    pAppbakhead = (p_table_head_t)unAppBakHeadAddr;    
       
    nAppRet = BOOT_Check_TableHead(APP_TYPE);         //���App�������ļ���Ч��Ϸ���
    
    nAppBakRet = BOOT_Check_TableHead(APPBAK_TYPE);   //���AppBak�������ļ���Ч��Ϸ���
       
    if((0 != nAppRet) && (0 != nAppBakRet))      //���߼�鶼���Ϸ������Appû�������ؽ�App��ͷ
    {
       
        nChkResult = ALL_IMAGE_INVALID;        
    }
   
    if((0 != nAppRet) && (0 == nAppBakRet))      //��鵽ֻ��AppBak�Ϸ�
    {
        nChkResult = SINGLE_APPBAK_VALID;
    }
    
    if((0 == nAppRet) && (0 != nAppBakRet))      //��鵽ֻ��App���Ϸ�
    {
        nChkResult = SINGLE_APP_VALID;
    }
    
    if((0 == nAppRet) && (0 == nAppBakRet))      //����������Ϸ�
    {
        nChkResult = ALL_IMAGE_VALID;      
    }
    
    switch(nChkResult)                           //��Ӧ�ļ����������Ӧ�Ĵ���
    {
        case ALL_IMAGE_INVALID:            //����ӳ���ļ������Ƿ��ж�App���Ƿ�Ϸ��Ϸ����ؽ���ͷ
        {
             uint32_t unAppAddr = 0; 
             BOOT_Caculate_TableHeadAddr(&unAppHeadAddr,APP_TYPE);        
             unAppAddr = unAppHeadAddr + TABLE_SIZE;
        
             if((((*(uint32_t*)unAppAddr) & 0x2ffe0000) == 0x20000000) 
                 && (unAppAddr == 0x08004800) 
                 && ((*(uint32_t*)unAppAddr) % 4 == 0))   //���App��ջ��ָ���pcָ���Ƿ���ȷ              
             {                                   
                BOOT_Write_TableHead();         //��д��ͷ                    
                 
                BOOT_Strap();                  //App���� 
             } 
             
             BOOT_Process_Err(ERR_APP_FAILED);                                               
        }break;
        case SINGLE_APPBAK_VALID:                //��鵽ֻ��AppBak���Ϸ�����п���������������           
        {
            nErr = BOOT_Copy_AppBak();
        
            if(nErr != 0)
            {
              BOOT_Process_Err(nErr);
            }
        
            BOOT_Strap();     
        }break;
        case SINGLE_APP_VALID:                  //��鵽ֻ��App���Ϸ��������������
        {
            BOOT_Strap();
        }break;
        case ALL_IMAGE_VALID: //���߶��Ϸ����ж��Ƿ���Ҫ�����ļ��������Ҫ�����򿽱�����������������ֱ������
        {
             if((pApphead->usImageCrc == pAppbakhead->usImageCrc) && \
            (0 == memcmp(pApphead->ucDevType,pAppbakhead->ucDevType,sizeof(pAppbakhead->ucDevType))) \
                && (pApphead->usImageVersion == pAppbakhead->usImageVersion))
            {
                BOOT_Strap();         
            }
            else
            {
               nErr = BOOT_Copy_AppBak();                      
               if(nErr != 0)
               {
                 BOOT_Process_Err(nErr);
               }
             
               BOOT_Strap();
            }       
        }break;
        default:;
    }
}


#if 0
/**
* @brief ��Ԫ����д���ͷ
* @param itype app������ 
*     @arg APP_TYPE ��App��ͷ���ݽ���д��
*     @arg APPBAK_TYPE ��AppBakͷ����д��
*     @arg 0 ��App��AppBakͷ���ݽ��в���
* @note None
* @return None
*/
void UnitTest_Write_Head(app_type_t itype)
{
    uint32_t unAppHeadAddr = 0;
    uint32_t unAppBakHeadAddr = 0;
    
    table_head_t head;
           
    BOOT_Caculate_HeadAddr(&unAppHeadAddr, APP_TYPE);
    BOOT_Caculate_HeadAddr(&unAppBakHeadAddr, APPBAK_TYPE);
   
    if(APP_TYPE == itype) 
    {
       head.ucDevType[0] = 'M';
       head.ucDevType[1] = 'C';
       head.ucDevType[2] = 'B';
      
       head.usTableHead = TABLE_MAGIC_NUM;
       head.usTableCrc = 0x0545;
       head.usImageVersion = 0x0101;    
       head.usImageCrc = 0x2343; 
        
       head.usImageLength = 0x4800; 
         
       DRV_Erase_Flash(unAppHeadAddr,1);

       DRV_Write_Flash(unAppHeadAddr, (uint8_t*)&head, sizeof(table_head_t));     
    }
    else if(APPBAK_TYPE == itype) 
    {
       head.ucDevType[0] = 'M';
       head.ucDevType[1] = 'C';
       head.ucDevType[2] = 'B';

   
       head.usTableHead = TABLE_MAGIC_NUM;
   //    head.sTableCrc = 0x79f5;
       head.usTableCrc = 0x1234;
       head.usImageVersion = 0x0102;    
       head.usImageCrc = 0x6745;  
        
       head.usImageLength = 0x4800; 
        
       DRV_Erase_Flash(unAppBakHeadAddr, 1);

       DRV_Write_Flash(unAppBakHeadAddr, (uint8_t*)&head, sizeof(table_head_t));           
    }
    else
    {      
       DRV_Erase_Flash(unAppHeadAddr,1);      
    }
}


/**
* @brief ��Ԫ����д���ͷ
* @param itype app������ 
*     @arg APP_TYPE ��App��ͷ���ݽ���д��
*     @arg APPBAK_TYPE ��AppBakͷ����д��
*     @arg 0 ��App��AppBakͷ���ݽ��в���
* @note None
* @return None
*/
void UnitTest_CRC(void)
{
    uint16_t usChkSum0 = 0;
    uint16_t usChkSum1 = 0;
    uint8_t i = 0;
    uint8_t buf[6] = {0x11, 0x22, 0x33, 0x44, 0xB1, 0xF5};
    
    usChkSum0 = BOOT_CaculateCrc(buf, 4, 0x0);
    usChkSum1 = BOOT_CaculateCrc(buf, 6, 0x0);      
}
#endif
