#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QCoreApplication>
#include <QListWidgetItem>
#include <QIcon>
#include <QRegExp>
#include <QValidator>
#include <QMutex>
#include "game_window.h"
#include "ui_game_window.h"
#include "task_stru.h"
#include <vector>

QMutex mutex;
std::vector<TASK_STRU> TASK_VEC;


game_window::game_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::game_window)

{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    trand = new tranDialog();
    tlist = new tranlist();
    con_thread = new CON_THREAD(this);
    pdisk = new playdisk();

    connect(trand,SIGNAL(sendData()),this,SLOT(dealChips()));
    connect(trand,SIGNAL(sendClear()),this, SLOT(clearDealer()));
    connect(con_thread,SIGNAL(startFlash(int,int,int)),this,SLOT(startFlash(int,int,int)));
    connect(pdisk,SIGNAL(stopFlash(int)),this,SLOT(stopFlash(int)));
    connect(con_thread,SIGNAL(sendCard(int)),this,SLOT(sendCard(int)));
    connect(con_thread,SIGNAL(moveChip(int,int)),this,SLOT(moveChip(int,int)));



    QListWidgetItem* aItem;
    QIcon qic;
    qic.addFile("image/n_user.ico");



    for(int i = 1; i < 6; i++) {
        QString str = QString::asprintf("[ R%d ]-[ 1 / 100 / Pot ]-[Gaming]\n%s,%s", i,"dingaoen","huxinchao");
        aItem = new QListWidgetItem();

        aItem->setText(str);
        aItem->setIcon(qic);
        ui->gameList->addItem(aItem);

    }

    ui->dl_target->addItem("[Target Account]");
    for(int i = 4; i < 7; i++) {
        ui->dl_target->addItem("username" + QString::number(i+1));
    }

    QRegExp regx("[0-9]*");
    QValidator *vali = new QRegExpValidator(regx, ui->dl_number);
    ui->dl_number->setValidator(vali);



    //----- Background -----

    if(background_img.load("image/background.bmp")) {

    }else {
        ui->serdataseq_edit->append("Background Load Failed");
    }

    //---- Playdisk ----


    if(playdisk_img.load("image/playdisk2.png")) {

    } else {
        ui->serdataseq_edit->append("Playdisk Load Failed");
    }

    //---- Headframe ----

    QSize headframe_size(210,80);
    QSize flash_size(80,80);
    if(own_headframe_img.load("image/headw_own.png")) {
        own_headframe_img = own_headframe_img.scaled(headframe_size,Qt::KeepAspectRatio,Qt::SmoothTransformation);

    } else {
        ui->serdataseq_edit->append("Own Headframe Load Failed");
    }


    if(riv_headframe_img.load("image/headw_rival.png")) {
        riv_headframe_img = riv_headframe_img.scaled(headframe_size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    } else {
        ui->serdataseq_edit->append("Riv Headframe Load Failed");
    }

    if(flashhead_img.load("image/flash.png")) {
        flashhead_img = flashhead_img.scaled(flash_size,Qt::KeepAspectRatio,Qt::SmoothTransformation);


    } else {
        ui->serdataseq_edit->append("FlashHead Load Failed");
    }

    //----- Pokers -----




    QString pokers_path = "image/poker/";
    QString pokers_path_a[4] = {"a","b","c","d"};
    QString pokers_path_b[13] = {"1","2","3","4","5","6","7","8","9","10","j","q","k"};
    QString pokers_respath;

    QSize pokerSize(120,90);
    QSize allpokerSize(150,110);
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 13; j++) {
            pokers_respath = pokers_path + pokers_path_a[i] + pokers_path_b[j] + ".png";
            if(pokers_img[i][j].load(pokers_respath)) {
                pokers_img[i][j]= pokers_img[i][j].scaled(pokerSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            } else {
                ui->serdataseq_edit->append(pokers_path_a[i] + pokers_path_b[j] + " Poker Load Error");
            }
        }
    }

    if(blank_poker_img.load(pokers_path + "blank.png")) {
        blank_poker_img = blank_poker_img.scaled(pokerSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    } else {
        ui->serdataseq_edit->append("Blank Poker Load Error");
    }

    if(fold_poker_img.load(pokers_path + "hcard.png")) {
        fold_poker_img = fold_poker_img.scaled(pokerSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    } else {
        ui->serdataseq_edit->append("Fold Poker Load Error");
    }

    if(allpoker_img.load(pokers_path+"allpoker.png")) {
        allpoker_img = allpoker_img.scaled(allpokerSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    } else {
        ui->serdataseq_edit->append("All Poker Load Error");
    }



    //---- Chips -----

    QSize chipSize(40,100);
    QString chips_path = "image/chip/";
    QString chips_path_a[5] = {"1_","2_","3_","4_","5_"};
    QString chips_path_b[9] = {"1","2","3","4","5","6","7","8","9"};
    QString chips_respath;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 9; j++) {
            chips_respath = chips_path + chips_path_a[i] + chips_path_b[j];
            if(chips_img[i][j].load(chips_respath)) {
                chips_img[i][j] = chips_img[i][j].scaled(chipSize,Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                ui->serdataseq_edit->append(chips_path_a[i] + chips_path_b[j] + "Chip Load Error");
            }
        }
    }

    if(blank_chip_img.load(chips_path + "blank_chip.png")) {
        blank_chip_img = blank_chip_img.scaled(chipSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    } else {
        ui->serdataseq_edit->append("Blank Chip Load Failed");
    }

    //--------------------- Load Ok ---------------------------



    pdisk->setBackgroundImg(&background_img);
    pdisk->setDiskImg(&playdisk_img);
    pdisk->setOwnHeadFrame(&own_headframe_img);
    pdisk->setRivHeadFrame(&riv_headframe_img);

    for(int i = 0; i < 5; i++) {
        pdisk->setOwnPokerSeq(i+1,&pokers_img[0][i]);
        pdisk->setRivPokerSeq(i+1,&pokers_img[1][i]);
    }

    pdisk->setRivPokerSeq(1,&fold_poker_img);
    pdisk->setAllPoker(&allpoker_img);

    pdisk->setOwnHeadFlash(&flashhead_img);
    pdisk->setRivHeadFlash(&flashhead_img);
    pdisk->setPubPoker(&fold_poker_img);

    //----- add task -----


    //----- add task -----

    con_thread->start();
    pdisk->show();



}

game_window::~game_window()
{
    delete ui;
}

void game_window::on_gameList_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug() << item->text();
}


void game_window::dealChips() {
    QString target_account = ui->dl_target->currentText();
    QString target_num = ui->dl_target->currentText();
    ui->serdataseq_edit->append(target_account + " " + target_num);

    ui->dl_target->setCurrentIndex(0);
    ui->dl_number->setText("");

}

void game_window::clearDealer() {
    ui->dl_target->setCurrentIndex(0);
    ui->dl_number->setText("");
}


void game_window::on_send_button_clicked()
{
    QString str = ui->sendedit->text();
    int value = str.toInt();
    pdisk->setOwnChipNumber(value,chips_img,&blank_chip_img);


}

void game_window::on_pushButton_3_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(1,2));
    mutex.unlock();


}

