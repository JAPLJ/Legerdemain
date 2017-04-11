#ifndef CHARTGENERATOR_H
#define CHARTGENERATOR_H

#include "Chart.h"
#include <QJsonObject>

class ChartGenerator
{
public:
    virtual void generate(Chart& chart, int bars) = 0;
    virtual double expectedNotesPerBar() const = 0;
    virtual void fromJson(const QJsonObject& settings) = 0;
    virtual QJsonObject toJson() const = 0;
};

#endif // CHARTGENERATOR_H
