
// Controls a DC motor driver with IN1/IN2 style pins.


#include "Digits.h"
#include <Arduino.h>



Digits::Digits()
{
    // Default constructor.
}

// Add digit.
void Digits::AddDigit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom, int duration)
{
    digits.push_back(Digit(pinA, pinB, pinTop, pinBottom, duration));

    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinTop, INPUT_PULLUP);
    pinMode(pinBottom, INPUT_PULLUP);
}

// Time in milliseconds of motor actuation between digits.
void Digits::SetDurationBetweenDigits(int digitIndex, int value)
{
    if (digitIndex > digits.size() - 1)
        return;

    digits[digitIndex].durationBetweenDigits = value;
}

// Return false if unable to set target digit.
// Only allowed to set a target digit if not in motion.
bool Digits::SetTargetValue(int digitIndex, int target)
{
    if (digitIndex > digits.size() - 1)
        return false;

    if (target < 0 || target > 9)
        return false;

    if (digits[digitIndex].state != State::Resting)
        return false;

    if (target == digits[digitIndex].currentDigit)
        return false;

    if (target > digits[digitIndex].currentDigit)
    {
        digits[digitIndex].motionStartMillis = millis();
        digits[digitIndex].durationForNextDigitMs = digits[digitIndex].durationForNextDigitMs * (target - digits[digitIndex].currentDigit);
        digits[digitIndex].targetDigit = target;
        digits[digitIndex].currentDigit = target;
        digits[digitIndex].state = State::Moving;
        Serial.printf("Value set for digit %u with value %u for duration %ums.\n", digitIndex, target, digits[digitIndex].durationForNextDigitMs);
    }
    else if (target == 0)
    {
        digits[digitIndex].targetDigit = target;
        digits[digitIndex].currentDigit = target;
        digits[digitIndex].state = State::Homing;
    }

    return true;
}

// Return true of any digit motor is running.
bool Digits::IsInMotion()
{
    bool motionFlag = false;

    for (Digit d : digits)
    {
        if (d.state != State::Resting)
        {
            motionFlag = true;
        }
    }

    return motionFlag;
}

// Home all digits (run motor until limit switch activated).
// Must be called before digits can be moved into position by the user.
void Digits::Home()
{
    for (Digit &d : digits)
    {
        d.targetDigit = 0;
        d.currentDigit = 0;
        d.state = State::Homing;
    }
}


// Updates digit movement, must be called continuously.
void Digits::Tick()
{
    for (Digit &d : digits)
    {
        if (d.state == State::NotHomed)
        {
        }
        else if (d.state == State::Homing)
        {
            SetMotion(d, Motion::Down);

            if (digitalRead(d.pinLimitTop) == HIGH)
            {
                d.state = State::Resting;
                d.motionStartMillis = millis();
            }
        }
        /*
        else if (d.state == State::Zeroing)
        {
            SetMotion(d, Motion::Down);

            if (millis() - d.motionStartMillis >= d.durationFromHomeToZeroMs)
            {
                if (d.targetDigit > d.currentDigit)
                {
                    d.motionStartMillis = millis();
                    d.durationForNextDigitMs = d.durationForNextDigitMs * (d.targetDigit - d.currentDigit);
                    d.currentDigit = d.targetDigit;
                    d.state = State::Moving;
                }
                else
                {
                    d.state = State::Resting;
                }
            }
        }
        */
        else if (d.state == State::Resting)
        {
            SetMotion(d, Motion::Brake);
        }
        else if (d.state == State::Moving)
        {
            SetMotion(d, Motion::Up);

            if (millis() - d.motionStartMillis >= d.durationForNextDigitMs)
            {
                d.state = State::Resting;
            }
        }
    }
}

void Digits::SetMotion(Digit d, Motion motion)
{
    if (motion == Motion::Up)
    {
        if (d.inverseMotorDirection)
        {
            digitalWrite(d.pinA, LOW);
            digitalWrite(d.pinB, HIGH);
        }
        else
        {
            digitalWrite(d.pinA, HIGH);
            digitalWrite(d.pinB, LOW);
        }
    }
    else if (motion == Motion::Down)
    {
        if (d.inverseMotorDirection)
        {
            digitalWrite(d.pinA, HIGH);
            digitalWrite(d.pinB, LOW);
        }
        else
        {
            digitalWrite(d.pinA, LOW);
            digitalWrite(d.pinB, HIGH);
        }
    }
    else if (motion == Motion::Coast)
    {
        digitalWrite(d.pinA, LOW);
        digitalWrite(d.pinB, LOW);
    }
    else if (motion == Motion::Brake)
    {
        digitalWrite(d.pinA, HIGH);
        digitalWrite(d.pinB, HIGH);
    }
}