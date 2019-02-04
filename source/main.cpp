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
	adc12_channel_config_t channel_config;

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
	gpio.clear(gpio::PortC, 16);	// POS_AIR_ENABLE
	gpio.clear(gpio::PortC, 17);	// ENABLE_COOLANT_PUMP
	gpio.clear(gpio::PortC, 6);		// DCDC_DISABLE
	gpio.clear(gpio::PortC, 7);		// PRECHARGE_FAILED
	gpio.set(gpio::PortD, 15);		// LED

    SysTick_Config(10000000);

    while(1) {
    	if(vcu.flag == true) {
    		vcu.shutdown_loop();
    		gpio.toggle(gpio::PortD, 16);

    	    channel_config.channelNumber = 8U;
    	    adc.config_channel(ADC0, 0, &channel_config);
    	    vcu.input[MC_VOLTAGE] = adc.read(ADC0, 0);

    	    channel_config.channelNumber = 9U;
    	    adc.config_channel(ADC0, 0, &channel_config);
    	    vcu.input[BMS_VOLTAGE] = adc.read(ADC0, 0);

    	    vcu.input[TSREADY] = gpio.read(gpio::PortE, 8);
    	    vcu.input[CHARGER_CONNECTED] = gpio.read(gpio::PortD, 5);

    	    if(vcu.input[POS_AIR_ENABLE] == HIGH)
    	    	gpio.set(gpio::PortC, 16);
    	    else
    	    	gpio.clear(gpio::PortC, 16);

    	    if(vcu.input[ENABLE_COOLANT_PUMP] == HIGH)
    	    	gpio.set(gpio::PortC, 17);
    	    else
    	    	gpio.clear(gpio::PortC, 17);

    	    if(vcu.input[DCDC_DISABLE] == HIGH)
    	    	gpio.set(gpio::PortC, 6);
    	    else
    	    	gpio.clear(gpio::PortC, 6);

    	    if(vcu.input[PRECHARGE_FAILED] == HIGH)
    	    	gpio.set(gpio::PortC, 7);
    	    else
    	    	gpio.clear(gpio::PortC, 7);
    	}
    }

    return 0;
}

extern "C" {
	void SysTick_Handler() {
		vcu.flag = true;
	}
}
