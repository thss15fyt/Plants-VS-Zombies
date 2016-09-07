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

InGame::InGame(int level, QWidget *parent) :
    QWidget(parent), mLevel(level),
    ui(new Ui::InGame)
{
    ui->setupUi(this);

    mPlayBgm();
    mInitOtherUi();
    mInitTimer();
    mInitZombieTime();
    mInitPlant();
    mInitPreviewZombies();
    mInitSpade();
    mInitBlock();
    mInitPlantCostSun();
    mInitCard();
    mInitCursor();
    mInitRand();
    mBeginMove();

    this->setMouseTracking(true);

    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));
    QObject::connect(this, SIGNAL(mGameOver()), this, SLOT(mGameOverSlot()));
    QObject::connect(this, SIGNAL(mGameWin()), parent, SLOT(mGameWinSlot()));
}

/*****Update functions*****/

void InGame::mUpdateSlot()
{
    mTime += 0.064;

    mShowZombieUpdate();
    mPlantUpdate();
    mZombieUpdate();
    mPeaBallUpdate();
    mCardUpdate();
    mSunUpdate();
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
            if(mShowZombies == 0)
                QSound::play(":/music/src/music/awooga.wav");
            p->show();
            mZombies[n - 1].append(p);
            qDebug() << "new a zombie" << p->mZombieName << "in lawn " << n;
            mShowZombies++;
            qDebug() << "raise when new zombie!";
            for(int i = n; i < 5; i++)
            {
                for(int j = 0; j < mZombies[i].size(); j++)
                {
                    mZombies[i][j]->raise();
                }
            }
            qDebug() << "raise when new zombie end!";
        }
    }
}

void InGame::mPlantUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            qDebug() << "plantUpdate loop!" << i << "," << j;
            if(mPlants[i][j] != NULL)
            {
                mPlants[i][j]->mUpdate();
                mPlantFindZombieUpdate(mPlants[i][j]);
                qDebug() << "after plant find zombie";
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
            qDebug() << "plantUpdate loop end!";
        }
    }
}

void InGame::mZombieUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            qDebug() << "zombieUpdate loop!" << j;
            mZombies[i][j]->mUpdate();
            //GameOver judge
            if(mZombies[i][j]->pos().x() + mZombies[i][j]->mHSpace < 0)
                emit mGameOver();
            //Death judge
            if(mZombies[i][j]->HP <= 0)
            {
                qDebug() << "****************************************die a zombie" << (int)mZombies[i][j]->mZombieName;
                if(mZombies[i][j]->meetPlant)
                {
                    mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = false;
                }
                delete mZombies[i][j];
                mZombies[i].remove(j);
                mDieZombies++;
                if(mDieZombies == mTotalZombies)
                {
                    ui->win->show();
                    ui->win->raise();
                    mBGM->stop();
                    emit mGameWin();
                    QSound::play(":/music/src/music/winmusic~1.wav");
                    return;
                }
                qDebug() << "***************************************die a zombie end!";
                j--;
                continue;
            }
            qDebug() << "zombieUpdate loop end!";
        }
    }
    mZombieMeetPlantUpdate();
}

void InGame::mPeaBallUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mPeaBall[i].size(); j++)
        {
            qDebug() << "peaballUpdate loop!" << j;
            qDebug() << "now size" << mPeaBall[i].size();
            PeaBall* peaball = mPeaBall[i][j];
            peaball->mUpdate();
            qDebug() << "after update itself";
            if(mPeaBallMeetZombieUpdate(peaball))
            {
                j--;
                continue;
            }
            qDebug() << "after update meet zombie";
            if(peaball->mColumn < 9 && peaball->mName == peaBall)
            {
                qDebug() << "1";
                if(peaball->mx >= FIELD_X + peaball->mColumn * BLOCK_W)
                {
                    qDebug() << "2";
                    peaball->mColumn++;
                    if(!mBlock[peaball->mRow - 1][peaball->mColumn - 1]->isEmpty)
                    {
                        qDebug() << "3";
                        if(mPlants[peaball->mRow - 1][peaball->mColumn - 1]->mName == torchWood)
                        {
                            qDebug() << "new a fireball!";
                            int row = peaball->mRow;
                            int column = peaball->mColumn;
                            //delete
                            for(int i = 0; i < mPeaBall[row - 1].size(); i++)
                            {
                                if(mPeaBall[row - 1][i] == peaball)
                                {
                                    delete peaball;
                                    mPeaBall[row - 1].remove(i);
                                    j--;
                                    break;
                                }
                            }
                            //new a fire ball
                            PeaBall* fireball;
                            fireball = new PeaBall(fireBall, row, column, this);
                            fireball->show();
                            mPeaBall[row - 1].append(fireball);
                            qDebug() << "new a fireball end!";
                        }
                    }
                }
            }
            qDebug() << "peaBallUpdate loop end!";
        }
    }
}

