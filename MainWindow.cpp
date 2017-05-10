#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPixmap>

#include "ChartColors.h"
#include "ChartColorsDialog.h"
#include "ChartGenerator.h"
#include "RandomChartGenerator.h"
#include "LoopChartGenerator.h"
#include "ChartManager.h"
#include "Constants.h"
#include "ProfileManager.h"
#include "Util.h"

#include <deque>
#include <cmath>
#include <memory>

using std::deque;
using std::unique_ptr;
using std::shared_ptr;
using VNote = ChartManager::VNote;

int MainWindow::XS[16];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->installEventFilter(this);

    // menu signals/slots settings
    connect(ui->colorSettings, SIGNAL(triggered()), this, SLOT(menuColorSettings()));

    // ui signals/slots settings
    connect(ui->spinBoxBPM, SIGNAL(valueChanged(int)), this, SLOT(bpmChanged(int)));
    connect(ui->sliderHighspeed, SIGNAL(valueChanged(int)), this, SLOT(sliderHSChanged(int)));
    connect(ui->doubleSpinBoxHighspeed, SIGNAL(valueChanged(double)), this, SLOT(spinBoxHSChanged(double)));
    connect(ui->sliderSuddenPlus, SIGNAL(valueChanged(int)), this, SLOT(sliderSUDpChanged(int)));
    connect(ui->spinBoxSuddenPlus, SIGNAL(valueChanged(int)), this, SLOT(spinBoxSUDpChanged(int)));
    lock_sudden_plus = false;
    lock_highspeed = false;

    connect(ui->comboBoxGen1P, SIGNAL(currentIndexChanged(int)), this, SLOT(generator1PChanged(int)));
    connect(ui->comboBoxGen2P, SIGNAL(currentIndexChanged(int)), this, SLOT(generator2PChanged(int)));

    // timers
    timer = unique_ptr<QTimer>(new QTimer(this));
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(proceedTime()));
    timer->start(Constants::MilliSecPerFrame);

    // generators
    rand_gen1p = shared_ptr<RandomChartGenerator>(new RandomChartGenerator(100, 0, 0, 0, 0));
    rand_gen2p = shared_ptr<RandomChartGenerator>(new RandomChartGenerator(100, 0, 0, 0, 0));

    LoopChartGenerator::loadPracticeCharts();
    loop_gen1p = shared_ptr<LoopChartGenerator>(new LoopChartGenerator(""));
    loop_gen2p = shared_ptr<LoopChartGenerator>(new LoopChartGenerator(""));

    gen1p = shared_ptr<ChartGenerator>(rand_gen1p);
    gen2p = shared_ptr<ChartGenerator>(rand_gen2p);
    charts = unique_ptr<ChartManager>(new ChartManager(gen1p, gen2p, 1.0, 150));
    updateGreenNumber();

    ui->groupBox1Pconfig->initialize(this, rand_gen1p, ui->spinBox1P0, ui->spinBox1P1, ui->spinBox1P2, ui->spinBox1P3, ui->spinBox1P4);
    ui->groupBox2Pconfig->initialize(this, rand_gen2p, ui->spinBox2P0, ui->spinBox2P1, ui->spinBox2P2, ui->spinBox2P3, ui->spinBox2P4);
    ui->groupBoxLoopGen1Pconfig->initialize(this, loop_gen1p, ui->comboBoxLoopGen1P);
    ui->groupBoxLoopGen2Pconfig->initialize(this, loop_gen2p, ui->comboBoxLoopGen2P);
    updateDensityDisplay();

    // colors
    colors = ChartColors::fromFile(Constants::FileColorSettings);
    chart_bg = unique_ptr<QPixmap>(new QPixmap(WIDTH, HEIGHT));
    drawBackground();

    // profiles
    profiles = unique_ptr<ProfileManager>(new ProfileManager());
    profiles->fromFile(Constants::FileProfiles);
    profile_entries = profiles->getEntries();
    selectProfile(0);
}

void MainWindow::selectProfile(int idx) {
    ProfileManager::ProfileContents prof = profiles->readProfile(profile_entries[idx].id);

    charts->fromJson(prof.chart_manager_settings);
    charts->configureGeneratorsFromJson(prof.generator_settings);
    generatorSettingsChanged();

    bool fix_green = ui->checkBoxFixGreenNumber->isChecked();
    ui->checkBoxFixGreenNumber->setChecked(false);
    ui->doubleSpinBoxHighspeed->setValue(charts->highSpeed());
    ui->spinBoxSuddenPlus->setValue(charts->suddenPlus());
    ui->checkBoxFixGreenNumber->setChecked(fix_green);

    generatorChanged();
}

