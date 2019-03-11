#include "vcu.h"
#include "gpio.h"
#include "adc.h"

using namespace BSP;

// VCU class constructor
VCU::VCU() {
	flag = false;
	state = AIR_OFF_STATE;

	for(int i = 0; i < INPUT_COUNT; i++)
		input[i] = LOW;

	for(int i = 0; i < OUTPUT_COUNT; i++)
		output[i] = LOW;
}

// VCU motor loop
void VCU::motor_loop() {
	// TODO
}

// VCU shutdown loop
void VCU::shutdown_loop() {
	switch(state) {
	case AIR_OFF_STATE:
		output[AIR_POS] = LOW;
		output[AIR_NEG] = LOW;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;

		if(input[TS_RDY] == HIGH) {
			state = PRECHARGE_STATE;
			output[PRECHARGE] = LOW;
			timer = 0;
		}

		break;

	case PRECHARGE_STATE:
		output[AIR_POS] = LOW;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;
		timer++;

		if(((timer > ALLOWED_PRECHARGE_TIME) && (input[MC_VOLTAGE] < ((input[BMS_VOLTAGE] * BATTERY_THRESHOLD) / 100)) && (input[LATCH_SENSE] == LOW)) || (input[TS_RDY] == LOW)) {
			state = AIR_OFF_STATE;
			output[PRECHARGE] = HIGH;
		} else if((((timer > ALLOWED_PRECHARGE_TIME) && (input[MC_VOLTAGE] > ((input[BMS_VOLTAGE] * BATTERY_THRESHOLD) / 100))) || (input[LATCH_SENSE] == HIGH)) && (input[TS_RDY] == HIGH)) {
			state = AIR_ON_STATE;
		}

		break;

	case AIR_ON_STATE:
		output[AIR_POS] = HIGH;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		} else if((input[LATCH_SENSE] == LOW) && (input[TS_RDY] == HIGH)) {
			state = READY_TO_DRIVE_STATE;
			timer = 0;
		} else if((input[LATCH_SENSE] == HIGH) && (VCU::input[TS_RDY] == HIGH)) {
			state = READY_TO_CHARGE_STATE;
		}

		break;

	case READY_TO_CHARGE_STATE:
		output[AIR_POS] = HIGH;
		output[AIR_NEG] = HIGH;
		output[PUMP_EN] = LOW;
		output[DCDC_DISABLE] = HIGH;

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		}

		break;

	case READY_TO_DRIVE_STATE:
		timer++;

		if(timer > MC_CHARGE_TIME) {
			output[AIR_POS] = HIGH;
			output[AIR_NEG] = HIGH;
			output[PUMP_EN] = HIGH;
			output[DCDC_DISABLE] = LOW;
		}

		if(input[TS_RDY] == LOW) {
			state = AIR_OFF_STATE;
		}

		break;
	}
}

// VCU redundancy loop
void VCU::redundancy_loop() {
	if(!((input[CURRENT_SENSE] > CA) && ((input[BRAKE_FRONT] > BFA) || (input[BRAKE_REAR] > BRA))) && ((input[BRAKE_FRONT] > VOLTAGE_MIN) && (input[BRAKE_FRONT] < VOLTAGE_MAX) && (input[BRAKE_REAR] > VOLTAGE_MIN) && (input[BRAKE_REAR] < VOLTAGE_MAX))) {
		VCU::input[BSPD_OK] = HIGH;
	} else {
		VCU::input[BSPD_OK] = LOW;
	}

	if(input[BSPD_OK] && input[IMD_OK] && input[BMS_OK]) {
		output[REDUNDANT_1] = HIGH;
		output[REDUNDANT_2] = HIGH;
	} else {
		output[REDUNDANT_1] = LOW;
		output[REDUNDANT_2] = LOW;
	}
}

// map input signals from hardware
void VCU::input_map() {
	gpio::GPIO &gpio = gpio::GPIO::StaticClass();
	adc::ADC &adc = adc::ADC::ADC::StaticClass();

	input[THROTTLE_1] = adc.read(ADC0, 14);
	input[THROTTLE_2] = adc.read(ADC0, 15);
	input[TS_RDY] = gpio.read(gpio::PortE, 2);
	input[TS_READY_SENSE] = gpio.read(gpio::PortB, 6);
	input[TS_LIVE] = gpio.read(gpio::PortB, 7);
	input[LATCH_SENSE] = gpio.read(gpio::PortA, 1);
	input[BMS_OK] = gpio.read(gpio::PortE, 3);
	input[IMD_OK] = gpio.read(gpio::PortD, 16);
	input[BSPD_OK] = gpio.read(gpio::PortC, 8);
	input[CURRENT_SENSE] = adc.read(ADC0, 6);
	input[BRAKE_FRONT] = adc.read(ADC0, 7);
	input[BRAKE_REAR] = adc.read(ADC0, 12);
	input[WHEEL_SPEED_FR] = adc.read(ADC0, 13);
	input[WHEEL_SPEED_FL] = gpio.read(gpio::PortD, 6);
	input[WHEEL_SPEED_RR] = gpio.read(gpio::PortD, 5);
	input[WHEEL_SPEED_RL] = gpio.read(gpio::PortD, 7);
}

// map output signals to hardware
void VCU::output_map() {
	gpio::GPIO &gpio = gpio::GPIO::StaticClass();

    if(output[RTDS] == HIGH)
    	gpio.set(gpio::PortD, 4);
    else
    	gpio.clear(gpio::PortD, 4);

    if(output[BRAKE_LIGHT] == HIGH)
    	gpio.set(gpio::PortB, 1);
    else
    	gpio.clear(gpio::PortB, 1);

    if(output[AIR_POS] == HIGH)
    	gpio.set(gpio::PortB, 0);
    else
    	gpio.clear(gpio::PortB, 0);

    if(output[AIR_NEG] == HIGH)
    	gpio.set(gpio::PortC, 9);
    else
    	gpio.clear(gpio::PortC, 9);

    if(output[PUMP_EN] == HIGH)
    	gpio.set(gpio::PortE, 11);
    else
    	gpio.clear(gpio::PortE, 11);

    if(output[DCDC_DISABLE] == HIGH)
    	gpio.set(gpio::PortD, 13);
    else
    	gpio.clear(gpio::PortD, 13);

    if(output[PRECHARGE] == HIGH)
    	gpio.set(gpio::PortB, 13);
    else
    	gpio.clear(gpio::PortB, 13);

    if(output[DISCHARGE] == HIGH)
    	gpio.set(gpio::PortB, 12);
    else
    	gpio.clear(gpio::PortB, 12);

    if(output[REDUNDANT_1] == HIGH)
    	gpio.set(gpio::PortA, 6);
    else
    	gpio.clear(gpio::PortA, 6);

    if(output[REDUNDANT_2] == HIGH)
    	gpio.set(gpio::PortE, 7);
    else
    	gpio.clear(gpio::PortE, 7);

    if(output[FAN_EN] == HIGH)
    	gpio.set(gpio::PortD, 0);
    else
    	gpio.clear(gpio::PortD, 0);

    if(output[FAN_PWM] == HIGH)
    	gpio.set(gpio::PortD, 1);
    else
    	gpio.clear(gpio::PortD, 1);

    if(output[GENERAL_PURPOSE_1] == HIGH)
    	gpio.set(gpio::PortD, 2);
    else
    	gpio.clear(gpio::PortD, 2);

    if(output[GENERAL_PURPOSE_2] == HIGH)
    	gpio.set(gpio::PortA, 7);
    else
    	gpio.clear(gpio::PortA, 7);
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
state_t VCU::get_state() {
	return VCU::state;
}
