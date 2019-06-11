# VCU

## [Requirements](https://github.com/CalPolyFSAE/MKELibrary/wiki/Requirements---VCU-(Vehicle-Control-Unit))

## Parameters
`BYPASS_DRIVER` - allows motor control via UART

`BYPASS_BRAKES` - disables `brakes_valid()` and `brakes_active()` safety checks

`BYPASS_THROTTLES` - disables `throttles_valid()` safety check

`BYPASS_SAFETY` - disables check for `IMD_OK`, `BMS_OK`, and `BSPD_OK`

`POWER_LIMITING` - enables power limiting (UNTESTED)

`TRACTION_CONTROL` - enables traction control (UNTESTED)

`RTDS_TIME` - buzzer sound time

`ALLOWED_PRECHARGE_TIME` - precharging time

`MC_CHARGE_TIME` - motor controller charging time

`CHARGER_CONNECTED` - charger CAN bus timeout

`CA` - current sensor activation threshold

`BFA` - front brake pressure activation threshold

`BRA` - rear brake pressure activation threshold

`BRAKE_MIN` - minimum brake pressure sensor value

`BRAKE_MAX` - maximum brake pressure sensor value

`THROTTLE_1_MIN` - 0% throttle 1 sensor value

`THROTTLE_1_MAX` - 100% throttle 1 sensor value

`THROTTLE_2_MIN` - 0% throttle 2 sensor value

`THROTTLE_2_MAX` - 100% throttle 2 sensor value

`SUPPLY_THRESHOLD` - low voltage threshold divided by 3 to trigger `SUPPLY_OK`

`DEFAULT_POWER_LIMIT` - initial power limit on startup

`TORQUE_DIS` - torque value to disable motor controller

`TORQUE_MIN` - torque value at 0% throttle

`TORQUE_MAX` - torque value at 100% throttle

`TEMPERATURE_LIMIT` - battery pack temperature limit for determining `FAN_PWM`

`THROTTLE_LOW_LIMIT` - used to satisfy rule EV.2.4

`THROTTLE_HIGH_LIIMT` - used to satify rule EV.2.4

`BATTERY_LIMIT` - percentage to compare `MC_VOLTAGE` to `BMS_VOLTAGE` for precharging
