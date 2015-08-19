#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T14:13:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtGuider
TEMPLATE = app


SOURCES += \
    ./main.cpp \
    ./QtGuider.cpp

HEADERS  += \
    ./QtGuider.h

FORMS    += \
    ./QtGuider.ui

RESOURCES += \
    ./QtGuider.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../src/release/ -lQtFlex4
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../src/debug/ -lQtFlexd4
else:unix: LIBS += -L$$OUT_PWD/../../src/ -lQtFlex4

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src
