#ifndef IO_H
#define IO_H

#include "MKE18F16.h"

#define VCU_UPDATE      0x100
#define VCU_SAFETY      0x101
#define VCU_PRECHARGE   0x102
#define VCU_DRIVER      0x103
#define VCU_SPEED       0x104
#define VCU_STATUS      0x105

#define DASHBOARD_ID    0x200
#define BMS_ID          0x314
#define CHARGER_ID      0x18FF50E5

#define GEN_CAN_BUS 0
#define GEN_CAN_BAUD_RATE 250000

#define MC_CAN_BUS 1
#define MC_CAN_BAUD_RATE 1000000

#define RELUCTOR_TEETH 20
#define TIMER_FREQUENCY 125000

#define PWM_FREQUENCY 25000
#define PWM_MIN 40
#define PWM_MAX 95

void init_io();
void input_map();
void output_map();
void can_send(uint8_t bus, uint32_t address, uint8_t *data);
uint32_t can_receive(uint8_t bus, uint8_t *data);

#endif
