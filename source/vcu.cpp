#include <string.h>

#include "vcu.h"
#include "mc.h"

// VCU class constructor
VCU::VCU() {
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(input));
	flag = false;
}

// VCU motor loop
void VCU::motor_loop() {
	static state_t state = STATE_STANDBY;
	static uint32_t timer = 0;
	uint32_t THROTTLE_AVG;

	THROTTLE_AVG = ((((input.THROTTLE_1 - THROTTLE_POS_MIN) * 50) / (THROTTLE_POS_MAX - THROTTLE_POS_MIN)) -
	               (((input.THROTTLE_2 - THROTTLE_NEG_MIN) * 50) / (THROTTLE_NEG_MAX - THROTTLE_NEG_MIN))) + 50;

	// TODO - calculate wheel speeds

	switch(state) {
	case STATE_STANDBY:
		mc_torque_request(-1);

		if((input.MC_EN == DIGITAL_HIGH) && !input.MC_POST_FAULT && !input.MC_RUN_FAULT && (THROTTLE_AVG < THROTTLE_AVG_MIN) &&
				(output.AIR_POS == DIGITAL_HIGH) && (output.AIR_NEG == DIGITAL_HIGH) && ((input.BRAKE_FRONT > BFA) || (input.BRAKE_REAR > BRA)) &&
				((input.BRAKE_FRONT > BRAKE_MIN) && (input.BRAKE_FRONT < BRAKE_MAX) && (input.BRAKE_REAR > BRAKE_MIN) && (input.BRAKE_REAR < BRAKE_MAX))) {
			state = STATE_DRIVING;
			mc_clear_faults();
			mc_torque_request(-1);
			mc_torque_request(0);
			timer = 0;
		}

		break;

	case STATE_DRIVING:
		if(timer > RTDS_TIME) {
			output.RTDS = DIGITAL_LOW;
			// TODO - convert throttle input to torque request
			// TODO - power limiting
			// TODO - traction control
			mc_torque_request(0);
		} else {
			output.RTDS = DIGITAL_HIGH;
			mc_torque_request(0);
			timer++;
		}

		if((input.MC_EN == DIGITAL_LOW) || input.MC_POST_FAULT || input.MC_RUN_FAULT || (output.AIR_POS == DIGITAL_LOW) || (output.AIR_NEG == DIGITAL_LOW) ||
				((THROTTLE_AVG > THROTTLE_AVG_MAX) && ((input.BRAKE_FRONT > BFA) || (input.BRAKE_REAR > BRA))) ||
				((input.BRAKE_FRONT < BRAKE_MIN) || (input.BRAKE_FRONT > BRAKE_MAX) || (input.BRAKE_REAR < BRAKE_MIN) || (input.BRAKE_REAR > BRAKE_MAX))) {
			state = STATE_STANDBY;
		}

		break;

	default:
		break;
	}

	if((input.BRAKE_FRONT > BFA) || (input.BRAKE_REAR > BRA)) {
		output.BRAKE_LIGHT = DIGITAL_HIGH;
	} else {
		output.BRAKE_LIGHT = DIGITAL_LOW;
	}
}

