#include "pvz.h"
#include <QApplication>
#include <randnumber.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PVZ w;
    w.show();
    return a.exec();
}
