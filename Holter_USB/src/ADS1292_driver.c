/**
 * @file
 * @brief ADS1292R driver source
 */

#include "ADS1292_driver.h"

/**Global Variables	*/
uint8_t SPI_Tx_buf[10];
uint8_t ADS_reg_val[12] = {0};
uint8_t ADS_default_register_settings[12] = {
	//Device ID
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

uint8_t ADS_register_settings[12] = {
	//Device ID
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
/**
* Input : 0 - external											
* 		: 1 - internal
*/
void ADS1292_Clock_Select(uint8_t clock_in)
 {
  	if (clock_in == 1) {
  		P2OUT |= (enum PORT2_ADC_CONTROL)ADC_CLK_SEL;	// Choose internal clock input
  	}
  	else {
  		P2OUT &= ~(enum PORT2_ADC_CONTROL)ADC_CLK_SEL;	// Choose external clock input
  	}
}

void ADS1292_Reset ()
 {
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;		// Set High
	DELAY_1MS();		        					// Wait 1 mSec
	P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_RESET;	// Set to low
	DELAY_1MS();			        				// Wait 1 mSec
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;		// Set High
	DELAY_1MS();
}

void ADS1292_Disable_Start ()
{
    P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_START;	// Set to LOW
    DELAY_1MS();
}

void ADS1292_Enable_Start ()
{
    P8OUT |= (enum PORT8_ADC_CONTROL)ADC_START;		// Set to High
    DELAY_1MS();
}

void Hard_Stop_ADS1292 ()
{
	ADS1292_Disable_Start();						// Set Start pin to Low
	DELAY_100MS();
}

void Set_ADS1292_Chip_Enable ()
{
	P8OUT &= ~(enum PORT8_ADC_CONTROL)ADC_CS;		// Set to LOW
}

void Clear_ADS1292_Chip_Enable ()
{
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_CS;		// Set to High
}

      /** PERIPHERIAL INIT */
void Init_ADS1292_DRDY_Interrupt ()
{
	  P1DIR &= ~0x02;
	  P1REN |= BIT1;                            	// Enable P1.1 internal resistance
	  P1OUT |= BIT1;                            	// Set P1.1 as pull-Up resistance
	  P1IES |= BIT1;                           		// P1.1 Lo/Hi edge
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE &= ~BIT1;                             	// P1.1 interrupt disabled
}

void Enable_ADS1292_DRDY_Interrupt ()
{
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE |= BIT1;                             	// P1.1 interrupt enabled
}

void Disable_ADS1292_DRDY_Interrupt ()
{
	  P1IFG &= ~BIT1;                           	// P1.1 IFG cleared
	  P1IE &= ~BIT1;                             	// P1.1 interrupt disabled
}

static void Set_GPIO ()
{
	P2SEL = 0x00;
	P2DIR |= 0x8F;
	P2OUT |= (enum PORT2_ADC_CONTROL)POW_CE;
	P8DIR |= 0x07;
	P8OUT &= 0xF8;
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_CS;		// Set RESET, START to Low and CS to High
	P8OUT |= (enum PORT8_ADC_CONTROL)ADC_RESET;		// Set RESET, START to Low and CS to High
	P2OUT = 0x03;
}  


static void Set_UCB0_SPI ()
{
	P3SEL |= BIT2+BIT1+BIT0;  			// Set SPI peripheral bits
	P3DIR |= BIT0+BIT2;					// Clock and DOUT as output
	P3DIR &= ~BIT1;                     // Din as input
	UCB0CTL1 |= UCSWRST;               	// Enable SW reset
	UCB0CTL0 |= UCMSB+UCMST+UCSYNC;		//[b0]   1 -  Synchronous mode
										//[b2-1] 00-  3-pin SPI
										//[b3]   1 -  Master mode
										//[b4]   0 - 8-bit data
										//[b5]   1 - MSB first
										//[b6]   0 - Clock polarity low
										//[b7]   1 - Clock phase - Data is captured on the first UCLK edge and changed on the following edge
	UCB0CTL1 |= UCSSEL__SMCLK;          // SCLK
	UCB0BR0 = 24;                       // 1 MHz
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST;              	// Clear SW reset, resume operation
}  

 /** SPI COMMUNICATION WITH DEVICE */
bool ADS1292_SPI_Check_Communication ()
{
	uint8_t reg_value;
	reg_value = ADS1292_Reg_Read(ADS1x9x_REG_DEVID);
	if(reg_value == ADS1x9x_REG_ID_VALUE)
	return true;
	else
	return false;
}

void ADS1292_SPI_Command_Data(uint8_t data)
{
	uint8_t dummy;
	Set_ADS1292_Chip_Enable();
	DELAY_1MS();
	Clear_ADS1292_Chip_Enable();
	Set_ADS1292_Chip_Enable();
	
	UCB0TXBUF = data;
 	while ( (UCB0STAT & UCBUSY) );

 	dummy = UCB0RXBUF;
}

void ADS1292_Reg_Write (uint8_t address, uint8_t data)
{
  	uint8_t i;
  	switch (address)
  	{
  		case 1:
  			data = data & 0x87;
  		break;
  		case 2:
  			data = data & 0xFB;
  			data |= 0x80;
  			
  		break;
  		case 3:
  			data = data & 0xFD;
  			data |= 0x10;
  			
  		break;
  		case 7:
  			data = data & 0x3F;
  		break;
  		case 8:
  			data = data & 0x5F;
  		break;
  		case 9:
  			data |= 0x02;
  		break;
  		case 10:
  			data = data & 0x87;
  			data |= 0x01;
  		break;
  		case 11:
  			data = data & 0x0F;
  		break;
  		
  		default:
  		break;
  		
  	}
	SPI_Tx_buf[0] = address | WREG;
	SPI_Tx_buf[1] = 0;
	SPI_Tx_buf[2] = data;
	Set_ADS1292_Chip_Enable();
	
	for (i=0; i < 50;i++);

	UCB0TXBUF = SPI_Tx_buf[0];
 	while ( (UCB0STAT & UCBUSY) );
	i = UCB0RXBUF;

	UCB0TXBUF = SPI_Tx_buf[1];
	while ( (UCB0STAT & UCBUSY) );
	i = UCB0RXBUF;
	UCB0TXBUF = SPI_Tx_buf[2];
	while ( (UCB0STAT & UCBUSY) );
	i = UCB0RXBUF;
}

uint8_t ADS1292_Reg_Read(uint8_t address)
{
	uint8_t retVal;
	SPI_Tx_buf[0] = address | RREG;
	SPI_Tx_buf[1] = 0;

	Set_ADS1292_Chip_Enable();

	UCB0TXBUF = SPI_Tx_buf[0];
	while ((UCB0STAT & UCBUSY));
	UCB0TXBUF = SPI_Tx_buf[1];
	while ((UCB0STAT & UCBUSY));
	retVal = UCB0RXBUF;
	UCB0TXBUF = 0x00;
	while ((UCB0STAT & UCBUSY));
	retVal = UCB0RXBUF;

	Clear_ADS1292_Chip_Enable();
	return 	retVal;
}

/** SOFTWARE CONTROL */
void Wake_Up_ADS1292 ()
{
	ADS1292_SPI_Command_Data (WAKEUP);
}
void Put_ADS1292_In_Sleep ()
{
    ADS1292_SPI_Command_Data (STANDBY);
}

void Soft_Reset_ADS1292 ()
{
	ADS1292_SPI_Command_Data (RESET);
}

void Soft_Start_ADS1292 ()
{
    ADS1292_SPI_Command_Data (START);
}

void Soft_Stop_ADS1292 ()
{
	ADS1292_SPI_Command_Data (STOP);
}

void Start_Read_Data_Continuous ()
{
	ADS1292_SPI_Command_Data (RDATAC);
}

void Stop_Read_Data_Continuous ()
{
	ADS1292_SPI_Command_Data(SDATAC);
}

void Read_Data ()
{
	ADS1292_SPI_Command_Data (RDATA);
}

/** API FUNCTIONS */
static void ADS1292_Reg_Init(uint8_t* reg_tab)
{
	uint8_t i;
	Set_ADS1292_Chip_Enable();
	DELAY_1MS();
	Clear_ADS1292_Chip_Enable();
	for (i = 1; i < 12; i++){
        ADS1292_Reg_Write(i,reg_tab[i]);
    }
}

static void ADS1292_Read_All_Regs(uint8_t *reg_buf)
{
	uint8_t i;
	Set_ADS1292_Chip_Enable();
	DELAY_1MS();
	Clear_ADS1292_Chip_Enable();
	for (i = 0; i < 12; i++){
		reg_buf[i] = ADS1292_Reg_Read(i);
	}
}

static bool compare_reg_value()
{
	uint8_t i;
	for(i=0; i<8; ++i) {
	 if(ADS_reg_val[i] != ADS_register_settings[i])
		 return false;
	}

	return true;
}

uint8_t ADS1292_Init ()
{
   bool error = false;
   
   Set_GPIO();
   Set_UCB0_SPI();
   
   ADS1292_Reset();
   DELAY_200MS();
   
   ADS1292_Clock_Select(1);
   DELAY_200MS();
   
   Stop_Read_Data_Continuous();
   
   error = ADS1292_SPI_Check_Communication();
   if(error == false)
     return 1;
   
   ADS1292_Reg_Init(ADS_register_settings);
   DELAY_200MS();
   
   ADS1292_Read_All_Regs(ADS_reg_val);
   error = compare_reg_value();
   while(error == false){
      ADS1292_Reg_Init(ADS_register_settings);
      DELAY_200MS();
      
      ADS1292_Read_All_Regs(ADS_reg_val);
      error = compare_reg_value();
   }
     
   Init_ADS1292_DRDY_Interrupt();
   Disable_ADS1292_DRDY_Interrupt();
   
   Put_ADS1292_In_Sleep();

   return 0;
}

void ADS1292_enable_conversion ()
{
	Wake_Up_ADS1292();

	Soft_Start_ADS1292();
	Start_Read_Data_Continuous();
	Enable_ADS1292_DRDY_Interrupt();
}

void ADS1292_disable_conversion ()
{
	Stop_Read_Data_Continuous();
	Soft_Stop_ADS1292();
	Disable_ADS1292_DRDY_Interrupt();

	Put_ADS1292_In_Sleep();
}
