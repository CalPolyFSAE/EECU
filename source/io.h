#ifndef IO_H
#define IO_H

#include "MKE18F16.h"

#define VCU_PRECHARGE   0x0B0
#define VCU_DRIVER      0x0B1
#define VCU_SPEED       0x0B2
#define VCU_FAULTS      0x0B3
#define BMS_ID          0x314
#define CHARGER_ID      0xFFF

#define GEN_CAN_BUS 0
#define GEN_CAN_BAUD_RATE 500000

#define MC_CAN_BUS 1
#define MC_CAN_BAUD_RATE 500000

#define PWM_FREQUENCY 25000
#define PWM_MIN  5
#define PWM_MAX 95

#define PIN_FR  15
#define PIN_FL   6
#define PIN_RR   5
#define PIN_RL   7

void init_io();
void input_map();
void output_map();
void can_send(uint8_t bus, uint32_t address, uint8_t *data);
uint32_t can_receive(uint8_t bus, uint8_t *data);

#endif
