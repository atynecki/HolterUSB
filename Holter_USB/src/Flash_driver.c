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
 * 		P7.0]------------------>[!CE 		]
 * 		P7.1]------------------>[CLE		]
 * 		P7.2]------------------>[ALE    	]
 * 		P7.3]------------------>[!WE    	]
 * 		]			[		]
 * 		P7.4]------------------>[!RE 		]
 * 		P7.5]------------------>[!CE2		]
 *  		]			[	        ]
 * 		P1.2]------------------>[!RB		]
 * 		P1.4]------------------>[!RB2		]
 * 		    ]			[		]
 *         MSP430   ]			[ MT298G08AAAWP ]
* 		    ]			[		]
* 		    ]			[		]
* 		    ]-------------------[		]
* 	    Port6   ]<Port6<-->I/O bus >[ D0-D7		]
* 		    ]-------------------[		]
* 		    ]		        [		]
* 		    ]			[		]
 *    [ ____________]-------------------[_______________]
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
	int uiLenth;
	unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
	
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
	unsigned short uiLenth,length;
	unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
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
int uiLenth;
unsigned char *ucBuf_address = (unsigned char *)a_pBuf;
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
struct FlashAddress Write_Flash_Address;
struct FlashAddress Read_Flash_Address;

void clear_write_address()
{
    Write_Flash_Address.ucPageNum = 0;
    Write_Flash_Address.usBlockNum = 0;
    Write_Flash_Address.usColNum = 0;
}

void clear_read_address()
{
    Read_Flash_Address.ucPageNum = 0;
    Read_Flash_Address.usBlockNum = 0;
    Read_Flash_Address.usColNum = 0;
}

bool compare_address()
{
  if(Read_Flash_Address.usBlockNum < Write_Flash_Address.usBlockNum)
    return false;
  else if (Read_Flash_Address.usBlockNum == Write_Flash_Address.usBlockNum){
    if(Read_Flash_Address.ucPageNum < Write_Flash_Address.ucPageNum)
      return false;
    else if (Read_Flash_Address.ucPageNum == Write_Flash_Address.ucPageNum){
      if(Read_Flash_Address.usColNum <= Write_Flash_Address.usColNum)
         return false;
      else
        return true;
    }
    else 
      return false;
  }
  else
    return false;
}

void copy_write_address (struct FlashAddress address_dst)
{
	address_dst.ucPageNum = Write_Flash_Address.ucPageNum;
	address_dst.usBlockNum = Write_Flash_Address.usBlockNum;
	address_dst.usColNum = Write_Flash_Address.usColNum;
}

uint8_t flash_init()
{
    volatile uint8_t result;
	Flash_MT298G08AAAWP_init();
        
	/* check communication */
	unsigned char ucReadID [4];
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	WRITE_NAND_CLE(NAND_CMD_READ_ID);
	WRITE_NAND_ALE(0x00);	
	READ_NAND_ARRAY(&ucReadID, 4);
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;
        
	if ((ucReadID [0] == 0x2C) && (ucReadID [1] == 0x01))
	  result  = NAND_IO_RC_PASS;
	else
	  result  = NAND_IO_RC_FAIL;
	
	return result;
}

static short Flash_ReadStatus(void)
{
	int nReadStatusCount;
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
                    if((ucStatus & STATUS_BIT_0) == 0)	        /* If status bit 0 = 0 the last operation was succesful */
                            return NAND_IO_RC_PASS;
                    else
                            return NAND_IO_RC_FAIL;
            }			
            nReadStatusCount++;
	}
	return NAND_IO_RC_TIMEOUT;
}

short flash_reset()
{
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;
        
	WRITE_NAND_CLE(NAND_CMD_RESET);
        
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return NAND_IO_RC_PASS;
}


