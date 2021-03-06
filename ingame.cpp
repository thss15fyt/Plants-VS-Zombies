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

    //init
    mInitOtherUi();
    mInitTimer();
    mInitZombieTime();
    mInitPlant();
    mInitPreviewZombies();
    mInitSpade();
    mInitBlock();
    mInitPlantCostSun();
    mInitCard();
    mInitPlantCursor();
    mInitRand();

    mBeginMove();

    this->setMouseTracking(true);

    //main connects
    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));
    QObject::connect(this, SIGNAL(mGameWin()), parent, SLOT(mGameWinSlot()));
    QObject::connect(this, SIGNAL(mGameOver()), this, SLOT(mGameOverSlot()));
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
            if(mShowZombies == 0)   //special sound of the 1st zombie
                QSound::play(":/music/src/music/awooga.wav");
            p->show();
            mZombies[n - 1].append(p);
            mShowZombies++;
            for(int i = n; i < 5; i++)
            {
                for(int j = 0; j < mZombies[i].size(); j++)
                {
                    mZombies[i][j]->raise();    //raise zombies of the following lawns
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
                //self update
                mPlants[i][j]->mUpdate();
                //find zombies
                mPlantFindZombieUpdate(mPlants[i][j]);
                //produce sun
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
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            //self update
            mZombies[i][j]->mUpdate();
            //GameOver judge
            if(mZombies[i][j]->pos().x() + mZombies[i][j]->mHSpace < 0)
                emit mGameOver();
            //Death judge
            if(mZombies[i][j]->HP <= 0)
            {
                if(mZombies[i][j]->meetPlant)
                {
                    mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = false;
                }
                delete mZombies[i][j];
                mZombies[i].remove(j);
                mDieZombies++;
                if(mDieZombies == mTotalZombies)    //game win
                {
                    ui->win->show();
                    ui->win->raise();
                    mBGM->stop();
                    emit mGameWin();
                    QSound::play(":/music/src/music/winmusic~1.wav");
                    return;
                }
                j--;
                continue;
            }
        }
    }
    //meet plants update
    mZombieMeetPlantUpdate();
}

