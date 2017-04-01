#ifndef CHART_H
#define CHART_H

#include <deque>

using std::deque;

class Chart
{
public:
    struct Bar {
        int note[8][16];
        Bar();
    };

    Chart();

    deque<Bar> bars;

    int lastNotes() const;
    void clear();
    int size() const;
    bool hasNote(int bar, int pos, int lane) const;
    void dropBars(int num_bars);
};

#endif // CHART_H
