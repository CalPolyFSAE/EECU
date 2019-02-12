#include "vcu.h"

// VCU class constructor
VCU::VCU() {
	VCU::flag = false;
	VCU::state[SHUTDOWN] = AIR_OFF;

	for(int i = 0; i < INPUT_COUNT; i++)
		VCU::input[i] = LOW;

	for(int i = 0; i < OUTPUT_COUNT; i++)
		VCU::output[i] = LOW;
}

// VCU motor loop
void VCU::motor_loop() {}

// VCU shutdown loop
void VCU::shutdown_loop() {
	switch(VCU::state[SHUTDOWN]) {
	case AIR_OFF:
		VCU::output[AIR_POS] = LOW;
		VCU::output[AIR_NEG] = LOW;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(input[TSREADY] == HIGH) {
			VCU::state[SHUTDOWN] = PRECHARGE;
			VCU::output[PRECHARGE_FAILED] = LOW;
			VCU::timer = 0;
		}

		break;

	case PRECHARGE:
		VCU::output[AIR_POS] = LOW;
		VCU::output[AIR_NEG] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;
		VCU::timer++;

		if(((VCU::timer > ALLOWED_PRECHARGE_TIME) && (VCU::input[MC_VOLTAGE] < ((VCU::input[BMS_VOLTAGE] * BATTERY_PERCENTAGE) / 100)) && (VCU::input[CHARGER_CONNECTED] == LOW)) || (VCU::input[TSREADY] == LOW)) {
			VCU::state[SHUTDOWN] = AIR_OFF;
			VCU::output[PRECHARGE_FAILED] = HIGH;
		} else if((((VCU::timer > ALLOWED_PRECHARGE_TIME) && (VCU::input[MC_VOLTAGE] > ((VCU::input[BMS_VOLTAGE] * BATTERY_PERCENTAGE) / 100))) || (VCU::input[CHARGER_CONNECTED] == HIGH)) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state[SHUTDOWN] = AIR_ON;
		}

		break;

	case AIR_ON:
		VCU::output[AIR_POS] = HIGH;
		VCU::output[AIR_NEG] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(VCU::input[TSREADY] == LOW) {
			VCU::state[SHUTDOWN] = AIR_OFF;
		} else if((VCU::input[CHARGER_CONNECTED] == LOW) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state[SHUTDOWN] = READY_TO_DRIVE;
			VCU::timer = 0;
		} else if((VCU::input[CHARGER_CONNECTED] == HIGH) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state[SHUTDOWN] = READY_TO_CHARGE;
		}

		break;

	case READY_TO_CHARGE:
		VCU::output[AIR_POS] = HIGH;
		VCU::output[AIR_NEG] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(VCU::input[TSREADY] == LOW) {
			VCU::state[SHUTDOWN] = AIR_OFF;
		}

		break;

	case READY_TO_DRIVE:
		VCU::timer++;

		if(VCU::timer > MC_CHARGE_TIME) {
			VCU::output[AIR_POS] = HIGH;
			VCU::output[AIR_NEG] = HIGH;
			VCU::output[ENABLE_COOLANT_PUMP] = HIGH;
			VCU::output[DCDC_DISABLE] = LOW;
		}

		if(VCU::input[TSREADY] == LOW) {
			VCU::state[SHUTDOWN] = AIR_OFF;
		}

		break;
	}
}

// VCU redundancy loop
void VCU::redundancy_loop() {
	if(!((VCU::input[C] > CA) && ((VCU::input[BF] > BFA) || (VCU::input[BR] > BRA))) && ((VCU::input[BF] > VOLTAGE_MIN) && (VCU::input[BF] < VOLTAGE_MAX) && (VCU::input[BR] > VOLTAGE_MIN) && (BR < VOLTAGE_MAX))) {
		VCU::input[BSPD_OK] = HIGH;
	} else {
		VCU::input[BSPD_OK] = LOW;
	}

	if(VCU::input[BSPD_OK] && VCU::input[IMD_OK] && VCU::input[BMS_OK]) {
		VCU::output[MCU_REDUNDANCY_1] = HIGH;
		VCU::output[MCU_REDUNDANCY_2] = HIGH;
	} else {
		VCU::output[MCU_REDUNDANCY_1] = LOW;
		VCU::output[MCU_REDUNDANCY_2] = LOW;
	}
}

// map the VCU input signals
void VCU::map_input(uint32_t *input) {
	for(int i = 0; i < INPUT_COUNT; i++)
		VCU::input[i] = input[i];
}

// map the VCU output signals
void VCU::map_output(uint32_t *output) {
	for(int i = 0; i < OUTPUT_COUNT; i++)
		output[i] = VCU::output[i];
}

// gets the VCU interrupt flag
bool VCU::get_flag() {
	return VCU::flag;
}

// sets the VCU interrupt flag
void VCU::set_flag() {
	VCU::flag = true;
}

// clears the VCU interrupt flag
void VCU::clear_flag() {
	VCU::flag = false;
}

// gets the state of a VCU loop
state_t VCU::get_state(loop_t loop) {
	return VCU::state[loop];
}
