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

// receives a broadcast message from the motor controller
void receive_broadcast_message() {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame = can.readrx(MOTOR_CONTROLLER_CAN_CHANNEL);

	switch(frame.id - MOTOR_CONTROLLER_CAN_OFFSET) {
	case BROADCAST_MESSAGE_MOTOR_POSITION_INFORMATION:
		vcu.input[MC_TACH] = (frame.data[3] << 8) | frame.data[2];
		break;

	case BROADCAST_MESSAGE_INTERNAL_VOLTAGES:
		vcu.input[MC_VOLTAGE] = (frame.data[7] << 8) | frame.data[6];
		break;

	case BROADCAST_MESSAGE_FAULT_CODES:
		vcu.input[MC_FAULT] = frame.data[7] | frame.data[6] | frame.data[5] | frame.data[4] | frame.data[3] | frame.data[2] | frame.data[1] | frame.data[0];
		break;
	}
}

// receives a parameter message from the motor controller
void receive_parameter_message() {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame = can.readrx(MOTOR_CONTROLLER_CAN_CHANNEL);

	// TODO
}

// sends a command message to the motor controller
void send_command_message(int16_t torque, int16_t speed, uint8_t direction, uint8_t settings, int16_t limit) {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame;

	frame.id = COMMAND_MESSAGE + MOTOR_CONTROLLER_CAN_OFFSET;
	frame.ext = 1;
	frame.dlc = 8;
	frame.data[0] = torque & 0xFF;
	frame.data[1] = (torque >> 8) & 0xFF;
	frame.data[2] = speed & 0xFF;
	frame.data[3] = (speed >> 8) & 0xFF;
	frame.data[4] = direction & 0x01;
	frame.data[5] = settings & 0x03;
	frame.data[6] = limit & 0xFF;
	frame.data[7] = (limit >> 8) & 0xFF;

	can.tx(MOTOR_CONTROLLER_CAN_CHANNEL, frame);
}

// sends a parameter message to the motor controller
void send_parameter_message(uint16_t address, uint8_t write, uint16_t data) {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame;

	frame.id = PARAMETER_MESSAGE_SEND + MOTOR_CONTROLLER_CAN_OFFSET;
	frame.ext = 1;
	frame.dlc = 8;
	frame.data[0] = address & 0xFF;
	frame.data[1] = (address >> 8) & 0xFF;
	frame.data[2] = write & 0x01;
	frame.data[4] = data & 0xFF;
	frame.data[5] = (data >> 8) & 0xFF;

	can.tx(MOTOR_CONTROLLER_CAN_CHANNEL, frame);
}

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

	gpio.set(gpio::PortD, 15);

    SysTick_Config(TIMER_PERIOD);

    while(1) {
    	if(vcu.get_flag() == true) {
     		// input map - TODO

    	    // core VCU logic
    	    vcu.motor_loop();
    	    vcu.shutdown_loop();
    	    vcu.redundancy_loop();

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
