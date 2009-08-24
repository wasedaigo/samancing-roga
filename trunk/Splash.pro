# -------------------------------------------------
# Project created by QtCreator 2009-07-26T15:12:22
# -------------------------------------------------
include(ImagePaletDialog/ImagePaletDialog.pri)
include(DataModels/DataModels.pri)
include(AnimationViewer/AnimationViewer.pri)
include(Lib/jsoncpp/jsoncpp.pri)
include(Widgets/QTimelineWidget/QTimelineWidget.pri)

INCLUDEPATH += Widgets
QT += opengl
TARGET = Splash
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    GLSprite.cpp
HEADERS += mainwindow.h \
    GLSprite.h \
    Common.h \
    Macros.h
FORMS += mainwindow.ui
