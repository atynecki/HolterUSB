#ifndef FLASH_DRIVER_H_
#define FLASH_DRIVER_H_

#include "app_config.h"

/**
 *  MT298G08AAAWP: 8GBits Nand Flash is used to store the ECG data.
 *
 * 	The MT298G08AAAWP is interfaced with MSP430 as below,
 *
 * 	____________]-------------------[________________
 * 			P7.0]------------------>[!CE]
 * 			P7.1]------------------>[CLE]
 * 			P7.2]------------------>[ALE]
 * 			P7.3]------------------>[!WE]
 * 			P7.4]------------------>[!RE]
 * 			P7.5]------------------>[!CE2]
 * 			P1.2]------------------>[!RB]
 * 			P1.4]------------------>[!RB2]
 * 		    	]-------------------[
 * 	       Port6]<Port6<-->I/O bus >[ D0-D7]
 *[ ____________]-------------------[_______________]
 *
 *
*/

#define NAND_FALSH_DATA_DIR P6DIR
/*****************************************************************************************************************
 *  Control signals to port 7
 * P7.0 --Chip select 1 for first 512 MB data space
 * P7.1 --Clock
 * P7.2 --Address latch enable
 * P7.3 --Write enable
 * P7.4 --Read Enable
 * P7.5 --Chip select 2 for Second 512 MB data space
 *****************************************************************************************************************/
enum PORT7_FALSH_CONTROL
{
	FLASH_CE = 1,
	FLASH_CLE = 2,
	FLASH_ALE = 4,
	FLASH_WE = 8,
	FLASH_RE = 16,
	FLASH_CE2 = 32
};
/*****************************************************************************************************************
 *  Control signals to port 1
 * 
 * P1.2 -- Bussy interrupt for first 512 MB
 * P1.4 -- Bussy interrupt for Second 512 MB
 * 
 *****************************************************************************************************************/
enum PORT1_FALSH_CONTROL
{
	FLASH_RB = 4,
	FLASH_RB2 = 16
};
/*****************************************************************************************************************/

#define CLE_OFFSET 0xFFFFF		/*The offset value is for example purpose only */
#define ALE_OFFSET 0xFFFFE      /*The offset value is for example purpose only */
#define RW_OFFSET  0xFFFFD      /*The offset value is for example purpose only */

#define WRITE_NAND_CLE(command)(Flash_MT298G08AAAWP_Nand_Write_Cmd(CLE_OFFSET, command))
#define WRITE_NAND_ALE(address)(Flash_MT298G08AAAWP_Nand_Write_Addr(ALE_OFFSET, address))
#define WRITE_NAND_BYTE(data) (Flash_MT298G08AAAWP_Nand_Write_byte(RW_OFFSET, 1))
#define WRITE_NAND_ARRAY(data,n) (Flash_MT298G08AAAWP_Nand_Write(RW_OFFSET, data, n))
#define READ_NAND_BYTE(data)(Flash_MT298G08AAAWP_Nand_Read_byte(RW_OFFSET, &(data), 1))
#define READ_NAND_ARRAY(data,n) (Flash_MT298G08AAAWP_Nand_Read(RW_OFFSET, data, n))
#define WRITE_NAND_ARRAY1(data,n) (Flash_MT298G08AAAWP_Nand_Write_8Bytes(RW_OFFSET, data, n))

/***********************************************************************************************************
 *  General Functions.
 * *********************************************************************************************************/
void Flash_MT298G08AAAWP_init(void);
void Flash_MT298G08AAAWP_Nand_Write_Cmd( unsigned long a_ulOffset, unsigned char a_ucCmd);
void Flash_MT298G08AAAWP_Nand_Write_Addr(unsigned long a_ulOffset, unsigned char a_ucAddr);
void Flash_MT298G08AAAWP_Nand_Write( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen);
void Flash_MT298G08AAAWP_Nand_Write_byte( unsigned char a_pucBuf, unsigned short a_usLen);
void Flash_MT298G08AAAWP_Nand_Read( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen);
void Flash_MT298G08AAAWP_Nand_Read_byte( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen);
void Flash_MT298G08AAAWP_Nand_Write_8Bytes(void *a_pBuf, unsigned short a_usLen);

/***********************************************************************************************************
 *  Aplication Functions.
 * *********************************************************************************************************/