void MainWindow::generatorChanged() {
    const QString gen1_name = charts->generatorName(0);
    const QString gen2_name = charts->generatorName(1);

    ui->groupBox1Pconfig->setHidden(true);
    ui->groupBox2Pconfig->setHidden(true);
    ui->groupBoxLoopGen1Pconfig->setHidden(true);
    ui->groupBoxLoopGen2Pconfig->setHidden(true);

    if (gen1_name == RandomChartGenerator::GeneratorName) {
        ui->groupBox1Pconfig->generatorUpdated();
        ui->groupBox1Pconfig->setHidden(false);
    } else if (gen1_name == LoopChartGenerator::GeneratorName) {
        ui->groupBoxLoopGen1Pconfig->generatorUpdated();
        ui->groupBoxLoopGen1Pconfig->setHidden(false);
    }

    if (gen2_name == RandomChartGenerator::GeneratorName) {
        ui->groupBox2Pconfig->generatorUpdated();
        ui->groupBox2Pconfig->setHidden(false);
    } else if (gen2_name == LoopChartGenerator::GeneratorName) {
        ui->groupBoxLoopGen2Pconfig->generatorUpdated();
        ui->groupBoxLoopGen2Pconfig->setHidden(false);
    }

    generatorSettingsChanged();
}

void MainWindow::generator1PChanged(int index) {
    if (index == 0) {
        gen1p = rand_gen1p;
    } else if (index == 1) {
        gen1p = loop_gen1p;
    }
    charts->setGenerator(0, gen1p);
    generatorChanged();
}

void MainWindow::generator2PChanged(int index) {
    if (index == 0) {
        gen2p = rand_gen2p;
    } else if (index == 1) {
        gen2p = loop_gen2p;
    }
    charts->setGenerator(1, gen2p);
    generatorChanged();
}

void MainWindow::drawBackground() {
    QPainter painter(chart_bg.get());
    int xl = 0;
    for (int side = 0; side < 2; ++side) {
        for (int i = 0; i < 15; ++i) {
            if (i % 2 == 0) {
                XS[side * 8 + i / 2] = xl;
            }
            QColor col;
            const int ii = side == 0 ? i : 14 - i;
            if (COLS[ii] == 0) {
                col = colors.whiteScratchLaneBG();
            } else if (COLS[ii] == 1) {
                col = colors.blackLaneBG();
            } else {
                col = colors.laneBoundary();
            }
            painter.fillRect(xl, 0, WIDTHS[ii], HEIGHT, col);
            xl += WIDTHS[ii];
        }
        painter.fillRect(side * (PLAYAREA_WIDTH + CENTER_WIDTH), HEIGHT - 4, PLAYAREA_WIDTH, 4, colors.judgeLine());

        if (side == 0) {
            painter.fillRect(xl, 0, 92, HEIGHT, colors.playsidePartition());
            xl += 92;
        }
    }
}

void MainWindow::menuColorSettings() {
    unique_ptr<ChartColorsDialog> dialog(new ChartColorsDialog(this, colors));

    if (dialog->exec() == QDialog::Accepted) {
        colors = dialog->selectedColors();
        drawBackground();
    }
}

void MainWindow::updateDensityDisplay() {
    const double nps1 = gen1p->expectedNotesPerBar() * (charts->BPM() / 4 / 60);
    const double nps2 = gen2p->expectedNotesPerBar() * (charts->BPM() / 4 / 60);
    ui->label1PDensityDisplay->setText(QString::number(nps1, 'g', 4));
    ui->label2PDensityDisplay->setText(QString::number(nps2, 'g', 4));
}

void MainWindow::generatorSettingsChanged() {
    charts->resetCharts();
    updateDensityDisplay();
}

int MainWindow::getNoteY(double timing, double past = 0.0) {
    return (1.0 - (timing - charts->currentTime() + past) * charts->highSpeed()) * (HEIGHT + 8);
}

int MainWindow::getNoteXid(int lane) {
    return lane < 8 ? lane * 2 : (15 - lane) * 2;
}

void MainWindow::updateGreenNumber() {
    ui->labelGreenNumber->setText(QString::number(static_cast<int>(charts->greenNumber())));
}

void MainWindow::proceedTime() {
    charts->nextFrame();
    ui->widget->update();
}

void MainWindow::bpmChanged(int bpm) {
    const double cur_green = charts->greenNumber();
    charts->setBPM(bpm);

    if (ui->checkBoxFixGreenNumber->isChecked()) {
        const double new_hs = Util::clip(0.5, Constants::GreenNumberCoef / (bpm * cur_green / (1 - charts->suddenPlus() / 1000.0)), 10.0);
        charts->setHighSpeed(new_hs);

        lock_sudden_plus = true;
        ui->doubleSpinBoxHighspeed->setValue(new_hs);
        ui->sliderHighspeed->setValue(static_cast<int>(new_hs * 100 + 0.5));
        lock_sudden_plus = false;
    }
    updateGreenNumber();
    updateDensityDisplay();
}

