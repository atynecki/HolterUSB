/** Aplication service functions source */

#include "app_service.h"

app_data_t app_data;
general_flags_t flags;

Calendar time_data;
    
uint32_t test_counter = 0;
uint8_t sample_counter = 0;
unsigned char data_frame[DATA_FRAME_LENGTH] = {0};
unsigned char read_frame[DATA_FRAME_LENGTH] = {0};
unsigned char send_frame[DATA_SEND_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE, 0, 0, 0, 0, 0, 0, 0};
    
app_data_p app_get_data (void)
{
  return &app_data;
}

general_flags_p app_get_flags (void)
{
  return &flags;
}

void create_header_frame ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);

	data_frame[0] = time_data.DayOfMonth;
	data_frame[1] = time_data.Month;
	data_frame[2] = (uint8_t)(time_data.Year >> 8);
	data_frame[3] = (uint8_t)(time_data.Year);

	data_frame[4] = time_data.Hours;
	data_frame[5] = time_data.Minutes;
	data_frame[6] = time_data.Seconds;

	send_data_to_flash(data_frame);
}
void collect_data(unsigned char *data)
{
    time_data = RTC_A_getCalendarTime(RTC_A_BASE);
    
    data_frame[0] = ++sample_counter;
    
    //CH2 data
    data_frame[1] = data[3];
    data_frame[2] = data[4];
    data_frame[3] = data[5];
    
    data_frame[4] = time_data.Hours;
    data_frame[5] = time_data.Minutes;
    data_frame[6] = time_data.Seconds;
    
    send_data_to_flash(data_frame);
}

void visualization(void)
{
	if(app_get_flags()->device_error == 0){
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
		DELAY_1S();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
	}
	else {
		while(1){
			GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
		}
	}
}

transfer_result transfer_data ()
{
    short res;
    int i = 3;
    
    res = read_data_from_flash(read_frame);
    if(res!=0)
      return TRANSFER_READ_ERROR;

    for(i = 3; i<DATA_SEND_FRAME_LENGTH; i++){
    	send_frame[i] = read_frame[i-3];
    }

    res = cdcSendDataInBackground(send_frame, 10, CDC0_INTFNUM, 1000);
	if(res!=0)
		  return TRANSFER_SEND_ERROR;

    return TRANSFER_OK;
}
