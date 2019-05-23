# VCU

## [Project Requirements](https://github.com/CalPolyFSAE/MKELibrary/wiki/Requirements---VCU-(Vehicle-Control-Unit))

## Constants
* BENCH_TEST (vcu.cpp) - bench test flag (used to bypass throttle and brake checks and spin motors over UART)
* CA (vcu.cpp) - current sensor activation voltage
* BFA (vcu.cpp) - front brake activation voltage
* BRA (vcu.cpp) - rear brake activation voltage
* TORQUE_MAX (vcu.cpp) - maximum torque that can be requested by the driver (note that this value is scaled by 10 so 1000 = 100Nm)
* DEFAULT_POWER_LIMIT (vcu.cpp) - threshold in kW of when to start limiting power (currently disabled)

## Notes
* VCU runs computations at 100Hz
* Baudrate for both CAN channels is 1Mbps
* Maximum torque is limited to 100Nm
* Power limitting is currently off
* Throttles are calibrated on startup range from 0% - 80%
* Torque map is a linear mapping from 0% throttle yielding 0Nm and 100% throttle yielding 100Nm
* Driver signals (buttons, switches, brakes, throttles, etc.) are sampled at 10Hz while all other signals are sampled at 100Hz
* Logic for charging has been bench tested but has not been tested with the actual charger (making sure the VCU detects the charger on the CAN bus and detecting when it is disconnected after a timeout)
* Precharge signals, throttles, brakes, wheel speeds, and safety signals are all logged on the motor controller CAN bus at 100Hz
* Fan PWM is fixed to 5% since BMS temperature is not being read in yet
* To use power limiting, uncomment lines 22-26 in vcu.cpp (note that this feature is untested)
* The motor controller enable signal is currently ignored since the signal is not being read in properly
* Still working on getting wheel speeds working so front wheel speeds may be accurate but rear wheel speeds will most likely be complete garbage
