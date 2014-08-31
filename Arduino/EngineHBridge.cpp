// EngineHBridge.cpp

#include "EngineHBridge.h"
#include <Arduino.h>


EngineHBridge::EngineHBridge(const Config* config)
{
    this->_config = config;
}


void EngineHBridge::init()
{
    pinMode(_config->motor_left_a, OUTPUT);
    pinMode(_config->motor_left_b, OUTPUT);
    pinMode(_config->motor_right_a, OUTPUT);
    pinMode(_config->motor_right_b, OUTPUT);
}


void EngineHBridge::motorOn(MOTOR motor, bool reverse)
{
    uint8_t a_value = (reverse ? LOW : HIGH);
    uint8_t b_value = (reverse ? HIGH : LOW);

    if (motor == MOTOR_RIGHT)
    {
        // right motor
        digitalWrite(_config->motor_right_a, a_value);
        digitalWrite(_config->motor_right_b, b_value);
    }
    else
    {
        // left motor
        digitalWrite(_config->motor_left_a, a_value);
        digitalWrite(_config->motor_left_b, b_value);
    }
}


void EngineHBridge::motorOff(MOTOR motor, bool brake)
{
    uint8_t value = (brake ? HIGH : LOW);

    if (motor == MOTOR_RIGHT)
    {
        // motor dereita
        digitalWrite(_config->motor_right_a, value);
        digitalWrite(_config->motor_right_b, value);
    }
    else
    {
        // motor esquerda
        digitalWrite(_config->motor_left_a, value);
        digitalWrite(_config->motor_left_b, value);
    }
}


void EngineHBridge::turn(int32_t millis)
{
    // motors on
    if (millis > 0)
    {
        // clockwise
        motorOn(MOTOR_LEFT, false);
        motorOn(MOTOR_RIGHT, true);
    }
    else
    {
        // counterclockwise
        motorOn(MOTOR_RIGHT, false);
        motorOn(MOTOR_LEFT, true);
    }

    delay(abs(millis));

    // motors off
    motorOff(MOTOR_LEFT, true);
    motorOff(MOTOR_RIGHT, true);
}


void EngineHBridge::turn90Degrees(int8_t times)
{
    if (times != 0)
    {
        turn(_config->turn_millis * times);
    }
}


void EngineHBridge::moveStraight(int8_t units)
{
    if (units != 0)
    {
        bool reverse = (units < 0);

        // turn on both motors
        motorOn(MOTOR_RIGHT, reverse);
        motorOn(MOTOR_LEFT, reverse);

        delay(abs(units) * _config->step_millis);

        // turn off both motors
        motorOff(MOTOR_RIGHT, true);
        motorOff(MOTOR_LEFT, true);
    }
}


// EOF
