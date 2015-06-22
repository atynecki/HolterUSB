
#ifndef __APP_SERVICE_H__
#define __APP_SERVICE_H__

/** Aplication service functions header */

#include "periph_config.h"

/* Host command */
#define STREAM_COMMAND				1
#define SAVE_DATA_COMMAND			2
#define DOWNLOAD_DATA_COMMAND		3
#define ERASE_FLASH_COMMAND			4
#define TIME_RECEIVED_COMMAND		5

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
	uint32_t timestamp;
	struct Calendar exam_start;
	struct Calendar exam_end;
	struct FlashAddress tail_address;
} app_data_t, *app_data_p;

typedef struct {
  bool data_transfer;
  bool stream_enable;
  bool stream_start;
  bool stream_stop;
  bool backup_enable;
  bool backup_start;
  bool backup_stop;
  bool erase_flash;
  bool device_run;
  bool data_ready;
  uint8_t device_error;
} general_flags_t, *general_flags_p;

app_data_p app_get_data(void);
general_flags_p app_get_flags (void);

void general_flag_init(void);

void create_header_frame(void);
void collect_data(unsigned char *data);
void set_exam_start_time(void);
void set_exam_stop_time(void);
void send_data(void);

void parse_command (uint8_t* data_buff);

void transfer_data(void);

void power_manage(void);
#endif
