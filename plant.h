#ifndef PLANT_H
#define PLANT_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include "sun.h"

enum plantName{null, sunFlower, peaShooter, wallNut, cherryBomb, torchWood};
enum explosionName{cherryBombExplosion};

class Plant : public QWidget
{
    Q_OBJECT
public:
    explicit Plant(int row, int column, plantName name, QWidget *parent = 0);
    ~Plant();

    int mRow, mColumn;
    plantName mName;
    QLabel* mPlantLabel;
    QMovie* mPlantCurrentMovie;
    int mMovieIndex;
    int mMovieNum;
    double mSpecialCDTime;
    int HP;
    bool isAttacked;
    int ATKofZombie;

    void mUpdate();
    void mSunFlowerUpdate();
    void mPeaShooterUpdate();
    void mWallNutUpdate();

signals:
    void mExplodeSignal(explosionName, int row, int column);
    void mDeleteThis(int row, int column);

public slots:
    void mNextMovie();
    void mPlantExplodeSlot();
    void mDeleteThisSlot();

};

#endif // PLANT_H