/*---------------------------------*/
/* Maximum read status retry count */
/*---------------------------------*/
#define MAX_READ_STATUS_COUNT 			100000

/*----------------------*/
/* NAND status bit mask */
/*----------------------*/
#define STATUS_BIT_0 						0x01
#define STATUS_BIT_1 						0x02
#define STATUS_BIT_5 						0x20
#define STATUS_BIT_6 						0x40

/*----------------------*/
/* NAND status response */
/*----------------------*/
#define NAND_IO_RC_PASS 					0
#define NAND_IO_RC_FAIL 					1
#define NAND_IO_RC_TIMEOUT 					2
#define NAND_MEMORY_END             		3

/*------------------*/
/* NAND command set */
/*------------------*/
#define NAND_CMD_PAGE_READ_CYCLE1 			0x00
#define	NAND_CMD_PAGE_READ_CYCLE2 			0x30
#define NAND_CMD_READ_DATA_MOVE 			0x35
#define NAND_CMD_RESET 						0xFF
#define	NAND_CMD_PAGE_PROGRAM_CYCLE1 		0x80
#define NAND_CMD_PAGE_PROGRAM_CYCLE2 		0x10
#define NAND_CMD_CACHE_PRGM_CONFIRM 		0x15
#define NAND_CMD_PRGM_DATA_MOVE				0x85
#define NAND_CMD_BLOCK_ERASE_CYCLE1 		0x60
#define NAND_CMD_BLOCK_ERASE_CYCLE2 		0xD0
#define NAND_CMD_RANDOM_DATA_INPUT 			0x85
#define NAND_CMD_RANDOM_DATA_READ_CYCLE1	0x05
#define NAND_CMD_RANDOM_DATA_READ_CYCLE2	0xE0
#define NAND_CMD_READ_STATUS 				0x70
#define NAND_CMD_READ_CACHE_START 			0x31
#define NAND_CMD_READ_CACHE_LAST 			0X3F
#define NAND_CMD_READ_UNIQUE_ID 			0x65
#define NAND_CMD_DS 						0xB8
#define	NAND_CMD_READ_ID 					0x90
#define NAND_CMD_READ_UNIQUE_ID 			0x65
#define	NAND_CMD_PROGRAM_OTP 				0xA0
#define	NAND_CMD_PROTECT_OTP 				0xA5
#define	NAND_CMD_READ_OTP 					0xAF
#define	NAND_CMD_BLOCK_UNLOCK_CYCLE1 		0x23
#define	NAND_CMD_BLOCK_UNLOCK_CYCLE2 		0x24
#define	NAND_CMD_BLOCK_LOCK 				0x2A
#define	NAND_CMD_BLOCK_LOCK_TIGHT 			0x2C
#define	NAND_CMD_BLOCK_LOCK_STATUS 			0x7A
    
/*
 *
 * -------------------------------------------------------------
 * |Cycle   |I/O7 | I/O6 | I/O5 | I/O4 |I/O3 |I/O2 |I/O1 |I/O0 |
 * |--------|-----|------|------|------|-----|-----|-----|-----|
 * | First  | CA7 | CA6  | CA5  | CA4  | CA3 | CA2 | CA1 | CA0 | // CA[0-11] - cols[0-2111]
 * | Second | LOW | LOW  | LOW  | LOW  | CA11| CA10| CA9 | CA8 |
 * | Third  | BA7 | BA6  | PA5  | PA4  | PA3 | PA2 | PA1 | PA0 | //PA[0-5] - Page Number's 0-63
 * | Fourth | BA15| BA14 | BA13 | BA12 | BA11| BA10| BA9 | BA8 |
 * | Fifth  | LOW | LOW  | LOW  | LOW  | LOW | LOW | LOW | BA16| // BA - Number of Blocks
 * |-----------------------------------------------------------|
*/

#define PAGE_SIZE				        	2112
#define PAGES_PER_BLOCK			            64
#define MAX_BLOCKS				        	4096

struct FlashAddress{
	unsigned short usBlockNum;
	unsigned short ucPageNum;
	unsigned short usColNum;
};

uint8_t flash_init();
void flash_reset();

short flash_program_page(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);
short flash_program_page_start(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);
short flash_program_page_next(unsigned short usColNum, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);
short flash_program_page_last(void);

short flash_read_page(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);
short flash_read_page_start(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);
short flash_read_page_next(unsigned short a_usColNum, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf);

void erase_flash();

#endif
