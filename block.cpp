#include "block.h"
#include "FieldSize.h"

Block::Block(int row, int column, QWidget *parent) :
    QWidget(parent), mRow(row), mColumn(column)
{
    this->setGeometry(FIELD_X + (column - 1) * BLOCK_W, FIELD_Y + (row - 1) * BLOCK_H,
                      BLOCK_W, BLOCK_H);
    mBlockButton = new QPushButton(this);
    mBlockButton->setGeometry(0, 0, BLOCK_W, BLOCK_H);
    mBlockButton->setStyleSheet("border:none;");

    isEmpty = true;
    mName = null;
}
