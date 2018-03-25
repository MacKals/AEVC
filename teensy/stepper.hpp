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
    volatile int32_t currentStepCount = 0;
    volatile int32_t targetStepCount = 0;

    int32_t stepCounter = 0; // to determine moved distance
    bool reversing = false;
    bool stepping = false;

    void updateStepPeriod();

    double velocityTarget, accelerationTarget;
    double delta_stepVelocity, decelerationConstant;

public:

    // Operational constants
    struct Parameter {
        const float DISTANCE_PER_STEP;
        const float MAX_ACCELERATION;
        const float MAX_VELOCITY;

        Parameter(float dps, float ma, float mv):
            DISTANCE_PER_STEP(dps), MAX_ACCELERATION(ma), MAX_VELOCITY(mv) {}

    };


    MotionStepper(Parameter param, Pin pin, bool reverse = false):
        Stepper(pin, reverse), param(param) {
            setVelocityTarget();
            setAccelerationTarget();
        };

    const double currentVelocity();    // Given velocity of stepper

    void updateStepDirection();
    void setRelativeTarget(float distance);
    void setAbsoluteTarget(float distance);

    void step();    // Should be called every interrupt period

    void stop();    // full motion stop

    // setting current motion parameters
    // Defaults to 80% of max speed
    void setVelocityTarget(double speed = 0.0);
    void setAccelerationTarget(double acceleration = 0.0);


protected:
    const Parameter param;

    volatile double currentStepVelocity = 0;  // steps per second

    /**
     * Computes the current step period.
     * @return Period between steps at current velocity, given in multiples of interrupt period
     */
    const uint32_t currentStepPeriod() {
        return abs((float) INTERRUPT_FREQUENCY / (float) currentStepVelocity / 2.0);
    }

    int directionSign() {
        if (currentStepVelocity == 0.0) {
            return (targetStepCount-currentStepCount) > 0 ? 1 : -1;
        }
        return currentStepVelocity > 0.0 ? 1 : -1;
    }
};


class EndstopStepper : public MotionStepper {
private:
    const uint8_t ENDSTOP_PIN;
    const int ENDSTOP_THRESHOLD;
    const uint32_t RANGE;
    //const 

    double average = 0.0;
    double N = 15;

    double rollingAverage (int new_sample) {
        average -= average / N;
        average += double (new_sample) / N;

        return average;
    }

public:

    EndstopStepper(Parameter param, Pin pin, int endstopPin, float range, float zero, int threshold = 0, bool reverse = false):
        MotionStepper(param, pin, reverse), ENDSTOP_PIN(endstopPin), ENDSTOP_THRESHOLD(threshold), RANGE(range/param.DISTANCE_PER_STEP) {
            pinMode(ENDSTOP_PIN, INPUT_PULLUP);
    };

    void home();
    bool homing = false;
    bool homingCompleted = false;

    // set target position for stepper, returns false if position is invalid
    bool setRelativeTarget(float distance);
    bool setAbsoluteTarget(float distance);

    void step();     // Should be called every interrupt period

    bool endstopInactive();

};

#endif /* stepper_hpp */
