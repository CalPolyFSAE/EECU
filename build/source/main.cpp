#include "MKE18F16.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "mc.h"
#include "io.h"

VCU vcu;

int main() {
	// TODO - reconfigure pins for VCU board
	// initialize board hardware
	BOARD_InitBootPins();
    BOARD_InitBootClocks();

    // initialize drivers
    init_io();

    // initialize system timer
    SysTick_Config(TIMER_PERIOD);

// --------------------------------------------------------
    /*
    mc_clear_faults();
    mc_torque_request(-1);
    mc_torque_request(0);
    */
// --------------------------------------------------------

    // TODO - time main loop to verify VCU frequency
    while(1) {
    	if(vcu.flag == true) {
    		// read inputs
    		//input_map();

    	    // core logic
    	    //vcu.motor_loop();
    	    //vcu.shutdown_loop();
    	    //vcu.redundancy_loop();

    	    // write outputs
    	    //output_map();

// --------------------------------------------------------
    		//uint8_t data[8];

    		mc_torque_request(0);

    		/*
    		data[0] = vcu.input.MC_POST_FAULT & 0xFF;
    		data[1] = (vcu.input.MC_POST_FAULT >> 8) & 0xFF;
    		data[2] = (vcu.input.MC_POST_FAULT >> 16) & 0xFF;
    		data[3] = (vcu.input.MC_POST_FAULT >> 24) & 0xFF;
    		data[4] = vcu.input.MC_RUN_FAULT & 0xFF;
    		data[5] = (vcu.input.MC_RUN_FAULT >> 8) & 0xFF;
    		data[6] = (vcu.input.MC_RUN_FAULT >> 16) & 0xFF;
    		data[7] = (vcu.input.MC_RUN_FAULT >> 24) & 0xFF;
    		*/

    		//send_can_message(MC_CAN_CHANNEL, 0x0AA, data);
// --------------------------------------------------------

    	    // spinlock to synchronize thread
    	    vcu.flag = false;
    	}
    }

    return 0;
}

extern "C" {
	void SysTick_Handler() {
		vcu.flag = true;
	}
}
