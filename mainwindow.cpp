#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

#include <iostream>

#include <vector>
#include <random>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->installEventFilter(this);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, 7);

    int fl = dist(mt), fr = dist(mt);
    dist = std::uniform_int_distribution<int>(1, 6);

    t = 0;
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 16; ++j) {
            int l = dist(mt), r = dist(mt);
            if (fl == l) {
                ++l;
            }
            if (fr == r) {
                ++r;
            }
            notes.emplace_back(i + 1.0 * j / 16, l);
            notes.emplace_back(i + 1.0 * j / 16, 7 + r);
            fl = l;
            fr = r;
        }
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(proceedTime()));
    timer->start(33);
}

void MainWindow::proceedTime() {
    // bpm * (timer interval)[ms] / 240000.0
    t += 120 * 33 / 240000.0;
    ui->widget->update();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->widget && event->type() == QEvent::Paint) {
        QPainter painter(ui->widget);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const double HS = 2.5;
        const int CENTER_WIDTH = 92;
        const int PLAYAREA_WIDTH = 288;
        const int HEIGHT = 482;
        const int WIDTHS[15] = {60, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34};
        const int COLS[15] = {0, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0};
        static int XS[16];

        auto gety = [=] (const double tt) -> double {
            return HEIGHT - (tt - t) * HS * HEIGHT;
        };

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

        int hr = ceil(t);
        if (t <= hr && hr <= (1 / HS) + t) {
            const double y = gety(hr);
            painter.fillRect(0, y, PLAYAREA_WIDTH, 2, QColor(200, 200, 200));
            painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, y, PLAYAREA_WIDTH, 2, QColor(200, 200, 200));
        }

        auto it = std::lower_bound(notes.begin(), notes.end(), std::make_pair(t, -1));
        while (it != notes.end() && it->first <= (1 / HS) + t) {
            const double y = gety(it->first);
            QColor col;
            if (it->second == 0 || it->second == 15) {
                col = QColor(255, 0, 0);
            } else if ((it->second / 8 + it->second) % 2 == 1) {
                col = QColor(230, 230, 230);
            } else {
                col = QColor(80, 140, 255);
            }
            int idx = it->second < 8 ? it->second * 2 : (15 - it->second) * 2;
            painter.fillRect(XS[it->second], y, WIDTHS[idx], 8, col);
            ++it;
        }

        return true;
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}
