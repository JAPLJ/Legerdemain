#ifndef LOOPCHARTGENERATORCONFIGUI_H
#define LOOPCHARTGENERATORCONFIGUI_H

#include "LoopChartGenerator.h"

#include <QComboBox>
#include <QWidget>
#include <QGroupBox>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

class LoopChartGeneratorConfigUI;
class MainWindow;

class QComboBoxLoopChartGeneratorType : public QComboBox {
    Q_OBJECT

public:
    QComboBoxLoopChartGeneratorType(QWidget* parent) : QComboBox(parent) { }
    void initialize(const MainWindow* main_window, const shared_ptr<LoopChartGenerator>& gen);

private:
    shared_ptr<LoopChartGenerator> gen;
    vector<LoopChartGenerator::ChartEntry> chart_entries;

private slots:
    void updateChart(int index);
};

class LoopChartGeneratorConfigUI : public QGroupBox
{
    Q_OBJECT

private:
    shared_ptr<LoopChartGenerator> gen;
    QComboBoxLoopChartGeneratorType* comboBoxChart;

public:
    LoopChartGeneratorConfigUI(QWidget* parent) : QGroupBox(parent) { }

    void initialize(const MainWindow* main_window,
                    const shared_ptr<LoopChartGenerator>& gen,
                    QComboBoxLoopChartGeneratorType* comboBox);

    void updateChart();
    void generatorUpdated();
};

#endif // LOOPCHARTGENERATORCONFIGUI_H
