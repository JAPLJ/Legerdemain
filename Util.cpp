#include "Util.h"

int Util::to2p(int note_idx) {
    if (note_idx == 0) {
        return 15;
    } else {
        return note_idx + 7;
    }
}
