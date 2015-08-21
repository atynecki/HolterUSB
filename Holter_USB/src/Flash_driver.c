/***********************************************************************************************************
 *  MT298G08AAAWP: 8GBits Nand Flash is used to store the ECG data.
 * This file contains three mdules which are as below
 * Flash_MT298G08AAAWP_init()
 * 			: which initialzes the MT298G08AAAWP interface and GPIO regs.
 * Flash_MT298G08AAAWP_Page_Read()
 * 			: which reads the MT298G08AAAWP page of size 2048 bytes.
 * Flash_MT298G08AAAWP_Page_Write()
 * 			: which writes the MT298G08AAAWP page of size 2048 bytes.
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
 *******************************************************************************************************/

#include "Flash_driver.h"
//unsigned char ucPageReadBuf[2112];
//unsigned char ucPageProgBuf[2112];
//unsigned char FlashData2[64];

/***********************************************************************************************************
 * 	Function Name	: Flash_MT298G08AAAWP_init()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 *					: Low level driver code to initialize memory chip. 
 ***********************************************************************************************************/
void Flash_MT298G08AAAWP_init(void)
{
	NAND_FALSH_DATA_DIR = 0x00;		        // set as input port
	P7DIR |= 0x3F;					// Set CE2,RE,CE,CLE,ALE and WE as out pins
//	Disable_CS1();					
// 	Clear_CLE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CLE;	// Disable Command Latch Enable
// 	Set_CE();
 	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;	// Disable chip selects
// 	Set_CE2();
 	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE2;	// Disable Command Latch Enable Block2
// 	Set_RE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_RE;	// Disable Read Enable
// 	Set_WE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;	// Disable Write Enable
// 	Clear_ALE();
 	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_ALE;	// Disable Address Latch Enable

} /* Flash_MT298G08AAAWP_init */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Write_Cmd()
 * 	Purpose			:  Low level driver code to write a command to NAND flash.
 ***********************************************************************************************************/
void Flash_MT298G08AAAWP_Nand_Write_Cmd( unsigned long a_ulOffset, unsigned char a_ucCmd)
{

//Set_IO_DIR_Out();									// Set DIR as output
	P6DIR = 0xFF;									// Set DIR as output

// 	Set_CLE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CLE;	// Enable Command Latch
 	
// 	Clear_WE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;	// Set write pin to low
	
/* Load  command on Data bus */
 	P6OUT = a_ucCmd;								// Issue Command 1 

// 	Set_WE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;	// Disable !WE
	
// 	Clear_CLE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CLE; // Disable Command Latch

}/* Flash_MT298G08AAAWP_Nand_Write_Cmd */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Write_Addr()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/

void Flash_MT298G08AAAWP_Nand_Write_Addr( unsigned long a_ulOffset, unsigned char a_ucAddr)
{
	/* Add low level driver code to write an address to NAND flash. */
	//Set_IO_DIR_Out();								// Set DIR as output
	P6DIR = 0xFF;				// Set DIR output
// 	Set_ALE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_ALE;
// 	Clear_WE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
 	P6OUT = a_ucAddr;					// Issue address
// 	Set_WE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;
// 	Clear_ALE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_ALE;

}/* Flash_MT298G08AAAWP_Nand_Write_Addr */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Write()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/

void Flash_MT298G08AAAWP_Nand_Write( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen)
{
	volatile int uiLenth;
	volatile unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
	
	/* Add low level driver code to write data to NAND flash. */
	
	//Set_IO_DIR_Out();									// Set DIR as output
	P6DIR = 0xFF;										// Set DIR output
	for (uiLenth = 0; uiLenth < a_usLen; uiLenth++)
	{
// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;
	}
}/* Flash_MT298G08AAAWP_Nand_Write */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Write()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/

