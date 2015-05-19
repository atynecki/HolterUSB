
/** DELAY header file */

#include "msp430f5529.h" 

#define DELAY_100US() __delay_cycles(2400)
#define DELAY_100MS() __delay_cycles(120000)
#define DELAY_1S()    __delay_cycles(1200000)
