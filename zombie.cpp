#include "zombie.h"
#include "ZombieSize.h"
#include "FieldSize.h"
#include <qDebug>

Zombie::Zombie(int row, zombieName name, QWidget *parent) :
    QWidget(parent), mRow(row), mColumn(10), mFrozenTime(0),
    mZombieName(name), meetPlant(false), isExploded(false), isFrozen(false)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    mZombieLabel = new QLabel(this);
    mx = 900;
    mEatingSound = new QSound(":/music/src/music/chomp.wav");
    mEatingSound->setLoops(QSound::Infinite);
    switch(name)
    {
    case zombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        mStateNum = 1;
        mStateIndex = 1;
        mHSpace = ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 270;
        mSpeed = 1;
        break;
    case coneHeadZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombie.gif");
        mStateNum = 2;
        mStateIndex = 1;
        mHSpace = CONE_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 640;
        mSpeed = 1;
        break;
    case bucketHeadZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombie.gif");
        mStateNum = 2;
        mStateIndex = 1;
        mHSpace = BUCKET_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 1370;
        mSpeed = 1;
        break;
    case poleVaultingZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - POLE_ZOMBIE_V_SPACE,
                          POLE_ZOMBIE_W, POLE_ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - POLE_ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, POLE_ZOMBIE_W, POLE_ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombie.gif");
        mStateNum = 4;
        mStateIndex = 1;
        mHSpace = POLE_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 500;
        mSpeed = 2;
        break;
    case newspaperZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - NEWSPAPER_ZOMBIE_V_SPACE,
                          NEWSPAPER_ZOMBIE_W, NEWSPAPER_ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - NEWSPAPER_ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, NEWSPAPER_ZOMBIE_W, NEWSPAPER_ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadWalk1.gif");
        mStateNum = 3;
        mStateIndex = 1;
        mHSpace = NEWSPAPER_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 420;
        mSpeed = 1;
        break;
    case flagZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        my = FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE;
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/FlagZombie/src/zombies/FlagZombie/FlagZombie.gif");
        mStateNum = 1;
        mStateIndex = 1;
        mHSpace = ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 270;
        mSpeed = 1.5;
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();    
}



void Zombie::mUpdate()
{
    if(isExploded)
        return;
    if(!meetPlant)
    {
        mx -= mSpeed;
        move(QPoint(mx, my));
    }
    if(isFrozen)
    {
        if(mFrozenTime >= 0)
            mFrozenTime -= 0.064;
        else if(mFrozenTime < 0)
        {
            isFrozen = false;
            mSpeed *= 2;
        }
    }
    switch(mZombieName)
    {
    case coneHeadZombie:
        if(HP < 270 && HP >= 0 && mStateIndex == 1 && !isExploded)
        {
            if(!meetPlant)
                mNextMovie();
            else
            {
                mStateIndex++;
                mZombieAttack();
            }
        }
        break;
    case bucketHeadZombie:
        if(HP < 270 && HP >= 0 && mStateIndex == 1 && !isExploded)
        {
            if(!meetPlant)
                mNextMovie();
            else
            {
                mStateIndex++;
                mZombieAttack();
            }
        }
        break;
    case newspaperZombie:
        if(HP < 270 && mStateIndex == 1 && !isExploded)
        {
            mNextMovie();
        }
    }

}

void Zombie::mZombieNormal()
{
    if(isExploded)
        return;
    mEatingSound->stop();
    switch(mZombieName)
    {
    case zombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case coneHeadZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombie.gif");
        }
        else if(mStateIndex == 2)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        }
        break;
    case bucketHeadZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombie.gif");
        }
        else if(mStateIndex == 2)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        }
        break;
    case poleVaultingZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieWalk.gif");
        break;
    case newspaperZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadWalk1.gif");
        }
        else if(mStateIndex == 3)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadWalk0.gif");
        }
        break;
    case flagZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/FlagZombie/src/zombies/FlagZombie/FlagZombie.gif");
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}

void Zombie::mZombieAttack()
{
    if(isExploded)
        return;
    mEatingSound->play();
    switch(mZombieName)
    {
    case zombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/ZombieAttack.gif");
        break;
    case coneHeadZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombieAttack.gif");
        }
        else if(mStateIndex == 2)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/ZombieAttack.gif");
        }
        break;
    case bucketHeadZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombieAttack.gif");
        }
        else if(mStateIndex == 2)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/ZombieAttack.gif");
        }
        break;
    case poleVaultingZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieAttack.gif");
        break;
    case newspaperZombie:
        if(mStateIndex == 1)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadAttack1.gif");
        }
        else if(mStateIndex == 3)
        {
            delete mZombieMovie;
            mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadAttack0.gif");
        }
        break;
    case flagZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/FlagZombie/src/zombies/FlagZombie/FlagZombieAttack.gif");
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}


void Zombie::mBeExploded()
{
    if(isExploded)
        return;
    isExploded = true;
    mEatingSound->stop();
    if(mZombieName == zombie || mZombieName == coneHeadZombie || mZombieName == bucketHeadZombie || mZombieName == flagZombie)
    {
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/BoomDie.gif");
    }
    else if(mZombieName == poleVaultingZombie)
    {
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/BoomDie.gif");
    }
    else if(mZombieName == newspaperZombie)
    {
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/BoomDie.gif");
    }
    mZombieLabel->setMovie(mZombieMovie);
    QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mAfterExplosionSlot()));
    mZombieMovie->start();
}

void Zombie::mAfterExplosionSlot()
{
    HP -= 1800;
}

void Zombie::mNextMovie()
{
    if(isExploded)
        return;
    if(mStateIndex < mStateNum)
        mStateIndex++;
    switch(mZombieName)
    {
    case coneHeadZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case bucketHeadZombie:
        delete mZombieMovie;
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case poleVaultingZombie:
        if(mStateIndex == 2)
        {
            delete mZombieMovie;
            QSound::play(":/music/src/music/polevault.wav");
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieJump.gif");
            QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        }
        else if(mStateIndex == 3)
        {
            delete mZombieMovie;
            mx -= 105;
            this->move(mx, my);
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieJump2.gif");
            QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        }
        else if(mStateIndex == 4)
        {
            delete mZombieMovie;
            mSpeed = 1;
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieWalk.gif");
        }
        break;
    case newspaperZombie:
        if(mStateIndex == 2)
        {
            delete mZombieMovie;
            mSpeed = 0;
            mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/LostNewspaper.gif");
            QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        }
        else if(mStateIndex == 3)
        {
            delete mZombieMovie;
            mSpeed = 3;
            QSound::play(":/music/src/music/newspaper_rarrgh2.wav");
            if(meetPlant)
            {
                mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadAttack0.gif");
            }
            else
            {
                mZombieMovie = new QMovie(":/Zombies/NewspaperZombie/src/zombies/NewspaperZombie/HeadWalk0.gif");
            }
        }
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}

Zombie::~Zombie()
{
    delete mZombieMovie;
    delete mEatingSound;
}

