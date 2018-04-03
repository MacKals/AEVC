//
//  constants.h
//
//
//  Created by Morten Kals on 12/05/2017.
//
//  All units in microseconds
//

#ifndef constants_h
#define constants_h

template <class T>
extern void print(T s);

template <class T>
extern void println(T s);

// Teensy pinout

#define LED 13

// Motor 2
#define M2_DIR  11
#define M2_STEP 14
#define M2_EN   12
#define M2_CHOP 25
#define M2_TX   9
#define M2_RX   10
#define M2 Serial2

// Motor 3
#define M3_DIR  3
#define M3_STEP 5
#define M3_EN   4
#define M3_CHOP 6
#define M3_TX   7
#define M3_RX   8
#define M3 Serial3

// Motor 4
#define M4_DIR  44
#define M4_STEP 30
#define M4_EN   45
#define M4_CHOP 46
#define M4_TX   31
#define M4_RX   32
#define M4 Serial4

// Motor 5
#define M5_DIR  53
#define M5_STEP 37
#define M5_EN   52
#define M5_CHOP 35
#define M5_TX   34
#define M5_RX   33
#define M5 Serial5

// Motor 6
#define M6_DIR  39
#define M6_STEP 38
#define M6_EN   50
#define M6_CHOP 36
#define M6_TX   47
#define M6_RX   48
#define M6 Serial6

// Sense 2 pins
#define S2_1 23
#define S2_2 22
#define S2_3 A10
#define S2_4 21
#define S2_5 A11
#define S2_6 20

// Sense 3 pins (3.3V powered)
#define S3_1 57
#define S3_2 19
#define S3_3 56
#define S3_4 55

// Driven 24V pins
#define D2_1 18
#define D2_2 54
#define D2_3 16
#define D2_4 15



// Drivetrain
#define D_DISTANCE_PER_STEP 0.0005235987757 * 0.956 // m, 1.8deg * PI/180 * 5cm * 1/3
#define D_MAX_VEL   0.80                     // m/s
#define D_MAX_ACEL  0.10                     // m/s^2

#define WHEELBASE_RADIUS    0.27630/2 * 0.99       // m

// HeightMotor
#define H_DISTANCE_PER_STEP (0.008/200.0/16.0)          // m/step
#define H_MAX_ACEL 0.05         // m/s^2
#define H_MAX_VEL  0.08          // m/s

#define H_STOP_PIN S2_1         // pin number
#define H_RANGE 0.15            // m

#define QRD_THRESHOLD 650

// TurnMotor
#define TURN_WHEEL_RADIUS    0.135565       // m

#define T_DISTANCE_PER_STEP (1.8*0.035/TURN_WHEEL_RADIUS/16.0) // deg/step
#define T_MAX_ACEL 30.0          // deg/s^2
#define T_MAX_VEL  50.0          // deg/s

#define T_STOP_PIN D2_1          // pin number or 18
#define T_RANGE 360.0            // deg

#define T_ENDSTOP_ANGLE 180.0-6  // degrees

// Timing
#define US_TO_CLOCK(us) ((us)*72/2)

#define PERIOD 300000           // US_TO_CLOCK(1000000/120)
#define PERIODS_PER_CLOCK 3579  // 0x100000000/(PERIOD*5) truncated

#define STEP_INTERRUPT_PERIOD   100      // us
#define MICROSECOND             1000000  // s
#define INTERRUPT_FREQUENCY     (MICROSECOND/STEP_INTERRUPT_PERIOD)   // Hz
#define MAX_STEP_PERIOD         (0.1*INTERRUPT_FREQUENCY)             // step periods

#define BLINK_PERIOD 500000 //us

#define MAX_UINT 0xFFFF

#endif /* constants_h */
