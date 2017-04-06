#include "RandomChartGeneratorConfigUI.h"

#include "MainWindow.h"
#include <iostream>

void QSpinBoxRandomChartGeneratorDensity::initialize(
        const MainWindow* main_window, const shared_ptr<RandomChartGenerator> &gen, int granularity) {
    this->gen = gen;
    this->granularity = granularity;
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(updateDensity(int)));
    connect(this, SIGNAL(valueChanged(int)), main_window, SLOT(generatorSettingsChanged()));
}

void QSpinBoxRandomChartGeneratorDensity::updateDensity(int density) {
    gen->setDensity(granularity, density);
    dynamic_cast<RandomChartGeneratorConfigUI*>(parent())->updateDensity();
}

void RandomChartGeneratorConfigUI::initialize(
        const MainWindow* main_window,
        const shared_ptr<RandomChartGenerator> &gen,
        QSpinBoxRandomChartGeneratorDensity *spinBox16,
        QSpinBoxRandomChartGeneratorDensity *spinBox8,
        QSpinBoxRandomChartGeneratorDensity *spinBox4,
        QSpinBoxRandomChartGeneratorDensity *spinBox2,
        QSpinBoxRandomChartGeneratorDensity *spinBox1) {
    this->gen = gen;
    spinBoxDensity[0] = spinBox16;
    spinBoxDensity[1] = spinBox8;
    spinBoxDensity[2] = spinBox4;
    spinBoxDensity[3] = spinBox2;
    spinBoxDensity[4] = spinBox1;
    for (int i = 0; i < 5; ++i) {
        spinBoxDensity[i]->initialize(main_window, gen, i);
    }
}

void RandomChartGeneratorConfigUI::updateDensity() {
    dynamic_cast<MainWindow*>(parent()->parent())->updateDensityDisplay();
}
