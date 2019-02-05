#include "MKE18F16.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "adc.h"
#include "gpio.h"

using namespace BSP;

#define TIMER_PERIOD 3000000 	// 10Hz

VCU vcu;

int main(void) {
	adc12_channel_config_t channel_config;

	// initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    // initialize ADC driver
    adc::ADC::ConstructStatic(NULL);
    adc::ADC& adc = adc::ADC::StaticClass();
    adc.config_base(ADC0, NULL);
    channel_config.enableInterruptOnConversionCompleted = false;
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

    SysTick_Config(TIMER_PERIOD);

    while(1) {
    	if(vcu.flag == true) {
    		// indicator LED
    		if(vcu.state == AIR_OFF) {
    			gpio.clear(gpio::PortD, 16);
    			gpio.clear(gpio::PortD, 15);
    		} else if(vcu.state == PRECHARGE) {
    			gpio.clear(gpio::PortD, 16);
    			gpio.set(gpio::PortD, 15);
    		} else if(vcu.state == AIR_ON) {
    			gpio.set(gpio::PortD, 16);
    			gpio.clear(gpio::PortD, 15);
    		} else if(vcu.state == READY_TO_CHARGE) {
    			gpio.set(gpio::PortD, 16);
    			gpio.set(gpio::PortD, 15);
    		} else if(vcu.state == READY_TO_DRIVE) {
    			gpio.toggle(gpio::PortD, 16);
    		    gpio.toggle(gpio::PortD, 15);
    		}

    		// ADC input map
    	    channel_config.channelNumber = 8U;
    	    adc.config_channel(ADC0, 0, &channel_config);
    	    vcu.input[MC_VOLTAGE] = adc.read(ADC0, 0);

    	    channel_config.channelNumber = 9U;
    	    adc.config_channel(ADC0, 0, &channel_config);
    	    vcu.input[BMS_VOLTAGE] = adc.read(ADC0, 0);

    	    // GPIO input map
    	    vcu.input[TSREADY] = gpio.read(gpio::PortE, 8);
    	    vcu.input[CHARGER_CONNECTED] = gpio.read(gpio::PortD, 5);

    	    // run VCU shutdown loop
    	    vcu.shutdown_loop();

    	    // GPIO output map
    	    if(vcu.output[AIR_POS] == HIGH)
    	    	gpio.set(gpio::PortC, 16);
    	    else
    	    	gpio.clear(gpio::PortC, 16);

    	    if(vcu.output[AIR_NEG] == HIGH)
    	    	gpio.set(gpio::PortB, 1);
    	    else
    	    	gpio.clear(gpio::PortC, 16);

    	    if(vcu.output[ENABLE_COOLANT_PUMP] == HIGH)
    	    	gpio.set(gpio::PortC, 17);
    	    else
    	    	gpio.clear(gpio::PortC, 17);

    	    if(vcu.output[DCDC_DISABLE] == HIGH)
    	    	gpio.set(gpio::PortC, 6);
    	    else
    	    	gpio.clear(gpio::PortC, 6);

    	    if(vcu.output[PRECHARGE_FAILED] == HIGH)
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
