#ifndef PLANT_H
#define PLANT_H

#include <QWidget>
#include <QLabel>
#include <QMovie>

enum plantName{null, sunFlower, peaShooter, wallNut};

class Plant : public QWidget
{
    Q_OBJECT
public:
    explicit Plant(int row, int column, plantName name, QWidget *parent = 0);
    ~Plant();

    int mRow, mColumn;
    plantName mName;
    QLabel* mPlantLabel;
    QMovie* mPlantCurrentMovie;
    int mMovieIndex;
    int mMovieNum;
    int mCDTime;
    int HP;

    bool isAttacked;
    int ATKofZombie;

    void mNextMovie();

    void mUpdate();
    void mWallNutUpdate();
signals:

public slots:

};

#endif // PLANT_H
