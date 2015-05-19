/** Peripherals config header */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib.h"
#include "ADS1292_driver.h"
#include "Flash_driver.h"
#include "app_config.h"
#include "delay.h"

void CLOCK_init ();
void interrupt_enable (void);
void peripherial_init ();