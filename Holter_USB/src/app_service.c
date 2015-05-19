/** Aplication service functions source */

#include "app_service.h"

ads_data_t ads_data;
rtc_data_t time;
general_flags_t flags;

Calendar time_data;

//FATFS file system
    // Result code
    FRESULT rc;
    int res;
    // File system object
    FATFS fatfs;
    // File object
    FIL fil;
    UINT bw;   
    
SD_RESULT sd_card_check;
    
uint32_t test_counter = 0;
uint8_t sample_counter = 0;
unsigned char data_frame[8] = {0};
unsigned char read_frame[8] = {0};
    
ads_data_p app_get_ads_data (void){
  return &ads_data;
}

rtc_data_p app_get_rtc_data (void){
  return &time;
}

general_flags_p app_get_flags (void){
  return &flags;
}

void RTC_read_time(void)
{
  time_data = RTC_A_getCalendarTime(RTC_A_BASE);
 
  app_get_rtc_data()->time_data = ((uint32_t)(time_data.Hours) << 16) + ((uint32_t)(time_data.Minutes) << 8) + (uint32_t)(time_data.Seconds);
}

void RTC_read_date(void)
{
  time_data = RTC_A_getCalendarTime(RTC_A_BASE);
  
  app_get_rtc_data()->date = ((uint32_t)(time_data.DayOfMonth) << 24) + ((uint32_t)(time_data.Month) << 16) + (uint32_t)(time_data.Year);
}

void collect_data(unsigned char *data)
{
    time_data = RTC_A_getCalendarTime(RTC_A_BASE);
    
    data_frame[0] = ++sample_counter;
    
    if (sample_counter == 125)
      sample_counter = 0;
    
    //CH2 data
    data_frame[1] = data[3];
    data_frame[2] = data[4];
    data_frame[3] = data[5];
    
    data_frame[4] = time_data.Hours;
    data_frame[5] = time_data.Minutes;
    data_frame[6] = time_data.Seconds;
    
    send_data_to_flash(data_frame);
}

static void calculate_data_frame(unsigned char *data)
{
  app_get_ads_data()->signal_data = ((uint32_t)(data[1]) << 16) + ((uint32_t)(data[2]) << 8) + (uint32_t)(data[3]);
  app_get_rtc_data()->time_data = ((uint32_t)(data[0]) << 24) + ((uint32_t)(data[4]) << 16) + ((uint32_t)(data[5]) << 8) + (uint32_t)(data[6]);
}
void visualization(void)
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
    DELAY_1S();
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
}

SD_RESULT sd_card_init_file(void)
{
    RTC_read_date ();
    test_counter++;
    
    // Register volume work area (never fails)
    f_mount(0, &fatfs);
    
    // Open file
    rc = f_open(&fil, "0:result.csv", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc)
      return SD_OPEN_ERROR;
    
    // Write first frame to file
    res = f_printf(&fil, "%ld,%ld",  app_get_rtc_data()->date, test_counter);
    res = f_puts("\n", &fil);

    return SD_OK; 
}

SD_RESULT sd_card_write_data(uint32_t ads_value, uint32_t time_value)
{ 
    // Write date to file
    res = f_printf(&fil, "%ld,%ld", ads_value, time_value);
    res = f_puts("\n", &fil);
    if(res < 1)
      return SD_WRITE_ERROR;
    
    return SD_OK;
}

SD_RESULT sd_card_close_file(void)
{
    rc = f_close(&fil);
    if (rc)
      return SD_CLOSE_ERROR;
    
    return SD_OK;
}

TRANSFER_RESULT transfer_data_to_sd()
{
    short res;
    
    res = read_data_from_flash(read_frame);
    if(res!=0)
      return TRANSFER_READ_ERROR;
    calculate_data_frame(read_frame);
 
    sd_card_check = sd_card_write_data (app_get_ads_data()->signal_data, app_get_rtc_data()->time_data);
    if(sd_card_check!=0)
      return TRANSFER_WRITE_ERROR;
    return TRANSFER_OK;
}

void terminate_transfer(TRANSFER_RESULT err_check)
{
  bool check_value;
  check_value = compare_address();
  if ((check_value == true) || (err_check != TRANSFER_OK)){
      sd_card_check = sd_card_close_file();
      if (sd_card_check == SD_OK){
          GPIO_setOutputLowOnPin (GPIO_PORT_P5,GPIO_PIN1);
          app_get_flags()->sd_data_send = false;
      }
  }
}