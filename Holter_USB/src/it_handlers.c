/* Interrupt handlers source file */

#include "app_service.h"

uint8_t run_key_state;
uint16_t switch_counter;
uint8_t dummy_read;
uint8_t spi_rx_count=0;
uint8_t spi_rx_buf[9];
extern uint8_t packet_frame[PACKET_FRAME_LENGTH];

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
    				  conversion_start();
				  }

				  else if (app_get_flags()->device_run == true){
					  conversion_stop();
				  }
    			  return;
    		  }
    	  }
      }
    }
    
   else if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN1)) {
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);

     dummy_read = UCB0RXBUF; 		// Dummy Read
     UCB0TXBUF = 0;
     
     UCB0IE |= UCRXIE;  // Enable USCI_B0 RX interrupt
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

		if (spi_rx_count == 9) {
			spi_rx_count = 0;
			if((spi_rx_buf[0] & STATUS_MSB_BIT) && (spi_rx_buf[1] == 0) && (spi_rx_buf[2] == 0)){
				put_data_to_packet(spi_rx_buf);
			}

			UCB0IE &= ~UCRXIE;               // Disable USCI_B0 RX interrupt
		}
	  }
}

volatile Calendar actualTime;
#pragma vector=RTC_VECTOR
__interrupt void RTC_A_ISR(void)
{
    switch(__even_in_range(RTCIV,16))
    {
    case 0: break;      //No interrupts
    case 2: break;      //RTCRDYIFG
    case 4:             //RTCEVIFG
    	actualTime = RTC_A_getCalendarTime(RTC_A_BASE);
    	packet_frame[2] = (uint8_t)(actualTime.Hours);
    	packet_frame[3] = (uint8_t)(actualTime.Minutes);
        break;
    case 6: break;      //RTCAIFG
    case 8: break;      //RT0PSIFG
    case 10: break;     //RT1PSIFG
    case 12: break;     //Reserved
    case 14: break;     //Reserved
    case 16: break;     //Reserved
    default: break;
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
