#ifndef Q_BLOCK_H
#define Q_BLOCK_H

#include <QWidget>
#include <QPushButton>
#include "plant.h"

class Block : public QWidget
{
    Q_OBJECT
public:
    explicit Block(int row, int column, QWidget *parent = 0);

    int mRow, mColumn;
    QPushButton* mBlockButton;
    bool isEmpty;
    plantName mName;

signals:

public slots:

};

#endif // BLOCK_H
