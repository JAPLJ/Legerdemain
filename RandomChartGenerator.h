#ifndef RANDOMCHARTGENERATOR_H
#define RANDOMCHARTGENERATOR_H

#include "Chart.h"
#include "ChartGenerator.h"

class RandomChartGenerator : public ChartGenerator
{
public:
    RandomChartGenerator(int d16, int d8, int d4, int d2, int d1);
    virtual void generate(Chart &chart, int bars);

    void setDensity(int granularity, int density);
    virtual double expectedNotesPerBar() const;

private:
    int density[5];
};

#endif // RANDOMCHARTGENERATOR_H
