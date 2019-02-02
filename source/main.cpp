#include "MKE18F16.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "gpio.h"

using namespace BSP;

VCU vcu;

int main(void) {

	// initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO& gpio = gpio::GPIO::StaticClass();

    // turn on LED
    gpio.set(gpio::PortD, 15);

    vcu.flag = 0;
    SysTick_Config(10000000);

    while(1) {
    	if(vcu.flag == 1) {
    		gpio.toggle(gpio::PortD, 16);
    		vcu.flag = 0;
    	}
    }

    return 0;
}

extern "C" {
	void SysTick_Handler() {
		vcu.shutdown_loop();
	}
}
