#ifndef CONSTANTS_H
#define CONSTANTS_H

class Constants {
private:
    Constants() { }

public:
    static const int MilliSecPerFrame = 17;
    static const int NumBarsPrepared = 16;
    static const int NumBarsRegeneration = 4;
    static const int GreenNumberCoef = 174000;
};

#endif // CONSTANTS_H
