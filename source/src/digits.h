// Limit switches are active HIGH, default state is LOW.

#include <vector>

class Digits
{

public:
    Digits();
    void AddDigit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom, int time);
    void SetDurationBetweenDigits(int digitIndex, int value);
    bool SetTargetValue(int digitIndex, int value);
    bool IsInMotion();
    void Home();
    void Tick();

private:
    enum State
    {
        NotHomed,
        Homing,
        Zeroing,
        Resting,
        Moving
    };

    enum Motion
    {
        Up,
        Down,
        Coast,
        Brake
    };

    struct Digit
    {
        State state = State::NotHomed;

        int targetDigit = 0;
        int currentDigit = 0;

        unsigned char pinA;
        unsigned char pinB;
        unsigned char pinLimitTop;
        unsigned char pinLimitBottom;

        unsigned long motionStartMillis;
        int durationFromHomeToZeroMs = 1000; 
        int durationBetweenDigits = 0;
        int durationForNextDigitMs = 0;

        bool inverseMotorDirection = true;

        Digit(unsigned char a, unsigned char b, unsigned char limT, unsigned char limB, int d)
            : pinA(a), pinB(b), pinLimitTop(limT), pinLimitBottom(limB), durationForNextDigitMs(d) {}
    };

    std::vector<Digit> digits;

    void SetMotion(Digit digit, Motion motion);
};