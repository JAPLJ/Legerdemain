#include "ChartManager.h"

#include "Constants.h"
#include "Util.h"

#include <cassert>
#include <chrono>

ChartManager::ChartManager(
        const shared_ptr<ChartGenerator> &gen1p,
        const shared_ptr<ChartGenerator> &gen2p,
        double highspeed, double bpm) :
    highspeed(highspeed), bpm(bpm)
{
    gen[0] = gen1p;
    gen[1] = gen2p;

    resetCharts();
}

const deque<ChartManager::VNote>& ChartManager::getVisibleNotes() const {
    return visible_notes;
}

double ChartManager::currentTime() const {
    return cur_t;
}

double ChartManager::highSpeed() const {
    return highspeed;
}

void ChartManager::setHighSpeed(double new_highspeed) {
    highspeed = new_highspeed;
    recalcVisibleNotes();
}

void ChartManager::setBPM(double new_bpm) {
    bpm = new_bpm;
    recalcVisibleNotes();
}

void ChartManager::setGenerator(int side, const shared_ptr<ChartGenerator> &generator) {
    assert(0 <= side && side <= 1);
    gen[side] = generator;
}

void ChartManager::resetCharts() {
    for (int side = 0; side < 2; ++side) {
        charts[side].clear();
        gen[side]->generate(charts[side], Constants::NumBarsPrepared);
    }
    cur_t = 0;
    timer = std::chrono::system_clock::now();
    recalcVisibleNotes();
}

void ChartManager::recalcVisibleNotes() {
    const double upper_t = cur_t + 1.0 / highspeed;

    visible_notes.clear();
    for (int i = static_cast<int>(cur_t); i < Constants::NumBarsPrepared; ++i) {
        if (i > upper_t) {
            break;
        }
        for (int j = 0; j < 16; ++j) {
            const double t = i + j / 16.0;
            if (cur_t <= t && t <= upper_t) {
                last_visible_bar = i;
                last_visible_pos = j;
                for (int lane = 0; lane < 8; ++lane) {
                    if (charts[0].hasNote(i, j, lane)) {
                        visible_notes.emplace_back(t, lane);
                    }
                    if (charts[1].hasNote(i, j, lane)) {
                        visible_notes.emplace_back(t, Util::to2p(lane));
                    }
                }
            }
        }
    }
}

void ChartManager::nextFrame() {
    const auto cur = std::chrono::system_clock::now();
    const int elapsed_msec = std::chrono::duration_cast<std::chrono::milliseconds>(cur - timer).count();
    const double next_t = cur_t + bpm * elapsed_msec / 240000.0;
    timer = cur;

    while (!visible_notes.empty()) {
        const ChartManager::VNote note = visible_notes[0];
        if (note.timing >= next_t) {
            break;
        }
        visible_notes.pop_front();
    }

    const double next_upper_t = next_t + 1.0 / highspeed;
    int bar = last_visible_bar, pos = last_visible_pos;
    while (true) {
        ++pos;
        if (pos >= 16) {
            pos = 0;
            ++bar;
        }
        const double t = bar + pos / 16.0;
        if (t <= next_upper_t) {
            last_visible_bar = bar;
            last_visible_pos = pos;
            for (int lane = 0; lane < 8; ++lane) {
                if (charts[0].hasNote(bar, pos, lane)) {
                    visible_notes.emplace_back(t, lane);
                }
                if (charts[1].hasNote(bar, pos, lane)) {
                    visible_notes.emplace_back(t, Util::to2p(lane));
                }
            }
        } else {
            break;
        }
    }

    cur_t = next_t;
    if (cur_t >= Constants::NumBarsRegeneration) {
        cur_t -= Constants::NumBarsRegeneration;
        last_visible_bar = Constants::NumBarsRegeneration;
        for (ChartManager::VNote& note : visible_notes) {
            note.timing -= Constants::NumBarsRegeneration;
        }
        for (int side = 0; side < 2; ++side) {
            charts[side].dropBars(Constants::NumBarsRegeneration);
            gen[side]->generate(charts[side], Constants::NumBarsRegeneration);
        }
        recalcVisibleNotes();
    }
}