void InGame::mPeaBallUpdate()
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mPeaBall[i].size(); j++)
        {
            PeaBall* peaball = mPeaBall[i][j];
            //self update
            peaball->mUpdate();
            if(mPeaBallMeetZombieUpdate(peaball))
            {
                j--;
                continue;
            }
            //change while passing a fire wood
            if(peaball->mColumn < 9 && (peaball->mName == peaBall || peaball->mName == snowBall))
            {
                if(peaball->mx >= FIELD_X + peaball->mColumn * BLOCK_W)
                {
                    peaball->mColumn++;
                    if(!mBlock[peaball->mRow - 1][peaball->mColumn - 1]->isEmpty)
                    {
                        if(mPlants[peaball->mRow - 1][peaball->mColumn - 1]->mName == torchWood)
                        {
                            int row = peaball->mRow;
                            int column = peaball->mColumn;
                            peaBallName name = peaball->mName;
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
                            //new a ball
                            PeaBall* ball;
                            if(name == peaBall)
                                ball = new PeaBall(fireBall, row, column, this);    //peaball->fireball
                            else if(name == snowBall)
                                ball = new PeaBall(peaBall, row, column, this);     //snowball->peabal
                            ball->show();
                            mPeaBall[row - 1].append(ball);
                        }
                    }
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
            if(zombie->isExploded)
                continue;
            if(!zombie->meetPlant)
            {
                if(zombie->mx + zombie->mHSpace <= FIELD_X + (zombie->mColumn - 1) * BLOCK_W)  //enter the next block
                {
                   zombie->mColumn--;
                   if(zombie->mColumn == 0)
                       return;  //avoid out of index
                   if(!mBlock[zombie->mRow - 1][zombie->mColumn - 1]->isEmpty)    //zombie meets plant!
                   {
                       //special events
                       if(zombie->mZombieName == poleVaultingZombie && zombie->mStateIndex == 1)
                       {
                           //poleVaultingZombie jump over the 1st plant
                           zombie->mNextMovie();
                       }
                       else if(mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mName == potatoMine &&
                               mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mMovieIndex == 2)
                       {
                           //potatoMine explodes
                           mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mPlantExplodeSlot();
                       }
                       //normal events
                       else
                       {
                           zombie->meetPlant = true;
                           zombie->mZombieAttack();
                           mPlants[zombie->mRow - 1][zombie->mColumn - 1]->isAttacked = true;
                           mPlants[zombie->mRow - 1][zombie->mColumn - 1]->ATKofZombie += zombie->ATK;
                       }
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
                else
                {
                    if(mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mName == potatoMine &&
                            mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mMovieIndex == 2)
                    {
                        //potatoMine ready while being attacked
                        mPlants[zombie->mRow - 1][zombie->mColumn - 1]->mPlantExplodeSlot();
                    }
                }
            }
        }
    }
}

void InGame::mPlantFindZombieUpdate(Plant *plant)
{
    if(mZombies[plant->mRow - 1].size() == 0)   //no zombies
        return;
    if(mFindFirstZombie(mZombies[plant->mRow - 1], plant->pos().x()) == -1)     //no zombies in front of plant
        return;
    switch(plant->mName)
    {
    case peaShooter:
        //create peaball
        if(plant->mSpecialCDTime <= 0)
        {
            PeaBall* pb;
            pb = new PeaBall(peaBall, plant->mRow, plant->mColumn, this);
            pb->show();
            mPeaBall[plant->mRow - 1].append(pb);
            plant->mSpecialCDTime = 1.4;
        }
        break;
    case snowPea:
        //create snowball
        if(plant->mSpecialCDTime <= 0)
        {
            PeaBall* pb;
            pb = new PeaBall(snowBall, plant->mRow, plant->mColumn, this);
            pb->show();
            mPeaBall[plant->mRow - 1].append(pb);
            plant->mSpecialCDTime = 1.4;
        }
        break;
    }
}

bool InGame::mPeaBallMeetZombieUpdate(PeaBall *&peaball)    //if peaball is deleted here, return true
{
    int row = peaball->mRow;
    //out of the screen
    if(peaball->mx > 900)
    {
        for(int i = 0; i < mPeaBall[row - 1].size(); i++)
        {
            if(mPeaBall[row - 1][i] == peaball)
            {
                delete peaball;
                mPeaBall[row - 1].remove(i);
                return true;
            }
        }
    }
    //hit zombie
    if(mZombies[row - 1].size() != 0)
    {
        //find the 1st zombie in front of peaball
        int first = mFindFirstZombie(mZombies[row - 1], peaball->pos().x());
        if(first == -1 || first >= mZombies[row - 1].size() || mZombies[row - 1][first]->isExploded)
        {
            return false;
        }
        //peaBall meets Zombie!
        if((peaball->mx + peaball->size().width()) >= (mZombies[row - 1][first]->mx + mZombies[row - 1][first]->mHSpace))
        {
            //play sounds
            if(mZombies[row - 1][first]->mZombieName == bucketHeadZombie && mZombies[row - 1][first]->mStateIndex == 1)
                QSound::play(":/music/src/music/shieldhit.wav");
             else if(peaball->mName == peaBall)
                QSound::play(":/music/src/music/splat2.wav");
            else if(peaball->mName == fireBall)
                QSound::play(":/music/src/music/ignite.wav");
            else if(peaball->mName == snowBall)
            {
                QSound::play(":/music/src/music/splat2.wav");
                if(!mZombies[row - 1][first]->isFrozen)
                    mZombies[row - 1][first]->mSpeed /= 2;
                mZombies[row - 1][first]->isFrozen = true;
                mZombies[row - 1][first]->mFrozenTime = 10;
            }
            //zombie -HP
            mZombies[row - 1][first]->HP -= peaball->ATK;
            //delete the peaBall
            for(int i = 0; i < mPeaBall[row - 1].size(); i++)
            {
                if(mPeaBall[row - 1][i] == peaball)
                {
                    delete peaball;
                    mPeaBall[row - 1].remove(i);
                    return true;
                }
            }
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
        //not enough sun
        if(mCard[i]->mCostSun > mSunNum)
        {
            mCard[i]->mNotEnougnSun->show();
            mCard[i]->mCardButton->hide();
        }
        //enougn sun
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
    if(mSunTime >= 5.5)
    {
        mSunTime = 0;
        mDropSunSlot();
    }
}

/*****Funstions for initing*****/

void InGame::mInitOtherUi()
{
    ui->level->setText("Level " + QString::number(mLevel));

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
    //read zombie show time in the txt of of this level
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
    mTimer->setInterval(64);    //64ms
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
    //read preview zombies data in the txt file of this level
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
    //create 5*9 blocks
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
            //use a signalmapper to get its row&column
            QObject::connect(mBlock[i][j]->mBlockButton, SIGNAL(clicked()), mBlockMapper, SLOT(map()));
            mBlockMapper->setMapping(mBlock[i][j]->mBlockButton, (row - 1) * 9 + column);
        }
    }
    QObject::connect(mBlockMapper, SIGNAL(mapped(int)), this, SLOT(mBlockClickedSlot(int)));
}

void InGame::mInitPlant()
{
    mPlantNum = 8;  //see the enum type in plant.h
    isPlant = false;
    mPlantName = null;

    //5*9 plant pointers
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
    mSunNum = 50;   //begin sun number
    ui->sunNum->setText(QString::number(mSunNum));

    mPlantCostSun = new int[mPlantNum];
    mPlantCostSun[null] = 0;
    mPlantCostSun[sunFlower] = 50;
    mPlantCostSun[peaShooter] = 100;
    mPlantCostSun[wallNut] = 50;
    mPlantCostSun[cherryBomb] = 150;
    mPlantCostSun[torchWood] = 175;
    mPlantCostSun[potatoMine] = 25;
    mPlantCostSun[snowPea] = 175;
}

void InGame::mInitCard()
{
    mCard = new Card*[mPlantNum - 1];
    for(int i = 0; i < mPlantNum - 1; i++)
    {
        mCard[i] = new Card(i + 1, (plantName)(i + 1), this);
    }
}

void InGame::mInitPlantCursor()
{
    //change the cursor when seleted plants
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

    mPlantCursorPixmap[potatoMine] = new QPixmap(":/Plants/PotatoMine/src/plants/PotatoMine/0.gif");
    mPlantCursor[potatoMine] = new QCursor(*mPlantCursorPixmap[potatoMine]);

    mPlantCursorPixmap[snowPea] = new QPixmap(":/Plants/SnowPea/src/plants/SnowPea/0.gif");
    mPlantCursor[snowPea] = new QCursor(*mPlantCursorPixmap[snowPea]);
}

void InGame::mousePressEvent(QMouseEvent *e)
{
    //press right button to cancel
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
    //get a random lawn of a zombie
    mRandRow = new RandNumber(1, 5);
    //get a random point of sun
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
    }
    //remove plant
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

//begin animation
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
    QObject::connect(bgMove, SIGNAL(finished()), ui->level, SLOT(hide()));
}

int InGame::mFindFirstZombie(QVector<Zombie*> v, int x)     //return the index of first zombie, return -1 if there's none
{
    //no zombie
    if(v.size() == 0)
    {
        return -1;
    }

    //no zombie in front of x
    int count = 0;
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i]->mx + v[i]->mHSpace < x)
            count++;
    }
    if(count == v.size())   //no zombie in front of x
    {
        return -1;
    }

    //find the first one
    int n = 0;
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i]->mx + v[i]->mHSpace >= x)
        {
            n = i;
            break;
        }
    }
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i]->mx < v[n]->mx && v[i]->mx + v[i]->mHSpace >= x)
            n = i;
    }
    return n;
}

