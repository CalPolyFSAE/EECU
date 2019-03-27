#include "vcu.h"
#include "mc.h"

// VCU class constructor
VCU::VCU() {
	for(int i = 0; i < INPUT_COUNT; i++) {
		input[i] = LOW;
	}

	for(int i = 0; i < OUTPUT_COUNT; i++) {
		output[i] = LOW;
	}

	flag = false;
}

// VCU motor loop
void VCU::motor_loop() {
	static state_t state = STANDBY_STATE;
	static uint32_t timer = 0;

	if((input[BRAKE_FRONT] > BFA) || (input[BRAKE_REAR] > BRA)) {
		output[BRAKE_LIGHT] = HIGH;
	} else {
		output[BRAKE_LIGHT] = LOW;
	}

	switch(state) {
	case STANDBY_STATE:
		mc_torque_command(-1);

		if((input[MC_EN] == HIGH) && !input[MC_POST_FAULT] && !input[MC_RUN_FAULT] &&
		   (((input[THROTTLE_1] + input[THROTTLE_2]) / 2) < THROTTLE_MIN) &&
		   ((input[BRAKE_FRONT] > BFA) || (input[BRAKE_REAR] > BRA)) &&
		   ((input[BRAKE_FRONT] > BRAKE_MIN) && (input[BRAKE_FRONT] < BRAKE_MAX) && (input[BRAKE_REAR] > BRAKE_MIN) && (input[BRAKE_REAR] < BRAKE_MAX)) &&
		   (output[AIR_POS] == HIGH) && (output[AIR_NEG] == HIGH)) {
			state = DRIVING_STATE;
			mc_clear_faults();
			mc_torque_command(-1);
			mc_torque_command(0);
			timer = 0;
		}

		break;

	case DRIVING_STATE:
		if(timer > RTDS_TIME) {
			output[RTDS] = LOW;
			// TODO - convert throttle input to torque request
			// TODO - power limiting
			// TODO - traction control
			mc_torque_command(1);
		} else {
			output[RTDS] = HIGH;
			mc_torque_command(0);
			timer++;
		}

		if((input[MC_EN] == LOW) || input[MC_POST_FAULT] || input[MC_RUN_FAULT] ||
		((((input[THROTTLE_1] + input[THROTTLE_2]) / 2) > THROTTLE_MAX) && ((input[BRAKE_FRONT] > BFA) || (input[BRAKE_REAR] > BRA))) ||
		((input[BRAKE_FRONT] < BRAKE_MIN) || (input[BRAKE_FRONT] > BRAKE_MAX) || (input[BRAKE_REAR] < BRAKE_MIN) || (input[BRAKE_REAR] > BRAKE_MAX)) ||
		(output[AIR_POS] == LOW) || (output[AIR_NEG] == LOW)) {
			state = STANDBY_STATE;
		}

		break;

	default:
		break;
	}
}

// VCU shutdown loop
void VCU::shutdown_loop() {
	static state_t state = AIR_OFF_STATE;
	static uint32_t timer = 0;
	uint32_t PRECHARGE_FAILED;

	switch(state) {
	case AIR_OFF_STATE:
		output[AIR_POS] = LOW;
		output[AIR_NEG] = LOW;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;
		output[PRECHARGE] = LOW;
		output[DISCHARGE] = LOW;
		output[FAN_EN] = LOW;
		output[FAN_PWM] = 0;

		if(input[TS_RDY] == HIGH) {
			state = PRECHARGE_STATE;
			PRECHARGE_FAILED = LOW;
			timer = 0;
		}

		break;

	case PRECHARGE_STATE:
		output[AIR_POS] = LOW;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;
		output[PRECHARGE] = HIGH;
		output[DISCHARGE] = HIGH;
		output[FAN_EN] = LOW;
		output[FAN_PWM] = 0;

		if(((timer > ALLOWED_PRECHARGE_TIME) && (input[MC_VOLTAGE] < ((input[BMS_VOLTAGE] * BATTERY_MIN) / 100)) && (input[CHARGER_CONNECTED] == LOW)) ||
		   (input[TS_RDY] == LOW)) {
			state = AIR_OFF_STATE;
			PRECHARGE_FAILED = HIGH;
		} else if((((timer > ALLOWED_PRECHARGE_TIME) && (input[MC_VOLTAGE] > ((input[BMS_VOLTAGE] * BATTERY_MIN) / 100))) || (input[CHARGER_CONNECTED] == HIGH)) &&
				  (input[TS_RDY] == HIGH)) {
			state = AIR_ON_STATE;
		} else {
			timer++;
		}

		break;

	case AIR_ON_STATE:
		output[AIR_POS] = HIGH;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;
		output[PRECHARGE] = LOW;
		output[DISCHARGE] = HIGH;
		output[FAN_EN] = LOW;
		output[FAN_PWM] = 0;

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		} else if((input[CHARGER_CONNECTED] == LOW) && (input[TS_RDY] == HIGH)) {
			state = READY_TO_DRIVE_STATE;
			timer = 0;
		} else if((input[CHARGER_CONNECTED] == HIGH) && (VCU::input[TS_RDY] == HIGH)) {
			state = READY_TO_CHARGE_STATE;
		}

		break;

	case READY_TO_CHARGE_STATE:
		output[AIR_POS] = HIGH;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;
		output[PRECHARGE] = LOW;
		output[DISCHARGE] = HIGH;
		output[FAN_EN] = LOW;
		output[FAN_PWM] = 0;

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		}

		break;

	case READY_TO_DRIVE_STATE:
		if(timer > MC_CHARGE_TIME) {
			output[AIR_POS] = HIGH;
			output[AIR_NEG] = HIGH;
			output[PUMP_EN] = HIGH;
			output[DCDC_DISABLE] = LOW;
			output[PRECHARGE] = LOW;
			output[DISCHARGE] = HIGH;
			output[FAN_EN] = LOW;
			output[FAN_PWM] = (FAN_GAIN * input[BMS_TEMPERATURE]) + FAN_OFFSET;
		} else {
			timer++;
		}

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		}

		break;

	default:
		break;
	}

	output[GENERAL_PURPOSE_1] = PRECHARGE_FAILED;
}

// VCU redundancy loop
void VCU::redundancy_loop() {
	uint32_t BSPD_OK;

	if(!((input[CURRENT_SENSE] > CA) && ((input[BRAKE_FRONT] > BFA) || (input[BRAKE_REAR] > BRA))) &&
	   ((input[BRAKE_FRONT] > BRAKE_MIN) && (input[BRAKE_FRONT] < BRAKE_MAX) && (input[BRAKE_REAR] > BRAKE_MIN) && (input[BRAKE_REAR] < BRAKE_MAX))) {
		BSPD_OK = HIGH;
	} else {
		BSPD_OK = LOW;
	}

	if(BSPD_OK && input[IMD_OK] && input[BMS_OK]) {
		output[REDUNDANT_1] = HIGH;
		output[REDUNDANT_2] = HIGH;
	} else {
		output[REDUNDANT_1] = LOW;
		output[REDUNDANT_2] = LOW;
	}
}
