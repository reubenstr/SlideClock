class Digit
{

public:
    // Digit();
    Digit(unsigned char pinA, unsigned char pinB, unsigned char pinTop, unsigned char pinBottom);
    void SetTimeBetweenDigits(int value);
    void SetDigitValue(int value);
    void Tick();

private:
    unsigned char motorPinA;
    unsigned char motorPinB;
    unsigned char limitPinTop;
    unsigned char limitPinBottom;

    int targetDigit;
    int currentDigit;
    int timeBetweenDigits = 0;
};