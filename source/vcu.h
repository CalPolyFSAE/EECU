#ifndef VCU_H
#define VCU_H

#include "MKE18F16.h"
#include "clock_config.h"

//#define BYPASS_DRIVER
//#define BYPASS_BRAKES
//#define BYPASS_THROTTLES
//#define BYPASS_SAFETY

// EXPERIMENTAL FEATURES - ENABLE AT YOUR OWN RISK
//#define POWER_LIMITING
//#define TRACTION_CONTROL

#define VCU_FREQUENCY 100
#define TIMER_PERIOD (BOARD_BOOTCLOCKRUN_CORE_CLOCK / VCU_FREQUENCY)

#define SECONDS(S)          ((S) * VCU_FREQUENCY)
#define VOLTS(V)            (((V) * 4095) / 5)
#define KILOWATTS(KW)       (KW)
#define NEWTONMETERS(NM)    (NM * 10)
#define CELSIUS(C)          (C)
#define PERCENT(P)          (P)
#define UNITS(U)            (U)

#define RTDS_TIME               SECONDS(2)
#define ALLOWED_PRECHARGE_TIME  SECONDS(4)
#define MC_CHARGE_TIME          SECONDS(1)
#define CHARGER_CONNECTED_TIME  SECONDS(1)

#define CA                      VOLTS(2.80)
#define BFA                     VOLTS(0.55)
#define BRA                     VOLTS(0.55)
#define BRAKE_MIN               VOLTS(0.10)
#define BRAKE_MAX               VOLTS(4.50)
#define THROTTLE_1_MIN          VOLTS(2.64)
#define THROTTLE_1_MAX          VOLTS(1.03)
#define THROTTLE_2_MIN          VOLTS(2.08)
#define THROTTLE_2_MAX          VOLTS(3.79)
#define SUPPLY_THRESHOLD        VOLTS(3.66)

#define DEFAULT_POWER_LIMIT     KILOWATTS(60)

#define TORQUE_DIS              NEWTONMETERS(-1)
#define TORQUE_MIN              NEWTONMETERS(0)
#define TORQUE_MAX              NEWTONMETERS(20)

#define TEMPERATURE_LIMIT       CELSIUS(40)

#define THROTTLE_LOW_LIMIT      PERCENT(5)
#define THROTTLE_HIGH_LIMIT     PERCENT(25)
#define BATTERY_LIMIT           PERCENT(90)

#define THROTTLE_FULLSCALE      VOLTS(5.00)
#define THROTTLE_TRAVEL         UNITS(0.35)

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
    uint8_t MC_EN;              // GPIO E6
    uint32_t MC_POST_FAULT;     // CAN 1
    uint32_t MC_RUN_FAULT;      // CAN 1
    int16_t MC_CURRENT;         // CAN 1
    int16_t MC_VOLTAGE;         // CAN 1
    int16_t MC_SPEED;           // CAN 1
    int32_t THROTTLE_1;         // ADC 0.14
    int32_t THROTTLE_2;         // ADC 0.15
    //int32_t THROTTLE_1_BASE;    // ADC 0.14
    //int32_t THROTTLE_2_BASE;    // ADC 0.15
    uint8_t POWER_LIMIT;        // CAN 0
    uint8_t LATCH_SENSE;        // GPIO A1
    uint8_t TS_READY_SENSE;     // GPIO B6
    uint8_t TS_RDY;             // GPIO E2
    uint8_t TS_LIVE;            // GPIO B7
    uint8_t CHARGER_CONNECTED;  // CAN 0
    int16_t BMS_VOLTAGE;        // CAN 0
    int16_t BMS_TEMPERATURE;    // CAN 0
    uint8_t BMS_STATE;          // CAN 0
    uint8_t BMS_OK;             // GPIO E3
    uint8_t IMD_OK;             // GPIO D16
    uint8_t BSPD_OK;            // GPIO C8
    uint32_t CURRENT_SENSE;     // ADC 0.6
    uint32_t BRAKE_FRONT;       // ADC 0.7
    uint32_t BRAKE_REAR;        // ADC 0.12
    uint16_t WHEEL_SPEED_FR;    // GPIO C15
    uint16_t WHEEL_SPEED_FL;    // GPIO D6
    uint16_t WHEEL_SPEED_RR;    // GPIO D5
    uint16_t WHEEL_SPEED_RL;    // GPIO D7
    uint16_t SUPPLY_VOLTAGE;    // ADC 1.2
} input_t;

typedef struct {
    int16_t MC_TORQUE;          // CAN 1
    uint8_t RTDS;               // GPIO D4
    uint8_t BRAKE_LIGHT;        // GPIO B1
    uint8_t AIR_POS;            // GPIO B0
    uint8_t AIR_NEG;            // GPIO C9
    uint8_t PUMP_EN;            // GPIO E11
    uint8_t DCDC_DISABLE;       // GPIO D3
    uint8_t PRECHARGE;          // GPIO B13
    uint8_t DISCHARGE;          // GPIO B12
    uint8_t PRECHARGE_FAILED;   // CAN 0
    uint8_t REDUNDANT_1;        // GPIO E7
    uint8_t REDUNDANT_2;        // GPIO A6
    uint8_t FAN_EN;             // GPIO D0
    uint8_t FAN_PWM;            // FXIO D1
    uint8_t SUPPLY_OK;          // GPIO A7
    uint16_t STATUS;            // CAN 0
} output_t;

class VCU {
public:
    volatile input_t input;
    volatile output_t output;
    volatile bool flag;

    VCU();

    void motor_loop();
    void shutdown_loop();
    void redundancy_loop();
};

#endif
