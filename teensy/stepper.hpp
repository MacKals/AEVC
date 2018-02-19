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








protected:
    float lastStepVelocity = 0;     // steps per second
    float currentStepVelocity = 0;  // steps per second

    float directionSign() {
        return currentStepVelocity/abs(currentStepVelocity);
    }

    /**
     * Computes the current step period.
     * @return Period between steps at current velocity, given in multiples of interrupt period
     */
    const uint32_t currentStepPeriod() {
        return abs((float) INTERRUPT_FREQUENCY / (float) currentStepVelocity / 2.0);
    }

    bool stepping = false;
};


class DrivetrainStepper : public Stepper {
private:
    int32_t currentStepCount = 0;
    int32_t targetStepCount = 0;

    int32_t stepCounter = 0; // to determine moved distance

    void updateStepPeriod();

public:

    DrivetrainStepper(int d, int s, int e, int c,  int t, int r, bool reverse = false):
        Stepper(d, s, e, c, t, r, reverse) {};

    const double currentVelocity();    // Given velocity of stepper

    void updateStepDirection();
    void setRelativeTarget(float distance);
    void setAbsoluteTarget(float distance);

    void step();     // Should be called every interrupt period
};


class EndstopStepper : Stepper {
private:
    const uint8_t ENDSTOP_PIN;
    const uint32_t RANGE;

public:
    EndstopStepper(int d, int s, int e, int c, int t, int r, int pin, int range, float max_vel, float max_acel, bool reverse = false):
        Stepper(d, s, e, c, t, r, reverse), ENDSTOP_PIN(pin), RANGE(range) {
            pinMode(ENDSTOP_PIN, INPUT_PULLUP);
    };

    // set target position for stepper, returns false if position is invalid
    bool setRelativeTarget(float num);
    bool setAbsoluteTarget(float num);

    void step();     // Should be called every interrupt period
    void updateStepPeriod();

    bool endstopInactive();
};

#endif /* stepper_hpp */
