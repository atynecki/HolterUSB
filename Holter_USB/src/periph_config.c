/** Peripherals configuration source */

#include "periph_config.h"
#include "app_service.h"

Calendar my_calendar;

void initClocks(uint32_t mclkFreq)
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
			mclkFreq / 1000,
			mclkFreq / 32768);
}

static void GPIO_init ()
{
	GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN1);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
        
	//freq check pin
	GPIO_setAsOutputPin(GPIO_PORT_P7,GPIO_PIN6);
	GPIO_setOutputHighOnPin(GPIO_PORT_P7,GPIO_PIN6);
	//GPIO_toggleOutputOnPin(GPIO_PORT_P7,GPIO_PIN6); //check freq

	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN6);
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN7);
        
    GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN6,GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN7,GPIO_HIGH_TO_LOW_TRANSITION);
       
}

static void calender_init ()
{
    my_calendar.Seconds = SECONDS;
    my_calendar.Minutes = MINUTES;
    my_calendar.Hours = HOURS;
    my_calendar.DayOfMonth = DAY;
    my_calendar.Month = MONTH;
    my_calendar.Year = YEAR;
}

void set_calender_time(Calendar time)
{
	RTC_A_holdClock(RTC_A_BASE);

	my_calendar.Seconds = time.Seconds;
	my_calendar.Minutes = time.Minutes;
	my_calendar.Hours = time.Hours;
	my_calendar.DayOfMonth = time.DayOfMonth;
	my_calendar.Month = time.Month;
	my_calendar.Year = time.Year;

	RTC_A_calendarInit(RTC_A_BASE, my_calendar, RTC_A_FORMAT_BINARY);
	RTC_A_startClock(RTC_A_BASE);
}

static void RTC_init ()
{
    calender_init();
    RTC_A_calendarInit(RTC_A_BASE, my_calendar, RTC_A_FORMAT_BINARY);
    RTC_A_startClock(RTC_A_BASE);
}


void peripherial_init () 
{
	volatile uint8_t error_check;
    GPIO_init();
    RTC_init();

    error_check = ADS1x9x_Init();
    if(error_check == 1)
    	app_get_flags()->device_error |= 1 << ADS_ERR;

    error_check = flash_init();
    if(error_check != 0)
        	app_get_flags()->device_error |= 1 << FLASH_ERR;

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

void interrupt_enable ()
{
      GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN6);
      GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN7);
      
      __enable_interrupt();
}
