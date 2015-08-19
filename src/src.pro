#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T14:31:53
#
#-------------------------------------------------

CONFIG(debug, debug|release) {
    TARGET = QtFlexd4
} else {
    TARGET = QtFlex4
}

TEMPLATE = lib

DEFINES += QT_FLEX_BUILD

SOURCES += \
    QtDockGuider.cpp \
    QtDockSide.cpp \
    QtDockSite.cpp \
    QtDockWidget.cpp \
    QtFlexHelper.cpp \
    QtFlexManager.cpp \
    QtFlexStyle.cpp \
    QtFlexWidget.cpp

HEADERS += \
    QtDockGuider.h \
    QtDockSide.h \
    QtDockSite.h \
    QtDockWidget.h \
    QtFlexHelper.h \
    QtFlexManager.h \
    QtFlexStyle.h \
    QtFlexWidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    QtFlex.qrc
