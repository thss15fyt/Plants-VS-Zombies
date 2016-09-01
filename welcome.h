#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include <QLabel>
#include <QSound>
#include "pvz.h"

namespace Ui {
class Welcome;
}

class Welcome : public QWidget
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = 0);
    ~Welcome();

signals:
    void mGameStateChanged(gameStateType);

private slots:
    void mAdventureBeginSlot();
    void mAdventureButtonFlash();

    void on_Help_clicked();
    void on_Quit_clicked();
    void on_QuitGame_clicked();
    void on_QuitCancel_clicked();
    void on_Adventure_clicked();

private:
    Ui::Welcome *ui;
    QSound* mBgm;
    QTimer* mAdventureButtonFlashTimer;
    QLabel* mZombieHandLabel;
    QMovie* mZombieHandMovie;
    int mFlashTurn;

    void mWoodMove();
    void mInitZombieHand();
    void mPlayBgm();
};

#endif // WELCOME_H
