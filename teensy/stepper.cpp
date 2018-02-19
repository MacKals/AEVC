//
//  stepper.cpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//

#include "Stepper.hpp"

// 1 is logically on for motor, 0 logically off.


/* Stepper */

void Stepper::step() {
    digitalWriteFast(STEP, currentStep);
    currentStep = !currentStep;
}

void Stepper::forward() {
    digitalWriteFast(DIR, reversePositive);
}

void Stepper::backward() {
    digitalWriteFast(DIR, !reversePositive);
}

void Stepper::enable() {
    digitalWriteFast(EN, false);
}

void Stepper::disable() {
    digitalWriteFast(EN, true);
}

void Stepper::chopOn() {
    digitalWriteFast(CHOP, false);
}

void Stepper::chopOff() {
    digitalWriteFast(CHOP, true);
}


/* DrivetrainStepper */

// Given velocity of stepper
const double DrivetrainStepper::currentVelocity() {
    return currentStepVelocity * DISTANCE_PER_STEP;
}

// // TODO: what if we allready have velity in the other direction?
// void DrivetrainStepper::updateStepDirection() {
//     if (currentStepCount < targetStepCount) {
//         movingForward = true;
//         forward();
//     } else {
//         movingForward = false;
//         backward();
//     }
// }

void DrivetrainStepper::setRelativeTarget(float distance) {
    targetStepCount += distance / DISTANCE_PER_STEP;
    updateStepDirection();
    stepping = true;
}

void DrivetrainStepper::setAbsoluteTarget(float distance) {
    targetStepCount = distance / DISTANCE_PER_STEP;
    updateStepDirection();
    stepping = true;
}

// Should be called every interrupt period
void DrivetrainStepper::step() {

    if (!stepping) return;

    // step with indicated frequency
    if (stepCounter-- <= 0) {

        stepCounter = min(currentStepPeriod(), MAX_STEP_PERIOD);

        // No more steps to take
        if (currentStepCount == targetStepCount) {
            stepping = false;
            currentStepVelocity = 0.0;
            return;
        }

        this->Stepper::step();
        currentStepCount += directionSign();

        updateStepPeriod();
    }
}

void DrivetrainStepper::updateStepPeriod() {

    const int moveSteps = targetStepCount-currentStepCount;

    if (moveSteps) {
        const double moveDistance = moveSteps * DISTANCE_PER_STEP;
        const double targetAcceleration = pow(currentVelocity(), 2) / (2 * moveDistance); // v^2 = u^2 + 2as, so a = u^2/(2s)

        lastStepVelocity = currentStepVelocity;

        if (abs(targetAcceleration) < MAX_ACCELERATION) {
            if (abs(currentVelocity()) < MAX_VELOCITY) {
                currentStepVelocity += DELTA_SV * directionSign();
            }
        } else if (abs(targetAcceleration) > MAX_ACCELERATION) {
            currentStepVelocity -= DELTA_SV * directionSign();
        }

        // Ensure DIR pin is set as we pass zero velocity
        const int floatZero = 1.0/MAX_STEP_PERIOD;
        if (lastStepVelocity >= floatZero && currentStepVelocity < 0.0) {
            backward();
        } else if (lastStepVelocity <= floatZero && currentStepVelocity > 0.0) {
            forward();
        }
    }


    /* EndstopStepper */

    // set target position for stepper, returns false if position is invalid
    bool EndstopStepper::setRelativeTarget(float num) {

    }

    bool EndstopStepper::setAbsoluteTarget(float num) {

    }

    void EndstopStepper::step() {

    }

    void EndstopStepper::updateStepPeriod() {

    }

    bool EndstopStepper::endstopActive() {
        if (!digitalReadFast(ENDSTOP_PIN)) return true;

        //TODO: what if the endstop is depressed?

        return false;
    }

}
