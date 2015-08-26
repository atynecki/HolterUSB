
#include "app_service.h"

void main ()
{
	WDT_A_hold(WDT_A_BASE);

    PMM_setVCore(PMM_CORE_LEVEL_2);

    initClocks();

    general_flag_clear();

    peripherial_init();

    visualization();

    __enable_interrupt();

    system_standby();

    while (1) {
    	if(app_get_flags()->packet_data_ready){
    		app_get_flags()->packet_data_ready = false;
    		send_data_packet();
    	}

    	if(app_get_flags()->stream_start == true){
    		app_get_flags()->stream_start = false;
    		app_get_flags()->stream_enable = true;
    		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			send_state();
    	}

    	if(app_get_flags()->stream_stop == true){
    		app_get_flags()->stream_stop = false;
    		app_get_flags()->stream_enable = false;
			GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			send_state();
    	}

    	if(app_get_flags()->backup_start == true){
    		app_get_flags()->backup_start = false;
    		app_get_flags()->backup_enable = true;

    		clear_write_address();

			if(app_get_flags()->device_run == true){
				set_exam_start_time();
				GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
			else {
				GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
				DELAY_1S();
				GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
			send_state();
    	}

    	if(app_get_flags()->backup_stop == true){
			app_get_flags()->backup_stop = false;
			app_get_flags()->backup_enable = false;

			if(app_get_flags()->device_run == true){
				flash_program_page_last();
				set_exam_stop_time();
				GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
			else {
				GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
				DELAY_1S();
				GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			}
			send_state();
		}

    	if(app_get_flags()->data_transfer == true){
    		if(app_get_flags()->device_run == true)
    			conversion_stop();
    		if(app_get_flags()->backup_enable == true){
    			app_get_flags()->backup_stop = true;
    			break;
    		}

    		clear_read_address();
    		transfer_data();

    		app_get_flags()->data_transfer = false;
    	}

    	if(app_get_flags()->erase_flash == true){
    		app_get_flags()->erase_flash = false;

    		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);

			erase_flash();

			clear_write_address();
			clear_read_address();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
    	}

    	if(!(USB_connectionInfo() & kUSB_vbusPresent) && !app_get_flags()->device_run){
    		system_standby();
    	}
    }
}
