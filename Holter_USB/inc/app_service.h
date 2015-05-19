/** Aplication service functions header */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib.h"
#include "app_config.h"

#include "ff.h"
#include "diskio.h"
#include "HAL_SDCard.h"
#include "delay.h"
#include "Flash_driver.h"

/* Results of sd card function */
typedef enum {
	SD_OK = 0,		/* 0: Successful */
	SD_OPEN_ERROR,		/* 1: Open file error */
        SD_CLOSE_ERROR,		/* 2: Open file error */
	SD_WRITE_ERROR,		/* 3: Write data error */
        SD_LSEEK_ERROR          /* 4: Moves to the end of file error */
} SD_RESULT;

/* Results of data transfer function */
typedef enum {
	TRANSFER_OK = 0,		/* 0: Successful */
	TRANSFER_WRITE_ERROR,		/* 1: Write data to sd card error */
        TRANSFER_READ_ERROR		/* 2: Read data from flash error */
} TRANSFER_RESULT;

typedef struct{
  uint32_t signal_data;
} ads_data_t, *ads_data_p;

typedef struct {
  uint32_t date;
  uint32_t time_data;
} rtc_data_t, *rtc_data_p;

ads_data_p app_get_ads_data(void);
rtc_data_p app_get_rtc_data(void);
general_flags_p app_get_flags (void);

void RTC_read_time(void);
void RTC_read_date(void);
void collect_data(unsigned char *data);
void visualization(void);

SD_RESULT sd_card_init_file(void);
SD_RESULT sd_card_write_data(uint32_t ads_value, uint32_t time_value);
SD_RESULT sd_card_close_file(void);

TRANSFER_RESULT transfer_data_to_sd(void);
void terminate_transfer(TRANSFER_RESULT err_check);