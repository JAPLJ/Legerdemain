#ifndef RANDOMCHARTGENERATOR_H
#define RANDOMCHARTGENERATOR_H

#include "Chart.h"
#include "ChartGenerator.h"

class RandomChartGenerator : public ChartGenerator
{
public:
    RandomChartGenerator();
    virtual void generate(Chart &chart, int bars);
};

#endif // RANDOMCHARTGENERATOR_H
