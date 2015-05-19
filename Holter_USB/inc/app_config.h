/** app_config header */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
  bool ADS_ready;
  bool RTC_ready;
  bool hardware_ready;
  bool data_ready;
  bool sd_data_send;
} general_flags_t, *general_flags_p;


#endif