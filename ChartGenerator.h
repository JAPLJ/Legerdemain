#ifndef CHARTGENERATOR_H
#define CHARTGENERATOR_H

#include "Chart.h"

class ChartGenerator
{
public:
    virtual void generate(Chart& chart, int bars) = 0;
    virtual double expectedNotesPerBar() const = 0;
};

#endif // CHARTGENERATOR_H
