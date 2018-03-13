//
//  stepper.cpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//

#include "Stepper.hpp"
// 1 is logically on for motor, 0 logically off.


template <class T>
void print(T s) {
    Serial.print(s);
}

template <class T>
void println(T s) {
    Serial.println(s);
}



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
    digitalWriteFast(pin.CHOP, true);
}

void Stepper::chopOff() {
    digitalWriteFast(pin.CHOP, false);
}

void MotionStepper::setVelocityTarget(double speed) {
    if (speed == 0.0) {
        velocityTarget = param.MAX_VELOCITY * 0.8;
    } else if (speed > param.MAX_VELOCITY || speed < 0.0) {
        print("Speed: ");
        print(speed);
        print(" max: ");
        println(param.MAX_VELOCITY);
        return;
    } else {

        velocityTarget = speed;
    }

}

void MotionStepper::setAccelerationTarget(double acceleration) {
    if (acceleration == 0.0) {
        accelerationTarget = param.MAX_ACCELERATION * 0.8;
    } else if (acceleration > param.MAX_ACCELERATION || acceleration < 0.0) {
        print("Acceleration: ");
        print(acceleration);
        print(" max: ");
        println(param.MAX_ACCELERATION);
        return;
    } else {

        accelerationTarget = acceleration;
    }

    const double delta_v = accelerationTarget/INTERRUPT_FREQUENCY; // m/s
    delta_stepVelocity = delta_v / param.DISTANCE_PER_STEP;       // step/s
}



/* DrivetrainStepper */

// Given velocity of stepper
const double MotionStepper::currentVelocity() {
    return currentStepVelocity * param.DISTANCE_PER_STEP;
}

void MotionStepper::setRelativeTarget(float distance) {
    targetStepCount += distance / param.DISTANCE_PER_STEP;
}

void MotionStepper::setAbsoluteTarget(float distance) {
    targetStepCount = distance / param.DISTANCE_PER_STEP;
}

void MotionStepper::stop() {
    currentStepVelocity = 0.0;
    targetStepCount = currentStepCount;
}

// Should be called every interrupt period
void MotionStepper::step() {

    // No more steps to take
    if (currentStepCount == targetStepCount) {
        if (currentStepVelocity) currentStepVelocity = 0.0;
        return;
    }

    // step with indicated frequency
    if (stepCounter-- <= 0) {
        stepCounter = min(currentStepPeriod(), MAX_STEP_PERIOD);
        Stepper::step();
        currentStepCount += directionSign();
    }

    updateStepPeriod();
}

void MotionStepper::updateStepPeriod() {

    const int moveSteps = targetStepCount-currentStepCount;

    if (moveSteps) {
        const double moveDistance = moveSteps * param.DISTANCE_PER_STEP;
        const double targetAcceleration = pow(currentVelocity(), 2.0) / (2.0 * moveDistance); // v^2 = u^2 + 2as, so a = u^2/(2s)

        // print("moveSteps ");
        // Serial.print(moveSteps, 6);
        // Serial.print(" moveDistance ");
        // Serial.print(moveDistance, 6);
        // // Serial.print(" targetAcceleration ");
        // // Serial.print(targetAcceleration,6);
        // Serial.print("  currentVelocity ");
        // Serial.print(currentVelocity(),10);
        //
        // Serial.print("  accelerationTarget ");
        // Serial.print(accelerationTarget, 4);

        // Serial.print("  velocityTarget ");
        // Serial.print(velocityTarget, 4);

        if (abs(targetAcceleration) < accelerationTarget) {
            if (abs(currentVelocity()) < velocityTarget) {
                // Serial.print("  delta_SV+ ");
                // Serial.println(delta_stepVelocity * directionSign(), 10);

                currentStepVelocity += delta_stepVelocity * directionSign();
            }
        } else if (abs(targetAcceleration) > accelerationTarget) {

            // Serial.print("  delta_SV- ");
            // Serial.println(abs(targetAcceleration) * 2 / INTERRUPT_FREQUENCY / param.DISTANCE_PER_STEP * directionSign(), 10);

            currentStepVelocity -= abs(targetAcceleration) * 2 / INTERRUPT_FREQUENCY / param.DISTANCE_PER_STEP * directionSign();
            //currentStepVelocity -= param.DELTA_SV * directionSign();
        }

        // println("");
        // Set direction pin for going forward/backward
        if (currentStepVelocity < 0.0) {
            reversing = true;
            backward();
        }

        if (currentStepVelocity > 0.0) {
            reversing = false;
            forward();
        }
    }
}


/* EndstopStepper */

// set target position for stepper, returns false if position is invalid
bool EndstopStepper::setRelativeTarget(float distance) {
    const float absTarget = currentStepCount * param.DISTANCE_PER_STEP + distance;
    if (0 < absTarget && absTarget < RANGE) {
        MotionStepper::setRelativeTarget(distance);
        return true;
    }
    return false;
}

bool EndstopStepper::setAbsoluteTarget(float distance) {
    if (0 < distance && distance < RANGE) {
        MotionStepper::setAbsoluteTarget(distance);
        return true;
    }
    return false;
}

void EndstopStepper::step() {
    if (homing && !endstopInactive()) {
        currentStepCount = 0;
        stop();

        homing = false;
        homingCompleted = true;
        println(" Homing completed.");
    }

    MotionStepper::step();
}


bool EndstopStepper::endstopInactive() {
    if (ENDSTOP_THRESHOLD) {
        return rollingAverage(analogRead(ENDSTOP_PIN)) < ENDSTOP_THRESHOLD;
    }
    return digitalReadFast(ENDSTOP_PIN);
}


void EndstopStepper::home() {
    currentStepCount = RANGE/param.DISTANCE_PER_STEP; // assume we are at top
    MotionStepper::setAbsoluteTarget(0);              // go towards endstop
    homing = true; // keep track for button depressed for more cycles
}
