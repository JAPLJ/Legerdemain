#ifndef CHART_H
#define CHART_H

#include <deque>

using std::deque;

class Chart
{
    struct Bar {
        int note[16][16];
    };

public:
    Chart();

    deque<Bar> bars;
};

#endif // CHART_H
