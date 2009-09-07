# -------------------------------------------------
# Project created by QtCreator 2009-07-26T15:12:22
# -------------------------------------------------
include(ImagePaletDialog/ImagePaletDialog.pri)
include(DataModels/DataModels.pri)
include(AnimationViewer/AnimationViewer.pri)
include(Lib/jsoncpp/jsoncpp.pri)
include(Widgets/QTimelineWidget/QTimelineWidget.pri)
include(Widgets/ResourceTree/ResourceTree.pri)
INCLUDEPATH += Widgets
QT += opengl
QT += xml
TARGET = Splash
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    GLSprite.cpp \
    ResourceManager.cpp
HEADERS += mainwindow.h \
    GLSprite.h \
    Macros.h \
    ResourceManager.h
FORMS += mainwindow.ui
RESOURCES += ui.qrc
RC_FILE = Splash.rc
