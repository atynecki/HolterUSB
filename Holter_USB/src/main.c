
#include "app_service.h"

void main ()
{
    WDT_A_hold(WDT_A_BASE);

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_3
    PMM_setVCore(PMM_CORE_LEVEL_3);

   initClocks(USB_MCLK_FREQ);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz

    peripherial_init ();

    visualization();

    interrupt_enable();

    transfer_result transfer_err;
    while (1) {
    	if(app_get_flags()->data_transfer == true){
    		transfer_err = transfer_data();
    		if(transfer_err != TRANSFER_OK){
    			app_get_flags()->transfer_error = transfer_err;
    			app_get_flags()->data_transfer = false;
    		}
    	}
    }
}
