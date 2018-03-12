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
    digitalWriteFast(pin.CHOP, true);
}

void Stepper::chopOff() {
    digitalWriteFast(pin.CHOP, false);
}

void MotionStepper::setVelocityTarget(double speed) {
    if (speed == 0.0) {
        velocityTarget = param.MAX_VELOCITY * 0.8;
    } else if (speed > param.MAX_VELOCITY || speed < 0.0) {
        Serial.println("Speed not accepted.");
        return;
    }

    velocityTarget = speed;

    Serial.print("velocityTarget: ");
    Serial.println(velocityTarget);
}

void MotionStepper::setAccelerationTarget(double acceleration) {
    if (acceleration == 0.0) {
        accelerationTarget = param.MAX_ACCELERATION * 0.8;
    } else if (acceleration > param.MAX_ACCELERATION || acceleration < 0.0) {
        Serial.println("Acceleration not accepted.");
        return;
    }

    accelerationTarget = acceleration;

    const float delta_v = accelerationTarget/INTERRUPT_FREQUENCY; // m/s
    delta_stepVelocity = delta_v / param.DISTANCE_PER_STEP;       // step/s
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
            // Serial.print("currentStepVelocity ");
            // Serial.println(currentStepVelocity);
            currentStepVelocity = 0.0;
            return;
        }

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

        lastStepVelocity = currentStepVelocity;

        // Serial.print("moveSteps ");
        // Serial.print(moveSteps, 6);
        // Serial.print(" moveDistance ");
        // Serial.print(moveDistance, 6);
        // Serial.print(" targetAcceleration ");
        // Serial.print(targetAcceleration,6);
        // Serial.print("  currentVelocity ");
        // Serial.println(currentVelocity(),6);

        if (abs(targetAcceleration) < accelerationTarget) {
            if (abs(currentVelocity()) < velocityTarget) {
                currentStepVelocity += delta_stepVelocity * directionSign();
            }
        } else if (abs(targetAcceleration) > accelerationTarget) {
            currentStepVelocity -= abs(targetAcceleration) * 2 / INTERRUPT_FREQUENCY / param.DISTANCE_PER_STEP * directionSign();
            //currentStepVelocity -= param.DELTA_SV * directionSign();
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
    if (endstopInactive() || atEndstop) {
        MotionStepper::step();
        if (endstopInactive()) atEndstop = false;
    } else {
        endstopHit();
        atEndstop = true;
    }
}


bool EndstopStepper::endstopInactive() {
    if (ENDSTOP_THRESHOLD) {
        // Serial.print("An: ");
        // Serial.println(analogRead(ENDSTOP_PIN) < ENDSTOP_THRESHOLD);
        return rollingAverage(analogRead(ENDSTOP_PIN)) < ENDSTOP_THRESHOLD;
    }

    // Serial.print("Dig: ");
    // Serial.print(digitalReadFast(ENDSTOP_PIN));

    return digitalReadFast(ENDSTOP_PIN);
}

void EndstopStepper::endstopHit() {
    homeCompletedFunction(this);
    Serial.println("hit ");
    stop();
}

void MotionStepper::stop() {
    stepping = false;
    currentStepCount = 0;
    currentStepVelocity = 0;
}

void EndstopStepper::home(void (*f) (EndstopStepper*)) {
    homeCompletedFunction = f;

    currentStepCount = RANGE/param.DISTANCE_PER_STEP; // assume we are at top
    MotionStepper::setRelativeTarget(0);              // go towards endstop
    atEndstop = false; // keep track for button depressed for more cycles
}
