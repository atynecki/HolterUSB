/** Aplication service functions source */

#include "app_service.h"

app_data_t app_data;
general_flags_t flags;

Calendar time_data;

struct FlashAddress Write_Flash_Address;
struct FlashAddress Read_Flash_Address;

uint8_t packet_tail;
uint8_t header_frame[PACKET_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE};
uint8_t packet_frame[PACKET_FRAME_LENGTH] = {0,0,0,0};
uint8_t data_send_frame[PACKET_SEND_FRAME_LENGTH] = {0xA5, 0x5A, 0xFE};
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
	app_get_flags()->data_send_next = false;
	app_get_flags()->erase_flash = false;

	app_get_flags()->device_error = 0;
}

void conversion_start ()
{
	app_get_flags()->device_run = true;

	if(app_get_flags()->backup_enable == true){
		set_exam_start_time();
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
	}
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

	if(app_get_flags()->backup_enable == true){
		set_exam_stop_time();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
	}
	else {
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		DELAY_200MS();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
	}

	send_state();
}

void clear_write_address()
{
    Write_Flash_Address.ucPageNum = 0;
    Write_Flash_Address.usBlockNum = 0;
    Write_Flash_Address.usColNum = 0;
}

void clear_read_address()
{
    Read_Flash_Address.ucPageNum = 0;
    Read_Flash_Address.usBlockNum = 0;
    Read_Flash_Address.usColNum = 0;
}

bool compare_address()
{
  if(Read_Flash_Address.usBlockNum < Write_Flash_Address.usBlockNum)
    return true;
  else if (Read_Flash_Address.usBlockNum == Write_Flash_Address.usBlockNum){
    if(Read_Flash_Address.ucPageNum < Write_Flash_Address.ucPageNum)
      return true;
    else if (Read_Flash_Address.ucPageNum == Write_Flash_Address.ucPageNum){
      if(Read_Flash_Address.usColNum < Write_Flash_Address.usColNum)
         return true;
      else
        return false;
    }
    else
      return false;
  }
  else
    return false;
}

void send_data_to_flash(unsigned char *data_frame)
{
	if (Write_Flash_Address.usColNum == 0) {
		flash_program_page_start(Write_Flash_Address,64,data_frame);
	}
	else {
		flash_program_page_next(Write_Flash_Address.usColNum,64,data_frame);
	}

	Write_Flash_Address.usColNum +=64;

	if (Write_Flash_Address.usColNum == PAGE_SIZE) {
		Write_Flash_Address.usColNum=0;
		Write_Flash_Address.ucPageNum++;

		if (Write_Flash_Address.ucPageNum == PAGES_PER_BLOCK) {
			Write_Flash_Address.ucPageNum = 0;
			Write_Flash_Address.usBlockNum++;

			if (Write_Flash_Address.usBlockNum == MAX_BLOCKS) {
				Write_Flash_Address.usBlockNum = MAX_BLOCKS-1;
				Write_Flash_Address.ucPageNum = PAGES_PER_BLOCK-1;
			}
		}

		flash_program_page_last();
      }
}

short read_data_from_flash(unsigned char *data_frame)
{
	if(Read_Flash_Address.usColNum == 0){
		flash_read_page_start(Read_Flash_Address,64,data_frame);
	}
	else {
		flash_read_page_next(Read_Flash_Address.usColNum,64,data_frame);
	}

	Read_Flash_Address.usColNum += 64;
	if ( Read_Flash_Address.usColNum == PAGE_SIZE) {
		Read_Flash_Address.usColNum = 0;
		Read_Flash_Address.ucPageNum++;

		if (Read_Flash_Address.ucPageNum == PAGES_PER_BLOCK) {
			Read_Flash_Address.ucPageNum =0;
			Read_Flash_Address.usBlockNum++;

			if (Read_Flash_Address.usBlockNum == MAX_BLOCKS) {
				return NAND_MEMORY_END;
			 }
		  }
	}
	return NAND_IO_RC_PASS;
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

static void clean_header_frame()
{
	int i;
	for(i=3; i<PACKET_FRAME_LENGTH; i++)
		header_frame[i] = 0xFF;
}
void set_exam_start_time ()
{
	clean_header_frame();

	time_data = RTC_A_getCalendarTime(RTC_A_BASE);

	header_frame[3] = HEADER_TIME_FLAG;
	header_frame[4] = time_data.DayOfMonth;
	header_frame[5] = time_data.Month;
	header_frame[6] = (uint8_t)(time_data.Year >> 8);
	header_frame[7] = (uint8_t)(time_data.Year);

	header_frame[8] = time_data.Hours;
	header_frame[9] = time_data.Minutes;
	header_frame[10] = time_data.Seconds;

	send_data_to_flash(header_frame);
}

void set_exam_stop_time ()
{
	time_data = RTC_A_getCalendarTime(RTC_A_BASE);

	struct FlashAddress tmp_addres;
	tmp_addres.usColNum = 0;
	tmp_addres.ucPageNum = 0;
	tmp_addres.usBlockNum = 0;

	flash_read_page(tmp_addres,64,header_frame);

	header_frame[11] = time_data.DayOfMonth;
	header_frame[12] = time_data.Month;
	header_frame[13] = (uint8_t)(time_data.Year >> 8);
	header_frame[14] = (uint8_t)(time_data.Year);

	header_frame[15] = time_data.Hours;
	header_frame[16] = time_data.Minutes;
	header_frame[17] = time_data.Seconds;

	flash_program_page(tmp_addres,64,header_frame);
}

static void create_send_frame (uint8_t* frame)
{
	int i = 3;
	frame[0] = DATA_FRAME_FLAG;
	for(i = 3; i<PACKET_SEND_FRAME_LENGTH; i++){
		data_send_frame[i] = frame[i-3];
	}
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

uint8_t data_transfer_timeout;
void transfer_data ()
{
	read_data_from_flash(header_frame);
	cdcSendDataInBackground(header_frame, PACKET_FRAME_LENGTH, CDC0_INTFNUM, 1000);
	DELAY_10MS();

	data_transfer_timeout = 0;
	while(read_flash_data()) {
		create_send_frame(packet_frame);
		app_get_flags()->data_send_next = false;
		cdcSendDataInBackground(data_send_frame, PACKET_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);
		while(!app_get_flags()->data_send_next){
			data_transfer_timeout++;
			if(data_transfer_timeout == 80000){
				data_transfer_timeout = 0;
				cdcSendDataInBackground(data_send_frame, PACKET_SEND_FRAME_LENGTH, CDC0_INTFNUM, 1000);
			}
		}
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
			send_state();
		}

		else if(data_buff[2] == GET_STATE_COMMAND){
			send_state();
		}

		else if(data_buff[2] == SEND_NEXT_COMMAND){
			app_get_flags()->data_send_next = true;
		}
	}
}

void power_manage ()
{
	__bis_SR_register(LPM4_bits + GIE);
}
