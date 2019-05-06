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
            vcu.output.GENERAL_PURPOSE_1 = DIGITAL_HIGH;

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
            
            vcu.output.GENERAL_PURPOSE_1 = DIGITAL_LOW;
        }
    }

    return(EXIT_FAILURE);
}

extern "C" {

void SysTick_Handler() { 
    vcu.flag = true;

    if(vcu.output.GENERAL_PURPOSE_2 == DIGITAL_HIGH) {
        vcu.output.GENERAL_PURPOSE_2 = DIGITAL_LOW;
    } else {
        vcu.output.GENERAL_PURPOSE_2 = DIGITAL_HIGH;
    }
}

}
