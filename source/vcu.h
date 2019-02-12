#ifndef VCU_H_
#define VCU_H_

#include <stdint.h>

#define VCU_FREQUENCY 100U
#define CLOCK_FREQUENCY 60000000U

#define TIMER_PERIOD (CLOCK_FREQUENCY / VCU_FREQUENCY)

#define ALLOWED_PRECHARGE_TIME (4 * VCU_FREQUENCY)
#define MC_CHARGE_TIME (1 * VCU_FREQUENCY)
#define BATTERY_PERCENTAGE 90

#define CA 2048
#define BFA 2048
#define BRA 2048
#define VOLTAGE_MIN 410
#define VOLTAGE_MAX	3686

enum INPUT {
	TSREADY,
	MC_VOLTAGE,
	BMS_VOLTAGE,
	CHARGER_CONNECTED,
	C,
	BF,
	BR,
	BSPD_OK,
	IMD_OK,
	BMS_OK,
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

typedef enum LOOP {
	SHUTDOWN,
	REDUNDANCY,
	MOTOR,
	LOOP_COUNT,
} loop_t;

class VCU {
private:
	volatile bool flag;
	state_t state[LOOP_COUNT];
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];
	uint32_t timer;

public:
	VCU();

	void motor_loop();
	void shutdown_loop();
	void redundancy_loop();

	void map_input(uint32_t *input);
	void map_output(uint32_t *output);

	bool get_flag();
	void set_flag();
	void clear_flag();
	state_t get_state(loop_t loop);
};

#endif
