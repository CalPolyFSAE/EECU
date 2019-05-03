#ifndef IO_H
#define IO_H

#include "MKE18F16.h"

#define GEN_CAN_CHANNEL 0
#define GEN_CAN_BAUD_RATE 500000

#define MC_CAN_CHANNEL 1
#define MC_CAN_BAUD_RATE 500000

#define PWM_FREQUENCY 25000
#define PWM_MIN  5
#define PWM_MAX 95

void init_io();
void input_map();
void output_map();
uint8_t can_receive(uint8_t channel, uint8_t *data);
void can_send(uint8_t channel, uint32_t address, uint8_t *data);

#endif
