
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
    	if(app_get_flags()->data_transfer == true){
    		transfer_data();
    	}

    	power_manage();
    }
}