void MainWindow::sliderHSChanged(int hs) {
    const double cur_green = charts->greenNumber();
    charts->setHighSpeed(hs / 100.0);
    ui->doubleSpinBoxHighspeed->setValue(hs / 100.0);

    if (ui->checkBoxFixGreenNumber->isChecked()) {
        lock_highspeed = true;
        adjustSUDpToFixGreen(cur_green);
        lock_highspeed = false;
    }
    updateGreenNumber();
}

void MainWindow::spinBoxHSChanged(double hs) {
    const double cur_green = charts->greenNumber();
    charts->setHighSpeed(hs);
    ui->sliderHighspeed->setValue(static_cast<int>(hs * 100.0 + 0.5));

    if (ui->checkBoxFixGreenNumber->isChecked()) {
        lock_highspeed = true;
        adjustSUDpToFixGreen(cur_green);
        lock_highspeed = false;
    }
    updateGreenNumber();
}

void MainWindow::sliderSUDpChanged(int sudp) {
    const double cur_green = charts->greenNumber();
    charts->setSuddenPlus(sudp);
    ui->spinBoxSuddenPlus->setValue(sudp);

    if (ui->checkBoxFixGreenNumber->isChecked()) {
        lock_sudden_plus = true;
        adjustHSToFixGreen(cur_green);
        lock_sudden_plus = false;
    }
    updateGreenNumber();
}

void MainWindow::spinBoxSUDpChanged(int sudp) {
    const double cur_green = charts->greenNumber();
    charts->setSuddenPlus(sudp);
    ui->sliderSuddenPlus->setValue(sudp);

    if (ui->checkBoxFixGreenNumber->isChecked()) {
        lock_sudden_plus = true;
        adjustHSToFixGreen(cur_green);
        lock_sudden_plus = false;
    }
    updateGreenNumber();
}

void MainWindow::adjustHSToFixGreen(double green_number) {
    if (lock_highspeed) {
        return;
    }

    const double sud_factor = 1.0 / (1 - charts->suddenPlus() / 1000.0);
    const double highspeed = Util::clip(0.5, Constants::GreenNumberCoef / (sud_factor * charts->BPM() * green_number), 10.0);
    charts->setHighSpeed(highspeed);
    ui->doubleSpinBoxHighspeed->setValue(highspeed);
    ui->sliderHighspeed->setValue(static_cast<int>(highspeed * 100 + 0.5));
}

void MainWindow::adjustSUDpToFixGreen(double green_number) {
    if (lock_sudden_plus) {
        return;
    }

    const double sud_factor = Constants::GreenNumberCoef / (charts->highSpeed() * charts->BPM() * green_number);
    const int sud = Util::clip(0, static_cast<int>((-1.0 / sud_factor + 1) * 1000 + 0.5), 999);
    charts->setSuddenPlus(sud);
    ui->spinBoxSuddenPlus->setValue(sud);
    ui->sliderSuddenPlus->setValue(sud);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->widget && event->type() == QEvent::Paint) {
        QPainter painter(ui->widget);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const double t = charts->currentTime();

        painter.drawPixmap(0, 0, WIDTH, HEIGHT, *chart_bg);

        int hr = ceil(t);
        while (t <= hr && hr <= (1 / charts->highSpeed()) + t) {
            const double y = getNoteY(hr);
            painter.fillRect(0, y, PLAYAREA_WIDTH, 2, colors.barLine());
            painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, y, PLAYAREA_WIDTH, 2, colors.barLine());
            ++hr;
        }

        const deque<VNote>& notes = charts->getVisibleNotes();
        for (const VNote& note : notes) {
            const double y = getNoteY(note.timing);
            QColor col;
            if (note.lane == 0 || note.lane == 15) {
                col = colors.noteScratch();
            } else if ((note.lane / 8 + note.lane) % 2 == 1) {
                col = colors.noteWhite();
            } else {
                col = colors.noteBlack();
            }
            int idx = getNoteXid(note.lane);
            painter.fillRect(XS[note.lane], y - NOTE_HEIGHT, WIDTHS[idx], NOTE_HEIGHT, col);
        }

        const int sud_height = HEIGHT * (charts->suddenPlus() / 1000.0);
        painter.fillRect(0, 0, PLAYAREA_WIDTH, sud_height, colors.suddenPlus());
        painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, 0, PLAYAREA_WIDTH, sud_height, colors.suddenPlus());

        return true;
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *) {
    colors.writeToFile(Constants::FileColorSettings);

    ProfileManager::ProfileContents prof;
    prof.chart_manager_settings = charts->toJson();
    prof.generator_types.append(rand_gen1p->getName());
    prof.generator_types.append(rand_gen2p->getName());
    prof.generator_settings = charts->generatorsToJson();

    profiles->updateProfile(profile_entries[0].id, prof, "default");
    profiles->writeToFile(Constants::FileProfiles);
}

MainWindow::~MainWindow()
{
    delete ui;
}
