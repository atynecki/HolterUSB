/* Interrupt handlers source file */

#include "app_service.h"

uint8_t run_key_state = 0;
uint8_t stream_key_state = 0;
unsigned char spi_rx_count=0;
unsigned char data_check[3] = {0};
unsigned char spi_rx_buf[6] = {0};

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
    if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN7))
    {
      GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN7);
      
      if (run_key_state == 0){
        run_key_state = 1;
       
        clear_write_address();
        erase_flash();
      
        create_header_frame();
        GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
        enable_ADS1x9x_Conversion ();
      }
      
      else {
        run_key_state = 0;
        disable_ADS1x9x_Conversion ();
        
        GPIO_setOutputLowOnPin (GPIO_PORT_P5,GPIO_PIN0);
      }       
    }
    
   else if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN1)) {
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
   
     while ((data_check[0] != STATUS_MSB_BIT) || (data_check[1] != 0) || (data_check[2] != 0)) {
        data_check[0] = data_check[1];
        data_check[1] = data_check[2];
        data_check[2] = UCB0RXBUF;
        UCB0TXBUF = 0;
     }
     
     data_check[0] = 0;
     
     UCB0IE |= UCRXIE;  // Enable USCI_B0 RX interrupt
   }
   
   else if (GPIO_getInterruptStatus(GPIO_PORT_P1,GPIO_PIN6)) {
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN6);
     if (stream_key_state == 0){
    	 	 GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
    	 	 //TODO add live stream
     }
     else {
        stream_key_state = 0;
        GPIO_setOutputLowOnPin (GPIO_PORT_P5,GPIO_PIN1);
     }
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
                // If the CPU accesses USB memory while the USB module is
                // suspended, a "bus error" can occur.  This generates an NMI.  If
                // USB is automatically disconnecting in your software, set a
                // breakpoint here and see if execution hits it.  See the
                // Programmer's Guide for more information.
                SYSBERRIV = 0;  // Clear bus error flag
                USB_disable();  // Disable
        }
}
