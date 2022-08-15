#include "FileMerge.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileMerge w;
    w.show();
    return a.exec();
}
