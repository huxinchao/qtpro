#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QDebug>
#include "trandialog.h"
#include "playdisk.h"
#include "control_thread.h"
#include "tranlist.h"

namespace Ui {
class game_window;
}

class game_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit game_window(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent* event);
    tranDialog* trand;
    playdisk* pdisk;
    tranlist* tlist;

    QPixmap background_img;
    QPixmap pokers_img[4][13];
    QPixmap blank_poker_img;
    QPixmap fold_poker_img;
    QPixmap chips_img[5][9];
    QPixmap blank_chip_img;
    QPixmap riv_headframe_img;
    QPixmap own_headframe_img;
    QPixmap flashhead_img;
    QPixmap playdisk_img;
    QPixmap allpoker_img;

    CON_THREAD* con_thread;






    ~game_window();

private slots:


    void on_gameList_itemDoubleClicked(QListWidgetItem *item);

    void on_startDeal_button_clicked();

    void dealChips();
    void clearDealer();

    void on_send_button_clicked();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();
    void startFlash(int,int,int);
    void stopFlash(int);
    void sendCard(int);
    void moveChip(int,int);



    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_2_clicked();



    void on_getDealList_button_clicked();

    void on_pushButton_clicked();

private:
    Ui::game_window *ui;






};



#endif // GAME_WINDOW_H