void Flash_MT298G08AAAWP_Nand_Write_8Bytes(  void *a_pBuf, unsigned short a_usLen)
{
	volatile unsigned short uiLenth,length;
	volatile unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
	length = a_usLen >> 3;
	/* Add low level driver code to write data to NAND flash. */
	
	//Set_IO_DIR_Out();									// Set DIR as output
	P6DIR = 0xFF;										// Set DIR output
	for (uiLenth = 0; uiLenth < length; uiLenth++)
	{
// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;
		
// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;// 		Clear_WE();
		
// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;		

// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

// 		Clear_WE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
	 	P6OUT = *ucBuf_address++;						// Write data to data bus.
// 		Set_WE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

	}
}/* Flash_MT298G08AAAWP_Nand_Write */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Write_byte()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/
void Flash_MT298G08AAAWP_Nand_Write_byte( unsigned char a_pucBuf, unsigned short a_usLen)
{
	/* Add low level driver code to write data to NAND flash. */
	//Set_IO_DIR_Out();								// Set DIR as output
	P6DIR = 0xFF;				// Set DIR output

// 	Clear_WE();
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_WE;
 	P6OUT = a_pucBuf;
// 	Set_WE();
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_WE;

}/* Flash_MT298G08AAAWP_Nand_Write_byte */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_Nand_Read()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/
void Flash_MT298G08AAAWP_Nand_Read( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen)
{

	/* Add low level driver code to read data from NAND flash. */
	volatile int uiLenth;
	volatile unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
	//	Set_IO_DIR_In();								// Set DIR as input
	P6DIR = 0x00;				// Set DIR input
	/* Add low level driver code to write data to NAND flash. */
	for (uiLenth = 0; uiLenth < a_usLen; uiLenth++)
	{
		//	 	Clear_RE();
		P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_RE;
	 	*ucBuf_address++ = P6IN;
	 	//	 	Set_RE();
		P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_RE;
		
	}
}/* Flash_MT298G08AAAWP_Nand_Read */

/***********************************************************************************************************
 * 	Function Name	: 	Flash_MT298G08AAAWP_init()
 * 	Purpose			: This function Initializaes flash and memory pointer.
 ***********************************************************************************************************/
void Flash_MT298G08AAAWP_Nand_Read_byte( unsigned long a_ulOffset, void *a_pBuf, unsigned short a_usLen)
{
	volatile unsigned char *ucBuf_address = (unsigned char *)a_pBuf;

	/* Low level driver code to read data from NAND flash. */
	P6DIR = 0x00;				// Set BUS DIR as input

	/* Set Read Enable Pin to Low for NAND flash. */

	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_RE;	// Enable Chip
	
	/* Read data fro NAND flash. */

 	*ucBuf_address = P6IN;

	/* Set Read Enable Pin to High for NAND flash. */

	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_RE; 	// Disable Chip

}/* Flash_MT298G08AAAWP_Nand_Read_byte */


/** APPLICATION FUNCTIONS */
uint8_t flash_init()
{
    volatile uint8_t result;
	Flash_MT298G08AAAWP_init();
        
	/* check communication */
	volatile unsigned char ucReadID [4];
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	WRITE_NAND_CLE(NAND_CMD_READ_ID);
	WRITE_NAND_ALE(0x20);
	READ_NAND_ARRAY(&ucReadID, 4);
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;
        
	if ((ucReadID[0] == 0x4F) && (ucReadID[1] == 0x4E) && (ucReadID[2] == 0x46) && (ucReadID[3] == 0x49))
	  result  = NAND_IO_RC_PASS;
	else
	  result  = NAND_IO_RC_FAIL;
	
	return result;
}

static short flash_read_status()
{
	volatile int nReadStatusCount;
	unsigned char ucStatus;

	WRITE_NAND_CLE(NAND_CMD_READ_STATUS);
	nReadStatusCount = 0;
	while(nReadStatusCount < MAX_READ_STATUS_COUNT)
	{
            /* Read status byte */
            READ_NAND_BYTE(ucStatus);
            /* Check status */
            if((ucStatus & STATUS_BIT_6) == STATUS_BIT_6)       /* If status bit 6 = 1 device is ready */
            {
                    if((ucStatus & STATUS_BIT_0) == 0){	        /* If status bit 0 = 0 the last operation was succesful */
                    	return NAND_IO_RC_PASS;
                    }
                    else {
                         return NAND_IO_RC_FAIL;
                    }
            }			
            nReadStatusCount++;
	}
	return NAND_IO_RC_TIMEOUT;
}

void flash_reset()
{
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;
        
	WRITE_NAND_CLE(NAND_CMD_RESET);
        
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;
}

