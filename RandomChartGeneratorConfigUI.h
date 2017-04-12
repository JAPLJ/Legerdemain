#ifndef RANDOMCHARTGENERATORCONFIGUI_H
#define RANDOMCHARTGENERATORCONFIGUI_H

#include "RandomChartGenerator.h"
#include "MainWindow.h"

#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>

#include <memory>

using std::shared_ptr;

class RandomChartGeneratorConfigUI;
class MainWindow;

class QSpinBoxRandomChartGeneratorDensity : public QSpinBox {
    Q_OBJECT

public:
    QSpinBoxRandomChartGeneratorDensity(QWidget* parent) : QSpinBox(parent) { }
    void initialize(const MainWindow* main_window, const shared_ptr<RandomChartGenerator>& gen, int granularity);

private:
    shared_ptr<RandomChartGenerator> gen;
    int granularity;

private slots:
    void updateDensity(int density);
};

class RandomChartGeneratorConfigUI : public QGroupBox {
    Q_OBJECT

private:
    shared_ptr<RandomChartGenerator> gen;
    QSpinBoxRandomChartGeneratorDensity* spinBoxDensity[5];

public:
    RandomChartGeneratorConfigUI(QWidget* parent) : QGroupBox(parent) { }

    void initialize(const MainWindow* main_window,
                    const shared_ptr<RandomChartGenerator>& gen,
                    QSpinBoxRandomChartGeneratorDensity* spinBox16,
                    QSpinBoxRandomChartGeneratorDensity* spinBox8,
                    QSpinBoxRandomChartGeneratorDensity* spinBox4,
                    QSpinBoxRandomChartGeneratorDensity* spinBox2,
                    QSpinBoxRandomChartGeneratorDensity* spinBox1);
    void updateDensity();

    void generatorUpdated();
};

#endif // RANDOMCHARTGENERATORCONFIGUI_H
