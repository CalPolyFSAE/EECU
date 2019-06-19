#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "vcu.h"
#include "mc.h"
#include "io.h"
#include "canlight.h"
using namespace BSP;

VCU vcu;

int main() {
    // initialize board hardware
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    uint32_t time = 0;
    can::CANlight::frame ftime;
    ftime.id = 0x10000;
    ftime.ext = 1;

    // initialize drivers
    init_io();

    // initialize system timer
    SysTick_Config(TIMER_PERIOD);

    while(true) {
        if(vcu.flag) {

            ftime.data[0] = time & 0xff;
            ftime.data[1] = (time>>8)&0xff;
            ftime.data[2] = (time>>16)&0xff;
            ftime.data[3] = (time>>24)&0xff;
            can::CANlight::StaticClass().tx(0, ftime);
            time++;


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
