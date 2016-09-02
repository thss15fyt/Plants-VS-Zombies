#include "peaball.h"
#include "PeaBallSize.h"
#include "FieldSize.h"

PeaBall::PeaBall(peaBallName name, int row, int column, QWidget *parent) :
    QWidget(parent), mName(name), mRow(row), mColumn(column)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setGeometry(FIELD_X + mColumn * BLOCK_W - 15, FIELD_Y + (mRow - 1) * BLOCK_H + PEABALL_V_SPACE,
                        PEABALL_WIDTH, PEABALL_HEIGHT);
    mLabel = new QLabel(this);
    mLabel->setGeometry(0, 0, PEABALL_WIDTH, PEABALL_HEIGHT);
    mx = FIELD_X + mColumn * BLOCK_W - 15;
    my = FIELD_Y + (mRow - 1) * BLOCK_H + PEABALL_V_SPACE;
    mSpeed = 10;
    switch(mName)
    {
    case peaBall:
        mLabel->setStyleSheet("background-image: url(:/Plants/PeaShooter/src/plants/Peashooter/PB00.png)");
        ATK = 20;
        break;
    }
}

void PeaBall::mUpdate()
{
    mx += mSpeed;
    this->move(QPoint(mx, my));
}
