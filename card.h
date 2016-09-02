#ifndef CARD_H
#define CARD_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include "plant.h"

class Card : public QWidget
{
    Q_OBJECT
public:
    explicit Card(int index, plantName name, QWidget *parent = 0);
    int mIndex;
    int mCostSun;
    double mCDTime;
    double mLeftTime;
    plantName mName;
    QPushButton* mCardButton;
    QLabel* mCardLabel;
    QLabel* mCostSunLabel;
    QLabel* mNotEnougnSun;
    QLabel* mInCD;
    QPropertyAnimation *cd;
signals:
    void mButtonClicked(plantName);

public slots:
    void mButtonClickedSlot();
};

#endif // CARD_H
