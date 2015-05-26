
#ifndef __PERIPH_CONFIG_H__
#define __PERIPH_CONFIG_H__

/** Peripherals config header */


#include "ADS1292_driver.h"
#include "Flash_driver.h"
#include "app_config.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"

void CLOCK_init(void);
void initClocks(uint32_t mclkFreq);
void interrupt_enable (void);
void peripherial_init(void);

#endif