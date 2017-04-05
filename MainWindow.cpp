#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPixmap>

#include "ChartGenerator.h"
#include "RandomChartGenerator.h"
#include "ChartManager.h"
#include "Constants.h"

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

    timer = unique_ptr<QTimer>(new QTimer(this));
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(proceedTime()));
    timer->start(Constants::MilliSecPerFrame);

    gen = shared_ptr<ChartGenerator>(new RandomChartGenerator());
    charts = unique_ptr<ChartManager>(new ChartManager(gen, gen, 2.5, 120));

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

int MainWindow::getNoteY(double timing, double past = 0.0) {
    return (1.0 - (timing - charts->currentTime() + past) * charts->highSpeed()) * (HEIGHT + 8);
}

int MainWindow::getNoteXid(int lane) {
    return lane < 8 ? lane * 2 : (15 - lane) * 2;
}

void MainWindow::proceedTime() {
    charts->nextFrame();
    ui->widget->update();
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

        return true;
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}