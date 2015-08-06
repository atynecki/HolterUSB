/** Aplication service functions source */

#include "app_service.h"

app_data_t app_data;
general_flags_t flags;

Calendar time_data;

uint8_t packet_tail;
uint8_t packet_frame[PACKET_FRAME_LENGTH] = {0,0,0,0};
uint8_t data_send_frame[PACKET_SEND_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE};
uint8_t time_send_frame[TIME_SEND_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE};
uint8_t state_send_frame[STATE_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE};

app_data_p app_get_data ()
{
  return &app_data;
}

general_flags_p app_get_flags ()
{
  return &flags;
}

void general_flag_clear ()
{
	app_get_flags()->packet_data_ready = false;

	app_get_flags()->device_run = false;

	app_get_flags()->stream_start = false;
	app_get_flags()->stream_enable = false;
	app_get_flags()->stream_stop = false;
	app_get_flags()->backup_start = false;
	app_get_flags()->backup_enable = false;
	app_get_flags()->backup_stop = false;
	app_get_flags()->data_transfer = false;
	app_get_flags()->erase_flash = false;

	app_get_flags()->device_error = 0;
}

void conversion_start ()
{
	app_get_flags()->device_run = true;

	if(app_get_flags()->backup_enable == true)
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);

	else {
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		DELAY_200MS();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
		DELAY_200MS();
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		DELAY_200MS();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
	}

	enable_ADS1x9x_Conversion ();
	packet_tail = 4;
	send_state();
}

void conversion_stop ()
{
	app_get_flags()->device_run = false;
	disable_ADS1x9x_Conversion();

	if(app_get_flags()->backup_enable == true)
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
	else {
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		DELAY_200MS();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
	}

	send_state();
}

void put_data_to_packet(uint8_t *data)
{
	packet_frame[packet_tail++] = data[6];
	packet_frame[packet_tail++] = data[7];

	if(packet_tail == PACKET_FRAME_LENGTH) {
		packet_tail = 4;
		app_get_flags()->packet_data_ready = true;
	}
}

void set_exam_start_time ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);
	app_get_data()->exam_start.Seconds = time_data.Seconds;
	app_get_data()->exam_start.Minutes = time_data.Minutes;
	app_get_data()->exam_start.Hours = time_data.Hours;

	app_get_data()->exam_start.DayOfMonth = time_data.DayOfMonth;
	app_get_data()->exam_start.Month = time_data.Month;
	app_get_data()->exam_start.Year = time_data.Year;
}

void set_exam_stop_time ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);
	app_get_data()->exam_end.Seconds = time_data.Seconds;
	app_get_data()->exam_end.Minutes = time_data.Minutes;
	app_get_data()->exam_end.Hours = time_data.Hours;

	app_get_data()->exam_end.DayOfMonth = time_data.DayOfMonth;
	app_get_data()->exam_end.Month = time_data.Month;
	app_get_data()->exam_end.Year = time_data.Year;
}

static void create_send_frame (uint8_t* frame)
{
	int i = 3;
	frame[0] = DATA_FRAME_FLAG;
	for(i = 3; i<PACKET_SEND_FRAME_LENGTH; i++){
		data_send_frame[i] = frame[i-3];
	}
}

static void create_time_frame (uint8_t index, struct Calendar time)
{
	time_send_frame[3] = index;
	time_send_frame[4] = time.DayOfMonth;
	time_send_frame[5] = time.Month;
	time_send_frame[6] = (uint8_t)(time.Year >> 8);
	time_send_frame[7] = (uint8_t)(time.Year);

	time_send_frame[8] = time.Hours;
	time_send_frame[9] = time.Minutes;
	time_send_frame[10] = time.Seconds;
}

void send_state ()
{
	state_send_frame[3] = STATE_FRAME_FLAG;

	Calendar time = RTC_A_getCalendarTime(RTC_A_BASE);

	state_send_frame[4] = app_get_flags()->stream_enable;
	state_send_frame[5] = app_get_flags()->device_run;
	state_send_frame[6] = app_get_flags()->backup_enable;
	state_send_frame[7] = app_get_flags()->device_error;

	state_send_frame[8] = time.DayOfMonth;
	state_send_frame[9] = time.Month;
	state_send_frame[10] = (uint8_t)(time.Year >> 8);
	state_send_frame[11] = (uint8_t)(time.Year);

	state_send_frame[12] = time.Hours;
	state_send_frame[13] = time.Minutes;
	state_send_frame[14] = time.Seconds;

	cdcSendDataInBackground(state_send_frame, STATE_FRAME_LENGTH, CDC0_INTFNUM, 1000);
}

void send_data_packet ()
{
	packet_frame[2] = app_get_data()->actual_time.Hours;
	packet_frame[3] = app_get_data()->actual_time.Minutes;
	if(app_get_flags()->backup_enable == true)
		send_data_to_flash(packet_frame);
	if (app_get_flags()->stream_enable == true){
		create_send_frame(packet_frame);
		cdcSendDataInBackground(data_send_frame, PACKET_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);
	}
}
static void send_transfer_end_frame ()
{
	int i;
	state_send_frame[3] = TRANSFER_END_FLAG;
	for (i = 4; i < STATE_FRAME_LENGTH; i++)
		state_send_frame[i] = 0;

	cdcSendDataInBackground(state_send_frame, STATE_FRAME_LENGTH, CDC0_INTFNUM, 1000);
}

static bool read_flash_data ()
{
	read_data_from_flash(packet_frame);

	return compare_address();
}

void transfer_data ()
{
	create_time_frame(START_TIME_FLAG, app_get_data()->exam_start);
	cdcSendDataInBackground(time_send_frame, TIME_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);

	create_time_frame(STOP_TIME_FLAG, app_get_data()->exam_end);
	cdcSendDataInBackground(time_send_frame, TIME_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);

	while(read_flash_data()) {
		create_send_frame(packet_frame);
		cdcSendDataInBackground(data_send_frame, PACKET_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);
		DELAY_10MS();
	}

	send_transfer_end_frame();
}

void parse_command (uint8_t* data_buff)
{
	if(data_buff[0] == 0xEF && data_buff[1] == 0xFE){
		if(data_buff[2] == STREAM_COMMAND){
			if(data_buff[3] == 1)
				app_get_flags()->stream_start = true;
			else
				app_get_flags()->stream_stop = true;
		}

		else if (data_buff[2] == SAVE_DATA_COMMAND){
			if(data_buff[3] == 1)
				app_get_flags()->backup_start = true;
			else
				app_get_flags()->backup_stop = true;
		}

		else if (data_buff[2] == DOWNLOAD_DATA_COMMAND){
			if(data_buff[3] == 1)
				app_get_flags()->data_transfer= true;
		}

		else if (data_buff[2] == ERASE_FLASH_COMMAND){
			if(data_buff[3] == 1)
				app_get_flags()->erase_flash = true;
		}

		else if(data_buff[2] == TIME_RECEIVED_COMMAND){
			app_get_data()->actual_time.Seconds = data_buff[3];
			app_get_data()->actual_time.Minutes = data_buff[4];
			app_get_data()->actual_time.Hours = data_buff[5];
			app_get_data()->actual_time.DayOfMonth = data_buff[6];
			app_get_data()->actual_time.Month = data_buff[7];
			int year = (((uint16_t)(data_buff[8])) << 8) | ((uint16_t)(data_buff[9]));
			app_get_data()->actual_time.Year = year;

			set_calender_time();
		}

		else if(data_buff[2] == GET_STATE_COMMAND){
			send_state();
		}
	}
}

void power_manage ()
{
	__bis_SR_register(LPM4_bits + GIE);
}
