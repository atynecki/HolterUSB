
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/**
 * @file
 * @brief application config value header
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driverlib.h"
#include "delay.h"

#define USB_MCLK_FREQ 				8000000        // MCLK frequency of MCU, in Hz

#define STATUS_MSB_BIT      		0xC0

#define RUN_SWITCH_DELAY_MS			2000

/* Frame length */
#define PACKET_FRAME_LENGTH			64
#define PACKET_SEND_FRAME_LENGTH	67
#define STATE_FRAME_LENGTH			15

/* Frame flags */
#define DATA_FRAME_FLAG				0
#define STATE_FRAME_FLAG			1
#define HEADER_TIME_FLAG			2
#define TRANSFER_END_FLAG			3

/* Host command */
#define STREAM_COMMAND				1
#define SAVE_DATA_COMMAND			2
#define DOWNLOAD_DATA_COMMAND		3
#define ERASE_FLASH_COMMAND			4
#define TIME_RECEIVED_COMMAND		5
#define GET_STATE_COMMAND			6
#define SEND_NEXT_COMMAND			7

/* Peripherials error flags */
typedef enum{
	ADS_ERR = 0,
	USB_ERR,
	FLASH_ERR
} device_error_flags;

#endif
