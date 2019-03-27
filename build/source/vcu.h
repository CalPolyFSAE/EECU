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

#define CA			VOLTS(2.80)
#define BFA			VOLTS(2.10)
#define BRA			VOLTS(1.78)
#define BRAKE_MIN	VOLTS(0.50)
#define BRAKE_MAX	VOLTS(4.50)

#define FAN_X1 35
#define FAN_Y1 0
#define FAN_X2 58
#define FAN_Y2 60
#define FAN_GAIN ((FAN_Y2 - FAN_Y1) / (FAN_X2 - FAN_X1))
#define FAN_OFFSET (((FAN_X1 * FAN_Y2) - (FAN_X2 * FAN_Y1)) / (FAN_X1 - FAN_X2))

#define THROTTLE_MIN	5
#define THROTTLE_MAX	25
#define BATTERY_MIN		90

enum INPUT {
	MC_EN,					// GPIO E6
	MC_POST_FAULT,			// CAN 0
	MC_RUN_FAULT,			// CAN 0
	MC_VOLTAGE,				// CAN 0
	MC_TACHOMETER,			// CAN 0
	// TODO - convert input to percentage
	THROTTLE_1,				// ADC 0.14
	// TODO - convert input to percentage
	THROTTLE_2,				// ADC 0.15
	TS_RDY,					// GPIO E2
	// TODO - implement logic in shutdown loop
	TS_READY_SENSE,			// GPIO B6
	TS_LIVE,				// GPIO B7
	LATCH_SENSE,			// GPIO A1
	// TODO - read input from CAN bus
	CHARGER_CONNECTED,		// CAN 1
	// TODO - read input from CAN bus
	BMS_VOLTAGE,			// CAN 1
	// TODO - read input from CAN bus
	BMS_TEMPERATURE,		// CAN 1
	BMS_OK,					// GPIO E3
	IMD_OK,					// GPIO D16
	BSPD_OK,				// GPIO C8
	CURRENT_SENSE,			// ADC 0.6
	BRAKE_FRONT,			// ADC 0.7
	BRAKE_REAR,				// ADC 0.12
	WHEEL_SPEED_FR,			// ADC 0.13
	WHEEL_SPEED_FL,			// GPIO D6
	WHEEL_SPEED_RR,			// GPIO D5
	WHEEL_SPEED_RL,			// GPIO D7
	INPUT_COUNT,			// 24 input signals
};

enum OUTPUT {
	RTDS,					// GPIO D4
	BRAKE_LIGHT,			// GPIO B1
	AIR_POS,				// GPIO B0
	AIR_NEG,				// GPIO C9
	PUMP_EN,				// GPIO E11
	DCDC_DISABLE,			// GPIO D3
	PRECHARGE,				// GPIO B13
	DISCHARGE,				// GPIO B12
	REDUNDANT_1,			// GPIO A6
	REDUNDANT_2,			// GPIO E7
	FAN_EN,					// GPIO D0
	FAN_PWM,				// GPIO D1
	GENERAL_PURPOSE_1,		// GPIO D2
	GENERAL_PURPOSE_2,		// GPIO A7
	OUTPUT_COUNT,			// 14 output signals
};

enum SIGNAL {
	LOW,
	HIGH,
};

typedef enum STATE {
	STANDBY_STATE,
	DRIVING_STATE,
	AIR_OFF_STATE,
	PRECHARGE_STATE,
	AIR_ON_STATE,
	READY_TO_CHARGE_STATE,
	READY_TO_DRIVE_STATE,
} state_t;

class VCU {
public:
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];
	volatile bool flag;

	VCU();

	void motor_loop();
	void shutdown_loop();
	void redundancy_loop();
};

#endif
