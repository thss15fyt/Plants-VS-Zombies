#include "help.h"
#include "ui_help.h"

Help::Help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(mGameStateChanged(gameStateType)),
                     parent, SLOT(mGameStateChangedSlot(gameStateType)));
}

Help::~Help()
{
    delete ui;
}

void Help::on_helpMainMenu_clicked()
{
    emit mGameStateChanged(welcome);
}
