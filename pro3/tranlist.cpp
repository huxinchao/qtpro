#include "tranlist.h"
#include "ui_tranlist.h"

tranlist::tranlist(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tranlist)
{
    ui->setupUi(this);

    QStringList header;
    ui->tranTable->setRowCount(10);
    ui->tranTable->setColumnCount(4);
    ui->tranTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tranTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tranTable->setColumnWidth(0,150);
    ui->tranTable->setColumnWidth(1,110);
    ui->tranTable->setColumnWidth(2,110);
    ui->tranTable->setColumnWidth(3,105);

    header << "Date " << "Source Account" << "  Target Account  " << "Number";

    ui->tranTable->setHorizontalHeaderLabels(header);


    ui->tranTable->setItem(0,0,new QTableWidgetItem("2020-12-7 13:58:39"));
    ui->tranTable->setItem(0,1,new QTableWidgetItem("Huxinchao"));
    ui->tranTable->setItem(0,2,new QTableWidgetItem("DingAoEn"));
    ui->tranTable->setItem(0,3,new QTableWidgetItem("30000"));




    ui->tranTable->show();






}

tranlist::~tranlist()
{
    delete ui;
}
