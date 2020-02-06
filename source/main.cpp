#include "MKE18F16.h"
#include "pin_mux.h"
#include "clock_config.h"

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    SysTick_Config(60000);

    return 0;
}

extern "C" {
    void SysTick_Handler(void){

    }
}
