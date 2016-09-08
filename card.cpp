#include "card.h"
#include "plant.h"
#include "FieldSize.h"
#include "ingame.h"
#include <QPropertyAnimation>
#include <QRect>

Card::Card(int index, plantName name, QWidget *parent) :
    QWidget(parent), mIndex(index), mName(name), mLeftTime(0)
{
    this->setGeometry(CARD_BEGIN_X + (index - 1) * CARD_WIDTH + (index - 1) * CARD_SPACE, CARD_BEGIN_Y,
                        CARD_WIDTH, CARD_HEIGHT);
    this->setCursor(Qt::PointingHandCursor);

    mCardButton = new QPushButton(this);
    mCardButton->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
    mCardButton->setStyleSheet("border:none;");
    QObject::connect(mCardButton, SIGNAL(clicked()), this, SLOT(mButtonClickedSlot()));
    QObject::connect(this, SIGNAL(mButtonClicked(plantName)), parent, SLOT(mCardClickedSlot(plantName)));
    mCardButton->hide();

    mEmptyCardLabel = new QLabel(this);
    mEmptyCardLabel->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
    mEmptyCardLabel->setStyleSheet("background-image: url(:/src/interface/EmptyCard.png)");
    mEmptyCardLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    mCardLabel = new QLabel(this);
    mCardLabel->setGeometry(CARD_PLANT_X, CARD_PLANT_Y, CARD_PLANT_W, CARD_PLANT_H);
    mCardLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    mCostSunLabel = new QLabel(this);
    mCostSunLabel->setGeometry(CARD_COSTSUN_X, CARD_COSTSUN_Y,CARD_COSTSUN_W, CARD_COSTSUN_H);
    mCostSunLabel->setStyleSheet("font-size:12px;");
    mCostSunLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    mNotEnougnSun = new QLabel(this);
    mNotEnougnSun->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
    mNotEnougnSun->setStyleSheet("background-image: url(:/src/interface/NotEnoughSun.png)");
    mNotEnougnSun->setAttribute(Qt::WA_TransparentForMouseEvents);

    mInCD = new QLabel(this);
    mInCD->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
    mInCD->setStyleSheet("background-image: url(:/src/interface/InCD.png)");
    mInCD->setAttribute(Qt::WA_TransparentForMouseEvents);
    mInCD->hide();

    switch(name)
    {
    case null:
        mCostSun = 0;
        mCDTime = 0;
        break;
    case sunFlower:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/SunFlower/src/plants/SunFlower/SunFlowerCard.png)");
        mCostSun = 50;
        mCDTime = 0;//7.5;
        break;
    case peaShooter:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/PeaShooter/src/plants/Peashooter/PeashooterCard.png)");
        mCostSun = 100;
        mCDTime = 0;//7.5;
        break;
    case wallNut:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/WallNut/src/plants/WallNut/WallNutCard.png)");
        mCostSun = 50;
        mCDTime = 0;//30;
        break;
    case cherryBomb:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/CherryBomb/src/plants/CherryBomb/CherryBombCard.png)");
        mCostSun = 150;
        mCDTime = 0;//30;
        break;
    case torchWood:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/TorchWood/src/plants/TorchWood/TorchwoodCard.png)");
        mCostSun = 175;
        mCDTime = 0;//7.5;
        break;
    case potatoMine:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/PotatoMine/src/plants/PotatoMine/PotatoMineCard.png)");
        mCostSun = 25;
        mCDTime = 0;//30;
        break;
    }
    mCostSunLabel->setText(QString::number(mCostSun));

    cd = new QPropertyAnimation(mInCD,"geometry");
    cd->setDuration(mCDTime * 1000);
    cd->setStartValue(QRect(0, 0, CARD_WIDTH, CARD_HEIGHT));
    cd->setEndValue(QRect(0, 0, CARD_WIDTH, 0));
}

void Card::mButtonClickedSlot()
{
    if(mLeftTime <= 0)
        emit mButtonClicked(mName);
}

Card::~Card()
{
    delete cd;
}
