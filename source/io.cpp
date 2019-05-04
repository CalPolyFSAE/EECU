#include "vcu.h"
#include "mc.h"
#include "io.h"
#include "canlight.h"
#include "adc.h"
#include "gpio.h"
#include "fsl_flexio.h"

using namespace BSP;

extern VCU vcu;

// callback to process messages on the general CAN bus
static void gen_can_callback() {
    uint8_t buffer[8];

    switch(can_receive(GEN_CAN_BUS, buffer)) {
        // TODO - read CHARGER_CONNECTECD
        // TODO - read BMS_VOLTAGE
        // TODO - read BMS_TEMPERATURE
        default:
            break;
    }
}

// callback to process messages on the motor controller CAN bus
static void mc_can_callback() {
    mc_receive_broadcast_message();
}

// initializes the PWM driver
static void pwm_init() {
    flexio_config_t config;
    flexio_timer_config_t timer_config;
    uint32_t sum;
    uint8_t lower;
    uint8_t upper;
    
    FLEXIO_GetDefaultConfig(&config);
    FLEXIO_Init(FLEXIO, &config);

    sum = (((2 * CLOCK_GetIpFreq(kCLOCK_Flexio0)) / PWM_FREQUENCY) + 1) / 2;
    lower = (((sum * (100 - PWM_MIN)) / 50) + 1) / 2;
    upper = sum - lower;

    timer_config.triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(0U);
    timer_config.triggerSource = kFLEXIO_TimerTriggerSourceInternal;
    timer_config.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timer_config.pinConfig = kFLEXIO_PinConfigOutput;
    timer_config.pinPolarity = kFLEXIO_PinActiveHigh;
    timer_config.pinSelect = 1;
    timer_config.timerMode = kFLEXIO_TimerModeDisabled;
    timer_config.timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset;
    timer_config.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timer_config.timerDisable = kFLEXIO_TimerDisableNever;
    timer_config.timerEnable = kFLEXIO_TimerEnabledAlways;
    timer_config.timerReset = kFLEXIO_TimerResetNever;
    timer_config.timerStart = kFLEXIO_TimerStartBitDisabled;
    timer_config.timerStop = kFLEXIO_TimerStopBitDisabled;
    timer_config.timerCompare = ((upper - 1) << 8) | (lower - 1);

    FLEXIO_SetTimerConfig(FLEXIO, 0, &timer_config);
    FLEXIO->TIMCTL[0] |= FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);
}

// changes the PWM duty cycle
static void pwm_set(uint8_t duty) {
    uint32_t sum;
    uint8_t lower;
    uint8_t upper;
 
    if(duty > PWM_MAX) {
        duty = PWM_MAX;
    } else if(duty < PWM_MIN) {
        duty = PWM_MIN;
    }

    sum = (((2 * CLOCK_GetIpFreq(kCLOCK_Flexio0)) / PWM_FREQUENCY) + 1) / 2;
    lower = (((sum * (100 - duty)) / 50) + 1) / 2;
    upper = sum - lower;
    
    FLEXIO->TIMCMP[0] = FLEXIO_TIMCMP_CMP(((upper - 1) << 8) | (lower - 1));
}

// initializes VCU drivers
void init_io() {
    can::canlight_config config;
    can::CANlight::canx_config canx_config;

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    gpio.in_dir(gpio::PortE, 6);
    PORTE->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortA, 1);
    PORTA->PCR[1] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortB, 6);
    PORTB->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortE, 2);
    PORTE->PCR[2] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortB, 7);
    PORTB->PCR[7] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortE, 3);
    PORTE->PCR[3] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortD, 16);
    PORTD->PCR[16] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortC, 8);
    PORTC->PCR[8] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortD, 6);
    PORTD->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortD, 5);
    PORTD->PCR[5] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.in_dir(gpio::PortD, 7);
    PORTD->PCR[7] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    gpio.out_dir(gpio::PortD, 4);
    gpio.out_dir(gpio::PortB, 1);
    gpio.out_dir(gpio::PortB, 0);
    gpio.out_dir(gpio::PortC, 9);
    gpio.out_dir(gpio::PortE, 11);
    gpio.out_dir(gpio::PortD, 3);
    gpio.out_dir(gpio::PortB, 13);
    gpio.out_dir(gpio::PortB, 12);
    gpio.out_dir(gpio::PortE, 7);
    gpio.out_dir(gpio::PortA, 6);
    gpio.out_dir(gpio::PortD, 0);
    gpio.out_dir(gpio::PortA, 7);
    gpio.out_dir(gpio::PortD, 2);

    // initialize ADC driver
    adc::ADC::ConstructStatic(NULL);

    // initialize CAN driver
    can::CANlight::ConstructStatic(&config);
    can::CANlight &can = can::CANlight::StaticClass();
    canx_config.baudRate = GEN_CAN_BAUD_RATE;
    canx_config.callback = gen_can_callback;
    can.init(GEN_CAN_BUS, &canx_config);
    canx_config.baudRate = MC_CAN_BAUD_RATE;
    canx_config.callback = mc_can_callback;
    can.init(MC_CAN_BUS, &canx_config);

    // initialize PWM driver
    pwm_init();
}

