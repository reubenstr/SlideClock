
// Flaws:
// Class should not depend on the Tick() method as it's prone to delays causing
// position control errors.
// Instead a timer should be implemented for precise control and run away failure prevention.


class Digit
{

public:
    // Digit();
    Digit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom);
    void SetTimeBetweenDigits(int value);
    void SetDigitValue(int value);
    int GetDigitValue();
    void Home();
    void Tick();
    

private:
    unsigned char motorPinA;
    unsigned char motorPinB;
    unsigned char limitPinTop;
    unsigned char limitPinBottom;

    int targetDigit;
    int currentDigit = -1;
    int timeBetweenDigits = 0;

    bool homed = false;
};