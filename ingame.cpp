#include "ingame.h"
#include "ui_ingame.h"
#include "plant.h"
#include "previewzombie.h"
#include "block.h"
#include "zombie.h"
#include "FieldSize.h"
#include "ZombieSize.h"
#include "PeaBallSize.h"
#include <QPropertyAnimation>
#include <QPoint>
#include <QMouseEvent>
#include <QLabel>
#include <QSignalMapper>
#include <QFile>
#include <QTextStream>
#include <qalgorithms.h>
#include <QDebug>

InGame::InGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InGame)
{
    ui->setupUi(this);
    mInitTimer();
    mPlayBgm();
    mBeginMove();
    mInitZombieTime();
    mInitPlant();
    mInitPreviewZombies();
    mInitSpade();
    mInitBlock();
    mInitPlantCostSun();
    mInitCard();
    mInitCursor();
    mInitRand();

    this->setMouseTracking(true);

    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));
}

/*****Update functions*****/

void InGame::mUpdateSlot()
{
    mTime += 0.064;

    mShowZombieUpdate();
    mPlantUpdate();
    mZombieUpdate();
    mPeaBallUpdate();
}

void InGame::mShowZombieUpdate()
{
    if(mShowZombies < mTotalZombies)
    {
        if(mTime >= mZombieShowTime[mShowZombies].time)
        {
            Zombie* p;
            int n = mRandRow->getRandNumber();
            p = new Zombie(n, mZombieShowTime[mShowZombies].name, this);
            p->show();
            mZombies[n - 1].append(p);
            mShowZombies++;
            for(int i = n; i < 5; i++)
            {
                for(int j = 0; j < mZombies[i].length(); j++)
                {
                    mZombies[i][j]->raise();
                }
            }
        }
    }
}

void InGame::mPlantUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(mPlants[i][j] != NULL)
            {
                mPlants[i][j]->mUpdate();
                mPlantFindZombieUpdate(mPlants[i][j]);
                if(mPlants[i][j]->mName == sunFlower && mPlants[i][j]->mSpecialCDTime <= 0)
                    mProduceSun(mPlants[i][j]);
                //Death judge
                if(mPlants[i][j]->HP <= 0)
                {
                    delete mPlants[i][j];
                    mPlants[i][j] = NULL;
                    mBlock[i][j]->isEmpty = true;
                }
            }
        }
    }
}

void InGame::mZombieUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        //qSort(mZombies[i].begin(), mZombies[i].end());  //make sure to find the first zombie
                                                        //overload operator <
        for(int j = 0; j < mZombies[i].length(); j++)
        {
            mZombies[i][j]->mUpdate();
            //Death judge
            if(mZombies[i][j]->HP <= 0)
            {
                if(mZombies[i][j]->meetPlant)
                {
                    mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = false;
                }
                delete mZombies[i][j];
                mZombies[i].erase(mZombies[i].begin() + j);
            }
        }
    }
    mZombieMeetPlantUpdate();
}

void InGame::mPeaBallUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mPeaBall[i].length(); j++)
        {
            mPeaBall[i][j]->mUpdate();
            mPeaBallMeetZombieUpdate(mPeaBall[i][j]);
        }
    }
}

void InGame::mZombieMeetPlantUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            Zombie* zombie = mZombies[i][j];
            if(!zombie->meetPlant)
            {
                zombie->mx--; //may change speed
                zombie->move(QPoint(zombie->mx, zombie->my));

                if(zombie->mx + zombie->mHSpace <= FIELD_X + (zombie->mColumn - 1) * BLOCK_W)  //enter the next block
                {
                   zombie->mColumn--;
                   if(!mBlock[zombie->mRow - 1][zombie->mColumn - 1]->isEmpty)    //zombie meets plant!
                   {
                       zombie->meetPlant = true;
                       zombie->mZombieAttack();
                       mPlants[zombie->mRow - 1][zombie->mColumn - 1]->isAttacked = true;
                       mPlants[zombie->mRow - 1][zombie->mColumn - 1]->ATKofZombie += zombie->ATK;
                   }
                }
            }
            else    //meet plant, is it still there?
            {
                if(mBlock[zombie->mRow - 1][zombie->mColumn - 1]->isEmpty)
                {
                    zombie->meetPlant = false;
                    zombie->mZombieNormal();
                }
            }
        }
    }
}

