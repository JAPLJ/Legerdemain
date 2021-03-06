#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T13:32:27
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Legerdemain
TEMPLATE = app

CONFIG += opengl

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    RandomChartGenerator.cpp \
    Chart.cpp \
    ChartGenerator.cpp \
    ChartManager.cpp \
    MainWindow.cpp \
    Random.cpp \
    Util.cpp \
    RandomChartGeneratorConfigUI.cpp \
    ChartColors.cpp \
    Constants.cpp \
    ChartColorsDialog.cpp \
    ProfileManager.cpp \
    LoopChartGenerator.cpp \
    LoopChartGeneratorConfigUI.cpp

HEADERS  += \
    RandomChartGenerator.h \
    Chart.h \
    ChartGenerator.h \
    ChartManager.h \
    MainWindow.h \
    Random.h \
    Util.h \
    Constants.h \
    RandomChartGeneratorConfigUI.h \
    ChartColors.h \
    ChartColorsDialog.h \
    ProfileManager.h \
    LoopChartGenerator.h \
    LoopChartGeneratorConfigUI.h

FORMS    += mainwindow.ui \
    ChartColorsDialog.ui

RESOURCES += \
    Resources.qrc
