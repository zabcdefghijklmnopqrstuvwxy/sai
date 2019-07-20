/**
********************************************************************************
* @file    bootstrap.c
* @author  Sai
* @version UDFRM-B-S01-01V00-51X
* @date    2019-04-12
* @brief   检查是否需要升级并引导App源文件
* @note    
* @attention 
********************************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "bootstrap.h"
#include "drv_flash.h"


typedef void (*Function_Pointer)(void);    /* 跳转函数指针声明 */


//App区域类型
typedef enum{
    APP_TYPE = 0x01,                      // App区类型 
    APPBAK_TYPE = 0x02                    //AppBak区类型                  
}app_type_t;


//信息检查状态结果
typedef enum{
   ALL_IMAGE_INVALID = 0x01,            // App和AppBak都检查非法                
   SINGLE_APP_VALID = 0x02,             // 只有App区检查合法
   SINGLE_APPBAK_VALID = 0x03,          // 只有AppBak区检查合法 
   ALL_IMAGE_VALID = 0x04               // App和AppBak都检查合法 
}chk_result_t;


#define   FLASHSIZE_REG  0x1FFFF7E0        // flash大小寄存器地址 
#define   PAGE_SIZE   2048                 // 一页的大小为2048Byte 

static uint8_t *pcDevName = (uint8_t *)0x08004820;  /**< App区设备类型和版本号的地址，用于重建表头 */
static uint32_t nFlashSize = 0;                     /**< flash大小，单位为K */
static int32_t  nChkResult = 0;                     /**< App和AppBak的检查结果 */



/** CRC查询表 逆序算法 CRC16 0xA001 (x^16 + x^15 + x^2 + 1) */
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
* @brief 获取flash大小
* @param None
* @note 读取flash大小寄存器获取flash大小
* @return 返回片内的flash大小单位为K
*/
static uint32_t BOOT_Get_FlashSize(void)
{
    nFlashSize = *(uint32_t*)FLASHSIZE_REG; 

    nFlashSize = nFlashSize & 0xffff;  
    
    return nFlashSize;
}

/**
* @brief 计算AppBak的起始地址
* @param[in] itype 映像文件区类别
* @param[out] nAddr 计算出来的
* @note None
* @return None
*/
static void BOOT_Caculate_TableHeadAddr(uint32_t *nAddr, app_type_t itype)
{
    uint32_t nFlashSize = 0;
    uint32_t nOffest = 0;
        
   if(APP_TYPE == itype)  
   {
       nOffest = BOOTLOADER_SIZE*1024;       //App的相对地址偏移量在bootloader之后
       *nAddr = nOffest +  FLASH_BASE;                                            
   }
   else if(APPBAK_TYPE == itype)
   {
       nFlashSize = BOOT_Get_FlashSize();          //读取片内flash大小
       nOffest = ((nFlashSize - BOOTLOADER_SIZE)/2 
       + BOOTLOADER_SIZE)*1024;    //计算App(AppBak)的大小等于flash大小减掉bootloader后的一半
       
       *nAddr = nOffest +  FLASH_BASE; 
   }
   else
   {
       printf("[BOOT_Caculate_AppBak]apptyye is undefine\r\n");
   }
}

/**
* @brief boot的错误处理
* @param[in] nErr 错误码
* @note 当前版本错误处理进入逻辑死循环
* @return None
*/
static void BOOT_Process_Err(int32_t nErr)
{  
    while(1);
}

