/*********************************************
 *
 *  class name: RandNumber
 *  create a random number between min and max
 *
 *********************************************/
#ifndef RANDNUMBER_H
#define RANDNUMBER_H

#include <QTime>
class RandNumber
{
public:
    RandNumber(int min, int max);
    int getRandNumber();
    int mMax, mMin;
};

#endif // RANDNUMBER_H
