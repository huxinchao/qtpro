#ifndef PLAYDISK_H
#define PLAYDISK_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class playdisk;
}

class playdisk : public QMainWindow
{
    Q_OBJECT

public:
    /*ui->background_lb->setPixmap(background_img);
    ui->label_disk->setPixmap(playdisk_img);
    ui->headw_own->setPixmap(own_headframe_img);
    ui->headw_rival->setPixmap(riv_headframe_img);*/
    explicit playdisk(QWidget *parent = nullptr);
    void setBackgroundImg(QPixmap* img);
    void setDiskImg(QPixmap* img);

    void setOwnHeadFrame(QPixmap* img);
    void setOwnHeadFlash(QPixmap* img);
    void setRivHeadFrame(QPixmap* img);
    void setRivHeadFlash(QPixmap* img);


    void setOwnPokerSeq(int seq, QPixmap* pokerimg);
    void setRivPokerSeq(int seq, QPixmap* pokerimg);

    void setOwnChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip);
    void setRivChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip);
    void setPotChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip);

    void setAllPoker(QPixmap* apimg);
    void setPubPoker(QPixmap* blimg);

    void startFlash(int va, int vb,int vc);
    void sendCard(int v);
    void moveChip(int,int);



    ~playdisk();

private:
    QLabel* OwnPokersList[5];
    QLabel* RivPokersList[5];
    QLabel* OwnChipsList[5];
    QLabel* RivChipsList[5];
    QLabel* PotChipsList[5];

signals:
    void stopFlash(int);


private slots:
    void on_pushButton_call_clicked();
    void on_pushButton_fold_clicked();




    void on_raise_slider_valueChanged(int value);

public:
    Ui::playdisk *ui;
};

#endif // PLAYDISK_H
