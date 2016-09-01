#include "previewzombie.h"
#include "ZombieSize.h"
#include <QLabel>
#include <QMovie>
#include <QPoint>
#include <QPropertyAnimation>

PreviewZombie::PreviewZombie(QPoint beginPoint, zombieName name, QWidget *parent) :
    QWidget(parent)
{
    mBeginPoint = beginPoint;
    mShowLabel = new QLabel(this);
    mShowLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    switch(name)
    {
    case zombie:
        mShowMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/1.gif");
        this->setGeometry(beginPoint.x(), beginPoint.y(), ZOMBIE_W, ZOMBIE_H);
        mShowLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        break;
    case coneHeadZombie:
        mShowMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/1.gif");
        this->setGeometry(beginPoint.x(), beginPoint.y(), ZOMBIE_W, ZOMBIE_H);
        mShowLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        break;
    case bucketHeadZombie:
        mShowMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/1.gif");
        this->setGeometry(beginPoint.x(), beginPoint.y(), ZOMBIE_W, ZOMBIE_H);
        mShowLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        break;
    }

    mShowLabel->setMovie(mShowMovie);
    mShowMovie->start();

    mPreviewMove();
}

void PreviewZombie::mPreviewMove()
{
    QPropertyAnimation* previewMove;
    previewMove = new QPropertyAnimation(this, "pos");
    previewMove->setDuration(4500);
    previewMove->setKeyValueAt(0.25, mBeginPoint);
    previewMove->setKeyValueAt(0.5, QPoint(mBeginPoint.x() - 500, mBeginPoint.y()));
    previewMove->setKeyValueAt(0.75, QPoint(mBeginPoint.x() - 500, mBeginPoint.y()));
    previewMove->setEndValue(QPoint(mBeginPoint.x() - 190, mBeginPoint.y()));
    previewMove->start();
}

PreviewZombie::~PreviewZombie()
{
    delete mShowMovie;
}
