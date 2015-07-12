
#include "ADS1292_driver.h"

/**************************************************************************************************************************************************
*	        Global Variables										                              
**************************************************************************************************************************************************/
unsigned char SPI_Tx_buf[10];

/* ADS1x9x Register values*/
unsigned char 	ADS_reg_val[12] = {0};		
unsigned char 	ADS_default_register_settings[12] = {
	//Device ID read only
	ADS1x9x_REG_ID_VALUE,
   	//CONFIG1
	0x02,
	//CONFIG2
	0x80,
	//LOFF
	0x10,
	//CH1SET (PGA gain = 6)
	0x00,
	//CH2SET (PGA gain = 6)
	0x00,
	//RLD_SENS
	0x00,
	//LOFF_SENS
	0x00,    
	//LOFF_STAT
	0x00,
	//RESP1
	0x02,
	//RESP2
	0x03,
	//GPIO
	0x0C
};		
unsigned char 	ADS_register_settings[12] = {
	//Device ID read only
	ADS1x9x_REG_ID_VALUE,
   	//CONFIG1 (continous mode, sample rate 500SPS)
	0x02,
	//CONFIG2
	0xE0,
	//LOFF
	0x10,
	//CH1SET (Channel 1 power down)
	0x80,
	//CH2SET (PGA gain = 12)
	0x60,
	//RLD_SENS
	0x2C,
	//LOFF_SENS
	0x00,    
	//LOFF_STAT
	0x08,
	//RESP1
	0x02,
	//RESP2
	0x03,
	//GPIO
     0x0C
};

      /** HARDWARE CONTROL */
/**********************************************************************************************************
* ADS1x9x_Clock_Select		
* Input : 0 - external											
* 	: 1 - internal					                                         
**********************************************************************************************************/
void ADS1x9x_Clock_Select(unsigned char clock_in)
  {
  	
  	if (clock_in == 1)
  	{
  		P2OUT |= (enum PORT2_ADC_CONTROL)ADC_CLK_SEL;	// Choose internal clock input
  	}
  	else
  	{
  		P2OUT &= ~(enum PORT2_ADC_CONTROL)ADC_CLK_SEL;	// Choose external clock input
  	}
  	
}
/**********************************************************************************************************
* ADS1x9x_Reset			 			                                         	
**********************************************************************************************************/
void ADS1x9x_Reset(void)
  {
            unsigned short i;
	    P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;		// Set High
	    /* Provide suficient dealy*/
	    for(i= 0;	i < 5000; i++);			        // Wait 1 mSec
	    P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_RESET;	// Set to low
	    for(i= 0;	i < 5000; i++);			        // Wait 1 mSec
	    P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;		// Set High
	    for(i= 0;	i < 35000; i++);
    }
  
/**********************************************************************************************************
* ADS1x9x_Disable_Start						                                          
**********************************************************************************************************/
void ADS1x9x_Disable_Start(void)
{
    unsigned short i;
    P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_START;	// Set to LOW
    for(i=0; i<35; i++);        			// Small Delay to settle   
}
/*********************************************************************************************************/
/**********************************************************************************************************
* ADS1x9x_Enable_Start						                                         
**********************************************************************************************************/
void ADS1x9x_Enable_Start(void)
{
    unsigned short i;
    P8OUT |= (enum PORT8_ADC_CONTROL)ADC_START;		// Set to High
    for(i=0; i<50; i++);        			// Small Delay to settle   
}
/**********************************************************************************************************
* Hard_Stop_ADS1x9x					                                          	
**********************************************************************************************************/

void Hard_Stop_ADS1x9x (void)
  {
    unsigned short i, j;
    ADS1x9x_Disable_Start();		// Set Start pin to Low
    for (j = 0; j < DELAY_COUNT; j++)
    {
    	for ( i=0; i < 35000; i++);
    }
  }

/*********************************************************************************************************/
/**********************************************************************************************************
* Set_ADS1x9x_Chip_Enable									
**********************************************************************************************************/
void Set_ADS1x9x_Chip_Enable (void)
{
	P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_CS;		// Set to LOW
}
/**********************************************************************************************************
* Clear_ADS1x9x_Chip_Enable						                   
**********************************************************************************************************/

void Clear_ADS1x9x_Chip_Enable (void)
  {
  	unsigned char CsDelay;
  	for ( CsDelay = 0;  CsDelay < 100 ;CsDelay++);
	
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_CS;		// Set to High
  }


      /** PERIPHERIAL INIT */
