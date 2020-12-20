
#include "Digit.h"
#include <Arduino.h>

// Digit::Digit() {}

Digit::Digit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom)
{
    motorPinA = pinA;
    motorPinB = pinB;
    limitPinTop = pinTop;
    limitPinBottom = pinBottom;

    pinMode(motorPinA, OUTPUT);
    pinMode(motorPinB, OUTPUT);
    pinMode(limitPinTop, INPUT_PULLUP);
    pinMode(limitPinBottom, INPUT_PULLUP);
}

// Time in milliseconds of motor actuation between digits.
void Digit::SetTimeBetweenDigits(int value)
{
    timeBetweenDigits = value;
}

void Digit::SetDigitValue(int value)
{
    targetDigit = value;
}

// Updates digit movement, must be called continuously.
void Digit::Tick()
{
    digitalWrite(motorPinA, digitalRead(limitPinTop));
    digitalWrite(motorPinB, digitalRead(limitPinBottom));
}
