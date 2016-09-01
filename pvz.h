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

/*******Functions*********/
public:
    explicit PVZ(QWidget *parent = 0);
    ~PVZ();

signals:
    void mouseMove(QPoint point);

private slots:
    void mGameStateChangedSlot(gameStateType);
};

#endif // PVZ_H
