#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include "Chart.h"
#include "ChartGenerator.h"

#include <deque>
#include <memory>

using std::deque;
using std::shared_ptr;

class ChartManager
{
public:
    struct VNote {
        double timing;
        int lane;
        VNote(double timing, int lane)
            : timing(timing), lane(lane) {}
    };

private:
    Chart charts[2];    // 1P side, 2P side
    shared_ptr<ChartGenerator> gen[2];
    double cur_t, highspeed, bpm;

    int last_visible_bar, last_visible_pos;

    deque<VNote> visible_notes;

    void recalcVisibleNotes();

public:
    ChartManager(const shared_ptr<ChartGenerator>& gen1p,
                 const shared_ptr<ChartGenerator>& gen2p,
                 double highspeed, double bpm);

    const deque<VNote>& getVisibleNotes() const;
    double currentTime() const;
    double highSpeed() const;

    void setHighSpeed(double new_highspeed);
    void setBPM(double new_bpm);
    void setGenerator(int side, const shared_ptr<ChartGenerator>& generator);
    void resetCharts();
    void nextFrame();
};

#endif // CHARTMANAGER_H
