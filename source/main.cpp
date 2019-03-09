#include <stdio.h>

#include "MKE18F16.h"
#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "fsl_debug_console.h"

#include "vcu.h"
#include "gpio.h"
#include "adc.h"
#include "canlight.h"
#include "motor_controller.h"

using namespace BSP;

VCU vcu;

int main() {
	can::canlight_config config;
	can::CANlight::canx_config canx_config;

	// initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();

	// initialize ADC driver
	adc::ADC::ConstructStatic(NULL);
	adc::ADC &adc = adc::ADC::ADC::StaticClass();

	// initialize CAN driver
	can::CANlight::ConstructStatic(&config);
	can::CANlight &can = can::CANlight::StaticClass();
	canx_config.baudRate = MOTOR_CONTROLLER_BAUD_RATE;
	canx_config.callback = motor_controller_handler;
	can.init(MOTOR_CONTROLLER_CAN_CHANNEL, &canx_config);

    SysTick_Config(TIMER_PERIOD);
    gpio.set(gpio::PortD, 16);

    motor_controller_clear_faults();
    motor_controller_torque_command(-1);

    uint32_t i = 0;

    while(1) {
    	if(vcu.get_flag() == true) {
// ---------------------------------------------------------------------------------------------
    		if(i >= VCU_FREQUENCY)
    		{
    			gpio.toggle(gpio::PortD, 15);
    			PRINTF("MC POST_FAULT = 0x%08lX\n", vcu.input[MC_POST_FAULT]);
    			PRINTF("MC RUN_FAULT = 0x%08lX\n", vcu.input[MC_RUN_FAULT]);
    			PRINTF("MC STATE = 0x%04lX\n", vcu.input[MC_STATE]);
    			PRINTF("\n");
    			i = 0;
    		}
    		else
    		{
    			i++;
    		}

    		motor_controller_torque_command(10);
// ---------------------------------------------------------------------------------------------
    		/*
    		// input map - TODO

    	    // core VCU logic
    	    vcu.motor_loop();
    	    vcu.shutdown_loop();
    	    vcu.redundancy_loop();

    	    // output map - TODO
			*/
    		vcu.clear_flag();
    	}
    }

    return 0;
}

extern "C" {
	void SysTick_Handler() {
		vcu.set_flag();
	}
}