void InGame::mPlantFindZombieUpdate(Plant *plant)
{
    switch(plant->mName)
    {
    case peaShooter:
        if(mZombies[plant->mRow - 1].length() > 0 && plant->mSpecialCDTime <= 0)
        {
            PeaBall* pb;
            pb = new PeaBall(peaBall, plant->mRow, plant->mColumn, this);
            pb->show();
            mPeaBall[plant->mRow - 1].append(pb);
            plant->mSpecialCDTime = 1.4;
        }
        break;
    }
}

void InGame::mPeaBallMeetZombieUpdate(PeaBall *&peaball)
{
    int row = peaball->mRow;
    int i = mPeaBall[row - 1].count(peaball);
    if(peaball->mx > 900)
    {
        for(int i = 0; i < mPeaBall[row - 1].size(); i++)
        {
            if(mPeaBall[row - 1][i] == peaball)
            {
                delete peaball;
                mPeaBall[row - 1].erase(mPeaBall[row - 1].begin() + i);
            }
        }
    }
    if(mZombies[row - 1].length() != 0)
    {
        int first = mFindFirstZombie(mZombies[row - 1]);
        if((peaball->mx + PEABALL_WIDTH) >= (mZombies[row - 1][first]->mx + mZombies[row - 1][first]->mHSpace))  //peaBall meets Zombie!
        {
            //zombie -HP
            mZombies[row - 1][first]->HP -= peaball->ATK;
            //delete the peaBall
            for(int i = 0; i < mPeaBall[row - 1].size(); i++)
            {
                if(mPeaBall[row - 1][i] == peaball)
                {
                    delete peaball;
                    mPeaBall[row - 1].erase(mPeaBall[row - 1].begin() + i);
                }
            }
        }
    }
}

/*****Funstions for initing*****/

void InGame::mInitZombieTime()
{
    mShowZombies = 0;
    QFile file(":/txt/1.txt");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString str;
        str = stream.readLine();
        mTotalZombies = str.toInt();
        mZombieShowTime = new ZombieWithTime[mTotalZombies];
        int i = 0;
        while(!stream.atEnd())
        {
            str = stream.readLine();
            mZombieShowTime[i].name = (zombieName)str.toInt();
            str = stream.readLine();
            mZombieShowTime[i].time = str.toDouble();
            i++;
        }
        file.close();
    }
}

void InGame::mInitTimer()
{
    mTime = 0;

    mTimer = new QTimer(this);
    mTimer->setInterval(64);
    mTimer->setSingleShot(false);
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(mUpdateSlot()));

    mSunTimer = new QTimer(this);
    mSunTimer->setInterval(5000);
    mSunTimer->setSingleShot(false);
    QObject::connect(mSunTimer, SIGNAL(timeout()), this, SLOT(mDropSunSlot()));
}

void InGame::mInitSpade()
{
    isSpade = false;
    mSpadeCursorPixmap = new QPixmap(":/src/interface/Spade.png");
    mSpadeCursor = new QCursor(*mSpadeCursorPixmap);
}

void InGame::mPlayBgm()
{
    mChoosePlantsBgm = new QSound(":/music/src/music/ChoosePlantsBGM.wav", this);
    mChoosePlantsBgm->setLoops(QSound::Infinite);
    mChoosePlantsBgm->play();
}

void InGame::mBeginMove()
{
    ui->background->move(0, 0);
    QPropertyAnimation* bgMove;
    bgMove = new QPropertyAnimation(ui->background, "pos");
    bgMove->setDuration(4500);
    bgMove->setStartValue(QPoint(0, 0));
    bgMove->setKeyValueAt(0.25, QPoint(0, 0));
    bgMove->setKeyValueAt(0.5, QPoint(-500, 0));
    bgMove->setKeyValueAt(0.75, QPoint(-500, 0));
    bgMove->setEndValue(QPoint(-190, 0));
    bgMove->start();
    //if there's a new move, it sends the signal
    QObject::connect(bgMove, SIGNAL(finished()), mTimer, SLOT(start()));
    QObject::connect(bgMove, SIGNAL(finished()), mSunTimer, SLOT(start()));
}

