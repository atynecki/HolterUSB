
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/** app_config header */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driverlib.h"
#include "delay.h"

#define USB_MCLK_FREQ 				8000000        // MCLK frequency of MCU, in Hz

#define STATUS_MSB_BIT      		0xC0

#define RUN_SWITCH_DELAY_MS			2000
#define PACKET_FRAME_LENGTH			64
#define STATE_FRAME_LENGTH			15
#define PACKET_SEND_FRAME_LENGTH	67
#define TIME_SEND_FRAME_LENGTH		11

#define DATA_FRAME_FLAG				0
#define STATE_FRAME_FLAG			1
#define START_TIME_FLAG				2
#define STOP_TIME_FLAG				3
#define TRANSFER_END_FLAG			4

#endif
