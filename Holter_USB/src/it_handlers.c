/* Interrupt handlers source file */

#include "app_service.h"

uint8_t run_key_state;
uint16_t switch_counter;
unsigned char spi_rx_count=0;
unsigned char data_check[3] = {0};
unsigned char spi_rx_buf[6] = {0};

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
    if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN7))
    {
      GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN7);

      for(switch_counter = 0; switch_counter < RUN_SWITCH_DELAY_MS; switch_counter++){
    	  DELAY_1MS();
    	  run_key_state = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7);
    	  if(run_key_state == 0){
    		  if(switch_counter >=1000){
    			  if (app_get_flags()->device_run == false){
    				  app_get_flags()->device_run = true;

    				  enable_ADS1x9x_Conversion ();
    				  GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
				  }

				  else if (app_get_flags()->device_run == true){
					  app_get_flags()->device_run = false;

					  disable_ADS1x9x_Conversion ();
					  GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
				  }
    			  return;
    		  }
    	  }
      }
    }
    
   else if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN1)) {
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
   
     while ((data_check[0] != STATUS_MSB_BIT) || (data_check[1] != 0) || (data_check[2] != 0)) {
        data_check[0] = (data_check[1] & STATUS_MSB_BIT);
        data_check[1] = data_check[2];
        data_check[2] = UCB0RXBUF;
        UCB0TXBUF = 0;
     }
     
     data_check[0] = 0;
     
     UCB0IE |= UCRXIE;  // Enable USCI_B0 RX interrupt
   }
   
   else if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN6)) {

   }
}

#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	  if (__even_in_range(UCB0IV, 4) == 2) {   //Vector 2 - RXIFG
			  //USCI_A0 TX buffer ready?
		while (!USCI_B_SPI_getInterruptStatus(USCI_B0_BASE,
											  USCI_B_SPI_TRANSMIT_INTERRUPT)) ;
		spi_rx_buf[spi_rx_count++] =UCB0RXBUF;
		UCB0TXBUF = 0;

		if (spi_rx_count == 6) {
			spi_rx_count = 0;
			collect_data(spi_rx_buf);

			UCB0IE &= ~UCRXIE;               // Disable USCI_B0 RX interrupt
		}
	  }
}


#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
{
        switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG )) {
        case SYSUNIV_NONE:
                __no_operation();
                break;
        case SYSUNIV_NMIIFG:
                __no_operation();
                break;
        case SYSUNIV_OFIFG:
#ifndef DRIVERLIB_LEGACY_MODE
                UCS_clearFaultFlag(UCS_XT2OFFG);
                UCS_clearFaultFlag(UCS_DCOFFG);
                SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
#else
                UCS_clearFaultFlag(UCS_BASE, UCS_XT2OFFG);
                UCS_clearFaultFlag(UCS_BASE, UCS_DCOFFG);
                SFR_clearInterrupt(SFR_BASE, SFR_OSCILLATOR_FAULT_INTERRUPT);

#endif
                break;
        case SYSUNIV_ACCVIFG:
                __no_operation();
                break;
        case SYSUNIV_BUSIFG:
                SYSBERRIV = 0;  // Clear bus error flag
                USB_disable();  // Disable
        }
}