void InGame::mInitPreviewZombies()
{
    mPreviewZombie = new PreviewZombie*[3];
    mPreviewZombie[0] = new PreviewZombie(QPoint(1100, 130), zombie, this);
    mPreviewZombie[1] = new PreviewZombie(QPoint(1200, 200), coneHeadZombie, this);
    mPreviewZombie[2] = new PreviewZombie(QPoint(1150, 400), bucketHeadZombie, this);
}

void InGame::mInitBlock()
{
    mBlock = new Block**[5];
    mBlockMapper = new QSignalMapper(this);

    for(int i = 0; i < 5; i++)
    {
        mBlock[i] = new Block*[9];
        for(int j = 0; j < 9; j++)
        {
            int row = i + 1;
            int column = j + 1;
            mBlock[i][j] = new Block(row, column, this);
            QObject::connect(mBlock[i][j]->mBlockButton, SIGNAL(clicked()), mBlockMapper, SLOT(map()));
            mBlockMapper->setMapping(mBlock[i][j]->mBlockButton, (row - 1) * 9 + column);
        }
    }
    QObject::connect(mBlockMapper, SIGNAL(mapped(int)), this, SLOT(mBlockClickedSlot(int)));
}

void InGame::mInitPlant()
{
    mPlantNum = 4;
    isPlant = false;
    mPlantName = null;

    mPlants = new Plant**[5];
    for(int i = 0; i < 5; i++)
    {
        mPlants[i] = new Plant*[9];
        for(int j = 0; j < 9; j++)
        {
            mPlants[i][j] = NULL;
        }
    }
}

void InGame::mInitPlantCostSun()
{
    mSunNum = 1000;
    ui->sunNum->setText(QString::number(mSunNum));

    mPlantCostSun = new int[mPlantNum];
    mPlantCostSun[null] = 0;
    mPlantCostSun[sunFlower] = 50;
    mPlantCostSun[peaShooter] = 100;
    mPlantCostSun[wallNut] = 50;
}

void InGame::mInitCard()
{
    mCard = new Card*[mPlantNum - 1];
    mCard[0] = new Card(1, sunFlower, this);
    mCard[1] = new Card(2, peaShooter, this);
    mCard[2] = new Card(3, wallNut, this);
}

void InGame::mInitCursor()
{
    mPlantCursorPixmap = new QPixmap*[mPlantNum];
    mPlantCursor = new QCursor*[mPlantNum];

    mPlantCursorPixmap[null] = NULL;
    mPlantCursor[null] = NULL;

    mPlantCursorPixmap[sunFlower] = new QPixmap(":/Plants/SunFlower/src/plants/SunFlower/0.png");
    mPlantCursor[sunFlower] = new QCursor(*mPlantCursorPixmap[sunFlower]);

    mPlantCursorPixmap[peaShooter] = new QPixmap(":/Plants/PeaShooter/src/plants/Peashooter/0.png");
    mPlantCursor[peaShooter] = new QCursor(*mPlantCursorPixmap[peaShooter]);

    mPlantCursorPixmap[wallNut] = new QPixmap(":/Plants/WallNut/src/plants/WallNut/0.gif");
    mPlantCursor[wallNut] = new QCursor(*mPlantCursorPixmap[wallNut]);
}

void InGame::mousePressEvent(QMouseEvent *e)
{
    if((isPlant || isSpade) && e->button() == Qt::RightButton)
    {
        if(isPlant)
        {
            isPlant = false;
            mPlantName = null;
            this->setCursor(Qt::ArrowCursor);
        }
        else if(isSpade)
        {
            isSpade = false;
            this->setCursor(Qt::ArrowCursor);
        }
    }
}

void InGame::mInitRand()
{
    mRandRow = new RandNumber(1, 5);
    mRandSunX = new RandNumber(60, 700);
    mRandSunY = new RandNumber(100, 480);
}

/******click slots of widgets in ui******/

