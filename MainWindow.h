#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "ChartGenerator.h"
#include "ChartManager.h"

#include <memory>
#include <vector>

using std::vector;
using std::unique_ptr;
using std::shared_ptr;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MainWindow *ui;
    unique_ptr<QTimer> timer;

    shared_ptr<ChartGenerator> gen;
    unique_ptr<ChartManager> charts;
    int getNoteY(double timing, double past);
    int getNoteXid(int lane);

    /*
     * constants for paint
     */
    const int CENTER_WIDTH = 92;
    const int PLAYAREA_WIDTH = 288;
    const int HEIGHT = 482;
    const int WIDTHS[15] = {60, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34};
    const int COLS[15] = {0, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0};
    static int XS[16];

private slots:
    void proceedTime();
};

#endif // MAINWINDOW_H
