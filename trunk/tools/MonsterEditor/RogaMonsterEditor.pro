include(../Shared/jsoncpp/jsoncpp.pri)
include(../Shared/misc/misc.pri)
INCLUDEPATH += ../Shared
HEADERS += RogaMonsterEditor.h \
    ImageViewer.h
SOURCES += RogaMonsterEditor.cpp \
    main.cpp \
    ImageViewer.cpp
TEMPLATE = app
FORMS += RogaMonsterEditor.ui
RESOURCES += ../Shared/ui.qrc
