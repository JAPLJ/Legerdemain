#include "LoopChartGenerator.h"

#include "Constants.h"
#include "Random.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <vector>
#include <cassert>

using std::vector;

QJsonObject LoopChartGenerator::practice_charts_json;
vector<LoopChartGenerator::PracticeChartData> LoopChartGenerator::practice_charts;

LoopChartGenerator::LoopChartGenerator(const QString& chart_id)
{
    setChart(chart_id);
}

void LoopChartGenerator::loadPracticeCharts() {
    const QString keyName = "name";
    const QString keyChart = "chart";

    practice_charts.clear();

    QFile file(Constants::ResourcePathPracticeCharts);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    QJsonDocument json_doc = QJsonDocument::fromJson(in.readAll().toUtf8());
    practice_charts_json = json_doc.object();

    for (auto chart_entry = practice_charts_json.begin(); chart_entry != practice_charts_json.end(); ++chart_entry) {
        if (!chart_entry.value().isObject()) {
            continue;
        }

        QJsonObject chart_entry_value = chart_entry.value().toObject();
        if (!chart_entry_value.contains(keyName) || !chart_entry_value.contains(keyChart)) {
            continue;
        }

        QString id = chart_entry.key();
        QString name = chart_entry_value[keyName].toString();
        QJsonArray chart_str = chart_entry_value[keyChart].toArray();

        Chart chart;
        for (const auto& bar_entry : chart_str) {
            chart.bars.push_back(Chart::Bar());
            Chart::Bar& bar = chart.bars.back();

            QString bar_str = bar_entry.toString();
            int pos = 0;
            for (int i = 0; i < 16; ++i) {
                while (pos < bar_str.size()) {
                    if (bar_str[pos] == ',') {
                        break;
                    }
                    int lane = bar_str[pos].digitValue();
                    bar.note[lane][i] = 1;
                    ++pos;
                }
                ++pos;
            }
        }

        PracticeChartData chart_data;
        chart_data.id = std::move(id);
        chart_data.name = std::move(name);
        chart_data.chart = std::move(chart);
        practice_charts.push_back(std::move(chart_data));
    }
}

void LoopChartGenerator::setChart(const QString &chart_id) {
    bool matched = false;
    for (const auto& chart : practice_charts) {
        if (!matched || chart.id == chart_id) {
            selected_chart = chart.chart;
            selected_chart_id = chart_id;
            notes_in_selected_chart = selected_chart.notesCount();
            current_bar = 0;
            nextRandomLanes();
            if (!matched) {
                matched = true;
            }
            if (chart.id == chart_id) {
                return;
            }
        }
    }
}

vector<LoopChartGenerator::ChartEntry> LoopChartGenerator::getPracticeChartEntries() {
    vector<ChartEntry> res;
    for (const auto& chart : practice_charts) {
        res.push_back(ChartEntry());
        res.back().id = chart.id;
        res.back().name = chart.name;
    }
    return res;
}

void LoopChartGenerator::nextRandomLanes() {
    for (int i = 0; i < 7; ++i) {
        ran_lanes[i] = i;
    }
    Random::shuffle(ran_lanes, ran_lanes + 7);
}

void LoopChartGenerator::generate(Chart &chart, int bars) {
    while (bars--) {
        chart.bars.push_back(Chart::Bar());
        Chart::Bar& bar = chart.bars.back();
        for (int i = 0; i < 16; ++i) {
            for (int lane = 0; lane < 8; ++lane) {
                if (selected_chart.hasNote(current_bar, i, lane)) {
                    if (lane == 0) {
                        bar.note[lane][i] = 1;
                    } else {
                        bar.note[ran_lanes[lane - 1]][i] = 1;
                    }
                }
            }
        }

        ++current_bar;
        if (current_bar == selected_chart.size()) {
            current_bar = 0;
            nextRandomLanes();
        }
    }
}

double LoopChartGenerator::expectedNotesPerBar() const {
    return notes_in_selected_chart / static_cast<double>(selected_chart.size());
}

const QString LoopChartGenerator::settingNames[LoopChartGenerator::numJsonEntries] = {
    "selected_chart_id"
};

const QString LoopChartGenerator::name = "loop_chart_generator";

void LoopChartGenerator::fromJson(const QJsonObject &settings) {
    setChart(settings[settingNames[0]].toString());
}

QJsonObject LoopChartGenerator::toJson() const {
    QJsonObject settings;
    settings[settingNames[0]] = selected_chart_id;
    return settings;
}
