//
//  stepper.cpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//

#include "Stepper.hpp"

// 1 is logically on for motor, 0 logically off.

void Stepper::step() {
    digitalWriteFast(STEP, currentStep);
    currentStep = !currentStep;
}

void Stepper::forward() {
    digitalWriteFast(DIR, reversePositive);
    currentStep = !currentStep;
}

void Stepper::backward() {
    digitalWriteFast(DIR, !reversePositive);
    currentStep = !currentStep;
}

void Stepper::enable() {
    digitalWriteFast(EN, true);
    currentStep = !currentStep;
}

void Stepper::disable() {
    digitalWriteFast(EN, false);
    currentStep = !currentStep;
}
