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
        enableMotors();
    } else

    if (command == "DI") {
        disableMotors();
    } else

    if (command == "MF") {
        float distance = arg1.toFloat()/1000.0;
        leftMotor.setRelativeTarget(distance);
        rightMotor.setRelativeTarget(distance);
    } else

    if (command == "S") {
        float distance = arg1.toFloat()/180.0 * PI * WHEELBASE_RADIUS;
        leftMotor.setRelativeTarget(-distance);
        rightMotor.setRelativeTarget(distance);
    } else

    if (command == "A") {
        float height = arg1.toFloat()/1000.0;
        Serial.print(heightMotor.setRelativeTarget(height));
    } else

    if (command == "ST") {
        float angle = arg1.toFloat();
        Serial.print(turnMotor.setRelativeTarget(angle));
    } else

    if (command == "CY") {
        leftMotor.chopOn();
        rightMotor.chopOn();

        heightMotor.chopOn();
        turnMotor.chopOn();
    } else

    if (command == "CN") {
        leftMotor.chopOff();
        rightMotor.chopOff();

        heightMotor.chopOff();
        turnMotor.chopOff();
    } else

    if (command == "AE") {
        Serial.print(heightMotor.endstopInactive());
        Serial.print(" ");
    } else

    if (command == "TE") {
        Serial.print(turnMotor.endstopInactive());
        Serial.print(" ");
    } else

    if (command == "HA") {

    } else

    if (command == "HT") {
        turnMotor.
    } else

    {
        return false;
    }

    return true;
}
