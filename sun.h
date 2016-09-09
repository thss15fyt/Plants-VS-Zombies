/*********************************************
 *
 *  class name: Sun
 *  sun in the game, used to buy plants
 *
 *  2 kinds: dropped and produced
 *
 *******************************************/

#ifndef SUN_H
#define SUN_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QPoint>
#include <QTimer>

class Sun : public QWidget
{
    Q_OBJECT
public:
    explicit Sun(QPoint point, bool b, QWidget *parent = 0);
    ~Sun();

    QLabel* mSunLabel;
    QMovie* mSunMovie;
    QPushButton* mSunButton;
    QPoint mStayPoint;
    QTimer* mLifeTimer;

    bool isDrop;
    bool isReach;
    bool isToDie;

signals:
    void mDeleteThis();

public slots:
    void mCollectedSlot();
    void mReachSlot();
    void mLifeEndSlot();

};

#endif // SUN_H
