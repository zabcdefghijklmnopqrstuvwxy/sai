/**
********************************************************************************
* @file    drv_flash.c
* @author  Sai
* @version UDFRM-C-GEN-01V00-C01
* @date    2019-04-12
* @brief   flash����
* @note    
* @attention
********************************************************************************
*/

#include "drv_flash.h"


#define FLASH_SIZE   256                  /**< flash ��С */

/**
* @brief flash���ݲ���
* @param[in] nAddr������ַ nCountToErase������ҳ���� 
* @note flash����ֻ���ҳ����
* @return ����-1��ʾ��������Ƿ� ����-2��ʾ��������ʧ�� ����0��ʾд��������
*/
int32_t DRV_Erase_Flash(uint32_t nAddr,uint32_t nCountToErase)
{	  
    uint32_t nErr = 0;
    uint32_t unSize = 0;
    uint32_t nOffsetAddress = nAddr - FLASH_BASE;                             //����ȥ��0X08000000���ʵ��ƫ�Ƶ�ַ
    uint32_t nSectorPosition = nOffsetAddress/SECTOR_SIZE;                    //����������ַ
    uint32_t nSectorStartAddress = nSectorPosition*SECTOR_SIZE+FLASH_BASE;    //��Ӧ�������׵�ַ
    uint32_t nStartAddress = nSectorStartAddress;    //��Ӧ�������׵�ַ
    
    int32_t i = 0;
   
    unSize = nCountToErase * SECTOR_SIZE;   //�ܲ�����С��λΪByte
    
    if(nAddr<FLASH_BASE||((nAddr+nCountToErase*SECTOR_SIZE)>=(FLASH_BASE + SECTOR_SIZE * FLASH_SIZE)))
    {
       return -1;
    }
       
    for(i = 0; i < nCountToErase; i++)
    {		
       FLASH_Unlock();
    
       nErr = FLASH_ErasePage(nSectorStartAddress);
    
       FLASH_Lock();
			
       nSectorStartAddress = nSectorStartAddress + SECTOR_SIZE;
    }
		
    for(i = 0; i < unSize; i = i + 4)
    {
       if(0xffffffff != *(uint32_t*)(nStartAddress + i)) 
       {
            break;   
       }              
    }
    
    if(i < unSize)
    {
       return -2;    
    }
        
    return nErr;
}

/**
* @brief flash����д��
* @param nAddr д���ַ pdataд������ nLen д�����ݳ���
* @note None
* @return ����-1��ʾ�������д������Ч�Ƿ� ����-2��ʾ����д��ַ�Ƿ� ����0��ʾд������
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
          
    for(i = 0;i < nLen;i = i + 4)
    {
       lData = ((uint32_t)pdata[i + 3] << 24) | ((uint32_t)pdata[i + 2] << 16) | ((uint32_t)pdata[i + 1] << 8) | ((uint32_t)pdata[i]);
           
       FLASH_Unlock();         //����д���� 
			
       nErr = FLASH_ProgramWord(nAddr + i,lData);
						
       FLASH_Lock();         //����д����
    }
       
    return nErr;
}


#if 0
/**
* @brief  ��Ԫ����flash�Ķ�д
* @param None
* @note None
* @return ����0 ��ʾ��ȷ ���ط�0��ʾ��д�쳣
*/
int unittest_flash(void)
{
   uint32_t nAddr = 0x08060000;
   uint32_t nPageCnt = 4;	
   uint8_t wbuf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
	 uint8_t rbuf[16] = {0};
   uint32_t nLen = 16;

   DRV_Erase_Flash(nAddr,nPageCnt);

   DRV_Write_Flash(nAddr,wbuf,16);

   DRV_Read_Flash(nAddr,rbuf,nLen);

   if(0 != memncmp(rbuf,wbuf,nLen))
	 {
       return -1;
   }
   
   return 0;
}
#endif
