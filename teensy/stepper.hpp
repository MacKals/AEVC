//
//  stepper.hpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//


// Max STP pulse seperation 2 to 4 us.


#ifndef stepper_hpp
#define stepper_hpp

#include "core/Arduino.h"
#include "constants.h"

class Stepper {
private:

    const uint8_t DIR;
    const uint8_t STEP;
    const uint8_t EN;
    const uint8_t CHOP;

    const uint8_t TX;
    const uint8_t RX;

    const bool reversePositive; //to normalize "forward" for all motors

    bool currentStep = false;

public:

    Stepper(int d, int s, int e, int c,  int t, int r, bool reverse = false):
        DIR(d), STEP(s), EN(e), CHOP(c), TX(t), RX(r), reversePositive(reverse) {
            pinMode(DIR, OUTPUT);
            pinMode(STEP, OUTPUT);
            pinMode(EN, OUTPUT);
            pinMode(CHOP, OUTPUT);
        };

    void step();

    void forward();
    void backward();

    void enable();
    void disable();

    void chopOn();
    void chopOff();

// prot
    volatile float currentStepVelocity = 0; // steps per second

    // Period between steps at current velocity
    // Given in multiples of interrupt period
    const uint32_t currentStepPeriod() {
        return (float) INTERRUPT_FREQUENCY / (float) currentStepVelocity;
    }


protected:

    bool stepping = false;
    bool movingForward = true;


};

class DrivetrainStepper : public Stepper {
private:
    int32_t currentStepCount = 0;
    int32_t targetStepCount = 0;

public:

    DrivetrainStepper(int d, int s, int e, int c,  int t, int r, bool reverse = false) : Stepper(d, s, e, c, t, r, reverse) {};


    // Given velocity of stepper
    const double currentVelocity() {
        return currentStepVelocity * DISTANCE_PER_STEP;
    }

    void updateStepDirection() {
        if (currentStepCount < targetStepCount) {
            movingForward = true;
            forward();
        } else {
            movingForward = false;
            backward();
        }
    }

    void setRelativeTarget(float distance) {
        targetStepCount += distance / DISTANCE_PER_STEP;
        updateStepDirection();
        stepping = true;
    }

    void setAbsoluteTarget(float distance) {
        targetStepCount = distance / DISTANCE_PER_STEP;
        updateStepDirection();
        stepping = true;
    }

    void step(volatile uint32_t* time) {

        if (!stepping) return;

        // step with indicated frequency
        if (*time % currentStepPeriod() == 0) {

            // no more steps to take
            if (currentStepCount == targetStepCount) {
                stepping = false;
                currentStepVelocity = 0;
                Serial.print("MF done");
                return;
            }

            this->Stepper::step();
            currentStepCount += movingForward ? 1 : -1;
        }

    }

    void updateStepPeriod() {

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
};

// class ArmStepper : Stepper {
//
// }
//
// class TurnStepper : Stepper {
//
//
// }

#endif /* stepper_hpp */
