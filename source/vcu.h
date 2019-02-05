#ifndef VCU_H_
#define VCU_H_

#include <stdint.h>

#define VCU_FREQUENCY 10U
#define CLOCK_FREQUENCY 60000000U

#define TIMER_PERIOD (VCU_FREQUENCY / CLOCK_FREQUENCY / 2)
#define ALLOWED_PRECHARGE_TIME (4 * VCU_FREQUENCY)
#define MC_CHARGE_TIME (1 * VCU_FREQUENCY)
#define BATTERY_PERCENTAGE 90

enum INPUT {
	TSREADY,
	MC_VOLTAGE,
	BMS_VOLTAGE,
	CHARGER_CONNECTED,
	INPUT_COUNT,
};

enum OUTPUT {
	AIR_POS,
	AIR_NEG,
	ENABLE_COOLANT_PUMP,
	DCDC_DISABLE,
	PRECHARGE_FAILED,
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
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];
	uint32_t timer;

public:
	VCU();
	void shutdown_loop();
	bool get_flag();
	void set_flag();
	void clear_flag();
	state_t get_state();
	void set_input(uint32_t *input);
	void get_output(uint32_t *output);
};

#endif
