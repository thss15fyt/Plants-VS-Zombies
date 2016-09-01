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


namespace Ui {
class InGame;
}

class InGame : public QWidget
{
    Q_OBJECT

public:
    explicit InGame(QWidget *parent = 0);
    ~InGame();

signals:
    void mGameStateChanged(gameStateType);

protected:
    void mousePressEvent(QMouseEvent*);

private slots:
    void mUpdateSlot();
    void mBlockClickedSlot(int);
    void mCardClickedSlot(plantName);
    void on_menuButton_clicked();
    void on_SpadeBox_clicked();

private:
    Ui::InGame *ui;
    QTimer *mTimer;
    double mTime;    //one->16ms
    QSound *mChoosePlantsBgm;

    int mTotalZombies;
    int mShowZombies;   //num of zombies showed
    ZombieWithTime* mZombieShowTime;
    QVector<Zombie*> mZombies[5];
    PreviewZombie **mPreviewZombie;

    int mSunNum;
    int* mPlantCostSun;

    bool isSpade;
    QCursor* mSpadeCursor;
    QPixmap* mSpadeCursorPixmap;

    int mPlantNum;  //include "null", in reality is num-1
    bool isPlant;
    plantName mPlantName;
    QCursor** mPlantCursor;
    QPixmap** mPlantCursorPixmap;
    Plant ***mPlants;

    Block*** mBlock;
    QSignalMapper *mBlockMapper;

    Card **mCard;

    RandNumber* mRandRow;

    void mPlayBgm();
    void mBeginMove();

    void mInitZombieTime();
    void mInitTimer();
    void mInitSpade();
    void mInitPreviewZombies();
    void mInitBlock();
    void mInitPlant();
    void mInitPlantCostSun();
    void mInitCard();
    void mInitCursor();

    void mShowZombieUpdate();
    void mZombieMeetPlantUpdate();
    void mPlantUpdate();
};

#endif // INGAME_H
