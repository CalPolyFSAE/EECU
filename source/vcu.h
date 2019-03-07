#ifndef VCU_H_
#define VCU_H_

#include <stdint.h>

#define VCU_FREQUENCY 100U
#define CLOCK_FREQUENCY 60000000U

#define TIMER_PERIOD (CLOCK_FREQUENCY / VCU_FREQUENCY)

#define RTDS_TIME (2 * VCU_FREQUENCY)
#define ALLOWED_PRECHARGE_TIME (4 * VCU_FREQUENCY)
#define MC_CHARGE_TIME (1 * VCU_FREQUENCY)
#define BATTERY_PERCENTAGE 90

#define TA 41				// 0.05V
#define CA 2293				// 2.80V
#define BFA 1720			// 2.10V
#define BRA 1458			// 1.78V
#define VOLTAGE_MIN 410		// 0.50V
#define VOLTAGE_MAX	3686	// 4.50V

enum INPUT {
	//MC_ENABLE,
	MC_FAULT,
	MC_VOLTAGE,
	MC_TACHOMETER,
	//THROTTLE_AVG,
	//THROTTLE_OK,
	TSREADY,
	CHARGER_CONNECTED,
	BMS_VOLTAGE,
	BMS_OK,
	C,
	BF,
	BR,
	BSPD_OK,
	IMD_OK,
	INPUT_COUNT,
};

enum OUTPUT {
	AIR_POS,
	AIR_NEG,
	ENABLE_COOLANT_PUMP,
	DCDC_DISABLE,
	PRECHARGE_FAILED,
	MCU_REDUNDANCY_1,
	MCU_REDUNDANCY_2,
	OUTPUT_COUNT,
};

enum SIGNAL {
	LOW,
	HIGH,
};

typedef enum STATE {
	AIR_OFF,
	PRECHARGE,
	AIR_ON,
	READY_TO_CHARGE,
	READY_TO_DRIVE,
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

	bool get_flag();
	void set_flag();
	void clear_flag();

	state_t get_state();
};

#endif
