
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/** app_config header */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driverlib.h"
#include "delay.h"

#define ELECTRODE

#define UCS_XT1_CRYSTAL_FREQUENCY               32768
#define UCS_XT2_CRYSTAL_FREQUENCY               24000000

#define SECONDS   0
#define MINUTES   0
#define HOURS     0
#define DAY       1
#define MONTH     1
#define YEAR      1970

#ifdef ELECTRODE 
  #define STATUS_MSB_BIT      0xC0
#else
  #define STATUS_MSB_BIT      0xC4
#endif

#define DATA_FRAME_LENGTH			8
#define DATA_SEND_FRAME_LENGTH		10
#endif