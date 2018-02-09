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

    float targetVelocity = 0;
    float currentVelocity = 0;

public:

    Stepper(int d, int s, int e, int c,  int t, int r, bool reverse = false):
        DIR(d), STEP(s), EN(e), CHOP(c), TX(t), RX(r), reversePositive(reverse) {};

    void step();

    void forward();
    void backward();

    void enable();
    void disable();


protected:

};


#endif /* stepper_hpp */
