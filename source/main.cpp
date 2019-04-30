#include "MKE18F16.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "vcu.h"
#include "mc.h"
#include "io.h"

#include "fsl_flexio.h"

VCU vcu;

static void pwm_init(uint32_t freq, uint32_t duty) {
    uint32_t clock;
    uint32_t lower;
    uint32_t upper;
    uint32_t sum;
    flexio_timer_config_t config;

    if(duty > 99)
        duty = 99;
    else if(duty == 0)
        duty = 1;
    
    clock = CLOCK_GetIpFreq(kCLOCK_Flexio0);
    sum = (((2 * clock) / freq) + 1) / 2;
    lower = (((sum * duty) / 50) + 1) / 2;
    upper = sum - lower;
    
    config.triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(0U);
    config.triggerSource = kFLEXIO_TimerTriggerSourceInternal;
    config.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    config.pinConfig = kFLEXIO_PinConfigOutput;
    config.pinPolarity = kFLEXIO_PinActiveHigh;
    config.pinSelect = 1;
    config.timerMode = kFLEXIO_TimerModeDisabled;
    config.timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset;
    config.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    config.timerDisable = kFLEXIO_TimerDisableNever;
    config.timerEnable = kFLEXIO_TimerEnabledAlways;
    config.timerReset = kFLEXIO_TimerResetNever;
    config.timerStart = kFLEXIO_TimerStartBitDisabled;
    config.timerStop = kFLEXIO_TimerStopBitDisabled;
    config.timerCompare = ((upper - 1) << 8) | (lower - 1);

    FLEXIO_SetTimerConfig(FLEXIO, 0, &config);
    FLEXIO->TIMCTL[0] |= FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);
}

int main() {
    uint8_t data[8];
    flexio_config_t config;

    // initialize board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    // initialize drivers
    init_io();
    
    // initialize PWM timer
    CLOCK_SetIpSrc(kCLOCK_Flexio0, kCLOCK_IpSrcFircAsync);
    FLEXIO_GetDefaultConfig(&config);
    FLEXIO_Init(FLEXIO, &config);
    pwm_init(100, 50);

    // initialize system timer
    SysTick_Config(TIMER_PERIOD);
    
    // TODO - time main loop to verify VCU frequency
    while(true) {
        if(vcu.flag == true) {
            
            data[0] = 0x00;
            data[1] = 0x11;
            data[2] = 0x22;
            data[3] = 0x33;
            data[4] = 0x44;
            data[5] = 0x55;
            data[6] = 0x66;
            data[7] = 0x77;

            send_can_message(GEN_CAN_CHANNEL, 0x0AA, data);

            // read inputs
            //input_map();

            // core logic
            //vcu.motor_loop();
            //vcu.shutdown_loop();
            //vcu.redundancy_loop();

            // write outputs
            //output_map();
            
            // spinlock to synchronize thread
            vcu.flag = false;
        }
    }

    return 0;
}

extern "C" {
void SysTick_Handler() { vcu.flag = true; }
}
