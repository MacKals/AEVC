//
//  EventHandler.hpp
//  AEVCcode
//
//  Created by <author> on 05/02/2018.
//
//

#ifndef EventHandler_hpp
#define EventHandler_hpp

#include <stdio.h>

#include "core/QueueArray.h"
#include "Stepper.hpp"

#include "constants.h"

class EventHandler {
private:

    std::vector<Stepper*> activeMotors;

    Stepper rightMotor = Stepper(M2_DIR, M2_STEP, M2_EN, M2_CHOP, M2_TX, M2_RX);
    Stepper leftMotor = Stepper(M3_DIR, M3_STEP, M3_EN, M3_CHOP, M3_TX, M3_RX);

    Stepper turnMotor = Stepper(M4_DIR, M4_STEP, M4_EN, M4_CHOP, M4_TX, M4_RX);
    Stepper heightMotor = Stepper(M5_DIR, M5_STEP, M5_EN, M5_CHOP, M5_TX, M5_RX);


    //QueueArray<int>
    void test(int, int);

public:
    EventHandler();
    ~EventHandler();

    static void step();
    void addInstruction(String instruction);


protected:




};


#endif /* EventHandler_hpp */
