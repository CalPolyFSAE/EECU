#include "vcu.h"
#include "canlight.h"
#include "motor_controller.h"

#include <stdio.h>
#include "fsl_debug_console.h"

using namespace BSP;

extern VCU vcu;

// sends a command message to the motor controller
static void send_command_message(uint16_t torque, uint16_t speed, uint8_t direction, uint8_t settings, uint16_t limit) {
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
static void send_parameter_message(uint16_t address, uint8_t write, uint16_t data) {
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

// receives a broadcast message from the motor controller
static void receive_broadcast_message() {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame = can.readrx(MOTOR_CONTROLLER_CAN_CHANNEL);

	switch(frame.id - MOTOR_CONTROLLER_CAN_OFFSET) {
	case BROADCAST_MESSAGE_MOTOR_POSITION_INFORMATION:
		vcu.input[MC_TACHOMETER] = (frame.data[3] << 8) | frame.data[2];
		break;

	case BROADCAST_MESSAGE_INTERNAL_VOLTAGES:
		vcu.input[MC_VOLTAGE] = (frame.data[7] << 8) | frame.data[6];
		break;

	case BROADCAST_MESSAGE_FAULT_CODES:
		vcu.input[MC_POST_FAULT] = (frame.data[3] << 24) | (frame.data[2] << 16) | (frame.data[1] << 8) | frame.data[0];
		vcu.input[MC_RUN_FAULT] = (frame.data[7] << 24) | (frame.data[6] << 16) | (frame.data[5] << 8) | frame.data[4];
		break;



// ------------------------------------------------------------------------
	case BROADCAST_MESSAGE_INTERNAL_STATES:
		vcu.input[MC_STATE] = (frame.data[6] << 8) | frame.data[4];
		break;
	}
// ------------------------------------------------------------------------
}

// receives a parameter message from the motor controller
static void receive_parameter_message() {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame = can.readrx(MOTOR_CONTROLLER_CAN_CHANNEL);

	// TODO
}

// sends a torque command to the motor controller
void motor_controller_torque_command(int16_t torque) {
	if(torque < 0)
		send_command_message(0, 0, 0, 0, 0);
	else
		send_command_message(torque, 0, 1, 1, 0);
}

// clears all motor controller faults
void motor_controller_clear_faults() {
	send_parameter_message(COMMAND_PARAMETER_FAULT_CLEAR, 1, 0);
}

// handler to process CAN messages from the motor controller
void motor_controller_handler() {
	receive_broadcast_message();
	receive_parameter_message();
}
