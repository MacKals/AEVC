//
//  stepper.cpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//

#include "Stepper.hpp"

// 1 is logically on for motor, 0 logically off.


// Stepper

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


// DrivetrainStepper

// Given velocity of stepper
const double DrivetrainStepper::currentVelocity() {
    return currentStepVelocity * DISTANCE_PER_STEP;
}

void DrivetrainStepper::updateStepDirection() {
    if (currentStepCount < targetStepCount) {
        movingForward = true;
        forward();
    } else {
        movingForward = false;
        backward();
    }
}

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

        // no more steps to take
        if (currentStepCount == targetStepCount) {
            stepping = false;
            currentStepVelocity = 0;
            return;
        }

        this->Stepper::step();
        currentStepCount += movingForward ? 1 : -1;
    }
}

void DrivetrainStepper::updateStepPeriod() {

    int moveSteps = targetStepCount-currentStepCount;

    if (moveSteps) {

        // Serial.print("moveSteps: ");
        // Serial.print(moveSteps);
        // Serial.print(", targetAcceleration: ");

        double moveDistance = moveSteps * DISTANCE_PER_STEP;

        // v^2 = u^2 + 2as
        // a = u^2/(2s)
        double targetAcceleration = pow(currentVelocity(), 2) / (2 * moveDistance);

        // Serial.print(targetAcceleration);
        // Serial.print(", currentStepVelocity: ");

        if (abs(targetAcceleration) < MAX_ACCELERATION) {
            if (abs(currentVelocity()) < MAX_VELOCITY) {
                int negative = movingForward ? 1 : -1;
                currentStepVelocity += DELTA_SV * negative;
            }
        } else if (abs(targetAcceleration) > MAX_ACCELERATION) {
            int negative = movingForward ? 1 : -1;
                currentStepVelocity -= DELTA_SV * negative;
        }

        // Serial.print(currentVelocity());
        // Serial.print(", Period: ");
        // Serial.println(currentStepPeriod());

    }
}
