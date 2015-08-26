
#ifndef __PERIPH_CONFIG_H__
#define __PERIPH_CONFIG_H__

/**
 * @file
 * @brief peripherals config header
 */

#include "ADS1292_driver.h"
#include "Flash_driver.h"
#include "app_config.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"

#define GPIO_ALL        GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7

void initClocks(void);
void set_calender_time(void);
void peripherial_init(void);
void visualization(void);

#endif
