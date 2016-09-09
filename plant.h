/*********************************************************************************
 *
 *  class name: Plant
 *
 *  now 7 kinds of plant(see the enum type: plantName)
 *
 *  main properties:
 *      mName, mRow, mColumn,
 *      HP, ATK, ATKofZombie,
 *      mMovieIndex, mMovieNum
 *
 *  main functions:
 *      mUpdate: change HP, mMovieIndex, mSpecialCD,
 *               special update of each kind
 *      mNextMovie: mMovieIndex++ and then set a new movie
 *
 *  maybe better to be a base class and create several classes inherited from it,
 *  so that it's easy to create more plants with special uses
 *
 ********************************************************************************/
#ifndef PLANT_H
#define PLANT_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include "sun.h"

enum plantName{null, sunFlower, peaShooter, wallNut, cherryBomb, torchWood, potatoMine, snowPea};
enum explosionName{cherryBombExplosion, potatoMineExplosion};

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
    void mSnowPeaUpdate();

signals:
    void mExplodeSignal(explosionName, int row, int column);
    void mDeleteThis(int row, int column);

public slots:
    void mNextMovie();
    void mPlantExplodeSlot();
    void mDeleteThisSlot();

};

#endif // PLANT_H
