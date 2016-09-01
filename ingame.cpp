#include "ingame.h"
#include "ui_ingame.h"
#include "plant.h"
#include "previewzombie.h"
#include "block.h"
#include "zombie.h"
#include "FieldSize.h"
#include "ZombieSize.h"
#include <QPropertyAnimation>
#include <QPoint>
#include <QMouseEvent>
#include <QLabel>
#include <QSignalMapper>
#include <QFile>
#include <QTextStream>
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

    this->setMouseTracking(true);

    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));

    mRandRow = new RandNumber(1, 5);
}

/*****Update functions*****/

void InGame::mUpdateSlot()
{
    mTime += 0.064;

    mShowZombieUpdate();
    mZombieMeetPlantUpdate();
    mPlantUpdate();
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
            for(int k = n + 1; k < 5; k++)
            {
                for(int m = 0; m < mZombies[k].length(); m++)
                {
                    mZombies[k][m]->raise();
                }
            }
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

void InGame::mPlantUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(mPlants[i][j] != NULL)
            {
                mPlants[i][j]->mUpdate();
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




/*****Funstions for initing*****/

void InGame::mInitZombieTime()
{
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
    QObject::connect(bgMove, SIGNAL(finished()), mTimer, SLOT(start()));
    bgMove->setDuration(4500);
    bgMove->setStartValue(QPoint(0, 0));
    bgMove->setKeyValueAt(0.25, QPoint(0, 0));
    bgMove->setKeyValueAt(0.5, QPoint(-500, 0));
    bgMove->setKeyValueAt(0.75, QPoint(-500, 0));
    bgMove->setEndValue(QPoint(-190, 0));
    bgMove->start();
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
