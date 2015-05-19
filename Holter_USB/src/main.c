
#include <string.h>
#include "driverlib.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"


#include "hal.h"


void main(void)
{
    WDT_A_hold(WDT_A_BASE); //Stop watchdog timer

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2
#ifndef DRIVERLIB_LEGACY_MODE
    PMM_setVCore(PMM_CORE_LEVEL_2);
#else
    PMM_setVCore(PMM_BASE, PMM_CORE_LEVEL_2);
#endif

    initPorts();           // Config GPIOS for low-power (output low)
    initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE,TRUE);  // Init USB & events; if a host is present, connect
    initRTC();             // Start the real-time clock

    CLOCK_init();
    peripherial_init ();

    interrupt_enable();

    visualization();

    __enable_interrupt();  // Enable interrupts globally

    while (1) {
      //cdcSendDataInBackground(timeStr, 9, CDC0_INTFNUM, 1000))
    }  //while(1)
}  //main()
