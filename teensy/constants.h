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

#include "core/WProgram.h"

#define PRINTING false


// Drivetrain
#define DISTANCE_PER_STEP 0.0005235987757  // m, 1.8deg * PI/180 * 5cm
#define WHEELBASE_RADIUS  0.27630/2       // m

#define MAX_VELOCITY      1.0             // m/s
#define MAX_ACCELERATION  0.1             // m/s^2

// HeightMotor
#define HM_RANGE 100 // TODO: determine value
#define HM_MAX_ACEL 1.0
#define HM_MAX_VEL 1.0

// TurnMotor
#define TM_RANGE 100 // TODO: determine value
#define TM_MAX_ACEL 1
#define TM_MAX_VEL 1

// Timing
#define US_TO_CLOCK(us) ((us)*72/2)

#define PERIOD 300000           // US_TO_CLOCK(1000000/120)
#define PERIODS_PER_CLOCK 3579  // 0x100000000/(PERIOD*5) truncated

#define STEP_INTERRUPT_PERIOD   100      // us
#define MICROSECOND             1000000  // s
#define INTERRUPT_FREQUENCY     (MICROSECOND/STEP_INTERRUPT_PERIOD)    // Hz
#define DELTA_V                 (MAX_ACCELERATION/INTERRUPT_FREQUENCY) // m/s
#define DELTA_SV                (DELTA_V/DISTANCE_PER_STEP)            // step/s
#define MAX_STEP_PERIOD         0.1*INTERRUPT_FREQUENCY               // step periods

#define BLINK_PERIOD 500000 //us




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
#define M5_CHOP 36
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


#define MAX_UINT 0xFFFF


#endif /* constants_h */
