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
        DIR(d), STEP(s), EN(e), CHOP(c), TX(t), RX(r), reversePositive(reverse) {};

    void step();

    void forward();
    void backward();

    void enable();
    void disable();


protected:

    // volatile uint32_t targetStepPeriod = 0; // 0 is off, infinate period
    volatile uint32_t currentStepPeriod = 0; // 0 is off, infinate period

};

class DrivetrainStepper : Stepper {
private:
    int currentSteps = 0;
    int targetSteps = 0;

public:

    DrivetrainStepper(int d, int s, int e, int c,  int t, int r, bool reverse = false) : Stepper(d, s, e, c, t, r, reverse) {};

    void setRelativeTarget(int distance) {
        currentSteps += distance / DISTANCE_PER_STEP;
    }

    void setAbsoluteTarget(int distance) {
        currentSteps = distance / DISTANCE_PER_STEP;
    }

    void step(volatile uint32_t* time) {

        // no more steps to take
        if (currentSteps == targetSteps) {
            this->currentStepPeriod = 0;
            return;
        }

        // step with indicated frequency
        if (*time % currentStepPeriod) this->Stepper::step();
    }

    const double currentVelocity() {
        uint32_t f = currentStepPeriod * STEP_INTERRUPT_PERIOD * MICROSECONDS;
        return static_cast<double> (1) / f * DISTANCE_PER_STEP;
    }

    const double stepPeriodFromAcceleration(double acceleration) {

    }

    void updateStepPeriod() {

        // v^2 = u^2 + 2as
        // a = u^2/(2s)
        int moveSteps = targetSteps-currentSteps;
        double moveDistance = moveSteps * DISTANCE_PER_STEP;

        double targetAcceleration = pow(currentVelocity(), 2) / (2 * moveDistance);

        if (MAX_ACCELERATION < targetAcceleration) {
            currentStepPeriod++;
        } else if (MAX_ACCELERATION > targetAcceleration) {
            currentStepPeriod--;
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
