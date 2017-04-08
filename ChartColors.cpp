#include "ChartColors.h"

#include "Util.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

const QString ChartColors::ColorNames[ChartColors::NumColors] = {
    "white_scratch_lane_bg", "black_lane_bg", "lane_boundary", "judge_line",
    "playside_partition", "bar_line", "note_white", "note_black", "note_scratch", "sudden_plus"
};

ChartColors::ChartColors()
{

}

ChartColors ChartColors::defaultColors() {
    ChartColors res;
    res.colors[0] = QColor(0, 0, 0);
    res.colors[1] = QColor(44, 44, 44);
    res.colors[2] = QColor(144, 144, 144);
    res.colors[3] = QColor(255, 0, 0);
    res.colors[4] = QColor(180, 180, 180);
    res.colors[5] = QColor(200, 200, 200);
    res.colors[6] = QColor(230, 230, 230);
    res.colors[7] = QColor(80, 140, 255);
    res.colors[8] = QColor(255, 0, 0);
    res.colors[9] = QColor(66, 66, 66);
    return res;
}

ChartColors ChartColors::fromFile(const QString &filepath) {
    ChartColors res = defaultColors();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return res;
    }

    QTextStream in(&file);
    QJsonDocument json_doc = QJsonDocument::fromJson(in.readAll().toUtf8());
    QJsonObject color_setting = json_doc.object();

    for (int i = 0; i < NumColors; ++i) {
        if (color_setting.contains(ColorNames[i])) {
            if (!color_setting.value(ColorNames[i]).isArray()) {
                continue;
            }

            QJsonArray color_array = color_setting.value(ColorNames[i]).toArray();
            int rgb[3];
            if (color_array.size() < 3) {
                continue;
            }

            bool valid_color_object = true;
            for (int j = 0; j < 3; ++j) {
                if (!color_array[j].isDouble()) {
                    valid_color_object = false;
                    break;
                }
                rgb[j] = Util::clip(0, color_array[j].toInt(), 255);
            }

            res.colors[i] = QColor(rgb[0], rgb[1], rgb[2]);
        }
    }

    file.close();
    return res;
}

bool ChartColors::writeToFile(const QString &filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QJsonObject color_setting;
    for (int i = 0; i < NumColors; ++i) {
        QJsonArray color_array;
        color_array.append(colors[i].red());
        color_array.append(colors[i].green());
        color_array.append(colors[i].blue());
        color_setting[ColorNames[i]] = color_array;
    }

    QJsonDocument json_doc(color_setting);
    QByteArray data(json_doc.toJson());

    if (file.write(data) != data.length()) {
        file.close();
        return false;
    }

    file.close();
    return true;
}
