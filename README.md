# Autonomous-Obstacle-Avoiding-Robot
Autonomous mobile robot built on the dsPIC30F4013 microcontroller. The robot navigates independently using a left-wall-following algorithm, detects obstacles with ultrasonic sensors, and avoids them by turning and realigning. Autonomous mode is started and stopped wirelessly via Bluetooth.

**Features**

Left-wall-following algorithm implemented as a finite state machine (FSM)
Dual HC-SR04 ultrasonic sensors (front and left) for obstacle and wall detection
DC motor control via L298N driver with PWM speed regulation
HC-06 Bluetooth module for wireless start/stop of autonomous mode
Real-time distance reporting over UART (9600 bps)
On-board power supply: 9V battery regulated to 5V via LM7805CT
Custom soldered PCB

**Hardware**

dsPIC30F4013 microcontroller (Microchip)
HC-SR04 ultrasonic sensors x2 (front and left)
L298N DC motor driver module
HC-06 Bluetooth module
LM7805CT voltage regulator (9V - 5V)
Two DC motors (left and right drive)
Reset circuit (MCLR, R1 4.7kΩ, C5 100nF)
PICkit 3 programmer

**System Operation**

The control logic is implemented as a 4-state FSM. Autonomous mode is entered by sending 's' via Bluetooth and exited at any point with 'x'.


STANJE_NAPRED (**Forward**) - robot moves forward. Transitions to STOP if front sensor reads < 12 cm or left sensor reads > 22 cm.
STANJE_STOP (**Stop**) - robot halts and re-reads left sensor. If distance > 22 cm → turn left; otherwise → turn right.
SKRENI_LEVO (**Turn left**) - left motor reverses, right motor forward. After turn, direction correction realigns the robot with the wall, then returns to FORWARD.
SKRENI_DESNO (**Turn right**) - right motor reverses, left motor forward. After turn, direction correction runs and robot returns to FORWARD.


Direction correction (**korekcija_pravca**) reads the left sensor and nudges the robot left in short intervals until the distance to the wall stops decreasing.

**PWM Parameters**

StateDuty cycle valueMax valueMoving20002499Stop02499

**Software**

IDE / Compiler: MPLAB X + XC16
Target: dsPIC30F4013
UART1 baud rate: 9600 bps (Bluetooth via HC-06)
Timer: Timer1 used for ultrasonic echo pulse measurement
PWM: Output Compare modules OC1 and OC2 for left/right motor speed

**Project Structure**

/
├── src/

│   └── main.c          # Main application source

├── hardware/
│   └── schematic.png   # Block-level wiring diagram

└── README.md


**References**

V. Rajs, Praktikum za vežbe iz primenjene elektronike, FTN, 2024
dsPIC30F4013 Datasheet
HC-SR04 Ultrasonic Sensor Datasheet
UART Module Reference Manual
