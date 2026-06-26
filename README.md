# Autonomous Obstacle-Avoiding Robot

Embedded mobile robot built on the **dsPIC30F4013** microcontroller, capable of autonomous left-wall-following navigation using dual ultrasonic distance sensing. The robot is started and stopped remotely via Bluetooth, and avoids obstacles by tracking the contour of objects along its left side instead of simply stopping.

---

## Features

- Bluetooth (HC-06) remote start/stop of autonomous mode via single-character commands
- Dual HC-SR04 ultrasonic sensors (front + left) for obstacle and wall-distance detection
- Left-wall-following navigation algorithm implemented as a finite state machine
- Automatic direction correction after turns to realign with the wall
- DC motor control via L298N driver with PWM speed control
- Real-time sensor readings and state reporting over UART to a PC/Bluetooth terminal
- Reset circuit and regulated 5V power supply for stable operation

---

## Hardware

- dsPIC30F4013 microcontroller
- 2x HC-SR04 ultrasonic distance sensors (front, left)
- L298N dual H-bridge motor driver
- 2x DC motors (left, right) with wheels/chassis
- HC-06 Bluetooth module
- LM7805CT voltage regulator (9V - 5V)
- Reset circuit (MCLR, pull-up resistor, debounce capacitor, push button)
- PICkit 3 programmer

### Pin Connections

| Signal              | MCU Pin |
|---------------------|---------|
| Left sensor TRIG    | RF0     |
| Left sensor ECHO    | RD9     |
| Right/Front sensor TRIG | RF1 |
| Right/Front sensor ECHO | RD8 |
| UART1 TX            | RF2     |
| UART1 RX            | RF3     |
| UART2 TX            | RF4     |
| UART2 RX            | RF5     |
| Motor IN1 / IN2     | RC13 / RC14 |
| Motor IN3 / IN4     | RD2 / RD3 |
| Motor ENA (PWM, OC1)| RB11      |
| Motor ENB (PWM, OC2)| RB12    |
| MCLR (reset)        | Pin 1   |

---

## System Operation

The system executes the following control flow:

1. **Start / Initialization** - peripherals are configured:
   ADC, UART1, Timer1, PWM (OC1/OC2), and motor/sensor pins.
   The robot starts stopped, printing "Spreman!" over UART.

2. **Idle loop** - the system continuously listens for Bluetooth commands.
   - Character `'s'` - enters autonomous mode, state set to `STANJE_NAPRED`.
   - Character `'x'` - exits autonomous mode at any time, motors stopped.

3. **Sensor reading** - while in autonomous mode, the front and left
   ultrasonic sensors are read every loop iteration and reported over UART.

4. **State machine** - navigation logic runs as a 4-state FSM:
   - **STANJE_NAPRED (forward)** - drives forward until the front sensor
     detects an obstacle closer than 12 cm, or the left sensor detects the
     wall is farther than 22 cm away - transitions to `STANJE_STOP`.
   - **STANJE_STOP** - motors stop, left sensor is re-checked: if distance
     from the wall is too large, transitions to `SKRENI_LEVO`; otherwise to
     `SKRENI_DESNO`.
   - **SKRENI_LEVO (turn left)** - left motor reverses, right motor moves
     forward for a timed interval, then direction correction is run before
     returning to `STANJE_NAPRED`.
   - **SKRENI_DESNO (turn right)** - right motor reverses, left motor moves
     forward for a timed interval, then direction correction (applied twice)
     before returning to `STANJE_NAPRED`.

5. **Direction correction** - after each turn, the left sensor distance is
   checked; if greater than 4 cm, the robot switches between short forward/left
   pulses until it comes back toward the wall.

### Motor Control Signals

| Action       | IN1 | IN2 | IN3 | IN4 | PWM duty |
|--------------|-----|-----|-----|-----|----------|
| Forward      | 1   | 0   | 1   | 0   | 2000 / 2499 |
| Turn left    | 0   | 1   | 1   | 0   | 2000 / 2499 |
| Turn right   | 1   | 0   | 0   | 1   | 2000 / 2499 |
| Stop         | 0   | 0   | 0   | 0   | 0 |

---

## Software

- **IDE / Compiler:** MPLAB (XC compiler for dsPIC)
- **Target:** dsPIC30F4013
- **UART baud rate:** UART1 used for command input and status output (TX: RF2, RX: RF3); UART2 reserved for a second external device (TX: RF4, RX: RF5)
- **ADC:** 12-bit, configured via `ADPCFG`
- **PWM:** Output Compare modules OC1 and OC2 used for motor speed control
- **Timer:** Timer1 used for ultrasonic echo pulse timing
- **Distance calculation:** echo HIGH time x 0.01372 = distance in cm

---

## Project Structure

```
/
├── src/
│   └── main.c          # Main application source
├── hardware/
│   └── schematic.png   # Block-level wiring diagram
└── README.md
```

---

## References

- V. Rajs, *Praktikum za vežbe iz primenjene elektronike*, FTN izdavaštvo, 2024
- [dsPIC30F4013 Datasheet](https://ww1.microchip.com/downloads/en/devicedoc/70138c.pdf)
- [LM7805CT Voltage Regulator](https://www.indiamart.com/proddetail/lm7805ct-regulator-ic-23944319355.html)
- [HC-SR04 Ultrasonic Sensor Datasheet](https://datasheet4u.com/download_new.php?id=1380138)
- [UART Serial Communication Reference](https://ww1.microchip.com/downloads/en/DeviceDoc/70066b.pdf)
