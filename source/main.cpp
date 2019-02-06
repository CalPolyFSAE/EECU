#include "MKE18F16.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "adc.h"
#include "gpio.h"

using namespace BSP;

VCU vcu;

int main(void) {
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];

	// initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO& gpio = gpio::GPIO::StaticClass();
	gpio.set(gpio::PortD, 15);

    SysTick_Config(TIMER_PERIOD);

    while(1) {
    	if(vcu.get_flag() == true) {
    		gpio.toggle(gpio::PortD, 16);

    		// input map - TODO

    	    // core VCU logic
    	    vcu.map_input(input);
    	    vcu.shutdown_loop();
    	    vcu.redundancy_loop();
    	    vcu.motor_loop();
    	    vcu.map_output(output);

    	    // output map - TODO

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
