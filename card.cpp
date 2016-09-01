#include "card.h"
#include "plant.h"
#include "FieldSize.h"
#include "ingame.h"


Card::Card(int index, plantName name, QWidget *parent) :
    QWidget(parent), mIndex(index), mName(name), mLeftTime(0)
{
    this->setGeometry(CARD_BEGIN_X + (index - 1) * CARD_WIDTH + (index - 1) * CARD_SPACE, CARD_BEGIN_Y,
                        CARD_WIDTH, CARD_HEIGHT);
    this->setCursor(Qt::PointingHandCursor);

    mCardButton = new QPushButton(this);
    mCardButton->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
    mCardButton->setStyleSheet("background-image: url(://src/interface/EmptyCard.png);border:none;");
    QObject::connect(mCardButton, SIGNAL(clicked()), this, SLOT(mButtonClickedSlot()));
    QObject::connect(this, SIGNAL(mButtonClicked(plantName)), parent, SLOT(mCardClickedSlot(plantName)));

    mCardLabel = new QLabel(this);
    mCardLabel->setGeometry(CARD_PLANT_X, CARD_PLANT_Y, CARD_PLANT_W, CARD_PLANT_H);
    mCardLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    mCostSunLabel = new QLabel(this);
    mCostSunLabel->setGeometry(CARD_COSTSUN_X, CARD_COSTSUN_Y,CARD_COSTSUN_W, CARD_COSTSUN_H);
    mCostSunLabel->setStyleSheet("font-size:12px;");
    mCostSunLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    switch(name)
    {
    case null:
        mCostSun = 0;
        mCDTime = 0;
        break;
    case sunFlower:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/SunFlower/src/plants/SunFlower/SunFlowerCard.png)");
        mCostSun = 50;
        mCDTime = 7.5;
        break;
    case peaShooter:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/PeaShooter/src/plants/Peashooter/PeashooterCard.png)");
        mCostSun = 100;
        mCDTime = 7.5;
        break;
    case wallNut:
        mCardLabel->setStyleSheet("background-image: url(:/Plants/WallNut/src/plants/WallNut/WallNutCard.png)");
        mCostSun = 50;
        mCDTime = 30;
        break;
    }
    mCostSunLabel->setText(QString::number(mCostSun));
}

void Card::mButtonClickedSlot()
{
    if(mLeftTime <= 0)
        emit mButtonClicked(mName);
}
