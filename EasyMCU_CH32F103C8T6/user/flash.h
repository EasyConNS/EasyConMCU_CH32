#ifndef _FLASH_HANDLE_H_
#define _FLASH_HANDLE_H_

#include "ch32f10x.h"

/*  User define */
#ifdef CH32F103C8T6              // C8T6内存地址，大小64K
#define userMemAddr_Start       (uint32_t)0x0800F000                        // 起始地址
#define userMemAddr_End         (uint32_t)0x08010000                        // 结束地址
#define userMemPage_Size        (uint32_t)1024                              // 每页字节数/每页字节大小
#define userMemPageNum          (uint32_t)64                                // 可用flash总页数
#define ecMemAddr_Offset        (uint32_t)0x0                             // 存储伊机控的相对地址偏移位置
#define ecMemAddr_Start         (userMemAddr_Start + ecMemAddr_Offset)    // 伊机控脚本在flash中的开始位置
#endif

/*  easycon write script event enum */
enum WRITE_SCRIPT_ENUM
{
    WRITE_OK,
    WRITE_FAIL,
    WRITE_ADDR_ERROR
};

/*  EC Flash API */
extern int flash_writeUnlock(void);
extern int flash_writeLock(void);

extern int flash_writeErase(uint32_t write_addr, uint32_t pageNum);

extern int flash_ecScript_write8B(uint8_t *data, uint16_t len, uint16_t write_addr);
extern int flash_write16B(uint32_t write_addr, uint64_t data);


extern void flash_ecScript_read(uint8_t *data, uint16_t len, uint16_t read_addr);
extern uint8_t flash_ecScript_read8B(uint16_t read_addr);
extern uint16_t flash_ecScript_read16B(uint16_t read_addr);

extern uint32_t fl_ecScWrite_Addr;
extern uint8_t fl_LockState_Flag;

#endif
