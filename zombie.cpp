#include "zombie.h"
#include "ZombieSize.h"
#include "FieldSize.h"
#include <QDebug>

Zombie::Zombie(int row, zombieName name, QWidget *parent) :
    QWidget(parent), mRow(row), mColumn(10),
    mZombieName(name), meetPlant(false), isExploded(false)
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
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();    
}

void Zombie::mZombieNormal()
{
    if(HP < 0)
        return;
    mEatingSound->stop();
    delete mZombieMovie;
    switch(mZombieName)
    {
    case zombie:
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case coneHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombie.gif");
        break;
    case bucketHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombie.gif");
        break;
    case poleVaultingZombie:
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieWalk.gif");
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}

void Zombie::mZombieAttack()
{
    mEatingSound->play();
    delete mZombieMovie;
    switch(mZombieName)
    {
    case zombie:
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/ZombieAttack.gif");
        break;
    case coneHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombieAttack.gif");
        break;
    case bucketHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombieAttack.gif");
        break;
    case poleVaultingZombie:
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieAttack.gif");
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}


void Zombie::mUpdate()
{
    if(!meetPlant && !isExploded)
    {
        mx -= mSpeed;
        move(QPoint(mx, my));
    }
    switch(mZombieName)
    {
    case coneHeadZombie:
        if(HP < 270 && HP >= 0 && mStateIndex == 1)
            mNextMovie();
        break;
    case bucketHeadZombie:
        if(HP < 270 && HP >= 0 && mStateIndex == 1)
            mNextMovie();
        break;
    }

}

void Zombie::mBeExploded()
{
    isExploded = true;
    delete mZombieMovie;
    if(mZombieName == zombie || mZombieName == coneHeadZombie || mZombieName == bucketHeadZombie)
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/BoomDie.gif");
    else if(mZombieName == poleVaultingZombie)
        mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/BoomDie.gif");
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
    if(mStateIndex < mStateNum)
        mStateIndex++;
    delete mZombieMovie;
    switch(mZombieName)
    {
    case coneHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case bucketHeadZombie:
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        break;
    case poleVaultingZombie:
        if(mStateIndex == 2)
        {
            QSound::play(":/music/src/music/polevault.wav");
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieJump.gif");
            QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        }
        else if(mStateIndex == 3)
        {
            mx -= 105;
            this->move(mx, my);
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieJump2.gif");
            QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mNextMovie()));
        }
        else if(mStateIndex == 4)
        {
            mSpeed = 1;
            mZombieMovie = new QMovie(":/Zombies/PpleVaultingZombie/src/zombies/PoleVaultingZombie/PoleVaultingZombieWalk.gif");
        }
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}

Zombie::~Zombie()
{
    delete mZombieMovie;
    delete mEatingSound;
}

