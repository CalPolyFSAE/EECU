#include "MKE18F16.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "mc.h"
#include "io.h"

VCU vcu;

int main() {
    uint8_t data[8];

    // initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    // initialize drivers
    init_io();

    // initialize system timer
    SysTick_Config(TIMER_PERIOD);
    
    // TODO - time main loop to verify VCU frequency
    while(true) {
        if(vcu.flag) {
            data[0] = 0x00;
            data[1] = 0x11;
            data[2] = 0x22;
            data[3] = 0x33;
            data[4] = 0x44;
            data[5] = 0x55;
            data[6] = 0x66;
            data[7] = 0x77;

            can_send(GEN_CAN_CHANNEL, 0x0AA, data);

            // read inputs
            //input_map();

            // core logic
            //vcu.motor_loop();
            //vcu.shutdown_loop();
            //vcu.redundancy_loop();

            // write outputs
            //output_map();
            
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
