#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "vcu.h"
#include "mc.h"
#include "io.h"

VCU vcu;

int main() {
    
    // initialize board hardware
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    // initialize drivers
    init_io();

    // initialize system timer
    SysTick_Config(TIMER_PERIOD);
    
    while(true) {
        if(vcu.flag) {
            // read inputs
            input_map();

            // core logic
            vcu.motor_loop();
            vcu.shutdown_loop();
            vcu.redundancy_loop();

            // write outputs
            output_map();
            
            // spinlock to synchronize thread
            vcu.flag = false;
        }
    }

    return(EXIT_FAILURE);
}

extern "C" {

void SysTick_Handler() { 
    vcu.flag = true;
}

}
