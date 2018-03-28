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


    const MotionStepper::Pin tmPin = MotionStepper::Pin(M5_DIR, M5_STEP, M5_EN, M5_CHOP, M5_TX, M5_RX);
    const MotionStepper::Parameter hmParam = MotionStepper::Parameter(H_DISTANCE_PER_STEP, H_MAX_ACEL, H_MAX_VEL);

    const MotionStepper::Pin hmPin = MotionStepper::Pin(M6_DIR, M6_STEP, M6_EN, M6_CHOP, M6_TX, M6_RX);
    const MotionStepper::Parameter tmParam = MotionStepper::Parameter(T_DISTANCE_PER_STEP, T_MAX_ACEL, T_MAX_VEL);

    MotionStepper rightMotor = MotionStepper(dParam, rmPin);
    MotionStepper leftMotor = MotionStepper(dParam, lmPin, true);

    EndstopStepper heightMotor = EndstopStepper(hmParam, hmPin, H_STOP_PIN, H_RANGE);
    EndstopStepper turnMotor = EndstopStepper(tmParam, tmPin, T_STOP_PIN, T_RANGE, QRD_THRESHOLD);

public:

    void step() {
        if (heightMotor.homingCompleted) homingDoneHeightMotor();
        if (turnMotor.homingCompleted) homingDoneTurnMotor();

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

    void homingDoneHeightMotor() {
        heightMotor.homingCompleted = false;
        heightMotor.setRelativeTarget(0.05);  // take 5cm up
    }

    void homingDoneTurnMotor() {
        turnMotor.homingCompleted = false;

        leftMotor.stop();
        rightMotor.stop();

        // move to face straight ahead
        turnBase(-T_ENDSTOP_ANGLE);
        turnMotor.setRelativeTarget(T_ENDSTOP_ANGLE);
    }

    double angleForBase(double angle) {
        return angle/180.0 * PI * WHEELBASE_RADIUS;
    }

    void syncronizeBaseBodyRotationParameters() {
        // set velocities so they are equal for turn and left/right
        double tVelocity = T_MAX_VEL*0.8;
        double tAcceleration = T_MAX_ACEL*0.8;

        double dVelocity = angleForBase(tVelocity);
        double dAcceleration = angleForBase(tAcceleration);

        turnMotor.setVelocityTarget(tVelocity);
        turnMotor.setAccelerationTarget(tAcceleration);

        rightMotor.setVelocityTarget(dVelocity);
        rightMotor.setAccelerationTarget(dAcceleration);
        leftMotor.setVelocityTarget(dVelocity);
        leftMotor.setAccelerationTarget(dAcceleration);
    }

    void homeTurnInterface() {
        syncronizeBaseBodyRotationParameters();

        turnBase(360.0);
        turnMotor.home();
    }

    bool turnBaseBodyStill(double angle) {
        syncronizeBaseBodyRotationParameters();

        // Do not spin if turn stepper will go out of range

        turnBase(angle);

        if (turnMotor.setRelativeTarget(-angle)) {
            return true;
        }

        return false;
    }

    void turnBase(double angle) {
        leftMotor.setRelativeTarget( - angleForBase(angle));
        rightMotor.setRelativeTarget( angleForBase(angle));
    }

    bool executeCommand(String s);

    void resetMotionTargets() {
        rightMotor.setVelocityTarget();
        rightMotor.setAccelerationTarget();
        leftMotor.setVelocityTarget();
        leftMotor.setAccelerationTarget();
    }

    void moveAroundPointAtRadius(double angle) {

    }

protected:
    const String popArgument(String &in);

};


#endif /* Controller_hpp */
