//
//  stepper.hpp
//  AEVCcode
//
//  Created by <author> on 04/02/2018.
//
//

#ifndef stepper_hpp
#define stepper_hpp

#include <stdio.h>

class Stepper {
private:
    const int8_t DIR;
    const int8_t STEP;
    const int8_t EN;
    const int8_t CHOP;

    const int8_t RX;
    const int8_t TX;

    const bool constrained;

    const bool reversePositive; //to normalize "forward" for all motors


    bool stp = false;
    bool pos = true;


public:
    Stepper(int d, int s, int e, int c, int r, int t, bool constrained, bool reverse = false):
        DIR(d), STEP(s), EN(e), CHOP(c), RX(r), TX(t), constrained(constrained), reversePositive(reverse) {};

    //~Stepper();

    void step(int direction, int speed);
    void stepFor(int distance, int direction, int speed);


protected:

};


#endif /* stepper_hpp */
