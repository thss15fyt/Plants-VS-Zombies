/*********************************************************************************************
 *
 *  class name: PVZ
 *  the console of the whole game
 *
 *  main properties:
 *
 *      mGameState: store the current game state of the game,
 *                  each state points to a game scene,
 *                  see the enum type below
 *
 *      mCurrentWidget: a pointer of QWidget
 *                      points to the current widget of the game
 *
 *      mLevel: current of the game,
 *              now there are 5 levels,
 *              go back to 1st level after finish all
 *
 *  main functions:
 *
 *      mGameStateChangedSlot: change the game state when receive a signal of game change
 *                             delete the current widget and create a new one
 *
 ********************************************************************************************/
#ifndef PVZ_H
#define PVZ_H

#include <QMainWindow>
#include <QWidget>

enum gameStateType{welcome, help, ingame};

namespace Ui {
class PVZ;
}

class PVZ : public QMainWindow
{
    Q_OBJECT

/********ProPerties********/

private:
    Ui::PVZ *ui;
    QWidget *mCurrentWidget;
    gameStateType mGameState;  //current game status
    int mLevel;

/*******Functions*********/
public:
    explicit PVZ(QWidget *parent = 0);
    ~PVZ();

signals:
    void mouseMove(QPoint point);

private slots:
    void mGameStateChangedSlot(gameStateType);
    void mGameWinSlot();
};

#endif // PVZ_H
