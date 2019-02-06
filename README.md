# VCU

Requirements By: Oliver Ousterman

Development By: Ayusman Saha

## Project Description
---

This board will handle the main controls for the E-Car. It handles tasks like converting throttle input to torque requests, safety systems monitoring, control loops, and power limiting. Completion of the basic functionality of this code is paramount to the success of the E-car.

## Project Requirements

---

## Base requirements:

- Motor Controller(MC) FSM
	- Logic Diagram for MC FSM on [slide 2](https://docs.google.com/presentation/d/1vSBAEsSL6-lunfy04aZIRiZ92vXWrCinEGm5FgSTTIM/edit#slide=id.g4262beed33_0_27)
	- MC retuquires a "Heartbeat" comman message at min every 500ms
- Shutdown FSM
	- Logic Diagram for shutdown FSM on [slide 5](https://docs.google.com/presentation/d/1vSBAEsSL6-lunfy04aZIRiZ92vXWrCinEGm5FgSTTIM/edit#slide=id.g4262beed33_0_27)
- Software safety redundant loops
	- MCU_REDUNDANCY_1 = MCU_REDUNDANCY_2 = BSPD_OK && IMD_OK && BMS_OK
	- BSPD_OK
		- BSPD_OK = !{(C > CA) && [(BF > BFA) || (BR > BRA)]} && [(BF > .5V) && (BF < 4.5V) && (BR > .5V) && (BR < 4.5V)]
			- BF = Brake front current pressure - ADC input
			- BFA = Brake front, active pressure - Static value = **INSERT BFA NUM HERE**
			- BR = Brake rear current pressure - ADC input
			- BRA = Brake rear, active pressure - Static value = **INSERT BRA NUM HERE**
			- C = Current sense voltage - ADC input
			- CA = 5kW current sense voltage - Static value = **INSERT CRNT NUM HERE**
	- IMD_OK
		- IMD_OK GPIO
	- BMS_OK
		- BMS_OK GPIO
- CAN Communication
	- General Comments
		- MC CAN documentation [RMS CAN protocol](https://www.rinehartmotion.com/support.html)
	- encode
		- MC Command Message
		- MC Clear Faults
		- MC Disable
		- MC Enable
	- decode
		- MC Faults
		- MC State


## Extra Functionality:
- Listed in order of importance

1. Bootup Checks part 1
	- Self test within the MCU
		- CAN
		- ADCs
1. Power Limiting
	- Limit torque requests when more than 80kW is being pulled from the pack
	- Reference [EV.1.4.4](https://drive.google.com/file/d/1RSS1m9ykvSWBmKwihLittIKef6_Kmfaw/view?usp=sharing) for specific guidelines with respect to timing
1. Bootup Checks part 2
	- Check Communication with other components on the CAN bus
		- Motor Controller
		- BMS Master
		- PDC
		- Dashboard
		- Analog CAN Board Front
		- Analog CAN Board Rear
		- Strain Gauge Board Front
		- Strain Gauge Board Rear
	- If any of the components do not respond throw an error onto the dash warning which board is not responding
		- If one of the buttons on the dash is pressed move onto the next component that is not connected or begin regular operation

1. Data Collection - **Need to specify frequency  requirements**
	- Once collected send onto the CAN bus
	- Brake Pressures
		- Brake front
		- Brake rear
	- Current Sensor
	- Wheel Speeds
		- Front right/left
		- Rear right/left
	- Shutdown Faults
		- IMD_OK
		- BMS_OK
		- BSPD_OK
	- Shutdown Sense
1. Traction Control
	- Implement a control loop based on wheel speeds that limits the torque to the wheels when the rear wheels lose traction

## Change Log
---
1. 11/3/18 - Modified Formatting and added extra features
1. 11/10/18 - Added Bootup Checks
