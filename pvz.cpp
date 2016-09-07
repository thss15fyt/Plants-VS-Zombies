#include "pvz.h"
#include "ui_pvz.h"
#include "welcome.h"
#include "help.h"
#include "ingame.h"
#include <QMouseEvent>

PVZ::PVZ(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PVZ)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    mLevel = 1;
    mGameState = welcome;
    mCurrentWidget = new Welcome(this);
    this->setCentralWidget(mCurrentWidget);
}

PVZ::~PVZ()
{
    delete ui;
}

void PVZ::mGameStateChangedSlot(gameStateType stateType)
{
    if(mGameState == stateType && mGameState != ingame)
        return;
    mGameState = stateType;
    delete mCurrentWidget;
    switch(stateType)
    {
    case welcome:
        mCurrentWidget = new Welcome(this);
        break;
    case help:
        mCurrentWidget = new Help(this);
        break;
    case ingame:
        mCurrentWidget = new InGame(mLevel, this);
    }
    this->setCentralWidget(mCurrentWidget);
}

void PVZ::mGameWinSlot()
{
    mLevel++;
}