/**********************************************************************************************************
* Init_ADS1x9x_DRDY_Interrupt										
**********************************************************************************************************/
void Init_ADS1x9x_DRDY_Interrupt (void)
{
	  P1DIR &= ~0x02;
	  P1REN |= BIT1;                            	// Enable P1.1 internal resistance
	  P1OUT |= BIT1;                            	// Set P1.1 as pull-Up resistance
	  P1IES |= BIT1;                           	// P1.1 Lo/Hi edge
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE &= ~BIT1;                             	// P1.1 interrupt disabled
	
}
/**********************************************************************************************************
* Enable_ADS1x9x_DRDY_Interrupt									
**********************************************************************************************************/
void Enable_ADS1x9x_DRDY_Interrupt (void)
{
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE |= BIT1;                             	// P1.1 interrupt enabled
}
/**********************************************************************************************************
* Disable_ADS1x9x_DRDY_Interrupt									
**********************************************************************************************************/
void Disable_ADS1x9x_DRDY_Interrupt (void)
{
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE &= ~BIT1;                             	// P1.1 interrupt disabled
}

/**********************************************************************************************************
* Set_GPIO												
**********************************************************************************************************/
void Set_GPIO(void)
{
	
	  P2SEL = 0x00;                            
	  P2DIR |= 0x8F;
	  P2OUT |= (enum PORT2_ADC_CONTROL)POW_CE;                            
	  P8DIR |= 0x07;       
	  P8OUT &= 0xF8;
	  P8OUT |= (enum PORT8_ADC_CONTROL)ADC_CS;		// Set RESET, START to Low and CS to High
	  P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;	        // Set RESET, START to Low and CS to High
	  P2OUT = 0x03;
}  

/**********************************************************************************************************
* Set_UCB0_SPI												
**********************************************************************************************************/
void Set_UCB0_SPI(void)
{
	  P3SEL |= BIT2+BIT1+BIT0;  			// Set SPI peripheral bits
	  P3DIR |= BIT0+BIT2;				// Clock and DOUT as output
	  P3DIR &= ~BIT1;                         	// Din as input 
	  UCB0CTL1 |= UCSWRST;               		// Enable SW reset
	  UCB0CTL0 |= UCMSB+UCMST+UCSYNC;		//[b0]   1 -  Synchronous mode 
							//[b2-1] 00-  3-pin SPI
                                                        //[b3]   1 -  Master mode
							//[b4]   0 - 8-bit data
                                                        //[b5]   1 - MSB first
							//[b6]   0 - Clock polarity low.
							//[b7]   1 - Clock phase - Data is captured on the first UCLK edge and changed on the following edge.

	  UCB0CTL1 |= UCSSEL__SMCLK;               	// SCLK
	  UCB0BR0 = 24;                                 // 1 MHz
	  UCB0BR1 = 0;                                  
	  UCB0CTL1 &= ~UCSWRST;              		// Clear SW reset, resume operation
}  

      /** SPI COMMUNICATION WITH DEVICE */ 
/**********************************************************************************************************
* ADS1x9x_CHECK_COMMUNICATION						                            
**********************************************************************************************************/
bool ADS1x9x_SPI_Check_Communication()
{
      unsigned char reg_value;
      reg_value = ADS1x9x_Reg_Read(ADS1x9x_REG_DEVID);
      if(reg_value == ADS1x9x_REG_ID_VALUE)
        return true;
      else
        return false;
}

/**********************************************************************************************************
* ADS1x9x_SPI_Command_Data						                            
**********************************************************************************************************/
void ADS1x9x_SPI_Command_Data(unsigned char Data)
{
	unsigned char delayVar;
	Set_ADS1x9x_Chip_Enable();
	for (delayVar = 0; delayVar < 50; delayVar++);
	Clear_ADS1x9x_Chip_Enable();
	Set_ADS1x9x_Chip_Enable();
	
	UCB0TXBUF = Data;                                     // Send the data sitting at the pointer DATA to the TX Buffer
 	while ( (UCB0STAT & UCBUSY) );

	delayVar = UCB0RXBUF;

	for (delayVar = 0; delayVar < 150; delayVar++);
}

/*********************************************************************************************************
* ADS1x9x_Reg_Write											
**********************************************************************************************************/

