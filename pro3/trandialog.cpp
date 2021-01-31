#include "trandialog.h"
#include "ui_trandialog.h"

tranDialog::tranDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tranDialog)
{
    ui->setupUi(this);

}

tranDialog::~tranDialog()
{
    delete ui;
}

void tranDialog::on_confirm_button_clicked()
{
    emit sendData();
    this->close();
}

void tranDialog::on_cancel_button_clicked()
{
    emit sendClear();
    this->close();
}

void tranDialog::setAccountInfo() {

}
