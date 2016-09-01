#include "sun.h"
#include <QPropertyAnimation>

Sun::Sun(QPoint point, bool b, QWidget *parent) :
    QWidget(parent), mStayPoint(point),
    isDrop(b), isReach(false), isToDie(false)
{
    this->setGeometry(point.x(), point.y(), 79, 79);
    mSunLabel = new QLabel(this);
    mSunLabel->setGeometry(0, 0, 79, 79);
    mSunLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    mSunMovie = new QMovie("://src/interface/Sun.gif");
    mSunLabel->setMovie(mSunMovie);
    mSunMovie->start();

    mSunButton = new QPushButton(this);
    mSunButton->setGeometry(0, 0, 79, 79);
    mSunButton->setStyleSheet("border:none");
    mSunButton->setCursor(Qt::PointingHandCursor);

    QObject::connect(mSunButton, SIGNAL(clicked()), this, SLOT(mCollectedSlot()));

    mLifeTimer = new QTimer(this);
    mLifeTimer->setInterval(8000);
    mLifeTimer->setSingleShot(true);

    QObject::connect(mLifeTimer, SIGNAL(timeout()), this, SLOT(mLifeEndSlot()));

    if(isDrop)
    {
        QPropertyAnimation* drop;
        drop = new QPropertyAnimation(this, "pos");
        drop->setDuration((int)(point.y() * 20));
        drop->setStartValue(QPoint(point.x(), -100));
        drop->setEndValue(mStayPoint);
        QObject::connect(drop, SIGNAL(finished()), mLifeTimer, SLOT(start()));
        drop->start();
    }
    else
    {
        QPropertyAnimation* produce;
        produce = new QPropertyAnimation(this, "pos");
        produce->setDuration(450);
        produce->setKeyValueAt(0.3, QPoint(mStayPoint.x() + 8, mStayPoint.y() - 20));
        produce->setEndValue(QPoint(mStayPoint.x() + 20 , mStayPoint.y() + 30));
        QObject::connect(produce, SIGNAL(finished()), mLifeTimer, SLOT(start()));
        produce->start();
    }
}
void Sun::mCollectedSlot()
{
    delete mSunButton;
    QPropertyAnimation *collect;
    collect = new QPropertyAnimation(this, "pos");
    collect->setDuration(500);
    collect->setStartValue(this->pos());
    collect->setEndValue(QPoint(20, 10));
    collect->start();
    QObject::connect(collect, SIGNAL(finished()), this, SLOT(mReachSlot()));
}

void Sun::mReachSlot()
{
    isReach = true;
    emit mDeleteThis();
}

void Sun::mLifeEndSlot()
{
    isToDie = true;
    emit mDeleteThis();
}

Sun::~Sun()
{
    delete mSunMovie;
}
