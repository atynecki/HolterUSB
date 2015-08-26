
#ifndef __DELAY_H__
#define __DELAY_H__

/**
 * @file
 * @brief system delay header
 */

#include "msp430f5529.h"
#include "app_config.h"

#define DELAY_1MS()   __delay_cycles(USB_MCLK_FREQ/1000)
#define DELAY_2MS()   __delay_cycles(USB_MCLK_FREQ/500)
#define DELAY_10MS()   __delay_cycles(USB_MCLK_FREQ/100)
#define DELAY_20MS()   __delay_cycles(USB_MCLK_FREQ/50github)
#define DELAY_100MS() __delay_cycles(USB_MCLK_FREQ/10)
#define DELAY_200MS() __delay_cycles(USB_MCLK_FREQ/5)
#define DELAY_1S()    __delay_cycles(USB_MCLK_FREQ)

#endif
