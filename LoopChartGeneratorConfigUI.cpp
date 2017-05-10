#include "LoopChartGeneratorConfigUI.h"

#include "LoopChartGenerator.h"
#include "MainWindow.h"

void QComboBoxLoopChartGeneratorType::initialize(
        const MainWindow *main_window, const shared_ptr<LoopChartGenerator> &gen) {
    this->gen = gen;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChart(int)));
    connect(this, SIGNAL(currentIndexChanged(int)), main_window, SLOT(generatorSettingsChanged()));

    chart_entries = LoopChartGenerator::getPracticeChartEntries();
    for (const auto& chart : chart_entries) {
        addItem(chart.name, QVariant(chart.id));
    }
}

void QComboBoxLoopChartGeneratorType::updateChart(int index) {
    const QString& id = itemData(index).toString();
    gen->setChart(id);
    dynamic_cast<LoopChartGeneratorConfigUI*>(parent())->updateChart();
}

void LoopChartGeneratorConfigUI::initialize(
        const MainWindow *main_window,
        const shared_ptr<LoopChartGenerator> &gen,
        QComboBoxLoopChartGeneratorType *comboBox) {
    this->gen = gen;
    comboBoxChart = comboBox;
    comboBoxChart->initialize(main_window, gen);
}

void LoopChartGeneratorConfigUI::updateChart() {
    dynamic_cast<MainWindow*>(parent()->parent())->updateDensityDisplay();
}

void LoopChartGeneratorConfigUI::generatorUpdated() {
    const QString& selected = gen->selected();
    for (int i = 0; i < comboBoxChart->count(); ++i) {
        const QString& id = comboBoxChart->itemData(i).toString();
        if (id == selected) {
            comboBoxChart->setCurrentIndex(i);
            break;
        }
    }
}
