#include "vcu.h"

VCU::VCU() {
	int i;

	VCU::flag = false;
	VCU::state = AIR_OFF;

	for(i = 0; i < INPUT_COUNT; i++)
		VCU::input[i] = LOW;

	for(i = 0; i < OUTPUT_COUNT; i++)
		VCU::output[i] = LOW;
}

void VCU::shutdown_loop() {
	if(VCU::flag == true)
		return;
	else
		VCU::flag = true;

	switch(VCU::state) {
	case AIR_OFF:
		VCU::output[POS_AIR_ENABLE] = LOW;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if((VCU::input[TSREADY] == LOW) && (VCU::input[PRECHARGE_FAILED] == LOW)) {
			VCU::state = PRECHARGE;
			VCU::output[PRECHARGE_FAILED] = LOW;
			VCU::timer = time(nullptr);
		}

		break;

	case PRECHARGE:
		VCU::output[POS_AIR_ENABLE] = LOW;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(((difftime(time(nullptr), VCU::timer) > ALLOWED_PRECHARGE_TIME) && (VCU::input[MC_VOLTAGE] < BATTERY_PERCENTAGE * VCU::input[BMS_VOLTAGE] / 100) && (VCU::input[CHARGER_CONNECTED] == LOW)) || (VCU::input[TSREADY] == HIGH)) {
			VCU::state = AIR_OFF;
			VCU::output[PRECHARGE_FAILED] = HIGH;
		} else if((((difftime(time(nullptr), VCU::timer) > ALLOWED_PRECHARGE_TIME) && (VCU::input[MC_VOLTAGE] > BATTERY_PERCENTAGE * VCU::input[BMS_VOLTAGE] / 100)) || (VCU::input[CHARGER_CONNECTED] == HIGH)) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state = AIR_ON;
		}

		break;

	case AIR_ON:
		VCU::output[POS_AIR_ENABLE] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if((VCU::input[CHARGER_CONNECTED] == LOW) && (VCU::input[TSREADY]) == HIGH) {
			VCU::state = READY_TO_DRIVE;
			VCU::timer = time(nullptr);
		} else if((VCU::input[CHARGER_CONNECTED] == HIGH) && (VCU::input[TSREADY] == HIGH)) {
			VCU::state = READY_TO_CHARGE;
		}

		break;

	case READY_TO_CHARGE:
		VCU::output[POS_AIR_ENABLE] = HIGH;
		VCU::output[ENABLE_COOLANT_PUMP] = LOW;
		VCU::output[DCDC_DISABLE] = HIGH;

		if(VCU::input[TSREADY] == LOW)
			VCU::state = AIR_OFF;

		break;

	case READY_TO_DRIVE:
		if(difftime(time(nullptr), VCU::timer) > MC_CHARGE_TIME) {
			VCU::output[POS_AIR_ENABLE] = HIGH;
			VCU::output[ENABLE_COOLANT_PUMP] = HIGH;
			VCU::output[DCDC_DISABLE] = LOW;
		}

		if(VCU::input[TSREADY] == LOW)
			VCU::state = AIR_OFF;

		break;
	}
}
