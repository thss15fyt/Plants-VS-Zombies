/*********************************************************
 *
 *  class name: zombie
 *
 *  now 6 zombies(see the enum type zombieName below
 *
 *  main properties:
 *      mRow, mColumn, mZombieName
 *      mSpeed, ATK, HP,
 *      mStateNum, mStateIndex
 *
 *  main functions:
 *      mUpdate: move, change HP and change state index
 *      mNextMovie: mStateIndex++ and set a new movie
 *      mZombieNormal: set state to normal state
 *      mZombieAttack: set state to attack state
 *      mBeExploded: set state to exploded state
 *
 **********************************************************/

#ifndef Q_ZOMBIE_H
#define Q_ZOMBIE_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QSound>

enum zombieName{zombie, coneHeadZombie, bucketHeadZombie, poleVaultingZombie, newspaperZombie, flagZombie};

class Zombie : public QWidget
{
    Q_OBJECT
public:
    explicit Zombie(int row, zombieName name, QWidget *parent = 0);
    ~Zombie();

    int mRow;
    int mColumn;
    int mStateNum;
    int mStateIndex;
    double mx, my;
    double mSpeed;
    int mHSpace;
    int ATK;
    int HP;
    QLabel* mZombieLabel;
    QMovie* mZombieMovie;
    zombieName mZombieName;
    bool meetPlant;
    bool isExploded;
    bool isFrozen;
    double mFrozenTime;
    QSound* mEatingSound;

    void mZombieNormal();
    void mZombieAttack();
    void mBeExploded();

    void mUpdate();

signals:

public slots:
    void mAfterExplosionSlot();
    void mNextMovie();
};

struct ZombieWithTime
{
    double time;
    zombieName name;
};

#endif // ZOMBIE_H
