#ifndef LOOPCHARTGENERATOR_H
#define LOOPCHARTGENERATOR_H

#include <QJsonObject>

#include "Chart.h"
#include "ChartGenerator.h"

#include <vector>

using std::vector;

class LoopChartGenerator : public ChartGenerator
{
private:
    static QJsonObject practice_charts_json;

    struct PracticeChartData {
        QString id;
        QString name;
        Chart chart;
    };
    static vector<PracticeChartData> practice_charts;

    static void loadPracticeCharts();

    Chart selected_chart;
    QString selected_chart_id;
    int notes_in_selected_chart;
    int current_bar;
    int ran_lanes[7];
    void nextRandomLanes();

    static const int numJsonEntries = 1;
    static const QString settingNames[numJsonEntries];
    static const QString name;

public:
    LoopChartGenerator(const QString& chart_id);
    void setChart(const QString& chart_id);

    struct ChartEntry {
        QString id;
        QString name;
    };
    static vector<ChartEntry> getPracticeChartEntries();

    virtual void generate(Chart& chart, int bars);
    virtual double expectedNotesPerBar() const;
    virtual void fromJson(const QJsonObject& settings);
    virtual QJsonObject toJson() const;
};

#endif // LOOPCHARTGENERATOR_H
