#ifndef VCU_H
#define VCU_H

#include <stdint.h>

#define VCU_FREQUENCY 100U
#define CLOCK_FREQUENCY 60000000U

#define TIMER_PERIOD (CLOCK_FREQUENCY / VCU_FREQUENCY)

#define VOLTS(V)	(((V) * 4095) / 5)
#define SECONDS(S) 	((S) * VCU_FREQUENCY)

#define RTDS_TIME				SECONDS(2)
#define ALLOWED_PRECHARGE_TIME	SECONDS(4)
#define MC_CHARGE_TIME			SECONDS(1)

#define CA					VOLTS(2.80)
#define BFA					VOLTS(2.10)
#define BRA					VOLTS(1.78)
#define BRAKE_MIN			VOLTS(0.50)
#define BRAKE_MAX			VOLTS(4.50)
#define THROTTLE_POS_MIN	VOLTS(0.50)
#define THROTTLE_POS_MAX	VOLTS(4.50)
#define THROTTLE_NEG_MIN	VOLTS(0.50)
#define THROTTLE_NEG_MAX	VOLTS(4.50)

#define FAN_X1 35
#define FAN_Y1 0
#define FAN_X2 58
#define FAN_Y2 60
#define FAN_GAIN ((FAN_Y2 - FAN_Y1) / (FAN_X2 - FAN_X1))
#define FAN_OFFSET (((FAN_X1 * FAN_Y2) - (FAN_X2 * FAN_Y1)) / (FAN_X1 - FAN_X2))

#define THROTTLE_AVG_MIN	5
#define THROTTLE_AVG_MAX	25
#define BATTERY_MIN			90

enum DIGITAL {
	DIGITAL_LOW,
	DIGITAL_HIGH,
};

typedef enum STATE {
	STATE_STANDBY,
	STATE_DRIVING,
	STATE_AIR_OFF,
	STATE_PRECHARGE,
	STATE_AIR_ON,
	STATE_READY_TO_CHARGE,
	STATE_READY_TO_DRIVE,
} state_t;

typedef struct {
	uint8_t MC_EN;				// GPIO E6
	uint32_t MC_POST_FAULT;		// CAN 1
	uint32_t MC_RUN_FAULT;		// CAN 1
	int16_t MC_CURRENT;			// CAN 1
	int16_t MC_VOLTAGE;			// CAN 1
	int16_t MC_SPEED;			// CAN 1
	uint32_t THROTTLE_1;		// ADC 0.14
	uint32_t THROTTLE_2;		// ADC 0.15

	// TODO - put signal on CAN bus
	uint8_t LATCH_SENSE;		// GPIO A1

	uint8_t TS_READY_SENSE;		// GPIO B6
	uint8_t TS_RDY;				// GPIO E2
	uint8_t TS_LIVE;			// GPIO B7

	// TODO - read input from CAN bus
	uint8_t CHARGER_CONNECTED;	// CAN 0

	// TODO - read input from CAN bus
	uint32_t BMS_VOLTAGE;		// CAN 0

	// TODO - read input from CAN bus
	uint32_t BMS_TEMPERATURE;	// CAN 0

	uint8_t BMS_OK;				// GPIO E3
	uint8_t IMD_OK;				// GPIO D16
	uint8_t BSPD_OK;			// GPIO C8
	uint32_t CURRENT_SENSE;		// ADC 0.6
	uint32_t BRAKE_FRONT;		// ADC 0.7
	uint32_t BRAKE_REAR;		// ADC 0.12
	uint32_t WHEEL_SPEED_FR;	// ADC 0.13
	uint8_t WHEEL_SPEED_FL;		// GPIO D6
	uint8_t WHEEL_SPEED_RR;		// GPIO D5
	uint8_t WHEEL_SPEED_RL;		// GPIO D7
} input_t;

typedef struct {
	uint8_t RTDS;				// GPIO D4
	uint8_t BRAKE_LIGHT;		// GPIO B1
	uint8_t AIR_POS;			// GPIO B0
	uint8_t AIR_NEG;			// GPIO C9
	uint8_t PUMP_EN;			// GPIO E11
	uint8_t DCDC_DISABLE;		// GPIO D3
	uint8_t PRECHARGE;			// GPIO B13
	uint8_t DISCHARGE;			// GPIO B12

	// TODO - put signal on CAN bus
	uint8_t PRECHARGE_FAILED;	// CAN 0

	uint8_t REDUNDANT_1;		// GPIO E7
	uint8_t REDUNDANT_2;		// GPIO A6
	uint8_t FAN_EN;				// GPIO D0
	uint8_t FAN_PWM;			// GPIO D1
	uint8_t GENERAL_PURPOSE_1;	// GPIO A7
	uint8_t GENERAL_PURPOSE_2;	// GPIO D2
} output_t;

class VCU {
public:
	input_t input;
	output_t output;
	volatile bool flag;

	VCU();

	void motor_loop();
	void shutdown_loop();
	void redundancy_loop();
};

#endif
