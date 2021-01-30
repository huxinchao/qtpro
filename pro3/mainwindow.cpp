#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QRegExp>
#include <QValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    QRegExp regx("[0-9a-zA-Z]*");

    QValidator* vali_username = new QRegExpValidator(regx, ui->lineEdit);
    QValidator* vali_password = new QRegExpValidator(regx, ui->lineEdit_2);

    ui->lineEdit->setValidator(vali_username);
    ui->lineEdit_2->setValidator(vali_password);









}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    if(username == "" && password == "") {
        ui->textEdit->append("[+] 连接成功！");
        ui->textEdit->append("[+] 获取账户信息...");

        game_win = new game_window();
        game_win->show();
        this->~MainWindow();



    } else {
        ui->textEdit->append("[-] 连接失败 !");
        ui->textEdit->append("[-] 账户或密码错误 !");
    }




}

