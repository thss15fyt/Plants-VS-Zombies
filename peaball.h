#ifndef PEABALL_H
#define PEABALL_H

#include <QWidget>
#include <QLabel>
#include <QMovie>

enum peaBallName{peaBall, fireBall};

class PeaBall : public QWidget
{
    Q_OBJECT
public:
    explicit PeaBall(peaBallName name, int row, int column, QWidget *parent = 0);
    ~PeaBall();
    peaBallName mName;
    int mRow, mColumn;
    int mx, my;
    int mSpeed;
    int ATK;
    QLabel* mLabel;
    QMovie* mMovie;

    void mUpdate();
signals:

public slots:

};

#endif // PEABALL_H
