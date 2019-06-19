#include "vcu.h"
#include "mc.h"
#include "io.h"

#ifdef BYPASS_DRIVER
extern uint8_t userInput;
#endif

// determines fan power based on battery temperature
static uint8_t fan_curve(int16_t temperature) {
    uint8_t power;

    return 10;

    if(temperature > (TEMPERATURE_LIMIT * 0.8)) {
        power = 15;
    } else if(temperature > (TEMPERATURE_LIMIT * 0.6)) {
        power = 15;
    } else if(temperature > (TEMPERATURE_LIMIT * 0.4)) {
        power = 10;
    } else if(temperature > (TEMPERATURE_LIMIT * 0.2)) {
        power = 5;
    } else {
        power = 0;
    }

    return(power);
}

// maps throttle position to a torque value
static int16_t torque_map(int8_t throttle, int8_t power, uint8_t limit, uint16_t front_wheels, uint16_t rear_wheels) {
    int16_t torque;

#ifdef BYPASS_DRIVER
    torque = userInput * 10;

    if(torque > 100) {
        torque = 100;
    } else if(torque < 0) {
        torque = 0;
    }
#else    
    torque = (throttle * TORQUE_MAX) / 100;

#ifdef POWER_LIMITING
    if(power > limit) {
        torque -= (power - limit) * (power - limit) * 10;
    }
#endif

#ifdef TRACTION_CONTROL
    if(rear_wheels > front_wheels) {
        torque -= rear_wheels - front_wheels;
    }
#endif
    
    if(torque > TORQUE_MAX) {
        torque = TORQUE_MAX;
    } else if(torque < TORQUE_MIN) {
        torque = TORQUE_MIN;
    }
#endif

    return(torque);
}

// checks if brakes are active
static bool brakes_active(uint32_t front, uint32_t rear) {
#ifdef BYPASS_BRAKES
    return(true);
#else
    if((front > BFA) || (rear > BRA)) {
        return(true);
    } else {
        return(false);
    }
#endif
}

// checks if brakes are valid
static bool brakes_valid(uint32_t front, uint32_t rear) {
#ifdef BYPASS_BRAKES
    return(true);
#else
    if((front > BRAKE_MIN) && (front < BRAKE_MAX) && (rear > BRAKE_MIN) && (rear < BRAKE_MAX)) {
        return(true);
    } else {
        return(false);
    }
#endif
}

// checks if throttles are valid
static bool throttles_valid(int8_t throttle_1, int8_t throttle_2) {
#ifdef BYPASS_THROTTLES
    return(true);
#else
    if(((throttle_1 - throttle_2) < 10) && ((throttle_1 - throttle_2) > -10)) {
        return(true);
    } else {
        return(false);
    }
#endif
}

// VCU class constructor
VCU::VCU() {
    input.MC_EN = DIGITAL_LOW;
    input.MC_POST_FAULT = 0;
    input.MC_RUN_FAULT = 0;
    input.MC_CURRENT = 0;
    input.MC_VOLTAGE = 0;
    input.MC_SPEED = 0;
    input.THROTTLE_1 = 0;
    input.THROTTLE_2 = 0;
    //input.THROTTLE_1_BASE = 0;
    //input.THROTTLE_2_BASE = 0;
    input.POWER_LIMIT = DEFAULT_POWER_LIMIT;
    input.LATCH_SENSE = DIGITAL_LOW;
    input.TS_READY_SENSE = DIGITAL_LOW;
    input.TS_RDY = DIGITAL_LOW;
    input.TS_LIVE = DIGITAL_LOW;
    input.CHARGER_CONNECTED = 0;
    input.BMS_VOLTAGE = 0;
    input.BMS_TEMPERATURE = 0;
    input.BMS_STATE = 0;
    input.BMS_OK = DIGITAL_LOW;
    input.IMD_OK = DIGITAL_LOW;
    input.BSPD_OK = DIGITAL_LOW;
    input.CURRENT_SENSE = 0;
    input.BRAKE_FRONT = 0;
    input.BRAKE_REAR = 0;
    input.WHEEL_SPEED_FR = 0;
    input.WHEEL_SPEED_FL = 0;
    input.WHEEL_SPEED_RR = 0;
    input.WHEEL_SPEED_RL = 0;
    input.SUPPLY_VOLTAGE = 0;

    output.MC_TORQUE = TORQUE_DIS;
    output.RTDS = DIGITAL_LOW;
    output.BRAKE_LIGHT = DIGITAL_LOW;
    output.AIR_POS = DIGITAL_LOW;
    output.AIR_NEG = DIGITAL_LOW;
    output.PUMP_EN = DIGITAL_LOW;
    output.DCDC_DISABLE = DIGITAL_LOW;
    output.PRECHARGE = DIGITAL_LOW;
    output.DISCHARGE = DIGITAL_LOW;
    output.PRECHARGE_FAILED = DIGITAL_LOW;
    output.REDUNDANT_1 = DIGITAL_LOW;
    output.REDUNDANT_2 = DIGITAL_LOW;
    output.FAN_EN = DIGITAL_LOW;
    output.FAN_PWM = DIGITAL_LOW;
    output.SUPPLY_OK = DIGITAL_LOW;
    output.STATUS = 0;

    flag = false;
}

