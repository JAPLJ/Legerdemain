#include "ChartColorsDialog.h"
#include "ui_ChartColorsDialog.h"

#include <QScrollBar>
#include <QColorDialog>

#include <memory>

using std::unique_ptr;

int ChartColorsDialog::XS[16];

ChartColorsDialog::ChartColorsDialog(QWidget *parent, ChartColors init_colors) :
    QDialog(parent),
    ui(new Ui::ChartColorsDialog)
{
    ui->setupUi(this);
    ui->widgetPreview->installEventFilter(this);

    connect(ui->pushButtonInitialize, SIGNAL(clicked()), this, SLOT(initializeColors()));

    QTableWidget* tbl = ui->tableWidgetColors;

    // redo geometry
    tbl->hide();
    tbl->show();

    colors = init_colors;
    tbl->setColumnWidth(0, tbl->width() - tbl->verticalHeader()->width() - tbl->verticalScrollBar()->height());
    for (int i = 0; i < ChartColors::NumColors; ++i) {
        tbl->setItem(i, 0, new QTableWidgetItem(""));
        tbl->item(i, 0)->setBackgroundColor(colors.getById(i));
    }
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setFocusPolicy(Qt::NoFocus);

    connect(tbl, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(changeColor(int,int)));
    ui->widgetPreview->repaint();
}

void ChartColorsDialog::initializeColors() {
    colors = ChartColors::defaultColors();
    for (int i = 0; i < ChartColors::NumColors; ++i) {
        ui->tableWidgetColors->item(i, 0)->setBackgroundColor(colors.getById(i));
    }
    ui->widgetPreview->repaint();
}

void ChartColorsDialog::changeColor(int row, int) {
    QColor current_color = colors.getById(row);
    unique_ptr<QColorDialog> color_dialog(new QColorDialog(current_color));

    changing_id = row;
    connect(color_dialog.get(), SIGNAL(currentColorChanged(QColor)), this, SLOT(updatingColor(QColor)));

    if (color_dialog->exec() == QDialog::Accepted) {
        colors.getById(row) = color_dialog->selectedColor();
    } else {
        colors.getById(row) = current_color;
    }
    disconnect(color_dialog.get(), SIGNAL(currentColorChanged(QColor)), this, SLOT(updatingColor(QColor)));

    ui->tableWidgetColors->item(row, 0)->setBackgroundColor(colors.getById(row));
    ui->widgetPreview->repaint();
}

void ChartColorsDialog::updatingColor(const QColor &col) {
    colors.getById(changing_id) = col;
    ui->widgetPreview->repaint();
}

bool ChartColorsDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->widgetPreview && event->type() == QEvent::Paint) {
        QPainter painter(ui->widgetPreview);
        painter.setRenderHint(QPainter::Antialiasing, true);

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
            painter.fillRect(side * (PLAYAREA_WIDTH + CENTER_WIDTH), HEIGHT - 2, PLAYAREA_WIDTH, 2, colors.judgeLine());

            if (side == 0) {
                painter.fillRect(xl, 0, 46, HEIGHT, colors.playsidePartition());
                xl += 46;
            }
        }

        for (int i = 0; i < 3; ++i) {
            painter.fillRect(0, 100 * i + 16, PLAYAREA_WIDTH, 1, colors.barLine());
            painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, 100 * i + 16, PLAYAREA_WIDTH, 1, colors.barLine());
        }

        for (int lane = 0; lane < 16; ++lane) {
            QColor col;
            if (lane == 0 || lane == 15) {
                col = colors.noteScratch();
            } else if ((lane / 8 + lane) % 2 == 1) {
                col = colors.noteWhite();
            } else {
                col = colors.noteBlack();
            }
            int idx = lane < 8 ? lane * 2 : (15 - lane) * 2;
            painter.fillRect(XS[lane], 190 - 20 * (lane % 8) - NOTE_HEIGHT, WIDTHS[idx], NOTE_HEIGHT, col);
        }

        painter.fillRect(0, 0, PLAYAREA_WIDTH, 36, colors.suddenPlus());
        painter.fillRect(PLAYAREA_WIDTH + CENTER_WIDTH, 0, PLAYAREA_WIDTH, 36, colors.suddenPlus());

        return true;
    }
    return false;
}

ChartColors ChartColorsDialog::selectedColors() const {
    return colors;
}

ChartColorsDialog::~ChartColorsDialog()
{
    delete ui;
}