void InGame::mZombieMeetPlantUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            qDebug() << "zombieMeetPlantUpdate loop!" << j;
            Zombie* zombie = mZombies[i][j];
            if(zombie->isExploded)
                continue;
            if(!zombie->meetPlant)
            {
                if(zombie->mx + zombie->mHSpace <= FIELD_X + (zombie->mColumn - 1) * BLOCK_W)  //enter the next block
                {
                   zombie->mColumn--;
                   if(zombie->mColumn == 0)
                       return;
                   if(!mBlock[zombie->mRow - 1][zombie->mColumn - 1]->isEmpty)    //zombie meets plant!
                   {
                       qDebug() << "zombie" << (int)zombie->mZombieName << "meet plant!";
                       if(zombie->mZombieName == poleVaultingZombie && zombie->mStateIndex == 1)
                       {
                           zombie->mNextMovie();
                       }
                       else
                       {
                           zombie->meetPlant = true;
                           zombie->mZombieAttack();
                           mPlants[zombie->mRow - 1][zombie->mColumn - 1]->isAttacked = true;
                           mPlants[zombie->mRow - 1][zombie->mColumn - 1]->ATKofZombie += zombie->ATK;
                       }
                       qDebug() << "zombie meet plant end!";
                   }
                }
            }
            else    //meet plant, is it still there?
            {
                if(mBlock[zombie->mRow - 1][zombie->mColumn - 1]->isEmpty)
                {
                    qDebug() << "cancel zombie meet plant!";
                    zombie->meetPlant = false;
                    zombie->mZombieNormal();
                    qDebug() << "cancel zombie meet plant end!";
                }
            }
            qDebug() << "zombieMeetPlantUpdate loop end!";
        }
    }
}

void InGame::mPlantFindZombieUpdate(Plant *plant)
{
    if(mZombies[plant->mRow - 1].size() == 0)
        return;
    if(mFindFirstZombie(mZombies[plant->mRow - 1], plant->pos().x()) == -1)
        return;
    switch(plant->mName)
    {
    case peaShooter:
        if(plant->mSpecialCDTime <= 0)
        {
            qDebug() << "new a peaball!";
            PeaBall* pb;
            pb = new PeaBall(peaBall, plant->mRow, plant->mColumn, this);
            pb->show();
            mPeaBall[plant->mRow - 1].append(pb);
            plant->mSpecialCDTime = 1.4;
            qDebug() << "new a peaball end!";
        }
        break;
    }
}

bool InGame::mPeaBallMeetZombieUpdate(PeaBall *&peaball)
{
    int row = peaball->mRow;
    if(peaball->mx > 900)
    {
        qDebug() << ">900!";
        for(int i = 0; i < mPeaBall[row - 1].size(); i++)
        {
            qDebug() << "delete peaball loop" << i;
            if(mPeaBall[row - 1][i] == peaball)
            {
                qDebug() << "delete a peaball!";
                delete peaball;
                mPeaBall[row - 1].remove(i);
                qDebug() << "delete a peaball end!";
                return true;
            }
            qDebug() << "delete peaball loop end!(may return directly)";
        }
    }
    if(mZombies[row - 1].size() != 0)
    {
        qDebug() << "size != 0";
        int first = mFindFirstZombie(mZombies[row - 1], peaball->pos().x());
        if(first == -1 || first >= mZombies[row - 1].size() || mZombies[row - 1][first]->isExploded)
        {
            qDebug() << "nonono";
            return false;
        }
        if((peaball->mx + peaball->size().width()) >= (mZombies[row - 1][first]->mx + mZombies[row - 1][first]->mHSpace))  //peaBall meets Zombie!
        {
            qDebug() << "peaball meet zombie" << mZombies[row - 1][first]->mZombieName;
            if(peaball->mName == peaBall)
                QSound::play(":/music/src/music/splat2.wav");
            else if(peaball->mName == fireBall)
                QSound::play(":/music/src/music/firepea.wav");
            //zombie -HP
            mZombies[row - 1][first]->HP -= peaball->ATK;
            //delete the peaBall
            for(int i = 0; i < mPeaBall[row - 1].size(); i++)
            {
                            qDebug() << "delete peaball loop" << i;
                if(mPeaBall[row - 1][i] == peaball)
                {
                    delete peaball;
                    mPeaBall[row - 1].remove(i);
                    qDebug() << "peaball meet zombie end!";
                    return true;
                }
                            qDebug() << "delete peaball loop end!(may return directly)";
            }
            qDebug() << "peaball meet zombie end!";
        }
    }
    return false;
}

