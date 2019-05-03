#include "vcu.h"
#include "mc.h"
#include "io.h"

extern VCU vcu;

// sends a command message to the motor controller
void mc_send_command_message(uint16_t torque, uint16_t speed, uint8_t direction, uint8_t settings, uint16_t limit) {
    uint8_t buffer[8];

    buffer[0] = torque & 0xFF;
    buffer[1] = (torque >> 8) & 0xFF;
    buffer[2] = speed & 0xFF;
    buffer[3] = (speed >> 8) & 0xFF;
    buffer[4] = direction & 0x01;
    buffer[5] = settings & 0x03;
    buffer[6] = limit & 0xFF;
    buffer[7] = (limit >> 8) & 0xFF;

    can_send(MC_CAN_CHANNEL, MC_COMMAND_MESSAGE, buffer);
}

// sends a parameter message to the motor controller
void mc_send_parameter_message(uint16_t address, uint8_t write, uint16_t data) {
    uint8_t buffer[8];

    buffer[0] = address & 0xFF;
    buffer[1] = (address >> 8) & 0xFF;
    buffer[2] = write & 0x01;
    buffer[4] = data & 0xFF;
    buffer[5] = (data >> 8) & 0xFF;
    buffer[6] = 0x00;
    buffer[7] = 0x00;

    can_send(MC_CAN_CHANNEL, MC_PARAMETER_MESSAGE_SEND, buffer);
}

// receives a broadcast message from the motor controller
void mc_receive_broadcast_message() {
    uint8_t buffer[8];

    switch(can_receive(MC_CAN_CHANNEL, buffer)) {
        case MC_BROADCAST_MESSAGE_FAULT_CODES:
            vcu.input.MC_POST_FAULT = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
            vcu.input.MC_RUN_FAULT = (buffer[7] << 24) | (buffer[6] << 16) | (buffer[5] << 8) | buffer[4];
            break;

        case MC_BROADCAST_MESSAGE_CURRENT_INFORMATION:
            vcu.input.MC_CURRENT = ((buffer[7] << 8) | buffer[6]) / 10;
            break;

        case MC_BROADCAST_MESSAGE_VOLTAGE_INFORMATION:
            vcu.input.MC_VOLTAGE = ((buffer[1] << 8) | buffer[0]) / 10;
            break;

        case MC_BROADCAST_MESSAGE_MOTOR_POSITION_INFORMATION:
            vcu.input.MC_SPEED = (buffer[3] << 8) | buffer[2];
            break;

        default:
            break;
    }
}

// sends a torque request to the motor controller
void mc_torque_request(int16_t torque) {
    if(torque < 0)
        mc_send_command_message(0, 0, 0, 0, 0);
    else
        mc_send_command_message(torque, 0, 1, 1, 0);
}

// clears all motor controller faults
void mc_clear_faults() {
    mc_send_parameter_message(MC_COMMAND_PARAMETER_FAULT_CLEAR, 1, 0);
}
