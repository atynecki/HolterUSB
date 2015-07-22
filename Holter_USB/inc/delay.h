
#ifndef __DELAY_H__
#define __DELAY_H__

/** DELAY header file */

#include "msp430f5529.h" 

#define DELAY_1MS()   __delay_cycles(8000)
#define DELAY_10MS()   __delay_cycles(80000)
#define DELAY_100MS() __delay_cycles(800000)
#define DELAY_1S()    __delay_cycles(8000000)

#endif
