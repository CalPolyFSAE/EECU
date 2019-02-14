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

int main() {
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];

	// initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO& gpio = gpio::GPIO::StaticClass();

	// initialize ADC driver
	adc::ADC::ConstructStatic(NULL);
	adc::ADC& adc = adc::ADC::ADC::StaticClass();

    SysTick_Config(TIMER_PERIOD);

    while(1) {
    	if(vcu.get_flag() == true) {
     		// input map - TODO
    		input[BF] = adc.read(ADC0, 8);
    		input[BR] = adc.read(ADC1, 6);
    		input[C] = adc.read(ADC2, 12);
    		input[IMD_OK] = gpio.read(gpio::PortC, 16);
    		input[BMS_OK] = gpio.read(gpio::PortC, 17);

    	    // core VCU logic
    	    vcu.map_input(input);
    	    vcu.motor_loop();
    	    vcu.shutdown_loop();
    	    vcu.redundancy_loop();
    	    vcu.map_output(output);

    	    // output map - TODO
    	    if(output[MCU_REDUNDANCY_1] == HIGH)
    	    	gpio.set(gpio::PortD, 15);
    	    else
    	    	gpio.clear(gpio::PortD, 15);

    	    if(output[MCU_REDUNDANCY_2] == HIGH)
    	    	gpio.set(gpio::PortD, 16);
    	    else
    	    	gpio.clear(gpio::PortD, 16);

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
