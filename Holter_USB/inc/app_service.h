
#ifndef __APP_SERVICE_H__
#define __APP_SERVICE_H__

/**
 * @file
 * @brief aplication service functions header
 */

#include "periph_config.h"

/* Application data */
typedef struct{
	struct Calendar actual_time;
} app_data_t, *app_data_p;

/* Application general flags */
typedef struct {
	bool packet_data_ready;
	bool data_transfer;
	bool data_send_next;
	bool stream_enable;
	bool stream_start;
	bool stream_stop;
	bool backup_enable;
	bool backup_start;
	bool backup_stop;
	bool erase_flash;
	bool device_run;
	uint8_t device_error;
} general_flags_t, *general_flags_p;

app_data_p app_get_data(void);
general_flags_p app_get_flags(void);

void general_flag_clear(void);

void conversion_start(void);
void conversion_stop(void);

void clear_write_address(void);
void clear_read_address(void);
void send_data_to_flash(uint8_t *data_frame);
uint8_t read_data_from_flash(uint8_t *data_frame);

void put_data_to_packet(uint8_t *data);

void set_exam_start_time(void);
void set_exam_stop_time(void);

void send_state(void);
void send_data_packet(void);

void transfer_data(void);

void parse_command (uint8_t* data_buff);

void system_standby(void);

#endif