// reads VCU input signals from GPIO and ADC pins
void input_map() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    adc::ADC &adc = adc::ADC::ADC::StaticClass();

    vcu.input.THROTTLE_1 = adc.read(ADC0, 14);
    vcu.input.THROTTLE_2 = adc.read(ADC0, 15);
    vcu.input.LATCH_SENSE = gpio.read(gpio::PortA, 1);
    vcu.input.TS_READY_SENSE = gpio.read(gpio::PortB, 6);
    vcu.input.TS_RDY = gpio.read(gpio::PortE, 2);
    vcu.input.TS_LIVE = gpio.read(gpio::PortB, 7);
    vcu.input.BMS_OK = gpio.read(gpio::PortE, 3);
    vcu.input.IMD_OK = gpio.read(gpio::PortD, 16);
    vcu.input.BSPD_OK = gpio.read(gpio::PortC, 8);
    vcu.input.CURRENT_SENSE = adc.read(ADC0, 6);
    vcu.input.BRAKE_FRONT = adc.read(ADC0, 7);
    vcu.input.BRAKE_REAR = adc.read(ADC0, 12);
    vcu.input.WHEEL_SPEED_FR = adc.read(ADC0, 13);
    vcu.input.WHEEL_SPEED_FL = gpio.read(gpio::PortD, 6);
    vcu.input.WHEEL_SPEED_RR = gpio.read(gpio::PortD, 5);
    vcu.input.WHEEL_SPEED_RL = gpio.read(gpio::PortD, 7);
}

// writes VCU output signals to GPIO pins
void output_map() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    uint8_t buffer[8];
    
    buffer[0] = vcu.input.LATCH_SENSE;
    buffer[1] = vcu.output.PRECHARGE_FAILED;
    buffer[2] = vcu.output.DISCHARGE;
    buffer[3] = vcu.output.PRECHARGE;
    buffer[4] = vcu.output.DCDC_DISABLE;
    buffer[5] = vcu.output.PUMP_EN;
    buffer[6] = vcu.output.AIR_NEG;
    buffer[7] = vcu.output.AIR_POS;
    
    can_send(GEN_CAN_BUS, VCU_PRECHARGE, buffer);
    mc_torque_request(vcu.output.MC_TORQUE);
    pwm_set(vcu.output.FAN_PWM);

    vcu.output.RTDS ? gpio.set(gpio::PortD, 4) : gpio.clear(gpio::PortD, 4);
    vcu.output.BRAKE_LIGHT ? gpio.set(gpio::PortB, 1) : gpio.clear(gpio::PortB, 1);
    vcu.output.AIR_POS ? gpio.set(gpio::PortB, 0) : gpio.clear(gpio::PortB, 0);
    vcu.output.AIR_NEG ? gpio.set(gpio::PortC, 9) : gpio.clear(gpio::PortC, 9);
    vcu.output.PUMP_EN ? gpio.clear(gpio::PortE, 11) : gpio.set(gpio::PortE, 11);
    vcu.output.DCDC_DISABLE ? gpio.set(gpio::PortD, 13) : gpio.clear(gpio::PortD, 13);
    vcu.output.PRECHARGE ? gpio.set(gpio::PortB, 13) : gpio.clear(gpio::PortB, 13);
    vcu.output.DISCHARGE ? gpio.set(gpio::PortB, 12) : gpio.clear(gpio::PortB, 12);
    vcu.output.REDUNDANT_1 ? gpio.set(gpio::PortE, 7) : gpio.clear(gpio::PortE, 7);
    vcu.output.REDUNDANT_2 ? gpio.set(gpio::PortA, 6) : gpio.clear(gpio::PortA, 6);
    vcu.output.FAN_EN ? gpio.clear(gpio::PortD, 0) : gpio.set(gpio::PortD, 0);
    vcu.output.GENERAL_PURPOSE_1 ? gpio.set(gpio::PortA, 7) : gpio.clear(gpio::PortA, 7);
    vcu.output.GENERAL_PURPOSE_2 ? gpio.set(gpio::PortD, 2) : gpio.clear(gpio::PortD, 2);
}

// sends a CAN message on the specified bus
void can_send(uint8_t bus, uint32_t address, uint8_t *data) {
    can::CANlight &can = can::CANlight::StaticClass();
    can::CANlight::frame frame;

    frame.id = address;
    frame.ext = 1;
    frame.dlc = 8;
    memcpy(frame.data, data, sizeof(frame.data));

    can.tx(bus, frame);
}

// receives a CAN message from the specified bus
uint8_t can_receive(uint8_t bus, uint8_t *data) {
    can::CANlight &can = can::CANlight::StaticClass();
    can::CANlight::frame frame = can.readrx(bus);

    memcpy(data, frame.data, sizeof(frame.data));

    return(frame.id);
}
