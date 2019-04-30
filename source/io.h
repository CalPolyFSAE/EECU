#ifndef IO_H
#define IO_H

#include <stdlib.h>

#define GEN_CAN_CHANNEL 0
#define GEN_CAN_BAUD_RATE 500000

#define MC_CAN_CHANNEL 1
#define MC_CAN_BAUD_RATE 500000

void init_io();
void input_map();
void output_map();
uint8_t receive_can_message(uint8_t channel, uint8_t *data);
void send_can_message(uint8_t channel, uint32_t address, uint8_t *data);

#endif
