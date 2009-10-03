include(../Shared/jsoncpp/jsoncpp.pri)
include(../Shared/misc/misc.pri)
INCLUDEPATH += ../Shared
HEADERS += SpriteEditor.h \
    SpriteData.h \
    SpriteDef.h \
    ImageViewer.h
SOURCES += SpriteEditor.cpp \
    main.cpp \
    SpriteData.cpp \
    ImageViewer.cpp
TEMPLATE = app
FORMS += SpriteEditor.ui
RESOURCES += ../Shared/ui.qrc
