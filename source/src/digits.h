
// Flaws:
// Class should not depend on the Tick() method as it's prone to delays causing
// position control errors.
// Instead a timer should be implemented for precise control and run away failure prevention.

// Limit switches are active HIGH, default state is LOW.

#include <vector>

class Digits
{

public:
    Digits();
    void AddDigit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom, int time);
    bool Begin();
    void SetTimeBetweenDigits(int value);
    void SetDigitValue(int digitIndex, int value);
    int GetDigitValue();
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
        unsigned long timeFromHomeToZero = 1000;
        unsigned long timeForNextDigitMs = 0;

        Digit(unsigned char a, unsigned char b, unsigned char limT, unsigned char limB, int t)
            : pinA(a), pinB(b), pinLimitTop(limT), pinLimitBottom(limB), timeForNextDigitMs(t) {}
    };

    std::vector<Digit> digits;

    void SetMotion(Digit digit, Motion motion);
};