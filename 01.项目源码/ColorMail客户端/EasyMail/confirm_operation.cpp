#include "confirm_operation.h"
#include "ui_confirm_operation.h"

Confirm_operation::Confirm_operation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Confirm_operation)
{
    ui->setupUi(this);
}

Confirm_operation::~Confirm_operation()
{
    delete ui;
}

void Confirm_operation::on_confirm_clicked()
{
    emit confirmoperate(1); //确认操作
    this->hide();
}

void Confirm_operation::on_cancel_clicked()
{
    emit confirmoperate(0); //取消操作
    this->hide();
}
