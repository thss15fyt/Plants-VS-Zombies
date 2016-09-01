#ifndef Q_ZOMBIE_H
#define Q_ZOMBIE_H

#include <QWidget>
#include <QLabel>
#include <QMovie>

enum zombieName{zombie, coneHeadZombie, bucketHeadZombie};

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
    int mx, my;
    int mHSpace;
    int ATK;
    QLabel* mZombieLabel;
    QMovie* mZombieMovie;
    zombieName mZombieName;
    bool meetPlant;

    void mZombieNormal();
    void mZombieAttack();

signals:

public slots:

};

struct ZombieWithTime
{
    double time;
    zombieName name;
};

#endif // ZOMBIE_H
