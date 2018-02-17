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

//prot
    volatile float currentStepVelocity = 0; // steps per second

    // Period between steps at current velocity
    // Given in multiples of interrupt period
    const uint32_t currentStepPeriod() {
        return abs((float) INTERRUPT_FREQUENCY / (float) currentStepVelocity / 2.0);
    }


protected:
    bool stepping = false;
    bool movingForward = true;
};

class DrivetrainStepper : public Stepper {
private:
    int32_t currentStepCount = 0;
    int32_t targetStepCount = 0;

    int32_t stepCounter = 0; // to determine moved distance

public:

    DrivetrainStepper(int d, int s, int e, int c,  int t, int r, bool reverse = false):
        Stepper(d, s, e, c, t, r, reverse) {};

    const double currentVelocity();    // Given velocity of stepper

    void updateStepDirection();
    void setRelativeTarget(float distance);
    void setAbsoluteTarget(float distance);

    void step();     // Should be called every interrupt period
    void updateStepPeriod();
};


class EndstopStepper : Stepper {
private:
    const uint_8 ENDSTOP_PIN;

public:
    EndstopStepper(int d, int s, int e, int c, int t, int r, int pin, bool reverse = false):
        ENDSTOP_PIN(pin) {
            Stepper(d, s, e, c, t, r, reverse);
            pinMode(ENDSTOP_PIN, INPUT_PULLUP);
    };

    

};

#endif /* stepper_hpp */
