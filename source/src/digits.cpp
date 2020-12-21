
#include "Digits.h"
#include <Arduino.h>

Digits::Digits()
{
}

// Add digit.
void Digits::AddDigit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom, int duration)
{
    digits.push_back(Digit(pinA, pinB, pinTop, pinBottom, duration));
}

// Required call prior to operation setup ports.
bool Digits::Begin()
{
    for (Digit d : digits)
    {
        pinMode(d.pinA, OUTPUT);
        pinMode(d.pinB, OUTPUT);
        pinMode(d.pinLimitTop, INPUT_PULLUP);
        pinMode(d.pinLimitBottom, INPUT_PULLUP);
    }

    return true;
}

// Time in milliseconds of motor actuation between digits.
void Digits::SetTimeBetweenDigits(int value)
{
    // timeBetweenDigits = value;
}

// Return false if unable to set target digit.
// Only allowed to set a target digit if not in motion.
bool Digits::SetTargetDigit(int digitIndex, int target)
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
    }
    else
    {
        digits[digitIndex].state = State::Homing;
    }

    return true;
}


bool Digits::IsInMotion()
{
    bool motionFlag = false;

    for(Digit d : digits)
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

            if (digitalRead(d.pinLimitBottom) == HIGH)
            {
                d.state = State::Zeroing;
                d.motionStartMillis = millis();
            }
        }
        else if (d.state == State::Zeroing)
        {
            SetMotion(d, Motion::Up);

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
        else if (d.state == State::Resting)
        {
            SetMotion(d, Motion::Brake);
        }
        else if (d.state == State::Moving)
        {
            SetMotion(d, Motion::Down);

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
        digitalWrite(d.pinA, HIGH);
        digitalWrite(d.pinB, LOW);
    }
    else if (motion == Motion::Down)
    {
        digitalWrite(d.pinA, LOW);
        digitalWrite(d.pinB, HIGH);
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