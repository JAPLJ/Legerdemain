#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants {
private:
    Constants() { }

public:
    static const int MilliSecPerFrame = 17;
    static const int NumBarsPrepared = 16;
    static const int NumBarsRegeneration = 4;
    static const int GreenNumberCoef = 174000;

    static const QString FileColorSettings;
    static const QString FileProfiles;

    static const QString ResourcePathPracticeCharts;
};

#endif // CONSTANTS_H
