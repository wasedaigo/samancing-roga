#include <QtGui/QApplication>
#include "RogaMonsterEditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RogaMonsterEditor w;
    w.show();
    return a.exec();
}
