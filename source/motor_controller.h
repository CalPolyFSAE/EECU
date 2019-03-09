#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_

// motor controller CAN settings
#define MOTOR_CONTROLLER_CAN_OFFSET 0x160
#define MOTOR_CONTROLLER_CAN_CHANNEL 0
#define MOTOR_CONTROLLER_BAUD_RATE 500000

// broadcast message addresses
#define BROADCAST_MESSAGE_TEMPERATURES_1				0x0A0
#define BROADCAST_MESSAGE_TEMPERATURES_2				0x0A1
#define BROADCAST_MESSAGE_TEMPERATURES_3				0x0A2
#define BROADCAST_MESSAGE_ANALOG_INPUT_VOLTAGES			0x0A3
#define BROADCAST_MESSAGE_DIGITAL_INPUT_STATUS			0x0A4
#define BROADCAST_MESSAGE_MOTOR_POSITION_INFORMATION	0x0A5
#define BROADCAST_MESSAGE_CURRENT_INFORMATION			0x0A6
#define BROADCAST_MESSAGE_VOLTAGE_INFORMATION			0x0A7
#define BROADCAST_MESSAGE_FLUX_INFORMATION				0x0A8
#define BROADCAST_MESSAGE_INTERNAL_VOLTAGES				0x0A9
#define BROADCAST_MESSAGE_INTERNAL_STATES				0x0AA
#define BROADCAST_MESSAGE_FAULT_CODES					0x0AB
#define BROADCAST_MESSAGE_TORQUE_INFORMATION			0x0AC
#define BROADCAST_MESSAGE_MODULATION_INDEX_INFORMATION	0x0AD
#define BROADCAST_MESSAGE_FIRMWARE_INFORMATION			0x0AE
#define BROADCAST_MESSAGE_DIAGNOSTIC_DATA				0x0AF

// post fault masks
#define POST_FAULT_HARDWARE_GATE 						0x00000001
#define POST_FAULT_HW_OVER_CURRENT						0x00000002
#define POST_FAULT_ACCELERATOR_SHORTED					0x00000004
#define POST_FAULT_ACCELERATOR_OPEN						0x00000008
#define POST_FAULT_CURRENT_SENSOR_LOW					0x00000010
#define POST_FAULT_CURRENT_SENSOR_HIGH					0x00000020
#define POST_FAULT_MODULE_TEMPERATURE_LOW				0x00000040
#define POST_FAULT_MODULE_TEMPERATURE_HIGH				0x00000080
#define POST_FAULT_CONTROL_PCB_TEMPERATURE_LOW			0x00000100
#define POST_FAULT_CONTROL_PCB_TEMPERATURE_HIGH			0x00000200
#define POST_FAULT_GATE_DRIVE_PCB_TEMPERATURE_LOW		0x00000400
#define POST_FAULT_GATE_DRIVE_PCB_TEMPERATURE_HIGH		0x00000800
#define POST_FAULT_5V_SENSE_VOLTAGE_LOW					0x00001000
#define POST_FAULT_5V_SENSE_VOLTAGE_HIGH				0x00002000
#define POST_FAULT_12V_SENSE_VOLTAGE_LOW				0x00004000
#define POST_FAULT_12V_SENSE_VOLTAGE_HIGH				0x00008000
#define POST_FAULT_2_5V_SENSE_VOLTAGE_LOW				0x00010000
#define POST_FAULT_2_5V_SENSE_VOLTAGE_HIGH				0x00020000
#define POST_FAULT_1_5V_SENSE_VOLTAGE_LOW				0x00040000
#define POST_FAULT_1_5V_SENSE_VOLTAGE_HIGH				0x00080000
#define POST_FAULT_DC_BUS_VOLTAGE_HIGH					0x00100000
#define POST_FAULT_DC_BUS_VOLTAGE_LOW					0x00200000
#define POST_FAULT_PRECHARGE_TIMEOUT					0x00400000
#define POST_FAULT_PRECHARGE_VOLTAGE_FAILURE			0x00800000
#define POST_FAULT_EEPROM_CHECKSUM_INVALID				0x01000000
#define POST_FAULT_EEPROM_DATA_OUT_OF_RANGE				0x02000000
#define POST_FAULT_EEPROM_UPDATE_REQUIRED				0x04000000
#define POST_FAULT_BRAKE_SHORTED						0x40000000
#define POST_FAULT_BRAKE_OPEN							0x80000000

