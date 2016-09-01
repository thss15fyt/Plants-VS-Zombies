#ifndef PEABALL_H
#define PEABALL_H

#include <QWidget>
#include <QLabel>

enum peaBallName{peaBall};

class PeaBall : public QWidget
{
    Q_OBJECT
public:
    explicit PeaBall(peaBallName name, int row, int column, QWidget *parent = 0);

    peaBallName mName;
    int mRow, mColumn;
    int mx, my;
    int mSpeed;
    int ATK;
    QLabel* mLabel;

    void mUpdate();
signals:

public slots:

};

#endif // PEABALL_H
