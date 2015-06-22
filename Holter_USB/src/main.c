
#include "app_service.h"

void main ()
{
    WDT_A_hold(WDT_A_BASE);

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_3
    PMM_setVCore(PMM_CORE_LEVEL_3);

    initClocks(USB_MCLK_FREQ);

    peripherial_init ();

    visualization();

    general_flag_init();

    interrupt_enable();

    while (1) {
    	if(app_get_flags()->stream_start == true){
    		app_get_flags()->stream_start = false;
    		app_get_flags()->stream_enable = true;
    		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
			create_header_frame();
			app_get_data()->timestamp = 0;
    	}

    	if(app_get_flags()->stream_stop == true){
    		app_get_flags()->stream_stop = false;
    		app_get_flags()->stream_enable = false;
			GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			DELAY_1S();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
    	}

    	if(app_get_flags()->backup_start == true){
    		app_get_flags()->backup_start = false;
    		app_get_flags()->backup_enable = true;

			set_exam_start_time();
			app_get_data()->timestamp = 0;

			GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
    	}

    	if(app_get_flags()->backup_stop == true){
			app_get_flags()->backup_stop = false;
			app_get_flags()->backup_enable = false;

			copy_write_address(app_get_data()->tail_address);
			set_exam_stop_time();

			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);

		}

    	if(app_get_flags()->data_transfer == true){
    		//TODO zatrzymanie wszystkich pomiar�w

    		transfer_data();
    	}

    	if(app_get_flags()->data_ready == true){
    		app_get_flags()->data_ready = false;
    		send_data();
    	}

    	if(app_get_flags()->erase_flash == true){
    		app_get_flags()->erase_flash = false;
    		GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN1);
			erase_flash();
			GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
    	}

    	power_manage();
    }
}
