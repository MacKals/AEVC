//
//  Controller.cpp
//  teensy
//
//  Created by <author> on 18/02/2018.
//
//

#include "Controller.hpp"


const String Controller::popArgument(String &in) {
    int i = in.indexOf(' ');

    if (i == -1) {
        String ret = in;
        in = "";
        return ret;
    }

    String ret = in.substring(0, i);
    in = in.substring(i+1, in.length());
    return ret;
}

bool Controller::executeCommand(String s) {

    String command, arg1, arg2;

    command = popArgument(s);
    // Serial.print(s);
    // Serial.print("; ");
    arg1 = popArgument(s);
    arg2 = popArgument(s);

    // Serial.print(command);
    // Serial.print(", ");
    // Serial.print(arg1);
    // Serial.print(", ");
    // Serial.print(arg2);
    // Serial.print("; ");

    if (command == "EN") {
        leftMotor.enable();
        rightMotor.enable();
    } else

    if (command == "DI") {
        leftMotor.disable();
        rightMotor.disable();
    } else

    if (command == "MF") {

        leftMotor.enable();
        rightMotor.enable();

        float distance = arg1.toFloat()/1000.0;

        // Serial.print("distance: ");
        // Serial.println(distance);

        leftMotor.setRelativeTarget(distance);
        rightMotor.setRelativeTarget(distance);
    } else

    if (command == "S") {
        int distance = arg1.toFloat()/1000.0 * WHEELBASE_RADIUS;
        leftMotor.setRelativeTarget(-distance);
        rightMotor.setRelativeTarget(distance);
    } else

    if (command == "CY") {
        leftMotor.chopOn();
        rightMotor.chopOn();
    } else

    if (command == "CN") {
        leftMotor.chopOff();
        rightMotor.chopOff();
    } else


    {
        return false;
    }

    return true;
}