short flash_program_page(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	short rc;
	unsigned char ucnandAddressByte;

	//	Clear_CE(); 		// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE1);					/* Issue 0x80 command */

	/* Issue Address */
	ucnandAddressByte = (structNandAddress.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 0 */
	ucnandAddressByte = (structNandAddress.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 1 */

	ucnandAddressByte = (structNandAddress.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((structNandAddress.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 0 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 2 */

	/* Issue Command */
	Flash_MT298G08AAAWP_Nand_Write_8Bytes(a_pucReadBuf, a_usReadSizeByte);				/* Input data to NAND device */

	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE2);					/* Issue 0x10 command */

	rc = flash_read_status();											/* Wait for status */

	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

short flash_program_page_start(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	unsigned char ucnandAddressByte;

	//	Clear_CE(); 			// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;
	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE1);	/* Issue 0x80 command */

	/* Issue Address */
	ucnandAddressByte = (structNandAddress.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 0 */
	ucnandAddressByte = (structNandAddress.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 1 */

	ucnandAddressByte = (structNandAddress.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((structNandAddress.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 0 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 2 */

	Flash_MT298G08AAAWP_Nand_Write_8Bytes(a_pucReadBuf, a_usReadSizeByte);

	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return 0;
}

short flash_program_page_next(unsigned short usColNum, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	//	Clear_CE(); 		// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;
	/* Issue Command */
	WRITE_NAND_CLE(NAND_CMD_RANDOM_DATA_INPUT);					/* Issue 0x85 command */

	/* Issue Address */
	WRITE_NAND_ALE((unsigned char)(usColNum & 0xFF));			/* Set column address byte 0 */
	WRITE_NAND_ALE((unsigned char)((usColNum >> 8) & 0x0F));	/* Set column address byte 1 */

	Flash_MT298G08AAAWP_Nand_Write_8Bytes(a_pucReadBuf, a_usReadSizeByte);

	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return 0;
}

short flash_program_page_last()
{
	short rc;
	//	Clear_CE(); 		// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE2);

	rc = flash_read_status();
	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

short flash_read_page(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	short rc;
	unsigned char ucnandAddressByte;
	//	Clear_CE(); 		// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);		/* Issue page read command cycle 1 */

	/* Issue Address */
	ucnandAddressByte = (structNandAddress.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 0 */
	ucnandAddressByte = (structNandAddress.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 1 */

	ucnandAddressByte = (structNandAddress.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((structNandAddress.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 0 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 2 */

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE2);

	rc = flash_read_status();
	if(rc != NAND_IO_RC_PASS)
		return rc;

	/* Issue Command - Set device to read from data register. */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);

	READ_NAND_ARRAY(a_pucReadBuf, a_usReadSizeByte);

	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

short flash_read_page_start(struct FlashAddress structNandAddress, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	short rc;
	unsigned char ucnandAddressByte;

	//	Clear_CE(); 			// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);					/* Issue page read command cycle 1 */

	/* Issue Address */
	ucnandAddressByte = (structNandAddress.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 0 */
	ucnandAddressByte = (structNandAddress.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set column address byte 1 */
	
	ucnandAddressByte = (structNandAddress.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((structNandAddress.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 0 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((structNandAddress.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		/* Set page address byte 2 */

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE2);

	rc = flash_read_status();						/* Wait for ready status */

	if(rc != NAND_IO_RC_PASS)
		return rc;

	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);		/* Set device to read data mode by issuing a page read command */

	READ_NAND_ARRAY(a_pucReadBuf, a_usReadSizeByte);

	return rc;
}

short flash_read_page_next(unsigned short a_usColNum, unsigned short a_usReadSizeByte, unsigned char *a_pucReadBuf)
{
	//	Clear_CE(); 		// Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_RANDOM_DATA_READ_CYCLE1);			/* Issue 0x05 command. */

	/* Issue Column Address */
	WRITE_NAND_ALE((unsigned char)(a_usColNum & 0xFF));
	WRITE_NAND_ALE((unsigned char)((a_usColNum >> 8) & 0x0F));	/* Set column address byte 1 */

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_RANDOM_DATA_READ_CYCLE2);			/* Issue 0xE0 command. */

	READ_NAND_ARRAY(a_pucReadBuf, a_usReadSizeByte);

	//	Set_CE(); 			// Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;
	return 0;
}

static short flash_erase_block(unsigned short usBlockNum)
{
	volatile short rc;
	volatile unsigned short ucnandAddressByte;

	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	WRITE_NAND_CLE(NAND_CMD_BLOCK_ERASE_CYCLE1);
	
	ucnandAddressByte = usBlockNum & 0x3;
	ucnandAddressByte = ucnandAddressByte << 6;
	
	WRITE_NAND_ALE((unsigned char)ucnandAddressByte);	/* Set page address byte 0 */ 
	
	ucnandAddressByte = usBlockNum >> 0x2;
	ucnandAddressByte = ucnandAddressByte & 0xff;
	
	WRITE_NAND_ALE((unsigned char)ucnandAddressByte);	/* Set page address byte 1 */
	 
	ucnandAddressByte = usBlockNum >> 10;
	ucnandAddressByte = ucnandAddressByte & 0x07;
	
	WRITE_NAND_ALE((unsigned char)ucnandAddressByte);	/* Set page address byte 2 */
	WRITE_NAND_CLE(NAND_CMD_BLOCK_ERASE_CYCLE2);

	rc = flash_read_status();		                        /* Wait for ready status */

	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

void erase_flash()
{
	volatile unsigned short BlockCount =0;
	for ( BlockCount= 1; BlockCount < MAX_BLOCKS; BlockCount++) {
		 flash_erase_block(BlockCount);
	}
}
