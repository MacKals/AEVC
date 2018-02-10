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