// run fault masks
#define RUN_FAULT_MOTOR_OVERSPEED 						0x00000001
#define RUN_FAULT_OVERCURRENT							0x00000002
#define RUN_FAULT_OVERVOLTAGE							0x00000004
#define RUN_FAULT_INVERTER_OVERTEMPERATURE				0x00000008
#define RUN_FAULT_ACCELERATOR_INPUT_SHORTED				0x00000010
#define RUN_FAULT_ACCELERATOR_INPUT_OPEN				0x00000020
#define RUN_FAULT_DIRECTION_COMMAND						0x00000040
#define RUN_FAULT_INVERTER_RESPONSE_TIMEOUT				0x00000080
#define RUN_FAULT_HARDWARE_GATE							0x00000100
#define RUN_FAULT_HARDWARE_OVERCURRENT					0x00000200
#define RUN_FAULT_UNDERVOLTAGE							0x00000400
#define RUN_FAULT_CAN_COMMAND_MESSAGE_LOST				0x00000800
#define RUN_FAULT_MOTOR_OVERTEMPERATURE					0x00001000
#define RUN_FAULT_BRAKE_INPUT_SHORTED					0x00010000
#define RUN_FAULT_BRAKE_INPUT_OPEN						0x00020000
#define RUN_FAULT_MODULE_A_OVERTEMPERATURE				0x00040000
#define RUN_FAULT_MODULE_B_OVERTEMPERATURE				0x00080000
#define RUN_FAULT_MODULE_C_OVERTEMPERATURE				0x00100000
#define RUN_FAULT_PCB_OVERTEMPERATURE					0x00200000
#define RUN_FAULT_GATE_DRIVE_BOARD_1_OVERTEMPERATURE	0x00400000
#define RUN_FAULT_GATE_DRIVE_BOARD_2_OVERTEMPERATURE	0x00800000
#define RUN_FAULT_GATE_DRIVE_BOARD_3_OVERTEMPERATURE	0x01000000
#define RUN_FAULT_CURRENT_SENSOR						0x02000000
#define RUN_FAULT_RESOLVER_NOT_CONNECTED				0x40000000
#define RUN_FAULT_INVERTER_DISCHARGE_ACTIVE				0x80000000

// command message constants
#define COMMAND_MESSAGE									0x0C0

// parameter message constants
#define PARAMETER_MESSAGE_SEND							0x0C1
#define PARAMETER_MESSAGE_RECEIVE						0x0C2

// command parameter addresses
#define COMMAND_PARAMETER_RELAY							1
#define COMMAND_PARAMETER_FLUX							10
#define COMMAND_PARAMETER_RESOLVER_PWM_DELAY			11
#define COMMAND_PARAMETER_GAMMA_ADJUST_GUI				12
#define COMMAND_PARAMETER_FAULT_CLEAR					20

