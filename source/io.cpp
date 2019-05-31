#include "vcu.h"
#include "mc.h"
#include "io.h"
#include "ui.h"
#include "canlight.h"
#include "adc.h"
#include "gpio.h"
#include "fsl_lptmr.h"
#include "fsl_flexio.h"

using namespace BSP;

extern VCU vcu;

// converts wheel speed sensor reading to RPM
static uint16_t wheel_conversion(uint16_t period) {
    uint16_t speed;

    if(period == 0) {
        speed = 0;
    } else {
        speed = ((60 / RELUCTOR_TEETH) * TIMER_FREQUENCY) / period;
    }

    return(speed);
}

// callback to process wheel speed sensors on GPIO pins
static void wheel_gpio_callback() {
    static uint16_t timer_fr = 0;
    static uint16_t timer_fl = 0;
    static uint16_t timer_rr = 0;
    static uint16_t timer_rl = 0;
    uint16_t timer;
    uint8_t pin;

    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    pin = gpio.int_source(gpio::PortC);

    if(pin == 32) {
        pin = gpio.int_source(gpio::PortD);
    }

    timer = LPTMR_GetCurrentTimerCount(LPTMR0);

    switch(pin) {
        case 15:
            vcu.input.WHEEL_SPEED_FR = wheel_conversion(timer - timer_fr);
            timer_fr = timer;
            gpio.ack_interrupt(gpio::PortC, 15);
            break;

        case 6:
            vcu.input.WHEEL_SPEED_FL = wheel_conversion(timer - timer_fl);
            timer_fl = timer;
            gpio.ack_interrupt(gpio::PortD, 6);
            break;

        case 5:
            vcu.input.WHEEL_SPEED_RR = wheel_conversion(timer - timer_rr);
            timer_rr = timer;
            gpio.ack_interrupt(gpio::PortD, 5);
            break;

        case 7:
            vcu.input.WHEEL_SPEED_RL = wheel_conversion(timer - timer_rl);
            timer_rl = timer;
            gpio.ack_interrupt(gpio::PortD, 7);
            break;
        
        default:
            break;
    }
}

// callback to process messages on the general CAN bus
static void gen_can_callback() {
    uint8_t buffer[8];

    switch(can_receive(GEN_CAN_BUS, buffer)) {
        case DASHBOARD_ID:
            vcu.input.POWER_LIMIT = buffer[0];

        case BMS_ID:
            vcu.input.BMS_STATE = buffer[4];
            vcu.input.BMS_TEMPERATURE = (buffer[3] << 8) | buffer[2];
            vcu.input.BMS_VOLTAGE = (buffer[1] << 8) | buffer[0];
            break;

        case CHARGER_ID:
            vcu.input.CHARGER_CONNECTED = (vcu.input.CHARGER_CONNECTED % 255) + 1;
            break;

        default:
            break;
    }
}

// callback to process messages on the motor controller CAN bus
static void mc_can_callback() {
    mc_receive_broadcast_message();
}

// updates the dashboard
static void update_dashboard() {
    uint8_t buffer[8];
    
    buffer[7] = 0x00;
    buffer[6] = vcu.input.LATCH_SENSE;
    buffer[5] = (vcu.input.MC_SPEED >> 8) & 0xFF;
    buffer[4] = vcu.input.MC_SPEED & 0xFF;
    buffer[3] = (vcu.input.SUPPLY_VOLTAGE >> 8) & 0xFF;
    buffer[2] = vcu.input.SUPPLY_VOLTAGE & 0xFF;
    buffer[1] = (vcu.input.BMS_VOLTAGE >> 8) & 0xFF;
    buffer[0] = vcu.input.BMS_VOLTAGE & 0xFF;
    
    can_send(GEN_CAN_BUS, VCU_UPDATE, buffer);
}

// logs safety signals
static void log_safety() {
    uint8_t buffer[8];
    
    buffer[7] = 0x00;
    buffer[6] = 0x00;
    buffer[5] = 0x00;
    buffer[4] = vcu.input.IMD_OK;
    buffer[3] = vcu.input.BMS_OK;
    buffer[2] = vcu.input.BSPD_OK;
    buffer[1] = vcu.output.REDUNDANT_1;
    buffer[0] = vcu.output.REDUNDANT_2;
    
    can_send(GEN_CAN_BUS, VCU_SAFETY, buffer);
}

