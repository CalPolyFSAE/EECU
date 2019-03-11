#ifndef VCU_H_
#define VCU_H_

#include <stdint.h>

#define VCU_FREQUENCY 100U
#define CLOCK_FREQUENCY 60000000U

#define TIMER_PERIOD (CLOCK_FREQUENCY / VCU_FREQUENCY)

#define RTDS_TIME (2 * VCU_FREQUENCY)
#define ALLOWED_PRECHARGE_TIME (4 * VCU_FREQUENCY)
#define MC_CHARGE_TIME (1 * VCU_FREQUENCY)
#define BATTERY_THRESHOLD 90

#define TA 41				// 0.05V
#define CA 2293				// 2.80V
#define BFA 1720			// 2.10V
#define BRA 1458			// 1.78V
#define VOLTAGE_MIN 410		// 0.50V
#define VOLTAGE_MAX	3686	// 4.50V

enum INPUT {
	MC_STATE,				// CAN 0 - DEBUG
	MC_EN,					// GPIO E6
	MC_POST_FAULT,			// CAN 0
	MC_RUN_FAULT,			// CAN 0
	MC_VOLTAGE,				// CAN 0
	MC_TACHOMETER,			// CAN 0
	THROTTLE_1,				// ADC 0.14
	THROTTLE_2,				// ADC 0.15
	TS_RDY,					// GPIO E2
	TS_READY_SENSE,			// GPIO B6 - ???
	TS_LIVE,				// GPIO B7 - ???
	LATCH_SENSE,			// GPIO A1 - ???
	BMS_VOLTAGE,			// CAN 1
	BMS_OK,					// GPIO E3
	IMD_OK,					// GPIO D16
	BSPD_OK,				// GPIO C8 - ???
	CURRENT_SENSE,			// ADC 0.6
	BRAKE_FRONT,			// ADC 0.7
	BRAKE_REAR,				// ADC 0.12
	WHEEL_SPEED_FR,			// ADC 0.13
	WHEEL_SPEED_FL,			// GPIO D6
	WHEEL_SPEED_RR,			// GPIO D5
	WHEEL_SPEED_RL,			// GPIO D7
	INPUT_COUNT,			// 23 input signals
};

enum OUTPUT {
	RTDS,					// GPIO D4
	BRAKE_LIGHT,			// GPIO B1 - ???
	AIR_POS,				// GPIO B0
	AIR_NEG,				// GPIO C9
	PUMP_EN,				// GPIO E11
	DCDC_DISABLE,			// GPIO D3
	PRECHARGE,				// GPIO B13 - ???
	DISCHARGE,				// GPIO B12 - ???
	REDUNDANT_1,			// GPIO A6
	REDUNDANT_2,			// GPIO E7
	FAN_EN,					// GPIO D0 - ???
	FAN_PWM,				// GPIO D1 - ???
	GENERAL_PURPOSE_1,		// GPIO D2
	GENERAL_PURPOSE_2,		// GPIO A7
	OUTPUT_COUNT,			// 14 output signals
};

enum SIGNAL {
	LOW,
	HIGH,
};

typedef enum STATE {
	AIR_OFF_STATE,
	PRECHARGE_STATE,
	AIR_ON_STATE,
	READY_TO_CHARGE_STATE,
	READY_TO_DRIVE_STATE,
} state_t;

class VCU {
private:
	volatile bool flag;
	state_t state;
	uint32_t timer;


public:
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];

	VCU();

	void motor_loop();
	void shutdown_loop();
	void redundancy_loop();

	void input_map();
	void output_map();

	bool get_flag();
	void set_flag();
	void clear_flag();

	state_t get_state();
};

#endif
