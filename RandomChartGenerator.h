#ifndef RANDOMCHARTGENERATOR_H
#define RANDOMCHARTGENERATOR_H

#include "Chart.h"
#include "ChartGenerator.h"

#include <QJsonObject>

class RandomChartGenerator : public ChartGenerator
{
public:
    RandomChartGenerator(int d16, int d8, int d4, int d2, int d1);
    virtual void generate(Chart &chart, int bars);

    void setDensity(int granularity, int density);
    virtual double expectedNotesPerBar() const;

    virtual void fromJson(const QJsonObject &settings);
    virtual QJsonObject toJson() const;

    static QString getName();

private:
    static const int numJsonEntries = 1;
    static const QString settingNames[numJsonEntries];
    static const QString name;

    int density[5];
};

#endif // RANDOMCHARTGENERATOR_H
