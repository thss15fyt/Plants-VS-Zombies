#include "zombie.h"
#include "ZombieSize.h"
#include "FieldSize.h"

Zombie::Zombie(int row, zombieName name, QWidget *parent) :
    QWidget(parent), mRow(row), mColumn(10),
    mZombieName(name), meetPlant(false), isExploded(false)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    mZombieLabel = new QLabel(this);
    mx = 900;
    my = FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE;
    switch(name)
    {
    case zombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/Zombie.gif");
        mStateNum = 1;
        mStateIndex = 1;
        mHSpace = ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 270;
        break;
    case coneHeadZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/ConeHeadZombie/src/zombies/ConeheadZombie/ConeheadZombie.gif");
        mStateNum = 1;
        mStateIndex = 1;
        mHSpace = CONE_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 640;
        break;
    case bucketHeadZombie:
        this->setGeometry(900, FIELD_Y + (mRow - 1) * BLOCK_H - ZOMBIE_V_SPACE, ZOMBIE_W, ZOMBIE_H);
        mZombieLabel->setGeometry(0, 0, ZOMBIE_W, ZOMBIE_H);
        mZombieMovie = new QMovie(":/Zombies/BucketHeadZombie/src/zombies/BucketheadZombie/BucketheadZombie.gif");
        mStateNum = 1;
        mStateIndex = 1;
        mHSpace = BUCKET_ZOMBIE_H_SPACE;
        ATK = 6;
        HP = 1370;
        break;
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();    
}

void Zombie::mZombieNormal()
{
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
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}

void Zombie::mZombieAttack()
{
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
    }
    mZombieLabel->setMovie(mZombieMovie);
    mZombieMovie->start();
}


void Zombie::mUpdate()
{
    if(!meetPlant && !isExploded)
    {
        mx--; //may change speed
        move(QPoint(mx, my));
    }

}

void Zombie::mBeExploded()
{
    isExploded = true;
    delete mZombieMovie;
    mZombieMovie = new QMovie(":/Zombies/Zombie/src/zombies/Zombie/BoomDie.gif");
    mZombieLabel->setMovie(mZombieMovie);
    QObject::connect(mZombieMovie, SIGNAL(finished()), this, SLOT(mAfterExplosionSlot()));
    mZombieMovie->start();
}

void Zombie::mAfterExplosionSlot()
{
    HP -= 1800;
}

Zombie::~Zombie()
{
    delete mZombieMovie;
}