// VCU shutdown loop
void VCU::shutdown_loop() {
	static state_t state = STATE_AIR_OFF;
	static uint32_t timer = 0;

	switch(state) {
	case STATE_AIR_OFF:
		output.AIR_POS = DIGITAL_LOW;
		output.AIR_NEG = DIGITAL_LOW;
		output.PUMP_EN = DIGITAL_LOW;
		output.DCDC_DISABLE = DIGITAL_HIGH;
		output.PRECHARGE = DIGITAL_LOW;
		output.DISCHARGE = DIGITAL_LOW;
		output.FAN_EN = DIGITAL_LOW;
		output.FAN_PWM = 0;

		if(input.TS_READY_SENSE == DIGITAL_HIGH) {
			state = STATE_PRECHARGE;
			output.PRECHARGE_FAILED = DIGITAL_LOW;
			timer = 0;
		}

		break;

	case STATE_PRECHARGE:
		output.AIR_POS = DIGITAL_LOW;
		output.AIR_NEG = DIGITAL_HIGH;
		output.PUMP_EN = DIGITAL_LOW;
		output.DCDC_DISABLE = DIGITAL_HIGH;
		output.PRECHARGE = DIGITAL_HIGH;
		output.DISCHARGE = DIGITAL_HIGH;
		output.FAN_EN = DIGITAL_LOW;
		output.FAN_PWM = 0;

		if(((timer > ALLOWED_PRECHARGE_TIME) && (input.MC_VOLTAGE < ((input.BMS_VOLTAGE * BATTERY_MIN) / 100)) && (input.CHARGER_CONNECTED == DIGITAL_LOW)) ||
				(input.TS_READY_SENSE == DIGITAL_LOW)) {
			state = STATE_AIR_OFF;
			output.PRECHARGE_FAILED = DIGITAL_HIGH;
		} else if((((timer > ALLOWED_PRECHARGE_TIME) && (input.MC_VOLTAGE > ((input.BMS_VOLTAGE * BATTERY_MIN) / 100))) || (input.CHARGER_CONNECTED == DIGITAL_HIGH)) &&
				(input.TS_READY_SENSE == DIGITAL_HIGH)) {
			state = STATE_AIR_ON;
		} else {
			timer++;
		}

		break;

	case STATE_AIR_ON:
		output.AIR_POS = DIGITAL_HIGH;
		output.AIR_NEG = DIGITAL_HIGH;
		output.PUMP_EN = DIGITAL_LOW;
		output.DCDC_DISABLE = DIGITAL_HIGH;
		output.PRECHARGE = DIGITAL_LOW;
		output.DISCHARGE = DIGITAL_HIGH;
		output.FAN_EN = DIGITAL_LOW;
		output.FAN_PWM = 0;

		if(input.TS_READY_SENSE == DIGITAL_LOW) {
			state = STATE_AIR_OFF;
		} else if((input.CHARGER_CONNECTED == DIGITAL_LOW) && (input.TS_READY_SENSE == DIGITAL_HIGH)) {
			state = STATE_READY_TO_DRIVE;
			timer = 0;
		} else if((input.CHARGER_CONNECTED == DIGITAL_HIGH) && (input.TS_READY_SENSE == DIGITAL_HIGH)) {
			state = STATE_READY_TO_CHARGE;
		}

		break;

	case STATE_READY_TO_CHARGE:
		output.AIR_POS = DIGITAL_HIGH;
		output.AIR_NEG = DIGITAL_HIGH;
		output.PUMP_EN = DIGITAL_LOW;
		output.DCDC_DISABLE = DIGITAL_HIGH;
		output.PRECHARGE = DIGITAL_LOW;
		output.DISCHARGE = DIGITAL_HIGH;
		output.FAN_EN = DIGITAL_LOW;
		output.FAN_PWM = 0;

		if(input.TS_READY_SENSE == DIGITAL_LOW) {
			state = STATE_AIR_OFF;
		}

		break;

	case STATE_READY_TO_DRIVE:
		if(timer > MC_CHARGE_TIME) {
			output.AIR_POS = DIGITAL_HIGH;
			output.AIR_NEG = DIGITAL_HIGH;
			output.PUMP_EN = DIGITAL_HIGH;
			output.DCDC_DISABLE = DIGITAL_LOW;
			output.PRECHARGE = DIGITAL_LOW;
			output.DISCHARGE = DIGITAL_HIGH;
			output.FAN_EN = DIGITAL_LOW;
			output.FAN_PWM = (FAN_GAIN * input.BMS_TEMPERATURE) + FAN_OFFSET;
		} else {
			timer++;
		}

		if(input.TS_READY_SENSE == DIGITAL_LOW) {
			state = STATE_AIR_OFF;
		}

		break;

	default:
		break;
	}
}

// VCU redundancy loop
void VCU::redundancy_loop() {
	uint32_t BSPD_OK;

	if(!((input.CURRENT_SENSE > CA) && ((input.BRAKE_FRONT > BFA) || (input.BRAKE_REAR > BRA))) &&
			((input.BRAKE_FRONT > BRAKE_MIN) && (input.BRAKE_FRONT < BRAKE_MAX) && (input.BRAKE_REAR > BRAKE_MIN) && (input.BRAKE_REAR < BRAKE_MAX))) {
		BSPD_OK = DIGITAL_HIGH;
	} else {
		BSPD_OK = DIGITAL_LOW;
	}

	if(BSPD_OK && input.IMD_OK && input.BMS_OK) {
		output.REDUNDANT_1 = DIGITAL_HIGH;
		output.REDUNDANT_2 = DIGITAL_HIGH;
	} else {
		output.REDUNDANT_1 = DIGITAL_LOW;
		output.REDUNDANT_2 = DIGITAL_LOW;
	}
}
