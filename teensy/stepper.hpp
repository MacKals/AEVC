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

public:
    struct Pin {
        const uint8_t DIR;
        const uint8_t STEP;
        const uint8_t EN;
        const uint8_t CHOP;

        const uint8_t TX;
        const uint8_t RX;

        Pin(uint8_t d, uint8_t s, uint8_t e, uint8_t c, uint8_t t, uint8_t r):
            DIR(d), STEP(s), EN(e), CHOP(c), TX(t), RX(r) {}
    };

private:
    const Pin pin;
    const bool reversePositive; //to normalize "forward" for all motors
    bool currentStep = false;

public:
    Stepper(Pin pin, bool reverse = false):
        pin(pin), reversePositive(reverse) {
            pinMode(pin.DIR, OUTPUT);
            pinMode(pin.STEP, OUTPUT);
            pinMode(pin.EN, OUTPUT);
            pinMode(pin.CHOP, OUTPUT);
        };

    void step();

    void forward();
    void backward();

    void enable();
    void disable();

    void chopOn();
    void chopOff();

};


class MotionStepper : public Stepper {
protected:
    int32_t currentStepCount = 0;
    int32_t targetStepCount = 0;

    int32_t stepCounter = 0; // to determine moved distance

    void updateStepPeriod();

public:

    // Operational constants
    struct Parameter {
        const float DISTANCE_PER_STEP;
        const float MAX_ACCELERATION;
        const float MAX_VELOCITY;
        float DELTA_SV;

        Parameter(float dps, float ma, float mv):
            DISTANCE_PER_STEP(dps), MAX_ACCELERATION(ma), MAX_VELOCITY(mv) {

                float DELTA_V = MAX_ACCELERATION/INTERRUPT_FREQUENCY; // m/s
                DELTA_SV = DELTA_V / DISTANCE_PER_STEP;               // step/s
            }
    };

    MotionStepper(Parameter param, Pin pin, bool reverse = false):
        Stepper(pin, reverse), param(param) {};

    const double currentVelocity();    // Given velocity of stepper

    void updateStepDirection();
    void setRelativeTarget(float distance);
    void setAbsoluteTarget(float distance);

    void step();     // Should be called every interrupt period


protected:
    const Parameter param;

    bool stepping = false;

    float lastStepVelocity = 0;     // steps per second
    float currentStepVelocity = 0;  // steps per second

    /**
     * Computes the current step period.
     * @return Period between steps at current velocity, given in multiples of interrupt period
     */
    const uint32_t currentStepPeriod() {
        return abs((float) INTERRUPT_FREQUENCY / (float) currentStepVelocity / 2.0);
    }

    float directionSign() {
        return currentStepVelocity/abs(currentStepVelocity);
    }
};


class EndstopStepper : MotionStepper {
private:
    const uint8_t ENDSTOP_PIN;
    const uint32_t RANGE;

public:

    EndstopStepper(Parameter param, Pin pin, int endstopPin, int range, bool reverse = false):
        MotionStepper(param, pin, reverse), ENDSTOP_PIN(endstopPin), RANGE(range) {
            pinMode(ENDSTOP_PIN, INPUT_PULLUP);
    };

    // set target position for stepper, returns false if position is invalid
    bool setRelativeTarget(float distance);
    bool setAbsoluteTarget(float distance);

    void step();     // Should be called every interrupt period

    bool endstopInactive();
    void endstopHit();
};

#endif /* stepper_hpp */
