
#include "app_service.h"

void main ()
{
    WDT_A_hold(WDT_A_BASE);

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2
    PMM_setVCore(PMM_CORE_LEVEL_2);

    //TODO sprawdzi� kt�ra opacja dzia�a
    initClocks(24000000);   // Config clocks. MCLK=SMCLK=FLL=24MHz; ACLK=REFO=32kHz
   // CLOCK_init();

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