void game_window::startFlash(int va,int vb,int vc) {
    pdisk->startFlash(va,vb,vc);


}

void game_window::on_pushButton_4_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(1,1));
    mutex.unlock();


}

void game_window::stopFlash(int v) {
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(2,1));
    mutex.unlock();

}



void game_window::on_pushButton_5_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(2,1));
    mutex.unlock();
}

void game_window::on_pushButton_6_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(2,2));
    mutex.unlock();
}

void game_window::sendCard(int v) {
    pdisk->sendCard(v);


}

void game_window::on_pushButton_7_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(3,1));
    mutex.unlock();
}

void game_window::on_pushButton_8_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(3,2));
    mutex.unlock();
}




void game_window::on_pushButton_2_clicked()
{
    pdisk->show();
}

void game_window::on_startDeal_button_clicked()
{
    int value = ui->dl_number->text().toInt();
    pdisk->setRivChipNumber(value,chips_img,&blank_chip_img);

}

void game_window::on_getDealList_button_clicked()
{
    tlist->show();

}






void game_window::on_pushButton_clicked()
{
    mutex.lock();
    TASK_VEC.push_back(TASK_STRU(4,1));
    mutex.unlock();
}

void game_window::moveChip(int va, int vb) {
    pdisk->moveChip(va,vb);

}

void game_window::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);


    pdisk->~playdisk();
    trand->~tranDialog();
    tlist->~tranlist();
    con_thread->quit();


}


