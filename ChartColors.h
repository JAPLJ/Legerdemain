#ifndef CHARTCOLORS_H
#define CHARTCOLORS_H

#include <QColor>
#include <QString>

class ChartColors
{
public:
    static const int NumColors = 10;

private:
    static const QString ColorNames[NumColors];
    QColor colors[NumColors];

public:
#define DEF_GETTER(name, idx) \
    QColor name() const { return colors[idx]; }

    DEF_GETTER(whiteScratchLaneBG, 0)
    DEF_GETTER(blackLaneBG, 1)
    DEF_GETTER(laneBoundary, 2)
    DEF_GETTER(judgeLine, 3)
    DEF_GETTER(playsidePartition, 4)
    DEF_GETTER(barLine, 5)
    DEF_GETTER(noteWhite, 6)
    DEF_GETTER(noteBlack, 7)
    DEF_GETTER(noteScratch, 8)
    DEF_GETTER(suddenPlus, 9)

    QColor& getById(int id) { return colors[id]; }

    ChartColors();

    static ChartColors defaultColors();
    static ChartColors fromFile(const QString& filepath);
    bool writeToFile(const QString& filepath);
};

#endif // CHARTCOLORS_H
