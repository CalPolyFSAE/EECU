#include "MKE18F16.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "gpio.h"
#include "can.h"

using namespace BSP;

void cb0(void){
	static volatile can::CANlight::frame f = can::CANlight::StaticClass().readrx(0);
}
void cb1(void){
	static volatile can::CANlight::frame f = can::CANlight::StaticClass().readrx(1);
}

can::CANlight::frame f0, f1;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

	can::can_config c;
	can::CANlight::ConstructStatic(&c);
	can::CANlight::canx_config cx;
	cx.callback = cb0;
	can::CANlight& can = can::CANlight::StaticClass();
	can.init(0, &cx);
	cx.callback = cb1;
	can.init(1, &cx);

    f0.id = 10;
    f0.id = 20;

    gpio::GPIO::clear(gpio::PortE, 3); // MCU_Relay
    gpio::GPIO::clear(gpio::PortB, 6); // Buzzer_Switch
    gpio::GPIO::clear(gpio::PortB, 0); // LED
    gpio::GPIO::clear(gpio::PortB, 1); // LED
    gpio::GPIO::clear(gpio::PortB, 2); // LED
    gpio::GPIO::clear(gpio::PortB, 3); // LED

    SysTick_Config(60000);

    for(;;);

    return 0;
}

extern "C" {
    void SysTick_Handler(void){

        static uint64_t time = 0;
        time++;

        static volatile uint8_t WheelSpeed1 = gpio::GPIO::read(gpio::PortC, 2);
        WheelSpeed1 = gpio::GPIO::read(gpio::PortC, 2);
        static volatile uint8_t WheelSpeed2 = gpio::GPIO::read(gpio::PortC, 3);
        WheelSpeed2 = gpio::GPIO::read(gpio::PortC, 3);
        static volatile uint8_t WheelSpeed3 = gpio::GPIO::read(gpio::PortE, 10);
        WheelSpeed3 = gpio::GPIO::read(gpio::PortE, 10);
        static volatile uint8_t WheelSpeed4 = gpio::GPIO::read(gpio::PortE, 11);
        WheelSpeed4 = gpio::GPIO::read(gpio::PortE, 11);

        static volatile uint8_t BSPDOK = gpio::GPIO::read(gpio::PortD, 15);
        BSPDOK = gpio::GPIO::read(gpio::PortD, 15);
        static volatile uint8_t IMDOK = gpio::GPIO::read(gpio::PortD, 16);
        IMDOK = gpio::GPIO::read(gpio::PortD, 16);
        static volatile uint8_t BMSOK = gpio::GPIO::read(gpio::PortE, 9);
        BMSOK = gpio::GPIO::read(gpio::PortE, 9);
        static volatile uint8_t LatchIn = gpio::GPIO::read(gpio::PortE, 8);
        LatchIn = gpio::GPIO::read(gpio::PortE, 8);
        static volatile uint8_t LatchOut = gpio::GPIO::read(gpio::PortB, 5);
        LatchOut = gpio::GPIO::read(gpio::PortB, 5);
        static volatile uint8_t TSReady = gpio::GPIO::read(gpio::PortB, 4);
        TSReady = gpio::GPIO::read(gpio::PortB, 4);

        if(!(time % 1000)){

            gpio::GPIO::toggle(gpio::PortE, 3); // MCU_Relay
            gpio::GPIO::toggle(gpio::PortB, 6); // Buzzer_Switch
            gpio::GPIO::toggle(gpio::PortB, 0); // LED
            gpio::GPIO::toggle(gpio::PortB, 1); // LED
            gpio::GPIO::toggle(gpio::PortB, 2); // LED
            gpio::GPIO::toggle(gpio::PortB, 3); // LED

            //can::CANlight::StaticClass().tx(0, f0);
            //can::CANlight::StaticClass().tx(1, f1);

        }
    }
}
