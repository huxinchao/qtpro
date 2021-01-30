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

void tranDialog::on_pushButton_clicked()
{
    emit sendData();
    this->close();
}

void tranDialog::on_pushButton_2_clicked()
{
    emit sendClear();
    this->close();
}

void tranDialog::setAccountInfo() {

}
