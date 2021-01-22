#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    QObject::connect(ui->horizontalSlider_2,SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    QObject::connect(ui->horizontalSlider_3,SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    Q_UNUSED(value);

    int R = ui->horizontalSlider->value();
    int G = ui->horizontalSlider_2->value();
    int B = ui->horizontalSlider_3->value();

    QPalette pal = ui->textEdit->palette();
    QColor color;
    color.setRgb(R, G, B, 100);

    pal.setColor(QPalette::Base, color);

    ui->textEdit->setPalette(pal);

    ui->lcdNumber->display(R);








}