/**
* @brief CRC校验
* @param[in] pdata 校验的数据 nLen 校验的数据长度 nIv 校验的初始值 
* @note 采用crc16进行校验，所用生成多项式为G(x) = x^16 + x^15 + x^2 + 1，采用反序算法，初始值为0x0
* @return 返回校验码
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
* @brief 写入App表头信息
* @param None
* @note None
* @return 返回0表示表头信息正常写入 返回-1表示表头信息写入失败
*/
static int32_t BOOT_Write_TableHead(void)
{
   uint32_t unAppHeadAddr = 0;
   uint8_t cRbuf[sizeof(table_head_t)] = {0};
   table_head_t table;
   
   memset(&table, 0, sizeof(table_head_t));
    
   BOOT_Caculate_TableHeadAddr(&unAppHeadAddr, APP_TYPE);         //计算AppHead的起始地址

   table.usTableHead = TABLE_MAGIC_NUM;
   table.usTableVersion = 0x0001;
   table.ucTag0Offest = 0x0c;
       
   memcpy(table.ucDevType, pcDevName, sizeof(table.ucDevType));
   table.usImageVersion = *(uint16_t*)(pcDevName + 4);
   table.usImageLength = ((BOOT_Get_FlashSize() - BOOTLOADER_SIZE)/2)*256;                          //获取映像文件长度
     
   table.usTag0Length = sizeof(table_head_t);
      
   table.usImageCrc = BOOT_Caculate_Crc16((uint8_t*)(unAppHeadAddr + TABLE_SIZE), table.usImageLength*4, 0x0);
     
   table.usTableCrc = BOOT_Caculate_Crc16((uint8_t*)&table, table.usTag0Length - 2, 0x0);	       //对表头进行CRC校验
     
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
* @brief 检查Table有效与合法性
* @param itype 映像文件区类别
* @note None
* @return 错误码
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
           
    ptable = (p_table_head_t)(unAddr);             //将表头地址地址进行赋值
    
    usTableCrc = ptable->usTableCrc;               //获取表头校验码
     

    if(ptable->usTableHead != TABLE_MAGIC_NUM)      //检查App表头信息是否正确
    {
       return ERR_MAGICNUM_WRONG;
    }    
        
    usCacuCrc = BOOT_Caculate_Crc16((uint8_t*)ptable,
                            sizeof(table_head_t) - 2,
                            0);       //对AppHead的表头进行CRC校验计算
   
    if(usTableCrc != usCacuCrc)
    {
       return ERR_CHECKSUM_WRONG;
    }
  
    if(0xFFFF == ptable->usImageVersion)           //检查App的imageVersion是否有效
    {
       return ERR_IMAGEVERSION_INVALID;
    }            
    
    unAppAddr = unAddr + TABLE_SIZE;
    
    if(((*(uint32_t*)unAppAddr) & 0x2FFE0000) != 0x20000000)   //检查App的栈顶指针是否正确
    {
       return ERR_STACKPOINT_ILLEGA;
    }       
     
    return 0;
}

