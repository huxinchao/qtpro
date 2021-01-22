#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmainwindow.h"
#include "QMainWindow"
#include "windows.h"
#include <QLabel>
#include <QRect>
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(1200,800);
    this->setMinimumSize(1200,800);


    ath = new AddThread(this);

    connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(setLabelText()));
    connect(ath, SIGNAL(message(int,int)), this, SLOT(setLabelPos(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::setLabelText() {
    ath->start();
}

void MainWindow::setLabelPos(int i, int j) {
    int w = ui->label_2->geometry().width();
    int h = ui->label_2->geometry().height();
    ui->label_2->setGeometry(i,j,w,h);
}


void MainWindow::on_pushButton_clicked(){}
