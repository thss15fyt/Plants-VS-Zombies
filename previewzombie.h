#ifndef PREVIEWZOMBIE_H
#define PREVIEWZOMBIE_H

#include <QWidget>
#include <QLabel>
#include <QPoint>
#include"zombie.h"

class PreviewZombie : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewZombie(QPoint beginPoint, zombieName name, QWidget *parent = 0);
    ~PreviewZombie();

public:
    QLabel* mShowLabel;
    QMovie *mShowMovie;
    QPoint mBeginPoint;

signals:

public slots:

private:
    void mPreviewMove();

};

#endif // PREVIEWZOMBIE_H
