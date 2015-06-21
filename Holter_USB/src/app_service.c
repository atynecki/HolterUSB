/** Aplication service functions source */

#include "app_service.h"

app_data_t app_data;
general_flags_t flags;

Calendar time_data;
uint32_t timestamp;

uint8_t sample_counter = 0;
extern uint8_t data_frame[DATA_FRAME_LENGTH] = {0};
extern uint8_t send_frame[DATA_SEND_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE, 0, 0, 0, 0, 0, 0, 0, 0};
    
app_data_p app_get_data ()
{
  return &app_data;
}

general_flags_p app_get_flags ()
{
  return &flags;
}

void general_flag_init ()
{
	app_get_flags()->device_run = false;

	app_get_flags()->stream_start = false;
	app_get_flags()->stream_enable = false;
	app_get_flags()->backup_start = false;
	app_get_flags()->backup_enable = false;
	app_get_flags()->data_transfer = false;

	app_get_flags()->data_ready = false;
}

static void create_send_frame (uint8_t * frame)
{
	int i = 3;
	for(i = 3; i<DATA_SEND_FRAME_LENGTH; i++){
		send_frame[i] = frame[i-3];
	 }
}

void create_header_frame ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);

	data_frame[0] = HEADER_FRAME_FLAG;
	data_frame[1] = time_data.DayOfMonth;
	data_frame[2] = time_data.Month;
	data_frame[3] = (uint8_t)(time_data.Year >> 8);
	data_frame[4] = (uint8_t)(time_data.Year);

	data_frame[5] = time_data.Hours;
	data_frame[6] = time_data.Minutes;
	data_frame[7] = time_data.Seconds;

	if(app_get_flags()->backup_enable == true)
		send_data_to_flash(data_frame);
	 if (app_get_flags()->stream_enable == true){
		create_send_frame(data_frame);
		cdcSendDataInBackground(send_frame, 11, CDC0_INTFNUM, 1000);
	}
}

void collect_data(unsigned char *data)
{
    //CH2 data
    data_frame[2] = data[3];
    data_frame[3] = data[4];
    data_frame[4] = data[5];
    
    app_get_flags()->data_ready = true;
}

void send_data ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);
	timestamp = time_data.Hours*3600 + time_data.Minutes*60 + time_data.Seconds;

	data_frame[0] = DATA_FRAME_FLAG;

	data_frame[1] = time_data.DayOfMonth;

	data_frame[5] = (uint8_t)(timestamp >> 16);
	data_frame[6] = (uint8_t)(timestamp >> 8);
	data_frame[7] = (uint8_t)(timestamp);

	if(app_get_flags()->backup_enable == true)
		send_data_to_flash(data_frame);
	if (app_get_flags()->stream_enable == true){
		create_send_frame(data_frame);
		cdcSendDataInBackground(send_frame, 11, CDC0_INTFNUM, 1000);
	}
}

void parse_command (uint8_t* data_buff)
{
	if(data_buff[0] == 0xEF && data_buff[1] == 0xFE){
		if(data_buff[2] == STREAM_COMMAND){
			app_get_flags()->stream_start = data_buff[3];
			if(app_get_flags()->stream_start == false){
				GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
				DELAY_1S();
				GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
		}
		else if (data_buff[2] == SAVE_DATA_COMMAND){
			app_get_flags()->backup_start = data_buff[3];
			if(app_get_flags()->backup_start == false){
				GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
		}
		else if (data_buff[2] == DOWNLOAD_DATA_COMMAND){
			app_get_flags()->data_transfer = data_buff[3];
			//TODO zatrzymanie wszystkich pomiarów
		}

		else if (data_buff[2] == ERASE_FLASH_COMMAND){
			app_get_flags()->erase_flash = data_buff[3];
		}

		else if(data_buff[2] == TIME_RECEIVED_COMMAND){
			Calendar current_time;
			current_time.Seconds = data_buff[3];
			current_time.Minutes = data_buff[4];
			current_time.Hours = data_buff[5];
			current_time.DayOfMonth = data_buff[6];
			current_time.Month = data_buff[7];
			current_time.Year = data_buff[8];

			set_calender_time(current_time);
		}
	}
}

transfer_result transfer_data ()
{
    short res;
    
    res = read_data_from_flash(data_frame);
    if(res!=0)
      return TRANSFER_READ_ERROR;

    create_send_frame(data_frame);

    res = cdcSendDataInBackground(send_frame, 11, CDC0_INTFNUM, 1000);
	if(res!=0)
		  return TRANSFER_SEND_ERROR;

    return TRANSFER_OK;
}

void power_manage ()
{

}
