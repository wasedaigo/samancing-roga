#include <QtGui/QApplication>
#include "RogaSkillEditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RogaSkillEditor w;
    w.show();
    return a.exec();
}
