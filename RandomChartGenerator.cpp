#include "RandomChartGenerator.h"

#include <QJsonObject>
#include <QJsonArray>

#include "Chart.h"
#include "Util.h"
#include "Random.h"

RandomChartGenerator::RandomChartGenerator(int d16, int d8, int d4, int d2, int d1)
{
    density[0] = d16;
    density[1] = d8;
    density[2] = d4;
    density[3] = d2;
    density[4] = d1;
}

void RandomChartGenerator::setDensity(int granularity, int density) {
    this->density[granularity] = Util::clip(0, density, 700);
}

int RandomChartGenerator::getDensity(int granularity) const {
    return density[granularity];
}

double RandomChartGenerator::expectedNotesPerBar() const {
    double res = 0.0;
    for (int i = 0; i < 5; ++i) {
        res += density[i] / 100.0 * (16 / (1 << i));
    }
    return res;
}

void RandomChartGenerator::generate(Chart &chart, int bars) {
    int last = chart.lastNotes() & ~1;  // remove turntable
    int num_last = Util::popcount(last);

    for (int i = 0; i < bars; ++i) {
        chart.bars.emplace_back();
        Chart::Bar& bar = chart.bars.back();

        for (int j = 0; j < 16; ++j) {
            int notes_cnt = 0;
            for (int k = 0; k < 5; ++k) {
                if (j % (1 << k) == 0) {
                    notes_cnt += density[k] / 100;
                }
            }

            int next_notes[7], next_pos[7], empty_lanes = 0;
            if (notes_cnt + num_last > 7) {
                notes_cnt = 7 - num_last;
            }
            for (int k = 1; k <= 7; ++k) {
                if (!(last & (1 << k))) {
                    next_pos[empty_lanes] = k;
                    next_notes[empty_lanes] = (empty_lanes < notes_cnt) ? 1 : 0;
                    ++empty_lanes;
                }
            }
            Random::shuffle(next_notes, next_notes + empty_lanes);

            int notes_mask = 0;
            for (int k = 0; k < empty_lanes; ++k) {
                if (next_notes[k]) {
                    notes_mask |= 1 << next_pos[k];
                    bar.note[next_pos[k]][j] = 1;
                }
            }

            int available_lanes = 7 - (notes_cnt + num_last);
            for (int k = 0; k < 5; ++k) {
                if (j % (1 << k) == 0 && Random::randint(1, 100) <= density[k] % 100) {
                    if (available_lanes == 0) {
                        break;
                    }
                    int next = Random::randint(1, available_lanes);
                    while ((last | notes_mask) & (1 << next)) {
                        next = (next % 7) + 1;
                    }
                    bar.note[next][j] = 1;
                    notes_mask |= 1 << next;
                }
            }

            last = notes_mask;
            num_last = Util::popcount(last);
        }
    }
}

const QString RandomChartGenerator::settingNames[RandomChartGenerator::numJsonEntries] = {
    "density"
};

void RandomChartGenerator::fromJson(const QJsonObject &settings) {
    const int defaultDensity[5] = {100, 0, 0, 0, 0};

    QJsonArray density_arr = settings[settingNames[0]].toArray();
    for (int i = 0; i < 5; ++i) {
        setDensity(i, density_arr[i].toInt(defaultDensity[i]));
    }
}

QJsonObject RandomChartGenerator::toJson() const {
    QJsonObject settings;
    QJsonArray density_arr;

    for (int i = 0; i < 5; ++i) {
        density_arr.append(density[i]);
    }
    settings[settingNames[0]] = density_arr;

    return settings;
}

const QString RandomChartGenerator::GeneratorName = "random_chart_generator";

QString RandomChartGenerator::getName() const {
    return GeneratorName;
}
