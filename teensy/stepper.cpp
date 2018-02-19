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
    digitalWriteFast(pin.STEP, currentStep);
    currentStep = !currentStep;
}

void Stepper::forward() {
    digitalWriteFast(pin.DIR, reversePositive);
}

void Stepper::backward() {
    digitalWriteFast(pin.DIR, !reversePositive);
}

void Stepper::enable() {
    digitalWriteFast(pin.EN, false);
}

void Stepper::disable() {
    digitalWriteFast(pin.EN, true);
}

void Stepper::chopOn() {
    digitalWriteFast(pin.CHOP, false);
}

void Stepper::chopOff() {
    digitalWriteFast(pin.CHOP, true);
}


/* DrivetrainStepper */

// Given velocity of stepper
const double MotionStepper::currentVelocity() {
    return currentStepVelocity * param.DISTANCE_PER_STEP;
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

void MotionStepper::setRelativeTarget(float distance) {
    targetStepCount += distance / param.DISTANCE_PER_STEP;
    stepping = true;
}

void MotionStepper::setAbsoluteTarget(float distance) {
    targetStepCount = distance / param.DISTANCE_PER_STEP;
    stepping = true;
}

// Should be called every interrupt period
void MotionStepper::step() {

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

        Stepper::step();
        currentStepCount += directionSign();

        updateStepPeriod();
    }
}

void MotionStepper::updateStepPeriod() {

    const int moveSteps = targetStepCount-currentStepCount;

    if (moveSteps) {
        const double moveDistance = moveSteps * param.DISTANCE_PER_STEP;
        const double targetAcceleration = pow(currentVelocity(), 2) / (2 * moveDistance); // v^2 = u^2 + 2as, so a = u^2/(2s)

        lastStepVelocity = currentStepVelocity;

        if (abs(targetAcceleration) < param.MAX_ACCELERATION) {
            if (abs(currentVelocity()) < param.MAX_VELOCITY) {
                currentStepVelocity += param.DELTA_SV * directionSign();
            }
        } else if (abs(targetAcceleration) > param.MAX_ACCELERATION) {
            currentStepVelocity -= param.DELTA_SV * directionSign();
        }

        // Ensure DIR pin is set as we pass zero velocity
        const int floatZero = 1.0/MAX_STEP_PERIOD;
        if (lastStepVelocity >= floatZero && currentStepVelocity < 0.0) {
            backward();
        } else if (lastStepVelocity <= floatZero && currentStepVelocity > 0.0) {
            forward();
        }
    }
}


/* EndstopStepper */

// set target position for stepper, returns false if position is invalid
bool EndstopStepper::setRelativeTarget(float distance) {
    const float absTarget = currentStepCount * param.DISTANCE_PER_STEP + distance;
    if (0 < absTarget || absTarget < RANGE) {
        MotionStepper::setRelativeTarget(distance);
        return true;
    }
    return false;
}

bool EndstopStepper::setAbsoluteTarget(float distance) {
    if (0 < distance || distance < RANGE) {
        MotionStepper::setAbsoluteTarget(distance);
        return true;
    }
    return false;
}

void EndstopStepper::step() {
    if (endstopInactive()) MotionStepper::step();
    else endstopHit();
}

bool EndstopStepper::endstopInactive() {
    if (!digitalReadFast(ENDSTOP_PIN)) return true;

    //TODO: what if the endstop is depressed?

    return false;
}

void EndstopStepper::endstopHit() {
    currentStepCount = 0;
    currentStepVelocity = 0;
}