/**
* @brief 将备份区的映像文件进行拷贝
* @param None
* @note None
* @return 返回错误码
*/
static int32_t BOOT_Copy_AppBak(void)
{
    uint32_t unAppAddr = 0;             
    uint32_t unAppBakAddr = 0;
    uint32_t unPageCnt = 0;                                       //需要烧写的页计数
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
    
    BOOT_Caculate_TableHeadAddr(&unAppAddr,APP_TYPE);                       //计算App的表头地址
    BOOT_Caculate_TableHeadAddr(&unAppBakAddr,APPBAK_TYPE);                 //计算AppBak的表头地址
    
    ptable = (p_table_head_t)(unAppBakAddr);                           //获取AppBak的表头信息
        
    usImageCrc = ptable->usImageCrc;                                   
    usCacuCrc = BOOT_Caculate_Crc16((uint8_t*)(unAppBakAddr + TABLE_SIZE),
            ptable->usImageLength*4, 0);   //对AppBak区进行CRC校验计算,image的长度为字单位
    
    if(usCacuCrc != usImageCrc)
    {
       if(nChkResult != SINGLE_APP_VALID || nChkResult != ALL_IMAGE_VALID)
       {
            BOOT_Process_Err(ERR_CHECKSUM_WRONG);
       }
    }
    else
    {        
        unPageCnt = (ptable->usImageLength*4 - 1)/PAGE_SIZE + 1;    //计算映像文件的的页数目   
        
        nErr = DRV_Erase_Flash(unAppAddr,
        unPageCnt); //ImageSize长度是字单位,整数相除由余数需要加一页
        
        if(nErr != 0)
        {
           return ERR_ERASEFLASH_FAILED;
        }
        
        for(i = 1; i < unPageCnt; i++)
        {        
              DRV_Write_Flash(unAppAddr + i*PAGE_SIZE, (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
              PAGE_SIZE);                 //将AppBak区读取到的映像文件写入到App区
                   
              nErr = memcmp((uint8_t*)(unAppAddr + i*PAGE_SIZE), (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
              PAGE_SIZE);                                    //比较读取的内容和写入的内容是否相同
            
              if(0 != nErr)              //比较不相同则再次写入
              {
                  DRV_Write_Flash(unAppAddr + i*PAGE_SIZE, (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
                  PAGE_SIZE);    //将AppBak区读取到的映像文件写入到App区
                  nErr = memcmp((uint8_t*)(unAppAddr + i*PAGE_SIZE), (uint8_t*)(unAppBakAddr + i*PAGE_SIZE),
                  PAGE_SIZE);                                    //比较读取的内容和写入的内容是否相同 
                  
                  if(0 != nErr)  //再次回读错误则将AppBak区的表头擦除掉，后续再次检查时可以避免校验过程。
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
        sizeof(table_head_t));                   //将AppBak区读取到的映像文件写入到App区        
            
        nErr = memcmp((uint8_t*)ptable, (uint8_t*)unAppAddr, sizeof(table_head_t)); 

        if(0 != nErr)
        {
            DRV_Write_Flash(unAppAddr, (uint8_t*)ptable,
            sizeof(table_head_t));             //将AppBak区读取到的映像文件写入到App区                  
            nErr = memcmp((uint8_t*)ptable, (uint8_t*)unAppAddr, sizeof(table_head_t)); 
            
            if(0 != nErr)          //再次回读错误则将AppBak区的表头擦除掉，后续再次检查时可以避免校验过程。
            {                 
                     DRV_Erase_Flash(unAppBakAddr, 1);              
                     nErr = ERR_PROGRAMFLASH_FAILED;
             }
        } 

        unOffest = (uint8_t*)&ptable->usImageVersion - (uint8_t*)ptable;        
        DRV_Write_Flash(unAppAddr + unOffest, (uint8_t*)&usImageVer,
        sizeof(tablebak.usImageVersion));                 //将AppBak区读取到的映像文件写入到App区         
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
* @brief 系统软复位
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
* @brief App引导
* @param None
* @note None
* @return None
*/
static void BOOT_JumpToApp(void)
{
   register Function_Pointer Jump_Function;                 //跳转函数变量
   
   uint32_t nAddr = 0;
   uint32_t nJump_address = 0;
   
   BOOT_Caculate_TableHeadAddr(&nAddr, APP_TYPE);        //获取AppHead头地址
   nJump_address = nAddr + TABLE_SIZE;             //获取App的地址
    
   Jump_Function = (Function_Pointer)*(uint32_t*)(nJump_address + 4);//跳转函数地址赋值
   
   __set_MSP(*(uint32_t*)nJump_address);  //重设用户栈指针
    
   Jump_Function();                      //进行跳转
    
   while(1);
}

/**
* @brief App引导
* @param None
* @note 对App映像文件进行crc校验并检查其SP和PC的合法性并进行App引导
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
       
      sChk = BOOT_Caculate_Crc16((uint8_t*)nAppAddr, nLen*4, 0);   //App映像文件CRC校验
    
      if(sChk != ptable->usImageCrc)
      {
          if(nChkResult == ALL_IMAGE_VALID)
          {
              nErr = BOOT_Copy_AppBak();              
              if(0 != nErr)
              {
                    sChk = BOOT_Caculate_Crc16((uint8_t*)nAppAddr, nLen, 0);   //CRC校验                   
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
      || (*(uint32_t*)nAppAddr) % 4 != 0)   //检查App的栈顶指针和pc指针是否正确
      {
         if(nChkResult == ALL_IMAGE_VALID)
         {
               nErr = BOOT_Copy_AppBak();
              
               if(((*(uint32_t*)nAppAddr) & 0x2FFE0000) != 0x20000000  || 
             ((nAppAddr & 0x08ff0000) != 0x08000000) 
               || (*(uint32_t*)nAppAddr) % 4 != 0)   //检查App的栈顶指针和pc指针是否正确
               {
                  BOOT_Process_Err(ERR_STACKPOINT_ILLEGA);
               }              
         }
         else
         {
              BOOT_Process_Err(ERR_APP_FAILED);
         }
      } 

      __disable_irq();      //关闭总中断
      
      BOOT_JumpToApp();            
}    

/**
* @brief 检查是否需要升级
* @param None
* @note 通过获取检查App和AppBak的TableHead得到的结果，根据其结果进行相应的处理。
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
       
    nAppRet = BOOT_Check_TableHead(APP_TYPE);         //检查App区升级文件有效与合法性
    
    nAppBakRet = BOOT_Check_TableHead(APPBAK_TYPE);   //检查AppBak区升级文件有效与合法性
       
    if((0 != nAppRet) && (0 != nAppBakRet))      //两者检查都不合法，检查App没问题则重建App表头
    {
       
        nChkResult = ALL_IMAGE_INVALID;        
    }
   
    if((0 != nAppRet) && (0 == nAppBakRet))      //检查到只有AppBak合法
    {
        nChkResult = SINGLE_APPBAK_VALID;
    }
    
    if((0 == nAppRet) && (0 != nAppBakRet))      //检查到只有App区合法
    {
        nChkResult = SINGLE_APP_VALID;
    }
    
    if((0 == nAppRet) && (0 == nAppBakRet))      //检查两个都合法
    {
        nChkResult = ALL_IMAGE_VALID;      
    }
    
    switch(nChkResult)                           //对应的检查结果进行相应的处理
    {
        case ALL_IMAGE_INVALID:            //两个映像文件都检查非法判断App区是否合法合法则重建表头
        {
             uint32_t unAppAddr = 0; 
             BOOT_Caculate_TableHeadAddr(&unAppHeadAddr,APP_TYPE);        
             unAppAddr = unAppHeadAddr + TABLE_SIZE;
        
             if((((*(uint32_t*)unAppAddr) & 0x2ffe0000) == 0x20000000) 
                 && (unAppAddr == 0x08004800) 
                 && ((*(uint32_t*)unAppAddr) % 4 == 0))   //检查App的栈顶指针和pc指针是否正确              
             {                                   
                BOOT_Write_TableHead();         //重写表头                    
                 
                BOOT_Strap();                  //App引导 
             } 
             
             BOOT_Process_Err(ERR_APP_FAILED);                                               
        }break;
        case SINGLE_APPBAK_VALID:                //检查到只有AppBak区合法则进行拷贝处理并引导处理           
        {
            nErr = BOOT_Copy_AppBak();
        
            if(nErr != 0)
            {
              BOOT_Process_Err(nErr);
            }
        
            BOOT_Strap();     
        }break;
        case SINGLE_APP_VALID:                  //检查到只有App区合法则进行引导处理
        {
            BOOT_Strap();
        }break;
        case ALL_IMAGE_VALID: //两者都合法则判断是否需要更新文件，如果需要更新则拷贝并引导处理，否则则直接引导
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
* @brief 单元测试写入表头
* @param itype app的类型 
*     @arg APP_TYPE 将App的头数据进行写入
*     @arg APPBAK_TYPE 将AppBak头数据写入
*     @arg 0 将App和AppBak头数据进行擦除
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
* @brief 单元测试写入表头
* @param itype app的类型 
*     @arg APP_TYPE 将App的头数据进行写入
*     @arg APPBAK_TYPE 将AppBak头数据写入
*     @arg 0 将App和AppBak头数据进行擦除
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
