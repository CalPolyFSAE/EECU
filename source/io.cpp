#include "vcu.h"
#include "mc.h"
#include "io.h"
#include "canlight.h"
#include "adc.h"
#include "gpio.h"

using namespace BSP;

extern VCU vcu;

// callback to process CAN messages on the general bus
static void gen_can_callback() {
    uint8_t buffer[8];

    switch(receive_can_message(GEN_CAN_CHANNEL, buffer)) {
        // TODO - read CHARGER_CONNECTECD
        // TODO - read BMS_VOLTAGE
        // TODO - read BMS_TEMPERATURE
        default:
            break;
    }
}

// callback to process CAN messages on the motor controller bus
static void mc_can_callback() {
    mc_receive_broadcast_message();
}

// initializes VCU drivers
void init_io() {
    can::canlight_config config;
    can::CANlight::canx_config canx_config;

    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    gpio.in_dir(gpio::PortE, 6);
    gpio.in_dir(gpio::PortA, 1);
    gpio.in_dir(gpio::PortB, 6);
    gpio.in_dir(gpio::PortE, 2);
    gpio.in_dir(gpio::PortB, 7);
    gpio.in_dir(gpio::PortE, 3);
    gpio.in_dir(gpio::PortD, 16);
    gpio.in_dir(gpio::PortC, 8);
    gpio.in_dir(gpio::PortD, 6);
    gpio.in_dir(gpio::PortD, 5);
    gpio.in_dir(gpio::PortD, 7);
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
    gpio.out_dir(gpio::PortD, 1);
    gpio.out_dir(gpio::PortA, 7);
    gpio.out_dir(gpio::PortD, 2);

    // initialize ADC driver
    adc::ADC::ConstructStatic(NULL);

    // initialize CAN driver
    can::CANlight::ConstructStatic(&config);
    can::CANlight &can = can::CANlight::StaticClass();
    canx_config.baudRate = GEN_CAN_BAUD_RATE;
    canx_config.callback = gen_can_callback;
    can.init(GEN_CAN_CHANNEL, &canx_config);
    canx_config.baudRate = MC_CAN_BAUD_RATE;
    canx_config.callback = mc_can_callback;
    can.init(MC_CAN_CHANNEL, &canx_config);
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

    vcu.output.RTDS ? gpio.set(gpio::PortD, 4) : gpio.clear(gpio::PortD, 4);
    vcu.output.BRAKE_LIGHT ? gpio.set(gpio::PortB, 1) : gpio.clear(gpio::PortB, 1);
    vcu.output.AIR_POS ? gpio.set(gpio::PortB, 0) : gpio.clear(gpio::PortB, 0);
    vcu.output.AIR_NEG ? gpio.set(gpio::PortC, 9) : gpio.clear(gpio::PortC, 9);
    vcu.output.PUMP_EN ? gpio.set(gpio::PortE, 11) : gpio.clear(gpio::PortE, 11);
    vcu.output.DCDC_DISABLE ? gpio.set(gpio::PortD, 13) : gpio.clear(gpio::PortD, 13);
    vcu.output.PRECHARGE ? gpio.set(gpio::PortB, 13) : gpio.clear(gpio::PortB, 13);
    vcu.output.DISCHARGE ? gpio.set(gpio::PortB, 12) : gpio.clear(gpio::PortB, 12);
    vcu.output.REDUNDANT_1 ? gpio.set(gpio::PortE, 7) : gpio.clear(gpio::PortE, 7);
    vcu.output.REDUNDANT_2 ? gpio.set(gpio::PortA, 6) : gpio.clear(gpio::PortA, 6);
    vcu.output.FAN_EN ? gpio.set(gpio::PortD, 0) : gpio.clear(gpio::PortD, 0);
    vcu.output.GENERAL_PURPOSE_1 ? gpio.set(gpio::PortA, 7) : gpio.clear(gpio::PortA, 7);
    vcu.output.GENERAL_PURPOSE_2 ? gpio.set(gpio::PortD, 2) : gpio.clear(gpio::PortD, 2);

    // TODO - write LATCH_SIZE
    // TODO - write PRECHARGE_FAILED
    // TODO - write FAN_PWM
}

// receives a CAN message from the specified channel
uint8_t receive_can_message(uint8_t channel, uint8_t *data) {
    can::CANlight &can = can::CANlight::StaticClass();
    can::CANlight::frame frame = can.readrx(channel);

    memcpy(data, frame.data, sizeof(frame.data));

    return(frame.id);
}

// sends a CAN message on the specified channel
void send_can_message(uint8_t channel, uint32_t address, uint8_t *data) {
    can::CANlight &can = can::CANlight::StaticClass();
    can::CANlight::frame frame;

    frame.id = address;
    frame.ext = 1;
    frame.dlc = 8;
    memcpy(frame.data, data, sizeof(frame.data));

    can.tx(channel, frame);
}
