#include "vcu.h"
#include "mc.h"
#include "canlight.h"

using namespace BSP;

extern VCU vcu;

// sends a command message to the motor controller
static void mc_send_command_message(uint16_t torque, uint16_t speed, uint8_t direction, uint8_t settings, uint16_t limit) {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame;

	frame.id = MC_COMMAND_MESSAGE;
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

	can.tx(MC_CAN_CHANNEL, frame);
}

// sends a parameter message to the motor controller
static void mc_send_parameter_message(uint16_t address, uint8_t write, uint16_t data) {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame;

	frame.id = MC_PARAMETER_MESSAGE_SEND;
	frame.ext = 1;
	frame.dlc = 8;
	frame.data[0] = address & 0xFF;
	frame.data[1] = (address >> 8) & 0xFF;
	frame.data[2] = write & 0x01;
	frame.data[4] = data & 0xFF;
	frame.data[5] = (data >> 8) & 0xFF;

	can.tx(MC_CAN_CHANNEL, frame);
}

// receives a broadcast message from the motor controller
static void mc_receive_broadcast_message() {
	can::CANlight &can = can::CANlight::StaticClass();
	can::CANlight::frame frame = can.readrx(MC_CAN_CHANNEL);

	switch(frame.id) {
	case MC_BROADCAST_MESSAGE_VOLTAGE_INFORMATION:
		vcu.input.MC_VOLTAGE = (frame.data[1] << 8) | frame.data[0];
		break;

	case MC_BROADCAST_MESSAGE_FAULT_CODES:
		vcu.input.MC_POST_FAULT = (frame.data[3] << 24) | (frame.data[2] << 16) | (frame.data[1] << 8) | frame.data[0];
		vcu.input.MC_RUN_FAULT = (frame.data[7] << 24) | (frame.data[6] << 16) | (frame.data[5] << 8) | frame.data[4];
		break;

	case MC_BROADCAST_MESSAGE_MOTOR_POSITION_INFORMATION:
		vcu.input.MC_SPEED = (frame.data[3] << 8) | frame.data[2];
		break;

	default:
		break;
	}
}

// sends a torque command to the motor controller
void mc_torque_command(int16_t torque) {
	if(torque < 0)
		mc_send_command_message(0, 0, 0, 0, 0);
	else
		mc_send_command_message(torque, 0, 1, 1, 0);
}

// clears all motor controller faults
void mc_clear_faults() {
	mc_send_parameter_message(MC_COMMAND_PARAMETER_FAULT_CLEAR, 1, 0);
}

// callback to process CAN messages from the motor controller
void mc_callback() {
	mc_receive_broadcast_message();
}
