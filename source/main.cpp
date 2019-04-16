#include "MKE18F16.h"
#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "peripherals.h"

#include "vcu.h"
#include "mc.h"
#include "canlight.h"
#include "adc.h"
#include "gpio.h"

using namespace BSP;

VCU vcu;

// TODO - move IO functions to separate file
void input_map();
void output_map();

int main() {
	can::canlight_config config;
	can::CANlight::canx_config canx_config;

	// initialize board hardware
    // TODO - reconfigure pins for VCU board
	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    /*
    gpio.in_dir(gpio::PortE, 6);
    gpio.in_dir(gpio::PortE, 2);
    gpio.in_dir(gpio::PortB, 6);
    gpio.in_dir(gpio::PortB, 7);
    gpio.in_dir(gpio::PortA, 1);
    gpio.in_dir(gpio::PortE, 3);
    gpio.in_dir(gpio::PortD, 16);
    gpio.in_dir(gpio::PortC, 8);
    gpio.in_dir(gpio::PortD, 6);
    gpio.in_dir(gpio::PortD, 5);
    gpio.in_dir(gpio::PortD, 7);
    gpio.out_dir(gpio::PortD, 4);
    gpio.out_dir(gpio::PortB, 1);
    gpio.out_dir(gpio::PortB, 0);
    gpio.out_dir(gpio::PortC, 9);
    gpio.out_dir(gpio::PortE, 11);
    gpio.out_dir(gpio::PortD, 3);
    gpio.out_dir(gpio::PortB, 13);
    gpio.out_dir(gpio::PortB, 12);
    gpio.out_dir(gpio::PortA, 6);
    gpio.out_dir(gpio::PortE, 7);
    gpio.out_dir(gpio::PortD, 0);
    gpio.out_dir(gpio::PortD, 1);
    gpio.out_dir(gpio::PortD, 2);
    gpio.out_dir(gpio::PortA, 7);
    */

	// initialize ADC driver
	adc::ADC::ConstructStatic(NULL);

	// initialize CAN driver
	can::CANlight::ConstructStatic(&config);
	can::CANlight &can = can::CANlight::StaticClass();
	canx_config.baudRate = MC_BAUD_RATE;
	canx_config.callback = mc_callback;
	can.init(MC_CAN_CHANNEL, &canx_config);

    SysTick_Config(TIMER_PERIOD);


// --------------------------------------------------------
    adc::ADC &adc = adc::ADC::ADC::StaticClass();
    mc_clear_faults();
    mc_torque_command(-1);
    mc_torque_command(0);
    gpio.set(gpio::PortD, 16);
// --------------------------------------------------------

    // TODO - time main loop to verify VCU frequency
    while(1) {
    	if(vcu.flag == true) {
    		/*
    		// read inputs
    		input_map();

    	    // core logic
    	    vcu.motor_loop();
    	    vcu.shutdown_loop();
    	    vcu.redundancy_loop();

    	    // write outputs
    	    output_map();
			*/

// --------------------------------------------------------
    		gpio.toggle(gpio::PortD, 15);
    		mc_torque_command(1);
// --------------------------------------------------------

    	    // spinlock to synchronize thread
    	    vcu.flag = false;
    	}
    }

    return 0;
}

void input_map() {
	gpio::GPIO &gpio = gpio::GPIO::StaticClass();
	adc::ADC &adc = adc::ADC::ADC::StaticClass();

	vcu.input.THROTTLE_1 = adc.read(ADC0, 14);
	vcu.input.THROTTLE_2 = adc.read(ADC0, 15);
	vcu.input.LATCH_SENSE = gpio.read(gpio::PortA, 1);
	vcu.input.TS_READY_SENSE = gpio.read(gpio::PortB, 6);
	vcu.input.TS_RDY = gpio.read(gpio::PortE, 2);
	vcu.input.TS_LIVE = gpio.read(gpio::PortB, 7);
	vcu.input.BMS_OK = gpio.read(gpio::PortE, 3);
	vcu.input.IMD_OK = gpio.read(gpio::PortD, 16);
	vcu.input.BSPD_OK = gpio.read(gpio::PortC, 8);
	vcu.input.CURRENT_SENSE = adc.read(ADC0, 6);
	vcu.input.BRAKE_FRONT = adc.read(ADC0, 7);
	vcu.input.BRAKE_REAR = adc.read(ADC0, 12);
	vcu.input.WHEEL_SPEED_FR = adc.read(ADC0, 13);
	vcu.input.WHEEL_SPEED_FL = gpio.read(gpio::PortD, 6);
	vcu.input.WHEEL_SPEED_RR = gpio.read(gpio::PortD, 5);
	vcu.input.WHEEL_SPEED_RL = gpio.read(gpio::PortD, 7);
}

void output_map() {
	gpio::GPIO &gpio = gpio::GPIO::StaticClass();

    vcu.output.RTDS ? gpio.set(gpio::PortD, 4) : gpio.clear(gpio::PortD, 4);
    vcu.output.BRAKE_LIGHT ? gpio.set(gpio::PortB, 1) : gpio.clear(gpio::PortB, 1);
    vcu.output.AIR_POS ? gpio.set(gpio::PortB, 0) : gpio.clear(gpio::PortB, 0);
    vcu.output.AIR_NEG ? gpio.set(gpio::PortC, 9) : gpio.clear(gpio::PortC, 9);
    vcu.output.PUMP_EN ? gpio.set(gpio::PortE, 11) : gpio.clear(gpio::PortE, 11);
    vcu.output.DCDC_DISABLE ? gpio.set(gpio::PortD, 13) : gpio.clear(gpio::PortD, 13);
    vcu.output.PRECHARGE ? gpio.set(gpio::PortB, 13) : gpio.clear(gpio::PortB, 13);
    vcu.output.DISCHARGE ? gpio.set(gpio::PortB, 12) : gpio.clear(gpio::PortB, 12);
    vcu.output.REDUNDANT_1 ? gpio.set(gpio::PortA, 6) : gpio.clear(gpio::PortA, 6);
    vcu.output.REDUNDANT_2 ? gpio.set(gpio::PortE, 7) : gpio.clear(gpio::PortE, 7);
    vcu.output.FAN_EN ? gpio.set(gpio::PortD, 0) : gpio.clear(gpio::PortD, 0);
    vcu.output.GENERAL_PURPOSE_1 ? gpio.set(gpio::PortD, 2) : gpio.clear(gpio::PortD, 2);
    vcu.output.GENERAL_PURPOSE_2 ? gpio.set(gpio::PortA, 7) : gpio.clear(gpio::PortA, 7);

    // TODO - create PWM signal
    vcu.output.FAN_PWM ? gpio.set(gpio::PortD, 1) : gpio.clear(gpio::PortD, 1);
}

extern "C" {
	void SysTick_Handler() {
		vcu.flag = true;
	}
}