// EEPROM parameter addresses
#define EEPROM_PARAMETER_MOTOR_PARAMETER_SET			150
#define EEPROM_PARAMETER_RESOLVER_PWM_DELAY				151
#define EEPROM_PARAMETER_GAMMA_ADJUST					152
#define EEPROM_PARAMETER_SIN_OFFSET						154
#define EEPROM_PARAMETER_COS_OFFSET						155
#define EEPROM_PARAMETER_SIN_ADC_OFFSET					156
#define EEPROM_PARAMETER_COS_ADC_OFFSET					157
#define EEPROM_PARAMETER_PRECHARGE_BYPASSED				140
#define EEPROM_PARAMETER_INVERTER_RUN_MODE				142
#define EEPROM_PARAMETER_INVERTER_COMMAND_MODE			143
#define EEPROM_PARAMETER_KEY_SWITCH_MODE				149
#define EEPROM_PARAMETER_RELAY_OUTPUT_STATE				170
#define EEPROM_PARAMETER_DISCHARGE_ENABLE				173
#define EEPROM_PARAMETER_ANALOG_OUTPUT_FUNCTION_SELECT	204
#define EEPROM_PARAMETER_SERIAL_NUMBER					174
#define EEPROM_PARAMETER_CAN_ID_OFFSET					141
#define EEPROM_PARAMETER_CAN_MESSAGE_IDENTIFIER			144
#define EEPROM_PARAMETER_CAN_J1939_OPTION_ACTIVE		171
#define EEPROM_PARAMETER_CAN_TERM_RESISTOR_PRESENT		145
#define EEPROM_PARAMETER_CAN_COMMAND_MESSAGE_ACTIVE		146
#define EEPROM_PARAMETER_CAN_BIT_RATE					147
#define EEPROM_PARAMETER_CAN_ACTIVE_MESSAGES_WORD		148
#define EEPROM_PARAMETER_CAN_DIAGNOSTIC_DATA_ACTIVE		158
#define EEPROM_PARAMETER_CAN_INVERTER_ENABLE_ACTIVE		159
#define EEPROM_PARAMETER_CAN_TIMEOUT					172
#define EEPROM_PARAMETER_CAN_OBD2_ENABLE				177
#define EEPROM_PARAMETER_CAN_BMS_LIMIT_ENABLE			178
#define EEPROM_PARAMETER_IQ_LIMIT						100
#define EEPROM_PARAMETER_ID_LIMIT						101
#define EEPROM_PARAMETER_IA_OFFSET						107
#define EEPROM_PARAMETER_IB_OFFSET						108
#define EEPROM_PARAMETER_IC_OFFSET						109
#define EEPROM_PARAMETER_DC_VOLTAGE_LIMIT				102
#define EEPROM_PARAMETER_DC_VOLTAGE_HYSTERESIS			103
#define EEPROM_PARAMETER_DC_UNDERVOLTAGE_LIMIT			104
#define EEPROM_PARAMETER_VEHICLE_FLUX_COMMAND			106
#define EEPROM_PARAMETER_INVERTER_OVERTEMPERATURE		112
#define EEPROM_PARAMETER_MOTOR_OVERTEMPERATURE			113
#define EEPROM_PARAMETER_ZERO_TORQUE_TEMPERATURE		114
#define EEPROM_PARAMETER_FULL_TORQUE_TEMPERATURE		115
#define EEPROM_PARAMETER_RTD_SELECTION					203
#define EEPROM_PARAMETER_ACCEL_PEDAL_LOW				120
#define EEPROM_PARAMETER_ACCEL_PEDAL_MIN				121
#define EEPROM_PARAMETER_ACCEL_COAST_LOW				122
#define EEPROM_PARAMETER_ACCEL_COAST_HIGH				123
#define EEPROM_PARAMETER_ACCEL_PEDAL_MAX				124
#define EEPROM_PARAMETER_ACCEL_PEDAL_HIGH				125
#define EEPROM_PARAMETER_ACCEL_PEDAL_FLIPPED			132
#define EEPROM_PARAMETER_MOTOR_TORQUE_LIMIT				129
#define EEPROM_PARAMETER_REGEN_TORQUE_LIMIT				130
#define EEPROM_PARAMETER_BRAKING_TORQUE_LIMIT			131
#define EEPROM_PARAMETER_KP_TORQUE						164
#define EEPROM_PARAMETER_KI_TORQUE						165
#define EEPROM_PARAMETER_KD_TORQUE						166
#define EEPROM_PARAMETER_KLP_TORQUE						167
#define EEPROM_PARAMETER_TORQUE_RATE_LIMIT				168
#define EEPROM_PARAMETER_MOTOR_OVERSPEED				111
#define EEPROM_PARAMETER_MAX_SPEED						128
#define EEPROM_PARAMETER_REGEN_FADE_SPEED				126
#define EEPROM_PARAMETER_BREAK_SPEED					127
#define EEPROM_PARAMETER_KP_SPEED						160
#define EEPROM_PARAMETER_KI_SPEED						161
#define EEPROM_PARAMETER_KD_SPEED						162
#define EEPROM_PARAMETER_KLP_SPEED						163
#define EEPROM_PARAMETER_SPEED_RATE_LIMIT				169
#define EEPROM_PARAMETER_SHUDDER_COMPENSATION_ENABLE	187
#define EEPROM_PARAMETER_KP_SHUDDER						188
#define EEPROM_PARAMETER_TCLAMP_SHUDDER					189
#define EEPROM_PARAMETER_SHUDDER_FILTER_FREQUENCY		190
#define EEPROM_PARAMETER_SHUDDER_SPEED_FADE				191
#define EEPROM_PARAMETER_SHUDDER_SPEED_LOW				192
#define EEPROM_PARAMETER_SHUDDER_SPEED_HIGH				193
#define EEPROM_PARAMETER_BRAKE_MODE						180
#define EEPROM_PARAMETER_BRAKE_LOW						181
#define EEPROM_PARAMETER_BRAKE_MIN						182
#define EEPROM_PARAMETER_BRAKE_MAX						183
#define EEPROM_PARAMETER_BRAKE_HIGH						184
#define EEPROM_PARAMETER_REGEN_RAMP_PERIOD				185
#define EEPROM_PARAMETER_BRAKE_PEDAL_FLIPPED			186
#define EEPROM_PARAMETER_BRAKE_INPUT_BYPASSED			199

// function prototypes
void motor_controller_torque_command(int16_t torque);
void motor_controller_clear_faults();
void motor_controller_handler();

#endif
