#include <stdio.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "adc.h"
//#include "candrv.h" 		/* missing Event.h */
#include "gpio.h"
#include "lpit.h"
#include "spi.h"

int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    printf("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}
