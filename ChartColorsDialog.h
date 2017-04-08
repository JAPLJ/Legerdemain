#ifndef CHARTCOLORSDIALOG_H
#define CHARTCOLORSDIALOG_H

#include "ChartColors.h"
#include <QDialog>

namespace Ui {
class ChartColorsDialog;
}

class ChartColorsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartColorsDialog(QWidget *parent, ChartColors init_colors);
    ~ChartColorsDialog();

    ChartColors selectedColors() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::ChartColorsDialog *ui;
    ChartColors colors;
    int changing_id;

    /*
     * constants for paint preview
     */
    const int CENTER_WIDTH = 46;
    const int PLAYAREA_WIDTH = 144;
    const int WIDTH = 334, HEIGHT = 241;
    const int WIDTHS[15] = {30, 1, 17, 1, 13, 1, 17, 1, 13, 1, 17, 1, 13, 1, 17};
    const int COLS[15] = {0, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0, 2, 1, 2, 0};
    const int NOTE_HEIGHT = 5;
    static int XS[16];

private slots:
    void changeColor(int row, int column);
    void updatingColor(const QColor& col);
    void initializeColors();
};

#endif // CHARTCOLORSDIALOG_H