/******other slots******/
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
            mSun.remove(i);
            i--;
            mSunNum += 25;
            ui->sunNum->setText(QString::number(mSunNum));
        }
        else if(mSun[i]->isToDie)
        {
            delete mSun[i];
            mSun.remove(i);
            i--;
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

void InGame::mExplodeSlot(explosionName name, int row, int column)
{
    QRect* rect;
    switch(name)    //different explosion, different size
    {
    case cherryBombExplosion:
        rect = new QRect(FIELD_X + (column - 2) * BLOCK_W,
                   FIELD_Y + (row - 2) * BLOCK_H,
                   BLOCK_W * 3, BLOCK_H * 3);
        break;
    case potatoMineExplosion:
        rect = new QRect(FIELD_X + (column - 1) * BLOCK_W - (BLOCK_W/2),
                         FIELD_Y + (row - 1) * BLOCK_H,
                         BLOCK_W * 2, BLOCK_H);
        break;
    }
    //find all the zombies in this rect & explode it
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mZombies[i].size(); j++)
        {
            if(rect->contains(mZombies[i][j]->pos().x() + mZombies[i][j]->mHSpace,
                             mZombies[i][j]->pos().y() + (mZombies[i][j]->size().height() * (3.0/5.0))) &&
                    !mZombies[i][j]->isExploded)
            {
                if(mZombies[i][j]->meetPlant)
                {
                    mPlants[mZombies[i][j]->mRow - 1][mZombies[i][j]->mColumn - 1]->isAttacked = false;
                    mZombies[i][j]->meetPlant = false;
                }
                mZombies[i][j]->mBeExploded();
            }
        }
    }
}

void InGame::mDeletePlantSlot(int row, int column)
{
    delete mPlants[row - 1][column - 1];
    mPlants[row - 1][column - 1] = NULL;
    mBlock[row - 1][column - 1]->isEmpty = true;
}

//the animation in the begining : ready set plants
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

