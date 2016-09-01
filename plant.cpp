#include "plant.h"
#include "FieldSize.h"
#include "PlantSize.h"

Plant::Plant(int row, int column, plantName name, QWidget *parent) :
    QWidget(parent), mRow(row), mColumn(column),
    mName(name), isAttacked(false), ATKofZombie(0)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    mPlantLabel = new QLabel(this);
    switch(name)
    {
    case null:
        break;
    case sunFlower:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_SUNFLOWER_SPACE, FIELD_Y + (row - 1) * BLOCK_H,
                          SUNFLOWER_W, SUNFLOWER_H);
        mPlantLabel->setGeometry(0, 0, SUNFLOWER_W, SUNFLOWER_H);
        mMovieNum = 1;
        mMovieIndex = 1;
        mSpecialCDTime = 24;    //produce sun
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/SunFlower/src/plants/SunFlower/SunFlower1.gif");
        break;
    case peaShooter:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_PEASHOOTER_SPACE, FIELD_Y + (row - 1) * BLOCK_H,
                          PEASHOOTER_W, PEASHOOTER_H);
        mPlantLabel->setGeometry(0, 0, PEASHOOTER_W, PEASHOOTER_H);
        mMovieNum = 1;
        mMovieIndex = 1;
        mSpecialCDTime = 1.4;   //fire a peaBall
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/PeaShooter/src/plants/Peashooter/Peashooter.gif");
        break;
    case wallNut:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_WALLNUT_SPACE, FIELD_Y + (row - 1) * BLOCK_H,
                          WALLNUT_W, WALLNUT_H);
        mPlantLabel->setGeometry(0, 0, WALLNUT_W ,WALLNUT_H);
        mMovieNum = 3;
        mMovieIndex = 1;
        mSpecialCDTime = 0;     //nothing
        HP = 4000;
        mPlantCurrentMovie = new QMovie(":/Plants/WallNut/src/plants/WallNut/WallNut.gif");
        break;
    }
    mPlantLabel->setMovie(mPlantCurrentMovie);
    mPlantCurrentMovie->start();
}

Plant::~Plant()
{
    delete mPlantCurrentMovie;
}

void Plant::mUpdate()
{
    switch(mName)
    {
    case peaShooter:
        mPeaShooterUpdate();
        break;
    case wallNut:
        mWallNutUpdate();
        break;
    }
    //-HP
    if(isAttacked)
    {
        HP -= ATKofZombie;
    }
}

void Plant::mNextMovie()
{
    if(mMovieIndex < mMovieNum)
        mMovieIndex++;
    switch(mName)
    {
    case wallNut:
        if(mMovieIndex == 2)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/WallNut/src/plants/WallNut/Wallnut_cracked1.gif");
            break;
        }
        else if(mMovieIndex == 3)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/WallNut/src/plants/WallNut/Wallnut_cracked2.gif");
            break;
        }
    }
    mPlantLabel->setMovie(mPlantCurrentMovie);
    mPlantCurrentMovie->start();
}

void Plant::mPeaShooterUpdate()
{
    if(mSpecialCDTime > 0)
        mSpecialCDTime -= 0.064;
}

void Plant::mWallNutUpdate()
{
    if((mMovieIndex == 1 && HP <= 2666) ||
            (mMovieIndex == 2 && HP <= 1333))
        mNextMovie();
}
