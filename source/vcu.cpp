#include "vcu.h"

// VCU class constructor
VCU::VCU() {
	int i;

	VCU::flag = false;
	VCU::state = AIR_OFF;

	for(i = 0; i < INPUT_COUNT; i++)
		VCU::input[i] = LOW;

	for(i = 0; i < OUTPUT_COUNT; i++)
		VCU::output[i] = LOW;
}

// VCU shutdown loop
void VCU::shutdown_loop() {
	switch(VCU::state) {
	case AIR_OFF:
		VCU::output[AIR_POS] = LOW;
		VCU::output[AIR_NEG] = LOW;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(input[TSREADY] == HIGH) {
			VCU::state = PRECHARGE;
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
			VCU::state = AIR_OFF;
			VCU::output[PRECHARGE_FAILED] = HIGH;
		} else if((((VCU::timer > ALLOWED_PRECHARGE_TIME) && (VCU::input[MC_VOLTAGE] > ((VCU::input[BMS_VOLTAGE] * BATTERY_PERCENTAGE) / 100))) || (VCU::input[CHARGER_CONNECTED] == HIGH)) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state = AIR_ON;
		}

		break;

	case AIR_ON:
		VCU::output[AIR_POS] = HIGH;
		VCU::output[AIR_NEG] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(VCU::input[TSREADY] == LOW) {
			VCU::state = AIR_OFF;
		} else if((VCU::input[CHARGER_CONNECTED] == LOW) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state = READY_TO_DRIVE;
			VCU::timer = 0;
		} else if((VCU::input[CHARGER_CONNECTED] == HIGH) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state = READY_TO_CHARGE;
		}

		break;

	case READY_TO_CHARGE:
		VCU::output[AIR_POS] = HIGH;
		VCU::output[AIR_NEG] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(VCU::input[TSREADY] == LOW) {
			VCU::state = AIR_OFF;
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
			VCU::state = AIR_OFF;
		}

		break;
	}
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

// gets the current state of the VCU
state_t VCU::get_state() {
	return VCU::state;
}

// sets the VCU input signals
void VCU::set_input(uint32_t *input) {
	int i;

	for(i = 0; i < INPUT_COUNT; i++)
		VCU::input[i] = input[i];
}

// gets the VCU output signals
void VCU::get_output(uint32_t *output) {
	int i;

	for(i = 0; i < OUTPUT_COUNT; i++)
		output[i] = VCU::output[i];
}