void ADS1x9x_Reg_Write (unsigned char READ_WRITE_ADDRESS, unsigned char DATA)
  { 
  	short i;
  	switch (READ_WRITE_ADDRESS)
  	{
  		case 1:
  			DATA = DATA & 0x87;
  		break;
  		case 2:
  			DATA = DATA & 0xFB;
  			DATA |= 0x80;
  			
  		break;
  		case 3:
  			DATA = DATA & 0xFD;
  			DATA |= 0x10;
  			
  		break;
  		case 7:
  			DATA = DATA & 0x3F;
  		break;
  		case 8:
  			DATA = DATA & 0x5F;
  		break;
  		case 9:
  			DATA |= 0x02;
  		break;
  		case 10:
  			DATA = DATA & 0x87;
  			DATA |= 0x01;
  		break;
  		case 11:
  			DATA = DATA & 0x0F;
  		break;
  		
  		default:
  		
  		break;
  		
  	}
	SPI_Tx_buf[0] = READ_WRITE_ADDRESS | WREG;
	SPI_Tx_buf[1] = 0;					// Write Single byte
	SPI_Tx_buf[2] = DATA;					// Write Single byte
	Set_ADS1x9x_Chip_Enable();
	
	for ( i =0; i < 50;i++);

	UCB0TXBUF = SPI_Tx_buf[0];                              // Send the first data to the TX Buffer
 	while ( (UCB0STAT & UCBUSY) );			        // USCI_B0 TX buffer ready?
	i = UCB0RXBUF;						// Read Rx buf

	UCB0TXBUF = SPI_Tx_buf[1];                              // Send the first data to the TX Buffer
	while ( (UCB0STAT & UCBUSY) );			        // USCI_B0 TX buffer ready?
	i = UCB0RXBUF;
	UCB0TXBUF = SPI_Tx_buf[2];                              // Send the first data to the TX Buffer
	while ( (UCB0STAT & UCBUSY) );		                // USCI_B0 TX buffer ready?
	i = UCB0RXBUF;

  }

/*********************************************************************************************************
* ADS1x9x_Reg_Read											
**********************************************************************************************************/
  unsigned char ADS1x9x_Reg_Read(unsigned char Reg_address)
  {
  		unsigned char retVal;
		SPI_Tx_buf[0] = Reg_address | RREG;
		SPI_Tx_buf[1] = 0;				// Read number of bytes - 1
		
		Set_ADS1x9x_Chip_Enable();			// Set chip select to low
		
		UCB0TXBUF = SPI_Tx_buf[0];                      // Send the first data to the TX Buffer
		while ( (UCB0STAT & UCBUSY) );		        // USCI_B0 TX buffer ready?
		UCB0TXBUF = SPI_Tx_buf[1];                      // Send the first data to the TX Buffer
		while ( (UCB0STAT & UCBUSY) );			// USCI_B0 TX buffer ready?
		retVal = UCB0RXBUF;			        // Read RX buff
		UCB0TXBUF = 0x00;                               // Send the first data to the TX Buffer
		while ( (UCB0STAT & UCBUSY) );			// USCI_B0 TX buffer ready?
		retVal = UCB0RXBUF;				// Read RX buff

		Clear_ADS1x9x_Chip_Enable();			// Disable chip select
		return 	retVal;
  }

          /** SOFTWARE CONTROL */
/**********************************************************************************************************
* Wake_Up_ADS1x9x						                                         
**********************************************************************************************************/
void Wake_Up_ADS1x9x (void)
  { 
    ADS1x9x_SPI_Command_Data (WAKEUP);                   // Send 0x02 to the ADS1x9x                                                      
  }

/**********************************************************************************************************
* Put_ADS1x9x_In_Sleep						                                         
**********************************************************************************************************/
void Put_ADS1x9x_In_Sleep (void)
  {
    ADS1x9x_SPI_Command_Data (STANDBY);                 // Send 0x04 to the ADS1x9x
  }
/**********************************************************************************************************
* Soft_Reset_ADS1x9x					                                          	
**********************************************************************************************************/

void Soft_Reset_ADS1x9x (void)
  {
    ADS1x9x_SPI_Command_Data (RESET);                   // Send 0x06 to the ADS1x9x
  }
/**********************************************************************************************************
* Soft_Start_ADS1x9x					                                          	
**********************************************************************************************************/

void Soft_Start_ADS1x9x (void)
  {
    ADS1x9x_SPI_Command_Data (START);                   // Send 0x08 to the ADS1x9x
  }

/**********************************************************************************************************
* Soft_Stop_ADS1x9x					                                          	
**********************************************************************************************************/

void Soft_Stop_ADS1x9x (void)
  {
    ADS1x9x_SPI_Command_Data (STOP);                   // Send 0x0A to the ADS1x9x
  }

/**********************************************************************************************************
* Start_Read_Data_Continuous			                                          		
**********************************************************************************************************/

void Start_Read_Data_Continuous (void)
  {
    ADS1x9x_SPI_Command_Data (RDATAC);					// Send 0x10 to the ADS1x9x
  }

/**********************************************************************************************************
* Stop_Read_Data_Continuous			                                          		
**********************************************************************************************************/

void Stop_Read_Data_Continuous (void)
  {
    ADS1x9x_SPI_Command_Data(SDATAC);					// Send 0x11 to the ADS1x9x
  }

