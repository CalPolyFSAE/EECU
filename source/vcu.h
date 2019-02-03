#ifndef VCU_H_
#define VCU_H_

#include <time.h>

#define ALLOWED_PRECHARGE_TIME 4
#define MC_CHARGE_TIME 1
#define BATTERY_PERCENTAGE 90

enum INPUT {
	TSREADY,
	MC_VOLTAGE,
	BMS_VOLTAGE,
	CHARGER_CONNECTED,
	INPUT_COUNT,
};

enum OUTPUT {
	POS_AIR_ENABLE,
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
public:
	volatile bool flag;
	volatile timer_t timer;
	volatile uint32_t input[INPUT_COUNT];
	volatile uint32_t output[OUTPUT_COUNT];
	volatile state_t state;

	VCU();
	void shutdown_loop();
};

#endif
