#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPixmap>

#include "ChartGenerator.h"
#include "RandomChartGenerator.h"
#include "ChartManager.h"
#include "Constants.h"
#include "Util.h"

#include <iostream>
#include <deque>
#include <cmath>

using std::deque;
using VNote = ChartManager::VNote;

int MainWindow::XS[16];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->installEventFilter(this);

    connect(ui->spinBoxBPM, SIGNAL(valueChanged(int)), this, SLOT(bpmChanged(int)));
    connect(ui->sliderHighspeed, SIGNAL(valueChanged(int)), this, SLOT(sliderHSChanged(int)));
    connect(ui->doubleSpinBoxHighspeed, SIGNAL(valueChanged(double)), this, SLOT(spinBoxHSChanged(double)));
    connect(ui->sliderSuddenPlus, SIGNAL(valueChanged(int)), this, SLOT(sliderSUDpChanged(int)));
    connect(ui->spinBoxSuddenPlus, SIGNAL(valueChanged(int)), this, SLOT(spinBoxSUDpChanged(int)));
    lock_sudden_plus = false;
    lock_highspeed = false;

    timer = unique_ptr<QTimer>(new QTimer(this));
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(proceedTime()));
    timer->start(Constants::MilliSecPerFrame);

    rand_gen1p = shared_ptr<RandomChartGenerator>(new RandomChartGenerator(100, 0, 0, 0, 0));
    rand_gen2p = shared_ptr<RandomChartGenerator>(new RandomChartGenerator(100, 0, 0, 0, 0));

    gen1p = shared_ptr<ChartGenerator>(rand_gen1p);
    gen2p = shared_ptr<ChartGenerator>(rand_gen2p);
    charts = unique_ptr<ChartManager>(new ChartManager(gen1p, gen2p, 1.0, 150));
    updateGreenNumber();

    ui->groupBox1Pconfig->initialize(this, rand_gen1p, ui->spinBox1P0, ui->spinBox1P1, ui->spinBox1P2, ui->spinBox1P3, ui->spinBox1P4);
    ui->groupBox2Pconfig->initialize(this, rand_gen2p, ui->spinBox2P0, ui->spinBox2P1, ui->spinBox2P2, ui->spinBox2P3, ui->spinBox2P4);
    updateDensityDisplay();

    chart_bg = unique_ptr<QPixmap>(new QPixmap(WIDTH, HEIGHT));

    // draw background
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
                col = QColor(0, 0, 0);
            } else if (COLS[ii] == 1) {
                col = QColor(44, 44, 44);
            } else {
                col = QColor(144, 144, 144);
            }
            painter.fillRect(xl, 0, WIDTHS[ii], HEIGHT, col);
            xl += WIDTHS[ii];
        }
        painter.fillRect(side * (PLAYAREA_WIDTH + CENTER_WIDTH), HEIGHT - 4, PLAYAREA_WIDTH, 4, QColor(255, 0, 0));

        if (side == 0) {
            painter.fillRect(xl, 0, 92, HEIGHT, QColor(180, 180, 180));
            xl += 92;
        }
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
        painter.setRenderHint(QPainter::Antialiasing, false);

        const double t = charts->currentTime();

        painter.drawPixmap(0, 0, WIDTH, HEIGHT, *chart_bg);

        int hr = ceil(t);
        while (t <= hr && hr <= (1 / charts->highSpeed()) + t) {
            const double y = getNoteY(hr);
            painter.fillRect(0, y, PLAYAREA_WIDTH, 2, QColor(200, 200, 200));
            painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, y, PLAYAREA_WIDTH, 2, QColor(200, 200, 200));
            ++hr;
        }

        const deque<VNote>& notes = charts->getVisibleNotes();
        for (const VNote& note : notes) {
            const double y = getNoteY(note.timing);
            QColor col;
            if (note.lane == 0 || note.lane == 15) {
                col = QColor(255, 0, 0);
            } else if ((note.lane / 8 + note.lane) % 2 == 1) {
                col = QColor(230, 230, 230);
            } else {
                col = QColor(80, 140, 255);
            }
            int idx = getNoteXid(note.lane);
            painter.fillRect(XS[note.lane], y - NOTE_HEIGHT, WIDTHS[idx], NOTE_HEIGHT, col);
        }

        const int sud_height = HEIGHT * (charts->suddenPlus() / 1000.0);
        const QColor sud_col(66, 66, 66);
        painter.fillRect(0, 0, PLAYAREA_WIDTH, sud_height, sud_col);
        painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, 0, PLAYAREA_WIDTH, sud_height, sud_col);

        return true;
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}