/**********************************************************************************************************
* Read_Data		                                          		
**********************************************************************************************************/

void Read_Data (void)
  {
    ADS1x9x_SPI_Command_Data (RDATA);					// Send 0x12 to the ADS1x9x
  }

      /** API FUNCTIONS */
/**********************************************************************************************************
*ADS1x9x_Reg_Init          				              
**********************************************************************************************************/

void ADS1x9x_Reg_Init(unsigned char* reg_tab)
{
	unsigned char Reg_Init_i;
	
        Set_ADS1x9x_Chip_Enable();
	for ( Reg_Init_i =0; Reg_Init_i <100;Reg_Init_i++);
	Clear_ADS1x9x_Chip_Enable();
	
	for ( Reg_Init_i = 1; Reg_Init_i < 12; Reg_Init_i++){
          ADS1x9x_Reg_Write(Reg_Init_i,reg_tab[Reg_Init_i]);
        }
}

/**********************************************************************************************************
*	        ADS1x9x_Read_All_Regs          				                  	
**********************************************************************************************************/

void ADS1x9x_Read_All_Regs(unsigned char ADS1x9xeg_buf[])
{
	unsigned char Regs_i;
	Set_ADS1x9x_Chip_Enable();
	for ( Regs_i =0; Regs_i <200;Regs_i++);
	Clear_ADS1x9x_Chip_Enable();

	for ( Regs_i = 0; Regs_i < 12; Regs_i++)
	{
		ADS1x9xeg_buf[Regs_i] = ADS1x9x_Reg_Read(Regs_i);
	}
}

static bool compare_reg_value()
{
       unsigned char i;
       for( i=0; i<8; ++i) {
         if(ADS_reg_val[i] != ADS_register_settings[i])
             return false;
       }
       
       return true;
}
/*********************************************************************************************************/
/**********************************************************************************************************
* ADS1x9x_Init          				                  		
***********************************************************************************************************/
uint8_t ADS1x9x_Init(void)
{
   volatile unsigned short Init_i, j;
   bool error = false;
   
   Set_GPIO();
   Set_UCB0_SPI();
   
   ADS1x9x_Reset();
   for (j = 0; j < DELAY_COUNT; j++){
      for ( Init_i =0; Init_i < 20000; Init_i++);
      for ( Init_i =0; Init_i < 20000; Init_i++);
      for ( Init_i =0; Init_i < 20000; Init_i++);
   }
   
   ADS1x9x_Clock_Select(1);		// Set internal clock
   for ( Init_i =0; Init_i < 20000; Init_i++);
   for ( Init_i =0; Init_i < 20000; Init_i++);
   for ( Init_i =0; Init_i < 20000; Init_i++);
   
   Stop_Read_Data_Continuous();					// SDATAC command
   
   error = ADS1x9x_SPI_Check_Communication();
   if(error == false)
     return 1;
   
   ADS1x9x_Reg_Init(ADS_register_settings);
   for (j = 0; j < DELAY_COUNT; j++){
        for ( Init_i =0; Init_i < 20000; Init_i++);
        for ( Init_i =0; Init_i < 20000; Init_i++);
        for ( Init_i =0; Init_i < 20000; Init_i++);
   }
   
   ADS1x9x_Read_All_Regs(ADS_reg_val);
   error = compare_reg_value();
   while(error == false){
      ADS1x9x_Reg_Init(ADS_register_settings);
      for (j = 0; j < DELAY_COUNT; j++){
        for ( Init_i =0; Init_i < 20000; Init_i++);
        for ( Init_i =0; Init_i < 20000; Init_i++);
        for ( Init_i =0; Init_i < 20000; Init_i++);
      }
      
      ADS1x9x_Read_All_Regs(ADS_reg_val);
      error = compare_reg_value();
   }
     
   Init_ADS1x9x_DRDY_Interrupt();
   Disable_ADS1x9x_DRDY_Interrupt();
   
   return 0;
}

/**********************************************************************************************************
* enable_ADS1x9x_Conversion										
**********************************************************************************************************/
void enable_ADS1x9x_Conversion (void)
  {
    Soft_Start_ADS1x9x();
    Start_Read_Data_Continuous ();		//RDATAC command
    Enable_ADS1x9x_DRDY_Interrupt();
  }
/*********************************************************************************************************/

/**********************************************************************************************************
* disable_ADS1x9x_Conversion										
**********************************************************************************************************/
void disable_ADS1x9x_Conversion (void)
  {
    Stop_Read_Data_Continuous();                //SDATAC command
    Soft_Start_ADS1x9x();
    Disable_ADS1x9x_DRDY_Interrupt();
  }
/*********************************************************************************************************/

// End of file
