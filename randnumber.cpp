#include "randnumber.h"
#include <QTime>

RandNumber::RandNumber(int min, int max) : mMax(max), mMin(min)
{
    QTime t = QTime::currentTime();
    qsrand(t.msec() + t.second()*1000);
}

int RandNumber::getRandNumber()
{
    return qrand() % (mMax - mMin + 1) + mMin;
}
