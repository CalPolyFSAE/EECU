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

    // initialize ADC driver
    adc::ADC::ConstructStatic(NULL);
    adc::ADC& adc = adc::ADC::StaticClass();
    adc.config_base(ADC0, NULL);
    if(adc.calibrate(ADC0) != kStatus_Success) assert(0);

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO& gpio = gpio::GPIO::StaticClass();
    gpio.in_dir(gpio::PortE, 8);	// TSREADY
    gpio.in_dir(gpio::PortD, 5);	// CHARGER_CONNECTED
	gpio.clear(gpio::PortC, 16);	// AIR_POS
	gpio.clear(gpio::PortB, 1);		// AIR_NEG
	gpio.clear(gpio::PortC, 17);	// ENABLE_COOLANT_PUMP
	gpio.clear(gpio::PortC, 6);		// DCDC_DISABLE
	gpio.clear(gpio::PortC, 7);		// PRECHARGE_FAILED
	gpio.set(gpio::PortD, 15);		// LED

    SysTick_Config(TIMER_PERIOD);

    while(1) {
    	if(vcu.get_flag() == true) {
    		// indicator LED
    		gpio.toggle(gpio::PortD, 16);

    		// input map
    	    input[MC_VOLTAGE] = adc.read(ADC0, 8);
    	    input[BMS_VOLTAGE] = adc.read(ADC0, 9);
    	    input[TSREADY] = gpio.read(gpio::PortE, 8);
    	    input[CHARGER_CONNECTED] = gpio.read(gpio::PortD, 5);

    	    // run VCU shutdown loop
    	    vcu.set_input(input);
    	    vcu.shutdown_loop();
    	    vcu.get_output(output);

    	    // output map
    	    if(output[AIR_POS] == HIGH)
    	    	gpio.set(gpio::PortC, 16);
    	    else
    	    	gpio.clear(gpio::PortC, 16);

    	    if(output[AIR_NEG] == HIGH)
    	    	gpio.set(gpio::PortB, 1);
    	    else
    	    	gpio.clear(gpio::PortC, 16);

    	    if(output[ENABLE_COOLANT_PUMP] == HIGH)
    	    	gpio.set(gpio::PortC, 17);
    	    else
    	    	gpio.clear(gpio::PortC, 17);

    	    if(output[DCDC_DISABLE] == HIGH)
    	    	gpio.set(gpio::PortC, 6);
    	    else
    	    	gpio.clear(gpio::PortC, 6);

    	    if(output[PRECHARGE_FAILED] == HIGH)
    	    	gpio.set(gpio::PortC, 7);
    	    else
    	    	gpio.clear(gpio::PortC, 7);

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