void InGame::mCardUpdate()
{
    for(int i = 0; i < mPlantNum - 1; i++)
    {
        //CD
        if(mCard[i]->mLeftTime > 0)
        {
            mCard[i]->mLeftTime -= 0.064;
        }
        if(mCard[i]->mCostSun > mSunNum)        //not enough sun
        {
            mCard[i]->mNotEnougnSun->show();
            mCard[i]->mCardButton->hide();
        }
        else if(mCard[i]->mCostSun <= mSunNum && mCard[i]->mLeftTime <= 0)
        {
            mCard[i]->mNotEnougnSun->hide();
            mCard[i]->mCardButton->show();
        }
    }
}

void InGame::mSunUpdate()
{
    mSunTime += 0.064;
    if(mSunTime >= 5)
    {
        mSunTime = 0;
        mDropSunSlot();
    }
}

/*****Funstions for initing*****/

void InGame::mInitOtherUi()
{
    ui->gameOver->hide();
    mReadyMovie = new QMovie(":/src/interface/readySetPlants.gif");
    ui->readySetPlant->setMovie(mReadyMovie);
    ui->readySetPlant->hide();

    ui->win->hide();
}

void InGame::mInitZombieTime()
{
    mShowZombies = 0;
    mDieZombies = 0;
    QFile file(":/txt/" + QString::number(mLevel) + ".txt");
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
    mSunTime = 0;

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

void InGame::mInitPreviewZombies()
{
    QFile file(":/txt/" + QString::number(mLevel) + "p.txt");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString str;
        int mTotalPreviewZombies = stream.readLine().toInt();
        mPreviewZombie = new PreviewZombie*[mTotalPreviewZombies];
        for(int i = 0; i < mTotalPreviewZombies && !stream.atEnd(); i++)
        {
            zombieName name = (zombieName)stream.readLine().toInt();
            int x = stream.readLine().toInt();
            int y = stream.readLine().toInt();
            mPreviewZombie[i] = new PreviewZombie(QPoint(x, y), name, this);
        }
        file.close();
    }
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
    mPlantNum = 6;
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
    mSunNum = 9999;
    ui->sunNum->setText(QString::number(mSunNum));

    mPlantCostSun = new int[mPlantNum];
    mPlantCostSun[null] = 0;
    mPlantCostSun[sunFlower] = 50;
    mPlantCostSun[peaShooter] = 100;
    mPlantCostSun[wallNut] = 50;
    mPlantCostSun[cherryBomb] = 150;
    mPlantCostSun[torchWood] = 175;
}

