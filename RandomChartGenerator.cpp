#include "RandomChartGenerator.h"

#include "Chart.h"
#include "Util.h"
#include "Random.h"

RandomChartGenerator::RandomChartGenerator()
{

}

void RandomChartGenerator::generate(Chart &chart, int bars) {
    int last = chart.lastNotes() & ~1;  // remove turntable
    int num_last = Util::popcount(last);

    for (int i = 0; i < bars; ++i) {
        chart.bars.emplace_back();
        Chart::Bar& bar = chart.bars.back();

        for (int j = 0; j < 16; ++j) {
            int next = Random::randint(1, 7 - num_last);
            while (last & (1 << next)) {
                next = (next % 7) + 1;
            }
            bar.note[next][j] = 1;
            last = 1 << next;
            num_last = 1;
        }
    }
}
