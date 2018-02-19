//
//  Controller.hpp
//  teensy
//
//  Created by <author> on 18/02/2018.
//
//

#ifndef Controller_hpp
#define Controller_hpp

#include <stdio.h>
#include "core/Arduino.h"
#include "Stepper.hpp"

class Controller {
private:

    // Link hardware

    const MotionStepper::Pin rmPin = MotionStepper::Pin(M2_DIR, M2_STEP, M2_EN, M2_CHOP, M2_TX, M2_RX);
    const MotionStepper::Pin lmPin = MotionStepper::Pin(M3_DIR, M3_STEP, M3_EN, M3_CHOP, M3_TX, M3_RX);

    const MotionStepper::Parameter dParam = MotionStepper::Parameter(D_DISTANCE_PER_STEP, D_MAX_ACEL, D_MAX_VEL);


    const MotionStepper::Pin hmPin = MotionStepper::Pin(M5_DIR, M5_STEP, M5_EN, M5_CHOP, M5_TX, M5_RX);
    const MotionStepper::Parameter hmParam = MotionStepper::Parameter(H_DISTANCE_PER_STEP, H_MAX_ACEL, H_MAX_VEL);

    const MotionStepper::Pin tmPin = MotionStepper::Pin(M4_DIR, M4_STEP, M4_EN, M4_CHOP, M4_TX, M4_RX);
    const MotionStepper::Parameter tmParam = MotionStepper::Parameter(T_DISTANCE_PER_STEP, T_MAX_ACEL, T_MAX_VEL);


    MotionStepper rightMotor = MotionStepper(dParam, rmPin, true);
    MotionStepper leftMotor = MotionStepper(dParam, lmPin);

    EndstopStepper heightMotor = EndstopStepper(hmParam, hmPin, H_STOP_PIN, H_RANGE);
    EndstopStepper turnMotor = EndstopStepper(tmParam, tmPin, T_STOP_PIN, T_RANGE);


public:

    bool stepping = false;

    void step() {
        rightMotor.step();
        leftMotor.step();

        heightMotor.step();
        turnMotor.step();
    }

    void disableMotors() {
        leftMotor.disable();
        rightMotor.disable();

        heightMotor.disable();
        turnMotor.disable();
    }


    bool executeCommand(String s);

protected:
    const String popArgument(String &in);

};


#endif /* Controller_hpp */