// VCU motor loop
void VCU::motor_loop() {
    static state_t state = STATE_STANDBY;
    static uint32_t timer = 0;
    static int8_t THROTTLE_AVG;
    static int8_t MC_POWER;
    static uint16_t WHEEL_AVG_F;
    static uint16_t WHEEL_AVG_R;
    
    MC_POWER = (input.MC_VOLTAGE * input.MC_CURRENT) / 1000;
    WHEEL_AVG_F = (input.WHEEL_SPEED_FR + input.WHEEL_SPEED_FL) / 2;
    WHEEL_AVG_R = (input.WHEEL_SPEED_RR + input.WHEEL_SPEED_RL) / 2;
    THROTTLE_AVG = (input.THROTTLE_1 + input.THROTTLE_2) / 2;

    if(THROTTLE_AVG > 100) {
        THROTTLE_AVG = 100;
    } else if(THROTTLE_AVG < 0) {
        THROTTLE_AVG = 0;
    }
    
    switch(state) {
        case STATE_STANDBY:
            output.RTDS = DIGITAL_LOW;
            output.MC_TORQUE = TORQUE_DIS;

#ifdef BYPASS_DRIVER
            if((output.AIR_POS == DIGITAL_HIGH) 
               && (output.AIR_NEG == DIGITAL_HIGH)) {
#else
            if((input.MC_EN == DIGITAL_HIGH) 
               && (output.AIR_POS == DIGITAL_HIGH) 
               && (output.AIR_NEG == DIGITAL_HIGH) 
               && (THROTTLE_AVG < THROTTLE_LOW_LIMIT) 
               && brakes_active(input.BRAKE_FRONT, input.BRAKE_REAR) 
               && brakes_valid(input.BRAKE_FRONT, input.BRAKE_REAR)
               && throttles_valid(input.THROTTLE_1, input.THROTTLE_2)) {
#endif
                state = STATE_DRIVING;
                timer = 0;
                mc_clear_faults();
            }

            break;

        case STATE_DRIVING:
            if(timer > RTDS_TIME) {
                output.RTDS = DIGITAL_LOW;
                output.MC_TORQUE = torque_map(THROTTLE_AVG, MC_POWER, input.POWER_LIMIT, WHEEL_AVG_F, WHEEL_AVG_R);

#ifdef BYPASS_DRIVER
                if(input.MC_POST_FAULT 
                   || input.MC_RUN_FAULT 
                   || (output.AIR_POS == DIGITAL_LOW) 
                   || (output.AIR_NEG == DIGITAL_LOW)) {
#else
                if((input.MC_EN == DIGITAL_LOW) 
                   || input.MC_POST_FAULT 
                   || input.MC_RUN_FAULT 
                   || (output.AIR_POS == DIGITAL_LOW) 
                   || (output.AIR_NEG == DIGITAL_LOW) 
                   || ((THROTTLE_AVG > THROTTLE_HIGH_LIMIT) && brakes_active(input.BRAKE_FRONT, input.BRAKE_REAR)) 
                   || !brakes_valid(input.BRAKE_FRONT, input.BRAKE_REAR)
                   /*|| !throttles_valid(input.THROTTLE_1, input.THROTTLE_2)*/) {
#endif                    
                    state = STATE_STANDBY;
                }
            } else {
                output.RTDS = DIGITAL_HIGH;
                output.MC_TORQUE = TORQUE_DIS;
                timer++;
            }

            break;
    
        default:
            break;
    }

    if(brakes_active(input.BRAKE_FRONT, input.BRAKE_REAR)) {
        output.BRAKE_LIGHT = DIGITAL_HIGH;
    } else {
        output.BRAKE_LIGHT = DIGITAL_LOW;
    }

    output.STATUS = 0;
    output.STATUS |= (input.MC_RUN_FAULT ? 1 : 0) << 8;
    output.STATUS |= (input.MC_POST_FAULT ? 1 : 0) << 7;
    output.STATUS |= (throttles_valid(input.THROTTLE_1, input.THROTTLE_2) ? 1 : 0) << 6;
    output.STATUS |= (brakes_valid(input.BRAKE_FRONT, input.BRAKE_REAR) ? 1 : 0) << 5;
    output.STATUS |= (brakes_active(input.BRAKE_FRONT, input.BRAKE_REAR) ? 1 : 0) << 4;
    output.STATUS |= (THROTTLE_AVG < THROTTLE_LOW_LIMIT ? 1 : 0) << 3;
    output.STATUS |= output.AIR_NEG << 2;
    output.STATUS |= output.AIR_POS << 1;
    output.STATUS |= input.MC_EN << 0;
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
            output.DCDC_DISABLE = DIGITAL_LOW;
            output.PRECHARGE = DIGITAL_LOW;
            output.DISCHARGE = DIGITAL_LOW;
            output.FAN_EN = DIGITAL_LOW;
            output.FAN_PWM = 50;

            if((input.TS_READY_SENSE == DIGITAL_LOW)
               && (output.PRECHARGE_FAILED == DIGITAL_HIGH)) {
                output.PRECHARGE_FAILED = DIGITAL_LOW;
            } else if((input.TS_READY_SENSE == DIGITAL_HIGH)
                      && (output.PRECHARGE_FAILED == DIGITAL_LOW)
                      && (input.MC_CONNECTED || input.CHARGER_CONNECTED)) {
                state = STATE_PRECHARGE;
                timer = 0;
            }

            break;

        case STATE_PRECHARGE:
            output.AIR_POS = DIGITAL_LOW;
            output.AIR_NEG = DIGITAL_HIGH;
            output.PUMP_EN = DIGITAL_LOW;
            output.DCDC_DISABLE = DIGITAL_LOW;
            output.PRECHARGE = DIGITAL_HIGH;
            output.DISCHARGE = DIGITAL_HIGH;
            output.FAN_EN = DIGITAL_LOW;
            output.FAN_PWM = PWM_MIN;

            if(((timer > ALLOWED_PRECHARGE_TIME) && (input.MC_VOLTAGE < ((input.BMS_VOLTAGE * BATTERY_LIMIT) / 100)) && !input.CHARGER_CONNECTED) 
               || (input.TS_READY_SENSE == DIGITAL_LOW)) {
                output.PRECHARGE_FAILED = DIGITAL_HIGH;
                state = STATE_AIR_OFF;
            } else if((((timer > ALLOWED_PRECHARGE_TIME) && (input.MC_VOLTAGE > ((input.BMS_VOLTAGE * BATTERY_LIMIT) / 100))) 
                        || ((timer > ALLOWED_PRECHARGE_TIME) && (input.CHARGER_CONNECTED)))
                      && (input.TS_READY_SENSE == DIGITAL_HIGH)) {
                state = STATE_AIR_ON;
            } else {
                timer++;
            }

            break;

        case STATE_AIR_ON:
            output.AIR_POS = DIGITAL_HIGH;
            output.AIR_NEG = DIGITAL_HIGH;
            output.PUMP_EN = DIGITAL_LOW;
            output.DCDC_DISABLE = DIGITAL_LOW;
            output.PRECHARGE = DIGITAL_LOW;
            output.DISCHARGE = DIGITAL_HIGH;
            output.FAN_EN = DIGITAL_LOW;
            output.FAN_PWM = PWM_MIN;

            if(input.TS_READY_SENSE == DIGITAL_LOW) {
                state = STATE_AIR_OFF;
            } else if((input.TS_READY_SENSE == DIGITAL_HIGH)
                      && input.CHARGER_CONNECTED) {
                state = STATE_READY_TO_CHARGE;
            } else if((input.TS_READY_SENSE == DIGITAL_HIGH)
                      && !input.CHARGER_CONNECTED) {
                state = STATE_READY_TO_DRIVE;
                timer = 0;
            }

            break;

        case STATE_READY_TO_CHARGE:
            output.AIR_POS = DIGITAL_HIGH;
            output.AIR_NEG = DIGITAL_HIGH;
            output.PUMP_EN = DIGITAL_LOW;
            output.DCDC_DISABLE = DIGITAL_LOW;
            output.PRECHARGE = DIGITAL_LOW;
            output.DISCHARGE = DIGITAL_HIGH;
            output.FAN_EN = DIGITAL_LOW;
            output.FAN_PWM = PWM_MIN;

            if((input.TS_READY_SENSE == DIGITAL_LOW)
                    || (!input.CHARGER_CONNECTED)) {
                state = STATE_AIR_OFF;
            }

            break;

        case STATE_READY_TO_DRIVE:
            if(timer > MC_CHARGE_TIME) {
                output.AIR_POS = DIGITAL_HIGH;
                output.AIR_NEG = DIGITAL_HIGH;
                output.PUMP_EN = DIGITAL_HIGH;
                output.DCDC_DISABLE = DIGITAL_HIGH;
                output.PRECHARGE = DIGITAL_LOW;
                output.DISCHARGE = DIGITAL_HIGH;
                output.FAN_EN = DIGITAL_HIGH;
                //output.FAN_PWM = fan_curve(input.BMS_TEMPERATURE);
            } else {
                output.AIR_POS = DIGITAL_HIGH;
                output.AIR_NEG = DIGITAL_HIGH;
                output.PUMP_EN = DIGITAL_LOW;
                output.DCDC_DISABLE = DIGITAL_LOW;
                output.PRECHARGE = DIGITAL_LOW;
                output.DISCHARGE = DIGITAL_HIGH;
                output.FAN_EN = DIGITAL_LOW;
                output.FAN_PWM = PWM_MIN;
                timer++;
            }

            if((input.TS_READY_SENSE == DIGITAL_LOW)
               || (input.BMS_TEMPERATURE > TEMPERATURE_LIMIT)) {
                state = STATE_AIR_OFF;
                output.PRECHARGE_FAILED = DIGITAL_HIGH;
            }

            break;
    
        default:
            break;
    }
}

// VCU redundancy loop
void VCU::redundancy_loop() {
    static uint8_t BSPD_OK;

    if(input.CHARGER_CONNECTED) input.CHARGER_CONNECTED--;
    if(input.MC_CONNECTED) input.MC_CONNECTED--;
    
    /*if(timer > CHARGER_CONNECTED_TIME) {
        if(input.CHARGER_CONNECTED == CHARGER_CONNECTED) {
            input.CHARGER_CONNECTED = 0;
        }
        
        CHARGER_CONNECTED = input.CHARGER_CONNECTED;
        timer = 0;
    } else {
        timer++;
    }*/

    if(input.SUPPLY_VOLTAGE > SUPPLY_THRESHOLD) {
        output.SUPPLY_OK = DIGITAL_HIGH;
    } else {
        output.SUPPLY_OK = DIGITAL_LOW;
    }

    if(!((input.CURRENT_SENSE > CA) && brakes_active(input.BRAKE_FRONT, input.BRAKE_REAR)) 
       && brakes_valid(input.BRAKE_FRONT, input.BRAKE_REAR)) {
        BSPD_OK = DIGITAL_HIGH;
    } else {
        BSPD_OK = DIGITAL_LOW;
    }

    if((BSPD_OK == DIGITAL_HIGH) 
       && (input.IMD_OK == DIGITAL_HIGH) 
       && (input.BMS_OK == DIGITAL_HIGH)) {
        output.REDUNDANT_1 = DIGITAL_HIGH;
    } else {
#ifdef BYPASS_SAFETY
        output.REDUNDANT_1 = DIGITAL_HIGH;
#else
        output.REDUNDANT_1 = DIGITAL_LOW;
#endif
    }

    output.REDUNDANT_2 = DIGITAL_HIGH;
}
