/*************************************************************************************
 *
 *  ui class: ingame
 *  the main console of adventure game
 *
 *  create events:
 *
 *      plants:   user decides
 *      zombies:  reading game data from txt file & create zombies in specified time
 *      sun:      dropped or produced by sunflowers
 *      other ui: map, card bar, spade box, go-back button and more
 *
 *  update events:
 *
 *      showZombie update: create zombies in specified time
 *      plant update:      specified behaviour(produce sun, create peaball...), death judge
 *      zombie update:     meet & attack plants, death judge, gameWin judge, gameOver judge
 *      peaball update:    move & hit zombies, change peaball after passing a fireWood
 *      card update:       switch between states: available, in cd & not enough sun
 *      sun update:        produce, collect or remove sun
 *
 *      ps: now update time interval: 64ms
 *          this console only finish the interaction between plants, zombies...
 *          other update will be finished by thier own methods
 *
 ***********************************************************************************/
#ifndef INGAME_H
#define INGAME_H

#include "pvz.h"
#include <QWidget>
#include <QSound>
#include <QSignalMapper>
#include <QPixmap>
#include <QCursor>
#include <QTimer>
#include <QVector>
#include "previewzombie.h"
#include "plant.h"
#include "card.h"
#include "block.h"
#include "zombie.h"
#include "randnumber.h"
#include "peaball.h"
#include "sun.h"

namespace Ui {
class InGame;
}

class InGame : public QWidget
{
    Q_OBJECT

public:
    explicit InGame(int level, QWidget *parent = 0);
    ~InGame();

signals:
    void mGameStateChanged(gameStateType);
    void mGameWin();
    void mGameOver();

protected:
    void mousePressEvent(QMouseEvent*);

private slots:
    void mUpdateSlot();     //connect with main timer

    //slots triggered by update
    void mDropSunSlot();
    void mDeleteSunSlot();
    void mExplodeSlot(explosionName, int row, int column);
    void mDeletePlantSlot(int row, int column);
    void mReadySetPlantSlot();
    void mGameOverSlot();
    void mRestartSlot();

    //slots triggered by ui items
    void mBlockClickedSlot(int);
    void mCardClickedSlot(plantName);
    void on_menuButton_clicked();
    void on_SpadeBox_clicked();
    void on_win_clicked();

private:

    /****properties*****/
    Ui::InGame *ui;
    int mLevel;

    //time
    QTimer *mTimer;
    double mTime;    //one->16ms
    double mSunTime;

    //BGM
    QSound *mChoosePlantsBgm;
    QSound *mBGM;

    //plants
    int mPlantNum;  //include "null", in reality is num-1
    bool isPlant;
    plantName mPlantName;
    QCursor** mPlantCursor;
    QPixmap** mPlantCursorPixmap;
    Plant ***mPlants;

    //zombies
    int mTotalZombies;
    int mShowZombies;   //num of zombies showed
    int mDieZombies;
    ZombieWithTime* mZombieShowTime;
    QVector<Zombie*> mZombies[5];
    PreviewZombie **mPreviewZombie;

    //sun
    int mSunNum;
    int* mPlantCostSun;
    QVector<Sun*> mSun;

    //spade
    bool isSpade;
    QCursor* mSpadeCursor;
    QPixmap* mSpadeCursorPixmap;

    //peaball
    QVector<PeaBall*> mPeaBall[5];

    //blocks
    Block*** mBlock;
    QSignalMapper *mBlockMapper;

    //cards
    Card **mCard;

    //random number
    RandNumber* mRandRow;
    RandNumber* mRandSunX;
    RandNumber* mRandSunY;

    //other ui movie
    QMovie *mGameOverMovie;
    QMovie *mReadyMovie;

    /*******functions******/

    void mPlayBgm();
    void mBeginMove();

    //functions for initing
    void mInitOtherUi();
    void mInitZombieTime();
    void mInitTimer();
    void mInitSpade();
    void mInitPreviewZombies();
    void mInitBlock();
    void mInitPlant();
    void mInitPlantCostSun();
    void mInitCard();
    void mInitPlantCursor();
    void mInitRand();

    //functions for update
    void mShowZombieUpdate();
    void mPlantUpdate();
    void mZombieUpdate();
    void mPeaBallUpdate();
    void mZombieMeetPlantUpdate();
    void mPlantFindZombieUpdate(Plant*);
    bool mPeaBallMeetZombieUpdate(PeaBall*&);
    void mCardUpdate();
    void mSunUpdate();

    //other tool function
    int mFindFirstZombie(QVector<Zombie*>, int x);
    void mProduceSun(Plant*);
};

#endif // INGAME_H