static short Flash_ProgramPage( struct FlashAddress write_address, unsigned short size_byte, unsigned char *write_buff)
{
	short rc;
	unsigned char ucnandAddressByte; 

        // Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

	/* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE1);					        /* Issue 0x80 command */

        /* Issue Address */
	ucnandAddressByte = (write_address.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set column address byte 0 */
	ucnandAddressByte = (write_address.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set column address byte 1 */
          
	ucnandAddressByte = (write_address.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((write_address.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 0 */ 
	ucnandAddressByte |= (unsigned char)(((write_address.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((write_address.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 2 */

        /* Issue Command */
	WRITE_NAND_ARRAY(write_buff,size_byte);				                        /* Input data to NAND device */	

	WRITE_NAND_CLE(NAND_CMD_PAGE_PROGRAM_CYCLE2);					        /* Issue 0x10 command */

	rc = Flash_ReadStatus();								        /* Wait for status */                                        

        // Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

void send_data_to_flash(unsigned char *data_frame)
{		
	Flash_ProgramPage(Write_Flash_Address,8,data_frame);
	Write_Flash_Address.usColNum +=8;
	
	if (Write_Flash_Address.usColNum == PAGE_SIZE)
	{
		  Write_Flash_Address.usColNum=0;
		  Write_Flash_Address.ucPageNum++;
		  if (Write_Flash_Address.ucPageNum == PAGES_PER_BLOCK)
		  {
			  Write_Flash_Address.ucPageNum = 0;
			  Write_Flash_Address.usBlockNum++;
			  if (Write_Flash_Address.usBlockNum == MAX_BLOCKS)
			  {
					  Write_Flash_Address.usBlockNum = MAX_BLOCKS-1;
					  Write_Flash_Address.ucPageNum = PAGES_PER_BLOCK-1;
			  }
		  }
      }
}

static short Flash_ReadPage( struct FlashAddress write_address, unsigned short size_byte, unsigned char *read_buff)
{
	short rc;
	unsigned char ucnandAddressByte; 
        // Enable Chip Select
	P7OUT &= ~(enum PORT7_FALSH_CONTROL)FLASH_CE;

        /* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);					        /* Issue page read command cycle 1 */

        /* Issue Address */
	ucnandAddressByte = (write_address.usColNum) & 0xFF;
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set column address byte 0 */
	ucnandAddressByte = (write_address.usColNum >> 8) & 0x0F;
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set column address byte 1 */
	
	ucnandAddressByte = (write_address.ucPageNum ) & 0x3F;
	ucnandAddressByte |= (unsigned char)((write_address.usBlockNum  & 0x0003) << 6);

	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 0 */ 
	ucnandAddressByte |= (unsigned char)(((write_address.usBlockNum  >> 3) & 0x00FF));
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 1 */
	ucnandAddressByte |= (unsigned char)(((write_address.usBlockNum  >> 11) & 0x0007));
	WRITE_NAND_ALE(ucnandAddressByte);		                                        /* Set page address byte 2 */

        /* Issue command */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE2);

	rc = Flash_ReadStatus();										                                      
	if(rc != NAND_IO_RC_PASS)
		return rc;

	/* Issue Command - Set device to read from data register. */
	WRITE_NAND_CLE(NAND_CMD_PAGE_READ_CYCLE1);					

	READ_NAND_ARRAY(read_buff, size_byte);

        // Disable Chip Select
	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

short read_data_from_flash(unsigned char *data_frame)
{							
      Flash_ReadPage(Read_Flash_Address,8,data_frame);
      Read_Flash_Address.usColNum += 8;
      if ( Read_Flash_Address.usColNum == PAGE_SIZE)
      {
          Read_Flash_Address.usColNum = 0;
          Read_Flash_Address.ucPageNum++;
          if (Read_Flash_Address.ucPageNum == PAGES_PER_BLOCK)
          {
              Read_Flash_Address.ucPageNum =0;
              Read_Flash_Address.usBlockNum++;
              if (Read_Flash_Address.usBlockNum == MAX_BLOCKS)
              {
                 return NAND_MEMORY_END;
              }        
          }
      }
      return NAND_IO_RC_PASS;
}

static short Flash_EraseBlock(unsigned short usBlockNum)
{
	short rc;
	unsigned short ucnandAddressByte; 

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

	rc = Flash_ReadStatus();		                        /* Wait for ready status */                                        

	P7OUT |= (enum PORT7_FALSH_CONTROL)FLASH_CE;

	return rc;
}

void erase_flash()
{
      unsigned short BlockCount =0;	
        for ( BlockCount= 1; BlockCount < MAX_BLOCKS; BlockCount++) {
             Flash_EraseBlock(BlockCount);		        /* Erase blank */  
        }
}
