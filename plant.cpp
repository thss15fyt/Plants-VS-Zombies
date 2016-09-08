#include "plant.h"
#include "FieldSize.h"
#include "PlantSize.h"
#include <QSound>
#include <QDebug>

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
        mSpecialCDTime = 8;    //produce sun, first 8s, the others 24s
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
    case cherryBomb:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_CHERRYBOMB_SPACE, FIELD_Y + (row - 1) * BLOCK_H + BLOCK_V_CHERRYBOMB_SPACE,
                          CHERRYBOMB_W, CHERRYBOMB_H);
        mPlantLabel->setGeometry(0, 0, CHERRYBOMB_W, CHERRYBOMB_H);
        mMovieNum = 2;
        mMovieIndex = 1;
        mSpecialCDTime = 0;
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/CherryBomb/src/plants/CherryBomb/CherryBomb.gif");
        QObject::connect(mPlantCurrentMovie, SIGNAL(finished()), this, SLOT(mPlantExplodeSlot()));
        QObject::connect(mPlantCurrentMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        QObject::connect(this, SIGNAL(mExplodeSignal(explosionName, int, int)), parent, SLOT(mExplodeSlot(explosionName, int, int)));
        break;
    case torchWood:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_TORCHWOOD_SPACE, FIELD_Y + (row - 1) * BLOCK_H + BLOCK_V_TORCHWOOD_SPACE,
                          TORCHWOOD_W, TORCHWOOD_H);
        mPlantLabel->setGeometry(0, 0, TORCHWOOD_W, TORCHWOOD_H);
        mMovieNum = 1;
        mMovieIndex = 1;
        mSpecialCDTime = 0;
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/TorchWood/src/plants/TorchWood/Torchwood.gif");
        break;
    case potatoMine:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_POTATOMINE_SPACE, FIELD_Y + (row - 1) * BLOCK_H + BLOCK_V_POTATOMINE_SPACE,
                          POTATOMINE_W, POTATOMINE_H);
        mPlantLabel->setGeometry(0, 0, POTATOMINE_W, POTATOMINE_H);
        mMovieNum = 3;
        mMovieIndex = 1;
        mSpecialCDTime = 0;
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/PotatoMine/src/plants/PotatoMine/PotatoMineNotReady.gif");
        QObject::connect(mPlantCurrentMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        QObject::connect(this, SIGNAL(mExplodeSignal(explosionName, int, int)), parent, SLOT(mExplodeSlot(explosionName, int, int)));
        break;
    case snowPea:
        this->setGeometry(FIELD_X + (column - 1) * BLOCK_W + BLOCK_W_PEASHOOTER_SPACE, FIELD_Y + (row - 1) * BLOCK_H,
                          PEASHOOTER_W, PEASHOOTER_H);
        mPlantLabel->setGeometry(0, 0, PEASHOOTER_W, PEASHOOTER_H);
        mMovieNum = 1;
        mMovieIndex = 1;
        mSpecialCDTime = 1.4;   //fire a peaBall
        HP = 300;
        mPlantCurrentMovie = new QMovie(":/Plants/SnowPea/src/plants/SnowPea/SnowPea.gif");
        break;
    }
    QObject::connect(this, SIGNAL(mDeleteThis(int,int)), parent, SLOT(mDeletePlantSlot(int , int)));
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
    case sunFlower:
        mSunFlowerUpdate();
        break;
    case peaShooter:
        mPeaShooterUpdate();
        break;
    case wallNut:
        mWallNutUpdate();
        break;
    case snowPea:
        mSnowPeaUpdate();
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
        }
        else if(mMovieIndex == 3)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/WallNut/src/plants/WallNut/Wallnut_cracked2.gif");
        }
        break;
    case cherryBomb:
        if(mMovieIndex == 2)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/CherryBomb/src/plants/CherryBomb/Boom.gif");
            QObject::connect(mPlantCurrentMovie, SIGNAL(finished()), this, SLOT(mDeleteThisSlot()));
            QSound::play(":/music/src/music/cherrybomb.wav");
        }
        break;
    case potatoMine:
        if(mMovieIndex == 2)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/PotatoMine/src/plants/PotatoMine/PotatoMine.gif");
        }
        else if(mMovieIndex == 3)
        {
            delete mPlantCurrentMovie;
            mPlantCurrentMovie = new QMovie(":/Plants/PotatoMine/src/plants/PotatoMine/PotatoMine_mashed.gif");
            QObject::connect(mPlantCurrentMovie, SIGNAL(finished()), this, SLOT(mDeleteThisSlot()));
            QSound::play(":/music/src/music/potato_mine.wav");
        }
    }
    mPlantLabel->setMovie(mPlantCurrentMovie);
    mPlantCurrentMovie->start();
}

void Plant::mSunFlowerUpdate()
{
    mSpecialCDTime -= 0.064;
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

void Plant::mSnowPeaUpdate()
{
    if(mSpecialCDTime > 0)
        mSpecialCDTime -= 0.064;
}

void Plant::mPlantExplodeSlot()
{
    switch(mName)
    {
    case cherryBomb:
        emit mExplodeSignal(cherryBombExplosion, mRow, mColumn);
        break;
    case potatoMine:
        mNextMovie();
        emit mExplodeSignal(potatoMineExplosion, mRow, mColumn);
        break;
    }
}

void Plant::mDeleteThisSlot()
{
    emit mDeleteThis(mRow, mColumn);
}
