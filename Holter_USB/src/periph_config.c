
/**
 * @file
 * @brief peripherals config source
 */

#include "periph_config.h"
#include "app_service.h"

void initClocks ()
{
	UCS_clockSignalInit(
			UCS_FLLREF,
			UCS_REFOCLK_SELECT,
			UCS_CLOCK_DIVIDER_1);

	UCS_clockSignalInit(
			UCS_ACLK,
			UCS_REFOCLK_SELECT,
			UCS_CLOCK_DIVIDER_1);

	UCS_initFLLSettle(
			USB_MCLK_FREQ / 1000,
			USB_MCLK_FREQ / 32768);
}

static void GPIO_init ()
{
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_ALL);

	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_ALL);

	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_ALL);

	GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_ALL);

	GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN1);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);

	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN7);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN7,GPIO_HIGH_TO_LOW_TRANSITION);

    GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN7);
}

static void calender_init ()
{
	app_get_data()->actual_time.Seconds = 0;
	app_get_data()->actual_time.Minutes = 0;
	app_get_data()->actual_time.Hours = 0;
	app_get_data()->actual_time.DayOfMonth = 1;
	app_get_data()->actual_time.Month = 1;
	app_get_data()->actual_time.Year = 1970;
}

static void RTC_init ()
{
    RTC_A_calendarInit(RTC_A_BASE, app_get_data()->actual_time, RTC_A_FORMAT_BINARY);
    RTC_A_setCalendarEvent(RTC_A_BASE, RTC_A_CALENDAREVENT_MINUTECHANGE);
    RTC_A_clearInterrupt(RTC_A_BASE, RTCRDYIFG + RTCTEVIFG + RTCAIFG);
    RTC_A_enableInterrupt(RTC_A_BASE, RTCTEVIE);
    RTC_A_startClock(RTC_A_BASE);
}

void set_calender_time ()
{
	RTC_A_holdClock(RTC_A_BASE);
	RTC_init();
}

uint8_t flash_test[PACKET_FRAME_LENGTH] = {0};
void peripherial_init () 
{
	volatile uint8_t error_check;
	struct FlashAddress tmp_addres;
	tmp_addres.usColNum = 0;
	tmp_addres.ucPageNum = 0;
	tmp_addres.usBlockNum = 0;

	GPIO_init();

	calender_init();
    RTC_init();

    error_check = ADS1292_Init();
    if(error_check == 1)
    	app_get_flags()->device_error |= 1 << ADS_ERR;

    error_check = flash_init();
    if(error_check != 0)
        	app_get_flags()->device_error |= 1 << FLASH_ERR;
    flash_read_page(tmp_addres, 64, flash_test);

    error_check = USB_setup(TRUE,TRUE);
	if(error_check != 0)
			app_get_flags()->device_error |= 1 << USB_ERR;
}

void visualization ()
{
	if(app_get_flags()->device_error == 0){
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
		DELAY_1S();
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
		GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
	}
	else {
		uint8_t counter;
		for(counter = 0; counter < 10; counter++){
			GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
			DELAY_1S();
		}
	}
}
