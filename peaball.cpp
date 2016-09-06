#include "peaball.h"
#include "PeaBallSize.h"
#include "FieldSize.h"

PeaBall::PeaBall(peaBallName name, int row, int column, QWidget *parent) :
    QWidget(parent), mName(name), mRow(row), mColumn(column)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    mLabel = new QLabel(this);
    mSpeed = 15;
    switch(mName)
    {
    case peaBall:
        this->setGeometry(FIELD_X + mColumn * BLOCK_W - PEABALL_H_SPACE, FIELD_Y + (mRow - 1) * BLOCK_H + PEABALL_V_SPACE,
                            PEABALL_WIDTH, PEABALL_HEIGHT);
        mLabel->setGeometry(0, 0, PEABALL_WIDTH, PEABALL_HEIGHT);
        mLabel->setStyleSheet("background-image: url(:/Plants/PeaShooter/src/plants/Peashooter/PB00.png)");
        ATK = 20;
        mx = FIELD_X + mColumn * BLOCK_W - 15;
        my = FIELD_Y + (mRow - 1) * BLOCK_H + PEABALL_V_SPACE;
        break;
    case fireBall:
        this->setGeometry(FIELD_X + mColumn * BLOCK_W - FIREBALL_H_SPACE, FIELD_Y + (mRow - 1) * BLOCK_H + FIREBALL_V_SPACE,
                            FIREBALL_W, FIREBALL_H);
        mLabel->setGeometry(0, 0, FIREBALL_W, FIREBALL_H);
        mMovie = new QMovie(":/Plants/TorchWood/src/plants/TorchWood/PB10.gif");
        mLabel->setMovie(mMovie);
        ATK = 50;
        mx = FIELD_X + mColumn * BLOCK_W - FIREBALL_H_SPACE;
        my = FIELD_Y + (mRow - 1) * BLOCK_H + FIREBALL_V_SPACE;
        mMovie->start();
        break;
    }
}

void PeaBall::mUpdate()
{
    mx += mSpeed;
    this->move(QPoint(mx, my));
}