void InGame::mInitCard()
{
    mCard = new Card*[mPlantNum - 1];
    for(int i = 0; i < mPlantNum - 1; i++)
    {
        mCard[i] = new Card(i + 1, (plantName)(i + 1), this);
    }
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

    mPlantCursorPixmap[cherryBomb] = new QPixmap(":/Plants/CherryBomb/src/plants/CherryBomb/0.gif");
    mPlantCursor[cherryBomb] = new QCursor(*mPlantCursorPixmap[cherryBomb]);

    mPlantCursorPixmap[torchWood] = new QPixmap(":/Plants/TorchWood/src/plants/TorchWood/0.gif");
    mPlantCursor[torchWood] = new QCursor(*mPlantCursorPixmap[torchWood]);
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
            ui->SpadeBox->setStyleSheet("background-position: left top; background-image: url(://src/interface/SpadeBox.png);border:none;");
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
        qDebug() << "*****************plant" << (plantName)mPlantName;
        QSound::play(":/music/src/music/plant1.wav");
        //set cards' CD
        for(int n = 0; n < mPlantNum - 1; n++)
        {
            if(mCard[n]->mName == mPlantName)
            {
                mCard[n]->mLeftTime = mCard[n]->mCDTime;
                mCard[n]->mNotEnougnSun->show();
                mCard[n]->mInCD->show();
                mCard[n]->cd->start();
            }
        }
        //raise zombies
        for(int k = i + 1; k < 5; k++)
        {
            for(int m = 0; m < mZombies[k].size(); m++)
            {
                mZombies[k][m]->raise();
            }
        }
        //state go back
        isPlant = false;
        mPlantName = null;
        this->setCursor(Qt::ArrowCursor);
        //find nearby zombies
        for(int i = 0; i < 5; i++)
        {
            for(int j = 0; j < mZombies[i].size(); j++)
            {
                if(mZombies[i][j]->mRow == row && mZombies[i][j]->mColumn == column && !mZombies[i][j]->isExploded)
                {
                    if(mZombies[i][j]->mZombieName == poleVaultingZombie && mZombies[i][j]->mStateIndex == 1)
                    {
                        mZombies[i][j]->mNextMovie();
                    }
                    else
                    {
                        mZombies[i][j]->meetPlant = true;
                        mZombies[i][j]->mZombieAttack();
                        mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = true;
                        mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->ATKofZombie += mZombies[i][j]->ATK;
                    }
                }
            }
        }
    qDebug() << "*****************plant end!";
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

//win
void InGame::on_win_clicked()
{
    mTimer->stop();
    if(mLevel < 5)
        emit mGameStateChanged(ingame);
    else if(mLevel == 5)
        emit mGameStateChanged(welcome);
}


/***tool func***/

void InGame::mPlayBgm()
{
    mChoosePlantsBgm = new QSound(":/music/src/music/ChoosePlantsBGM.wav", this);
    mChoosePlantsBgm->play();

    mBGM = new QSound(":/music/src/music/WateryGraves.wav");
    mBGM->setLoops(QSound::Infinite);
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
    QObject::connect(bgMove, SIGNAL(finished()), this, SLOT(mReadySetPlantSlot()));
}

int InGame::mFindFirstZombie(QVector<Zombie*> v, int x)
{
    qDebug() << "find first zomebie, the lawn size:" << v.size();
    if(v.size() == 0)
    {
        qDebug() << "return -1";
        return -1;
    }

    int count = 0;
    for(int i = 0; i < v.size(); i++)
    {
        qDebug() << "count loop" << i;
        if(v[i]->mx + v[i]->mHSpace < x)
            count++;
        qDebug() << "count loop end!";
    }
    qDebug() << count;
    if(count == v.size())   //no zombie in front of the plant
    {
        qDebug() << "return -1 too";
        return -1;
    }

    qDebug() << "really find first";
    int n = 0;
    for(int i = 0; i < v.size(); i++)
    {
        qDebug() << "really find loop1" << i;
        if(v[i]->mx + v[i]->mHSpace >= x)
        {
            n = i;
            qDebug() << "really find the first proper one, but not the first";
            break;
        }
    }
    for(int i = 0; i < v.size(); i++)
    {
        qDebug() << "really find loop2" << i;
        if(v[i]->mx < v[n]->mx && v[i]->mx + v[i]->mHSpace >= x)
            n = i;
        qDebug() << "really find loop2 end!";
    }
    qDebug() << "find first:" << n;
    return n;
}

/******other slots******/
void InGame::mDropSunSlot()
{
    qDebug() << "new a sun!";
    Sun* sun;
    sun = new Sun(QPoint(mRandSunX->getRandNumber(), mRandSunY->getRandNumber()), true, this);
    sun->show();
    QObject::connect(sun, SIGNAL(mDeleteThis()), this, SLOT(mDeleteSunSlot()));
    mSun.append(sun);
    qDebug() << "new a sun end!";
}

void InGame::mDeleteSunSlot()
{
    for(int i = 0; i < mSun.size(); i++)
    {
        qDebug() << "delete sun loop" << i;
        if(mSun[i]->isReach)
        {
            qDebug() << "a sun reach!";
            delete mSun[i];
            mSun.remove(i);
            i--;
            mSunNum += 25;
            ui->sunNum->setText(QString::number(mSunNum));
            qDebug() << "a sun reach end!";
        }
        else if(mSun[i]->isToDie)
        {
            qDebug() << "a sun die!";
            delete mSun[i];
            mSun.remove(i);
            i--;
            qDebug() << "a sun die end!";
        }
        qDebug() << "delete sun loop end!";
    }
}

void InGame::mProduceSun(Plant *plant)
{
    qDebug() << "produce a sun!";
    Sun* sun;
    sun = new Sun(plant->pos(), false, this);
    sun->show();
    QObject::connect(sun, SIGNAL(mDeleteThis()), this, SLOT(mDeleteSunSlot()));
    mSun.append(sun);
    plant->mSpecialCDTime = 24;
    qDebug() << "produce a sun end!";
}

void InGame::mExplodeSlot(explosionName name, int row, int column)
{
    QRect* rect;
    switch(name)
    {
    case cherryBombExplosion:
        rect = new QRect(FIELD_X + (column - 2) * BLOCK_W,
                   FIELD_Y + (row - 2) * BLOCK_H,
                   BLOCK_W * 3, BLOCK_H * 3);
        break;
    }
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            qDebug() << "explode zombies loop" << j;
            if(rect->contains(mZombies[i][j]->pos().x() + mZombies[i][j]->mHSpace,
                             mZombies[i][j]->pos().y() + (mZombies[i][j]->size().height() * (3.0/5.0))) &&
                    !mZombies[i][j]->isExploded)
            {
                qDebug() << "explode a zombie!";
                if(mZombies[i][j]->meetPlant)
                {
                    mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = false;
                    mZombies[i][j]->meetPlant = false;
                }
                mZombies[i][j]->mBeExploded();
                qDebug() << "delete a zombie end!";
            }
            qDebug() << "explode zombies loop end!";
        }
    }
}

