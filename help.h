/************************************
 *
 *  ui class: help
 *  a help scene with a go-back button
 *
 *************************************/

#ifndef HELP_H
#define HELP_H

#include <QWidget>
#include "pvz.h"

namespace Ui {
class Help;
}

class Help : public QWidget
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

private slots:
    void on_helpMainMenu_clicked();

signals:
    void mGameStateChanged(gameStateType);

private:
    Ui::Help *ui;
};

#endif // HELP_H
