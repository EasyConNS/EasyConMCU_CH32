#include "flash.h"
#include "debug.h"
const char *TAG_FLASH   = "[FLASH]";


uint32_t fl_ecScWrite_Addr = ecMemAddr_Start;                //伊机控脚本写flash当前记录的内存位置(绝对位置)，初始化值为easyconAddr_start
uint8_t fl_LockState_Flag = 0;                                   //flash状态标志位：0：flash处于上锁状态；1：flash处于解锁状态

int flash_writeUnlock(void)
{
		if(fl_LockState_Flag)
			return fl_LockState_Flag;
    fl_LockState_Flag = 1;  
		FLASH_Unlock();    		
    return fl_LockState_Flag;                          
}

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

/* flash擦除函数 */
int flash_writeErase(uint32_t write_addr, uint32_t pageNum)
{
	  uint32_t EraseCounter = 0x0;
	  uint32_t NbrOfPage = ( userMemAddr_End - userMemAddr_Start ) / userMemPage_Size;
	  FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );
		for( EraseCounter = 0; ( EraseCounter < NbrOfPage ) && ( FLASHStatus == FLASH_COMPLETE ); EraseCounter++ )
		{
				FLASHStatus = FLASH_ErasePage( userMemAddr_Start + ( userMemPage_Size * EraseCounter ) );
				if( FLASHStatus != FLASH_COMPLETE )
				{
						printf( "FLASH Erase ERR at Page%d\r\n", EraseCounter + 60 );
					  return 1;
				}
				//printf( "FLASH Erase Page%d...\r\n", EraseCounter + 60 );
		}
    return 0;
}

/*  flash写uint16_t数据函数 */
int flash_write16B(uint32_t write_addr, uint64_t data)
{
		FLASHStatus = FLASH_ProgramHalfWord( write_addr, (uint16_t)data );
    return FLASHStatus; // 写flash
}

/*  flash写上锁函数 */
int flash_writeLock(void)
{
		if(fl_LockState_Flag==0)
			return fl_LockState_Flag;
    fl_LockState_Flag = 0;                                  // 标志flash已处于上锁的状态
		FLASH_Lock();
    return fl_LockState_Flag;                                // flash上锁
}

/*  伊机控flash接口函数 */
/*  写伊机控脚本函数-通过长度 */
int flash_ecScript_write8B(uint8_t *data, uint16_t len, uint16_t write_addr)
{
	  uint16_t fl_easyconData_Len = 0;
    FLASH_Status ret = 0;
	
		if(write_addr % 2 == 1)
			return WRITE_FAIL;
	
		fl_ecScWrite_Addr = ecMemAddr_Start + write_addr;
	
		fl_easyconData_Len = len / 2;
    for (uint16_t i = 0; i < fl_easyconData_Len; i++)
    {
        ret = flash_write16B(fl_ecScWrite_Addr, data[2*i]|data[2*i+1]<<8);
        if(ret != FLASH_COMPLETE)
        {
            return WRITE_FAIL;
        }
        fl_ecScWrite_Addr += 2;
    }
		if(len % 2 == 1)
		{
			ret = flash_write16B(fl_ecScWrite_Addr, data[len-1]|0xFF<<8);
			if(ret != FLASH_COMPLETE)
			{
					return WRITE_FAIL;
			}
			fl_ecScWrite_Addr += 2;     
		}

    return WRITE_OK;
}

/*  读uint8_t函数-原始数据 */
uint8_t flash_ecScript_read8B(uint16_t read_addr)
{
    return *((volatile uint8_t *)(read_addr + ecMemAddr_Start));
}

/*  读uint8_t函数-原始数据 */
uint16_t flash_ecScript_read16B(uint16_t read_addr)
{
    return *((volatile uint16_t *)(read_addr + ecMemAddr_Start));
}

/*  读uint8_t函数-原始数据 */
void flash_ecScript_read(uint8_t *data, uint16_t len, uint16_t read_addr)
{
    for(uint16_t i=0;i<len;i++)
    {
        data[i] = *((volatile uint8_t *)(read_addr + ecMemAddr_Start+i));
    }
}