void InGame::mDeletePlantSlot(int row, int column)
{
    delete mPlants[row - 1][column - 1];
    mPlants[row - 1][column - 1] = NULL;
    mBlock[row - 1][column - 1]->isEmpty = true;
}

void InGame::mReadySetPlantSlot()
{
    ui->readySetPlant->show();
    QSound::play(":/music/src/music/readysetplant.wav");
    QObject::connect(mReadyMovie, SIGNAL(finished()), mChoosePlantsBgm, SLOT(stop()));
    QObject::connect(mReadyMovie, SIGNAL(finished()), mBGM, SLOT(play()));
    QObject::connect(mReadyMovie, SIGNAL(finished()), ui->readySetPlant, SLOT(hide()));
    QObject::connect(mReadyMovie, SIGNAL(finished()), ui->win, SLOT(raise()));
    for(int i = 0; i < mPlantNum - 1; i++)
    {
        QObject::connect(mReadyMovie, SIGNAL(finished()), mCard[i]->mNotEnougnSun, SLOT(hide()));
        QObject::connect(mReadyMovie, SIGNAL(finished()), mCard[i]->mCardButton, SLOT(show()));
    }
    QObject::connect(mReadyMovie, SIGNAL(finished()), mTimer, SLOT(start()));

    mReadyMovie->start();
}

void InGame::mGameOverSlot()
{
    mTimer->stop();
    mBGM->stop();
    mGameOverMovie = new QMovie(":/src/interface/gameover.gif");
    ui->gameOver->raise();
    ui->gameOver->setMovie(mGameOverMovie);
    ui->gameOver->show();

    QSound::play(":/music/src/music/scream.wav");
    mGameOverMovie->start();
    QObject::connect(mGameOverMovie, SIGNAL(finished()), this, SLOT(mRestartSlot()));
}

void InGame::mRestartSlot()
{
    emit mGameStateChanged(ingame);
}

/****~func******/
InGame::~InGame()
{
    //delete anything without parent
    delete ui;
    delete mBGM;
    delete mChoosePlantsBgm;
    delete mSpadeCursor;
    delete mSpadeCursorPixmap;
    delete mRandRow;
    for(int i = 0; i < mPlantNum; i++)
    {
        delete mPlantCursor[i];
        delete mPlantCursorPixmap[i];
    }
}

