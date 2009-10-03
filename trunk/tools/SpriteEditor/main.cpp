#include <QtGui/QApplication>
#include "SpriteEditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpriteEditor w;
    w.show();
    return a.exec();
}
