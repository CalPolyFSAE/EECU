#ifndef VCU_H_
#define VCU_H_

#include <time.h>

#define ALLOWED_PRECHARGE_TIME 40
#define MC_CHARGE_TIME 10
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
public:
	volatile bool flag;
	uint32_t timer;
	uint32_t input[INPUT_COUNT];
	uint32_t output[OUTPUT_COUNT];
	state_t state;

	VCU();
	void shutdown_loop();
};

#endif
