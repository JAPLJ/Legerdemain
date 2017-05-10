#include "Chart.h"

#include <algorithm>

Chart::Chart()
{

}

Chart::Bar::Bar() {
    for (int i = 0; i < 8; ++i) {
        std::fill(note[i], note[i] + 16, 0);
    }
}

int Chart::lastNotes() const {
    if (bars.empty()) {
        return 0;
    }

    int res = 0;
    for (int i = 0; i < 8; ++i) {
        if (bars.back().note[i][15]) {
            res |= 1 << i;
        }
    }
    return res;
}

void Chart::clear() {
    bars.clear();
}

int Chart::size() const {
    return bars.size();
}

bool Chart::hasNote(int bar, int pos, int lane) const {
    if (bar < 0 || bar >= size()) {
        return false;
    }
    return bars[bar].note[lane][pos] != 0;
}

void Chart::dropBars(int num_bars) {
    for (int i = 0; i < num_bars; ++i) {
        bars.pop_front();
    }
}

int Chart::notesCount() const {
    int res = 0;
    for (int i = 0; i < size(); ++i) {
        for (int j = 0; j < 16; ++j) {
            for (int k = 0; k < 8; ++k) {
                res += (hasNote(i, j, k) ? 1 : 0);
            }
        }
    }
    return res;
}
