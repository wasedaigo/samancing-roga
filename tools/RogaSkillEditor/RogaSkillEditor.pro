include(../Shared/jsoncpp/jsoncpp.pri)
include(../Shared/misc/misc.pri)
INCLUDEPATH += ../Shared

HEADERS += RogaSkillEditor.h
SOURCES += RogaSkillEditor.cpp\
main.cpp
TEMPLATE = app
FORMS += RogaSkillEditor.ui
RESOURCES += ../Shared/ui.qrc
