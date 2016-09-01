#include "pvz.h"
#include "welcome.h"
#include "ui_welcome.h"
#include <QPropertyAnimation>
#include <QPoint>
#include <QTimer>
#include <QMovie>
#include <QSound>

Welcome::Welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    mWoodMove();    //top left woodsign drop
    mPlayBgm();
    mInitZombieHand();
    ui->hideAll->hide();

    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));
}
Welcome::~Welcome()
{
    mBgm->stop();
    delete ui;
}

void Welcome::mWoodMove()
{
    QPropertyAnimation* wood, *namechange;
    wood = new QPropertyAnimation(ui->WoodSign, "pos");
    wood->setDuration(700);
    wood->setStartValue(QPoint(40, -211));
    wood->setKeyValueAt(0.57, QPoint(40, 10));
    wood->setKeyValueAt(0.8, QPoint(40, -8));
    wood->setEndValue(QPoint(40, 0));
    namechange = new QPropertyAnimation(ui->NameChange, "pos");
    namechange->setDuration(700);
    namechange->setStartValue(QPoint(40, -71));
    namechange->setKeyValueAt(0.57, QPoint(40, 155));
    namechange->setKeyValueAt(0.85, QPoint(40, 132));
    namechange->setEndValue(QPoint(40, 140));
    wood->start();
    namechange->start();
}

void Welcome::mPlayBgm()
{
    mBgm = new QSound(":/music/src/music/Faster.wav", this);
    mBgm->setLoops(QSound::Infinite);
    mBgm->play();
}

void Welcome::mInitZombieHand()
{
    mFlashTurn = 0;
    mZombieHandLabel = new QLabel(this);
    mZombieHandLabel->setGeometry(290, 255, 250, 310);
    mZombieHandMovie = new QMovie(":/src/interface/ZombiesHand.gif");
    mZombieHandLabel->setMovie(mZombieHandMovie);
    QObject::connect(mZombieHandMovie, SIGNAL(finished()), this, SLOT(mAdventureBeginSlot()));
    mAdventureButtonFlashTimer = new QTimer();
    mAdventureButtonFlashTimer->setInterval(110);
    mAdventureButtonFlashTimer->setSingleShot(false);
    QObject::connect(mAdventureButtonFlashTimer, SIGNAL(timeout()), this, SLOT(mAdventureButtonFlash()));
    mZombieHandLabel->hide();
}


void Welcome::mAdventureBeginSlot()
{
    emit mGameStateChanged(ingame);
}

void Welcome::mAdventureButtonFlash()
{
    mFlashTurn++;
    if(mFlashTurn % 2 == 0)
        ui->Adventure->setStyleSheet(
                    "background-image: url(:/src/interface/SelectorScreenStartAdventure.png);background-color:transparent;background-position: left bottom");
    else
        ui->Adventure->setStyleSheet(
                    "background-image: url(:/src/interface/SelectorScreenStartAdventure.png);background-color:transparent;background-position: left top");
}

void Welcome::on_Adventure_clicked()
{
    mBgm->stop();
    ui->hideAll->show();
    mZombieHandLabel->show();
    mZombieHandMovie->start();
    mAdventureButtonFlashTimer->start();
    QSound::play(":/music/src/music/ZombiesLaugh.wav");
}

void Welcome::on_Help_clicked()
{
    emit mGameStateChanged(help);
}


void Welcome::on_Quit_clicked()
{
    ui->hideAll->show();
    ui->QuitDialog->move(240, 180);
    ui->QuitGame->move(270, 350);
    ui->QuitCancel->move(443, 350);
}

void Welcome::on_QuitGame_clicked()
{
    qApp->quit();
}

void Welcome::on_QuitCancel_clicked()
{
    ui->QuitDialog->move(900, 600);
    ui->QuitGame->move(900, 600);
    ui->QuitCancel->move(900, 600);
    ui->hideAll->hide();
}

