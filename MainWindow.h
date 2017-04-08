#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPixmap>

#include "ChartColors.h"
#include "ChartGenerator.h"
#include "ChartManager.h"
#include "RandomChartGeneratorConfigUI.h"

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

    void updateDensityDisplay();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    unique_ptr<QTimer> timer;

    shared_ptr<ChartGenerator> gen1p, gen2p;
    shared_ptr<RandomChartGenerator> rand_gen1p, rand_gen2p;

    unique_ptr<ChartManager> charts;
    int getNoteY(double timing, double past);
    int getNoteXid(int lane);

    /*
     * constants for paint
     */
    const int CENTER_WIDTH = 92;
    const int PLAYAREA_WIDTH = 288;
    const int WIDTH = 668, HEIGHT = 482;
    const int WIDTHS[15] = {60, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34, 2, 26, 2, 34};
    const int COLS[15] = {0, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0};
    const int NOTE_HEIGHT = 10;
    static int XS[16];

    ChartColors colors;
    unique_ptr<QPixmap> chart_bg;
    void drawBackground();

    bool lock_sudden_plus, lock_highspeed;
    void updateGreenNumber();
    void adjustHSToFixGreen(double green_number);
    void adjustSUDpToFixGreen(double green_number);

private slots:
    void proceedTime();
    void bpmChanged(int bpm);
    void sliderHSChanged(int hs);
    void spinBoxHSChanged(double hs);
    void sliderSUDpChanged(int sudp);
    void spinBoxSUDpChanged(int sudp);
    void generatorSettingsChanged();

    void menuColorSettings();
};

#endif // MAINWINDOW_H