// logs precharge signals
static void log_precharge() {
    uint8_t buffer[8];
    
    buffer[7] = vcu.output.AIR_POS;
    buffer[6] = vcu.output.AIR_NEG;
    buffer[5] = vcu.output.PRECHARGE;
    buffer[4] = vcu.output.DISCHARGE;
    buffer[3] = vcu.output.PUMP_EN;
    buffer[2] = vcu.output.DCDC_DISABLE;
    buffer[1] = vcu.input.LATCH_SENSE;
    buffer[0] = vcu.output.PRECHARGE_FAILED;

    can_send(GEN_CAN_BUS, VCU_PRECHARGE, buffer);
}

// logs driver signals
static void log_driver() {
    uint8_t buffer[8];
    
    buffer[7] = (vcu.input.BRAKE_FRONT >> 16) & 0xFF;
    buffer[6] = (vcu.input.BRAKE_FRONT >> 8) & 0xFF;
    buffer[5] = vcu.input.BRAKE_FRONT & 0xFF;
    buffer[4] = (vcu.input.BRAKE_REAR >> 16) & 0xFF;
    buffer[3] = (vcu.input.BRAKE_REAR >> 8) & 0xFF;
    buffer[2] = vcu.input.BRAKE_REAR & 0xFF;
    buffer[1] = vcu.input.THROTTLE_1;
    buffer[0] = vcu.input.THROTTLE_2;

    can_send(GEN_CAN_BUS, VCU_DRIVER, buffer);
}

// logs speed signals
static void log_speed() {
    uint8_t buffer[8];
    
    buffer[7] = (vcu.input.WHEEL_SPEED_FR >> 8) & 0xFF;
    buffer[6] = vcu.input.WHEEL_SPEED_FR & 0xFF;
    buffer[5] = (vcu.input.WHEEL_SPEED_FL >> 8) & 0xFF;
    buffer[4] = vcu.input.WHEEL_SPEED_FL & 0xFF;
    buffer[3] = (vcu.input.WHEEL_SPEED_RR >> 8) & 0xFF;
    buffer[2] = vcu.input.WHEEL_SPEED_RR & 0xFF;
    buffer[1] = (vcu.input.WHEEL_SPEED_RL >> 8) & 0xFF;
    buffer[0] = vcu.input.WHEEL_SPEED_RL & 0xFF;

    can_send(GEN_CAN_BUS, VCU_SPEED, buffer);
}

// initializes the timer driver
static void timer_init() {
    lptmr_config_t config;
    
    LPTMR_GetDefaultConfig(&config);
    
    config.enableFreeRunning = true;
    config.bypassPrescaler = false;
    config.prescalerClockSource = kLPTMR_PrescalerClock_0;
    config.value = kLPTMR_Prescale_Glitch_1;

    LPTMR_Init(LPTMR0, &config);
    LPTMR_StartTimer(LPTMR0);
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

#ifdef BYPASS_DRIVER
    uiinit();
#endif

    // initialize timer driver
    timer_init();
    
    // initialize PWM driver
    pwm_init();

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    
    // configure GPIO inputs
    gpio.in_dir(gpio::PortE, 6);
    gpio.in_dir(gpio::PortA, 1);
    gpio.in_dir(gpio::PortB, 6);
    gpio.in_dir(gpio::PortE, 2);
    gpio.in_dir(gpio::PortB, 7);
    gpio.in_dir(gpio::PortE, 3);
    gpio.in_dir(gpio::PortD, 16);
    gpio.in_dir(gpio::PortC, 8);
    gpio.in_dir(gpio::PortC, 15);
    gpio.in_dir(gpio::PortD, 6);
    gpio.in_dir(gpio::PortD, 5);
    gpio.in_dir(gpio::PortD, 7);
    
    // enable pull-down resistors on GPIO inputs
    PORTE->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTA->PCR[1] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTB->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTE->PCR[2] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTB->PCR[7] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTE->PCR[3] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTD->PCR[16] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    PORTC->PCR[8] |= PORT_PCR_PE(1) | PORT_PCR_PS(0);
    
    // enable pull-up resistors on GPIO inputs
    PORTC->PCR[15] |= PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORTD->PCR[6] |= PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORTD->PCR[5] |= PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORTD->PCR[7] |= PORT_PCR_PE(1) | PORT_PCR_PS(1);
    
    // configure GPIO callbacks
    gpio.config_function(gpio::PortC, wheel_gpio_callback);
    gpio.config_function(gpio::PortD, wheel_gpio_callback);

    // enable GPIO interrupts
    gpio.config_interrupt(gpio::PortC, 15, kPORT_InterruptRisingEdge);
    gpio.config_interrupt(gpio::PortD, 6, kPORT_InterruptRisingEdge);
    gpio.config_interrupt(gpio::PortD, 5, kPORT_InterruptRisingEdge);
    gpio.config_interrupt(gpio::PortD, 7, kPORT_InterruptRisingEdge);

    // configure GPIO outputs
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

    // initialize throttle base voltages
    adc::ADC& adc = adc::ADC::StaticClass();
    vcu.input.THROTTLE_1_BASE = adc.read(ADC0, 14);
    vcu.input.THROTTLE_2_BASE = adc.read(ADC0, 15);
   
    // initialize CAN driver
    can::CANlight::ConstructStatic(&config);
    can::CANlight &can = can::CANlight::StaticClass();
    
    // configure general CAN bus
    canx_config.baudRate = GEN_CAN_BAUD_RATE;
    canx_config.callback = gen_can_callback;
    can.init(GEN_CAN_BUS, &canx_config);
    
    // configure motor controller CAN bus
    canx_config.baudRate = MC_CAN_BAUD_RATE;
    canx_config.callback = mc_can_callback;
    can.init(MC_CAN_BUS, &canx_config);
    
    // initialize motor controller
    mc_torque_request(TORQUE_DIS);
    mc_clear_faults();
}

