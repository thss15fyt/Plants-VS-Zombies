#include "pvz.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PVZ w;
    w.show();
    return a.exec();
}
