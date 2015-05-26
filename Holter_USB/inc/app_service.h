
#ifndef __APP_SERVICE_H__
#define __APP_SERVICE_H__

/** Aplication service functions header */

#include "periph_config.h"

/* Results of data transfer function */
typedef enum {
	TRANSFER_OK = 0,		/* 0: Successful */
	TRANSFER_SEND_ERROR,	/* 1: Send data by USB error */
    TRANSFER_READ_ERROR		/* 2: Read data from flash error */
} transfer_result;

typedef enum{
	ADS_ERR = 0,
	USB_ERR,
	FLASH_ERR
} device_error_flags;

typedef struct{
  uint32_t signal_data;
  uint32_t date;
  uint32_t time_data;
} app_data_t, *app_data_p;

typedef struct {
  bool data_transfer;
  bool stream_enable;
  bool backup_enable;
  uint8_t device_error;
  uint8_t transfer_error;
} general_flags_t, *general_flags_p;

app_data_p app_get_data(void);
general_flags_p app_get_flags (void);

void create_header_frame(void);
void collect_data(unsigned char *data);
void visualization(void);

transfer_result transfer_data(void);

#endif