// reads VCU input signals
void input_map() {
    static uint8_t timer = 0;
    
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    adc::ADC &adc = adc::ADC::ADC::StaticClass();

    vcu.input.TS_READY_SENSE = gpio.read(gpio::PortB, 6);
    vcu.input.TS_RDY = gpio.read(gpio::PortE, 2);
    vcu.input.TS_LIVE = gpio.read(gpio::PortB, 7);
    vcu.input.BMS_OK = gpio.read(gpio::PortE, 3);
    vcu.input.IMD_OK = gpio.read(gpio::PortD, 16);
    vcu.input.BSPD_OK = gpio.read(gpio::PortC, 8);
    vcu.input.CURRENT_SENSE = adc.read(ADC0, 6);
    vcu.input.SUPPLY_VOLTAGE = adc.read(ADC1, 2);

    if((timer % 10) == 0)
    {
        vcu.input.MC_EN = gpio.read(gpio::PortE, 6);
        vcu.input.LATCH_SENSE = gpio.read(gpio::PortA, 1);
        vcu.input.THROTTLE_1 =  - ((((int32_t)adc.read(ADC0, 14) - vcu.input.THROTTLE_1_BASE) * 100 ) / (THROTTLE_FULLSCALE * THROTTLE_TRAVEL));
        vcu.input.THROTTLE_2 = (((int32_t)adc.read(ADC0, 15) - vcu.input.THROTTLE_2_BASE) * 100 ) / (THROTTLE_FULLSCALE * THROTTLE_TRAVEL);
        //vcu.input.THROTTLE_1 = ((adc.read(ADC0, 14) - THROTTLE_NEG_MAX) * 100) / (THROTTLE_NEG_MIN - THROTTLE_NEG_MAX);
        //vcu.input.THROTTLE_2 = ((adc.read(ADC0, 15) - THROTTLE_POS_MIN) * 100) / (THROTTLE_POS_MAX - THROTTLE_POS_MIN);
        vcu.input.BRAKE_FRONT = adc.read(ADC0, 7);
        vcu.input.BRAKE_REAR = adc.read(ADC0, 12);
        timer = 0;
    }
    else
    {
        timer++;
    }
}

// writes VCU output signals
void output_map() {
    static uint8_t timer = 0;
    
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    
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
    vcu.output.SUPPLY_OK ? gpio.set(gpio::PortA, 7) : gpio.clear(gpio::PortA, 7);
    //vcu.output.GENERAL_PURPOSE_1 ? gpio.set(gpio::PortA, 7) : gpio.clear(gpio::PortA, 7);
    //vcu.output.GENERAL_PURPOSE_2 ? gpio.set(gpio::PortD, 2) : gpio.clear(gpio::PortD, 2);
    
    pwm_set(vcu.output.FAN_PWM);
    mc_torque_request(vcu.output.MC_TORQUE);
    
    if((timer % 10) == 0)
    {
        update_dashboard();
        log_safety();
        log_precharge();
        log_driver();
        log_speed();
        timer = 0;
    }
    else
    {
        timer++;
    }
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
uint32_t can_receive(uint8_t bus, uint8_t *data) {
    can::CANlight &can = can::CANlight::StaticClass();
    can::CANlight::frame frame = can.readrx(bus);

    memcpy(data, frame.data, sizeof(frame.data));

    return(frame.id);
}
