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

    const MotionStepper::Pin lmPin = MotionStepper::Pin(M2_DIR, M2_STEP, M2_EN, M2_CHOP, M2_TX, M2_RX);
    const MotionStepper::Pin rmPin = MotionStepper::Pin(M3_DIR, M3_STEP, M3_EN, M3_CHOP, M3_TX, M3_RX);

    const MotionStepper::Parameter dParam = MotionStepper::Parameter(D_DISTANCE_PER_STEP, D_MAX_ACEL, D_MAX_VEL);


    const MotionStepper::Pin tmPin = MotionStepper::Pin(M5_DIR, M5_STEP, M5_EN, M5_CHOP, M5_TX, M5_RX);
    const MotionStepper::Parameter hmParam = MotionStepper::Parameter(H_DISTANCE_PER_STEP, H_MAX_ACEL, H_MAX_VEL);

    const MotionStepper::Pin hmPin = MotionStepper::Pin(M6_DIR, M6_STEP, M6_EN, M6_CHOP, M6_TX, M6_RX);
    const MotionStepper::Parameter tmParam = MotionStepper::Parameter(T_DISTANCE_PER_STEP, T_MAX_ACEL, T_MAX_VEL);

    MotionStepper rightMotor = MotionStepper(dParam, rmPin, true);
    MotionStepper leftMotor = MotionStepper(dParam, lmPin);

    EndstopStepper heightMotor = EndstopStepper(hmParam, hmPin, H_STOP_PIN, H_RANGE);
    EndstopStepper turnMotor = EndstopStepper(tmParam, tmPin, T_STOP_PIN, T_RANGE, QRD_THRESHOLD);

    void (*homeCompletedFunction) (EndstopStepper*);


public:

    Controller (void (*f) (EndstopStepper*)) : homeCompletedFunction(f) {}

    bool stepping = false;
    bool homing = false;

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

    void enableMotors() {
        leftMotor.enable();
        rightMotor.enable();

        heightMotor.enable();
        turnMotor.enable();
    }

    void configureChop() {
        heightMotor.chopOn();
        turnMotor.chopOn();
    }

    void homingDone(EndstopStepper* s) {
        homing = false;

        if (s == &heightMotor) {
            heightMotor.setRelativeTarget(0.05); // take 5cm up
        } else

        if (s == &turnMotor) {
            leftMotor.stop();
            rightMotor.stop();

            // move to face straight ahead
            turnBase(180);
            turnMotor.setRelativeTarget(180);
        }
    }

    void homeTurnInterface() {
        // set velocities so they are equal for turn and left/right

        double tVelocity = T_MAX_VEL*0.8;
        double tAcceleration = T_MAX_ACEL*0.8;

        double dVelocity = tVelocity * WHEELBASE_RADIUS/TURN_WHEEL_RADIUS * PI/180.0;
        double dAcceleration = tAcceleration * WHEELBASE_RADIUS/TURN_WHEEL_RADIUS * PI/180.0;

        rightMotor.setVelocityTarget(dVelocity);
        rightMotor.setAccelerationTarget(dAcceleration);
        leftMotor.setVelocityTarget(dVelocity);
        leftMotor.setAccelerationTarget(dAcceleration);

        turnMotor.setVelocityTarget(tVelocity);
        turnMotor.setAccelerationTarget(tAcceleration);

        turnBase(-360);
        turnMotor.home(homeCompletedFunction);
    }

    void turnBase(double angle) {
        leftMotor.setRelativeTarget( -angle/180.0 * PI * WHEELBASE_RADIUS);
        rightMotor.setRelativeTarget( angle/180.0 * PI * WHEELBASE_RADIUS);
    }

    void resetMotionTargets() {
        rightMotor.setVelocityTarget();
        rightMotor.setAccelerationTarget();
        leftMotor.setVelocityTarget();
        leftMotor.setAccelerationTarget();
    }

    bool executeCommand(String s);

protected:
    const String popArgument(String &in);

};


#endif /* Controller_hpp */