//plants in&out
void InGame::mBlockClickedSlot(int n)
{
    int column = (n - 1) % 9 + 1;
    int row = ((n - column) / 9) + 1;
    int i = row - 1;
    int j = column - 1;
    if(isPlant && mBlock[i][j]->isEmpty &&
            mSunNum >= mPlantCostSun[mPlantName])
    {
        //planting
        mPlants[i][j] = new Plant(row, column, mPlantName, this);
        mPlants[i][j]->show();
        mSunNum -= mPlantCostSun[mPlantName];
        mBlock[i][j]->isEmpty = false;
        ui->sunNum->setText(QString::number(mSunNum));
        //raise zombies
        for(int k = i + 1; k < 5; k++)
        {
            for(int m = 0; m < mZombies[k].length(); m++)
            {
                mZombies[k][m]->raise();
            }
        }
        //state go back
        isPlant = false;
        mPlantName = null;
        this->setCursor(Qt::ArrowCursor);
    }
    else if(isSpade && mPlants[i][j] != NULL)
    {
        delete mPlants[i][j];
        mPlants[i][j] = NULL;
        mBlock[i][j]->isEmpty = true;
        isSpade = false;
        this->setCursor(Qt::ArrowCursor);
        ui->SpadeBox->setStyleSheet("background-position: left top; background-image: url(://src/interface/SpadeBox.png);border:none;");
    }

}

//select plants cards
void InGame::mCardClickedSlot(plantName name)
{
    if(isPlant && mPlantName == name)
    {
        isPlant = false;
        mPlantName = null;
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    isPlant = true;
    mPlantName = name;
    this->setCursor(*mPlantCursor[name]);

    isSpade = false;
    ui->SpadeBox->setStyleSheet("background-position: left top; background-image: url(://src/interface/SpadeBox.png);border:none;");

}

//select spade to remove plants
void InGame::on_SpadeBox_clicked()
{
    isSpade = !isSpade;
    isPlant = false;
    mPlantName = null;
    if(isSpade)
    {
        ui->SpadeBox->setStyleSheet("background-position: left bottom; background-image: url(://src/interface/SpadeBox.png);border:none;");
        this->setCursor(*mSpadeCursor);
    }
    else
    {
        ui->SpadeBox->setStyleSheet("background-position: left top; background-image: url(://src/interface/SpadeBox.png);border:none;");
        this->setCursor(Qt::ArrowCursor);
    }
}

//right top menu button
void InGame::on_menuButton_clicked()
{
    emit mGameStateChanged(welcome);
}

/***tool func***/
int InGame::mFindFirstZombie(QVector<Zombie*> v)
{
    int n = 0;
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i]->mx < v[n]->mx)
            n = i;
    }
    return n;
}

/******sun******/
void InGame::mDropSunSlot()
{
    Sun* sun;
    sun = new Sun(QPoint(mRandSunX->getRandNumber(), mRandSunY->getRandNumber()), true, this);
    sun->show();
    QObject::connect(sun, SIGNAL(mDeleteThis()), this, SLOT(mDeleteSunSlot()));
    mSun.append(sun);
}

void InGame::mDeleteSunSlot()
{
    for(int i = 0; i < mSun.size(); i++)
    {
        if(mSun[i]->isReach)
        {
            delete mSun[i];
            mSun.erase(mSun.begin() + i);
            mSunNum += 25;
            ui->sunNum->setText(QString::number(mSunNum));
        }
        else if(mSun[i]->isToDie)
        {
            delete mSun[i];
            mSun.erase(mSun.begin() + i);
        }
    }
}

void InGame::mProduceSun(Plant *plant)
{
    Sun* sun;
    sun = new Sun(plant->pos(), false, this);
    sun->show();
    QObject::connect(sun, SIGNAL(mDeleteThis()), this, SLOT(mDeleteSunSlot()));
    mSun.append(sun);
    plant->mSpecialCDTime = 24;
}

/****~func******/
InGame::~InGame()
{
    //delete anything without parent
    delete ui;
    delete mSpadeCursor;
    delete mSpadeCursorPixmap;
    delete mRandRow;
    for(int i = 0; i < mPlantNum; i++)
    {
        delete mPlantCursor[i];
        delete mPlantCursorPixmap[i];
    }
}
