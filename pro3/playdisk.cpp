#include "playdisk.h"
#include "ui_playdisk.h"
#include <QPixmap>
#include <QIcon>
#include <vector>

playdisk::playdisk(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::playdisk)
{
    ui->setupUi(this);

    OwnPokersList[0] = ui->own_card_1;
    OwnPokersList[1] = ui->own_card_2;
    OwnPokersList[2] = ui->own_card_3;
    OwnPokersList[3] = ui->own_card_4;
    OwnPokersList[4] = ui->own_card_5;

    RivPokersList[0] = ui->riv_card_1;
    RivPokersList[1] = ui->riv_card_2;
    RivPokersList[2] = ui->riv_card_3;
    RivPokersList[3] = ui->riv_card_4;
    RivPokersList[4] = ui->riv_card_5;

    OwnChipsList[0] = ui->own_cl_1;
    OwnChipsList[1] = ui->own_cl_2;
    OwnChipsList[2] = ui->own_cl_3;
    OwnChipsList[3] = ui->own_cl_4;
    OwnChipsList[4] = ui->own_cl_5;

    RivChipsList[0] = ui->riv_cl_1;
    RivChipsList[1] = ui->riv_cl_2;
    RivChipsList[2] = ui->riv_cl_3;
    RivChipsList[3] = ui->riv_cl_4;
    RivChipsList[4] = ui->riv_cl_5;

    PotChipsList[0] = ui->pot_cl_1;
    PotChipsList[1] = ui->pot_cl_2;
    PotChipsList[2] = ui->pot_cl_3;
    PotChipsList[3] = ui->pot_cl_4;
    PotChipsList[4] = ui->pot_cl_5;



    ui->own_bar->hide();
    ui->riv_bar->hide();
    ui->own_flashhead->hide();
    ui->riv_flashhead->hide();

    for(int i = 0; i < 5; i++) {
        OwnPokersList[i]->hide();
        RivPokersList[i]->hide();
        OwnChipsList[i]->hide();
        RivChipsList[i]->hide();
        PotChipsList[i]->hide();
    }

    ui->own_underchip_label->hide();
    ui->riv_underchip_label->hide();

    //ui->pub_card->hide();












}

playdisk::~playdisk()
{
    delete ui;
}



void playdisk::on_pushButton_call_clicked()
{



}

void playdisk::on_pushButton_fold_clicked()
{
   emit stopFlash(1);


}




void playdisk::setBackgroundImg(QPixmap* img) {
    ui->background_lb->setPixmap(*img);
}

void playdisk::setDiskImg(QPixmap* img) {
    ui->label_disk->setPixmap(*img);

}

void playdisk::setOwnHeadFrame(QPixmap* img) {
    ui->headw_own->setPixmap(*img);
}

void playdisk::setRivHeadFrame(QPixmap* img) {
    ui->headw_rival->setPixmap(*img);

}

void playdisk::setOwnPokerSeq(int seq, QPixmap* pokerimg) {
    OwnPokersList[seq-1]->setPixmap(*pokerimg);

}

void playdisk::setRivPokerSeq(int seq, QPixmap* pokerimg) {
    RivPokersList[seq-1]->setPixmap(*pokerimg);
}

void playdisk::setOwnChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip) {
    ui->own_underchip_label->setText(QString::number(number) + " B");
    ui->own_underchip_label->setGeometry(840,510,80,15);
    ui->own_underchip_label->show();

    struct  CHIPSTR {
        int i;
        int n;
        CHIPSTR(int _i, int _n) :i(_i), n(_n) {}
    };

    std::vector<int> bitvec;
    std::vector<CHIPSTR> chipvec;

    while(number > 0) {
        int bit = number % 10;
        bitvec.push_back(bit);
        number /= 10;
    }

    for(int i = 0; i < bitvec.size() / 2; i++) {
        std::swap(bitvec[i], bitvec[bitvec.size() - i - 1]);
    }

    int beginPaint = 5 - bitvec.size();
    for(int i = 0; i < beginPaint; i++) {
        OwnChipsList[i]->setPixmap(*bchip);
    }
    for(int i = 0, j = bitvec.size() - 1; beginPaint < 5; beginPaint++,i++,j--) {
        //ui->recv_edit->append(QString::number(beginPaint) + " " + QString::number(j) + " " + QString::number(bitvec[i]));
        if(bitvec[i] == 0) {
            OwnChipsList[beginPaint]->setPixmap(*bchip);
        }else {
            chipvec.push_back(CHIPSTR(beginPaint,bitvec[i]));
            OwnChipsList[beginPaint]->setPixmap(chipImg[j][bitvec[i]-1]);
        }
    }

    ui->recv_edit->append(QString::number(chipvec.size()));

    for(int i = 0; i < 5; i++) {
        OwnChipsList[i]->show();
    }


    if(chipvec.size() == 1) {
        OwnChipsList[chipvec[0].i]->setGeometry(861,390,40,140);
    }
    if(chipvec.size() == 2) {
        OwnChipsList[chipvec[0].i]->setGeometry(840,390,40,140);
        OwnChipsList[chipvec[1].i]->setGeometry(882,390,40,140);
    }
    if(chipvec.size() == 3) {
        OwnChipsList[chipvec[0].i]->setGeometry(840,390,40,140);
        OwnChipsList[chipvec[1].i]->setGeometry(840,390-5*chipvec[0].n,40,140);
        OwnChipsList[chipvec[2].i]->setGeometry(882,390,40,140);
    }
    if(chipvec.size() == 4) {
        OwnChipsList[chipvec[0].i]->setGeometry(840,390,40,140);
        OwnChipsList[chipvec[1].i]->setGeometry(840,390-5*chipvec[0].n,40,140);
        OwnChipsList[chipvec[2].i]->setGeometry(882,390,40,140);
        OwnChipsList[chipvec[3].i]->setGeometry(882,390-5*chipvec[2].n,40,140);
    }
    if(chipvec.size() == 5) {
        OwnChipsList[chipvec[0].i]->setGeometry(840,390,40,140);
        OwnChipsList[chipvec[1].i]->setGeometry(840,390-5*chipvec[0].n,40,140);
        OwnChipsList[chipvec[2].i]->setGeometry(840,390-5*(chipvec[0].n+chipvec[1].n),40,140);
        OwnChipsList[chipvec[3].i]->setGeometry(882,390,40,140);
        OwnChipsList[chipvec[4].i]->setGeometry(882,390-5*(chipvec[3].n),40,140);


    }



}

void playdisk::setRivChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip) {
    ui->riv_underchip_label->setText(QString::number(number) + " B");
    ui->riv_underchip_label->setGeometry(320,240,80,15);
    ui->riv_underchip_label->show();

    struct  CHIPSTRU {
        int i;
        int n;
        CHIPSTRU(int _i, int _n) :i(_i), n(_n) {}
    };

    std::vector<int> bitvec;
    std::vector<CHIPSTRU> chipvec;

    while(number > 0) {
        int bit = number % 10;
        bitvec.push_back(bit);
        number /= 10;
    }

    for(int i = 0; i < bitvec.size() / 2; i++) {
        std::swap(bitvec[i], bitvec[bitvec.size() - i - 1]);
    }

    int beginPaint = 5 - bitvec.size();
    for(int i = 0; i < beginPaint; i++) {
        RivChipsList[i]->setPixmap(*bchip);
    }
    for(int i = 0, j = bitvec.size() - 1; beginPaint < 5; beginPaint++,i++,j--) {
        //ui->recv_edit->append(QString::number(beginPaint) + " " + QString::number(j) + " " + QString::number(bitvec[i]));
        if(bitvec[i] == 0) {
            RivChipsList[beginPaint]->setPixmap(*bchip);
        }else {
            chipvec.push_back(CHIPSTRU(beginPaint,bitvec[i]));
            RivChipsList[beginPaint]->setPixmap(chipImg[j][bitvec[i]-1]);
        }
    }

    ui->recv_edit->append(QString::number(chipvec.size()));

    for(int i = 0; i < 5; i++) {
        RivChipsList[i]->show();
    }
    if(chipvec.size() == 1) {
        RivChipsList[chipvec[0].i]->setGeometry(341,120,40,140);
    }
    if(chipvec.size() == 2) {
        RivChipsList[chipvec[0].i]->setGeometry(320,120,40,140);
        RivChipsList[chipvec[1].i]->setGeometry(362,120,40,140);
    }
    if(chipvec.size() == 3) {
        RivChipsList[chipvec[0].i]->setGeometry(320,120,40,140);
        RivChipsList[chipvec[1].i]->setGeometry(320,120-5*chipvec[0].n,40,140);
        RivChipsList[chipvec[2].i]->setGeometry(362,120,40,140);
    }
    if(chipvec.size() == 4) {
        RivChipsList[chipvec[0].i]->setGeometry(320,120,40,140);
        RivChipsList[chipvec[1].i]->setGeometry(320,120-5*chipvec[0].n,40,140);
        RivChipsList[chipvec[2].i]->setGeometry(362,120,40,140);
        RivChipsList[chipvec[3].i]->setGeometry(362,120-5*chipvec[2].n,40,140);
    }
    if(chipvec.size() == 5) {
        RivChipsList[chipvec[0].i]->setGeometry(320,120,40,140);
        RivChipsList[chipvec[1].i]->setGeometry(320,120-5*chipvec[0].n,40,140);
        RivChipsList[chipvec[2].i]->setGeometry(320,120-5*(chipvec[0].n+chipvec[1].n),40,140);
        RivChipsList[chipvec[3].i]->setGeometry(362,120,40,140);
        RivChipsList[chipvec[4].i]->setGeometry(362,120-5*(chipvec[3].n),40,140);
    }
}

void playdisk::setPotChipNumber(int number, QPixmap chipImg[5][9], QPixmap* bchip) {
    ui->pot_underchip_label->setText(QString::number(number) + " B");


    struct  CHIPSTR {
        int i;
        int n;
        CHIPSTR(int _i, int _n) :i(_i), n(_n) {}
    };

    std::vector<int> bitvec;
    std::vector<CHIPSTR> chipvec;

    while(number > 0) {
        int bit = number % 10;
        bitvec.push_back(bit);
        number /= 10;
    }

    for(int i = 0; i < bitvec.size() / 2; i++) {
        std::swap(bitvec[i], bitvec[bitvec.size() - i - 1]);
    }

    int beginPaint = 5 - bitvec.size();
    for(int i = 0; i < beginPaint; i++) {
        PotChipsList[i]->setPixmap(*bchip);
    }
    for(int i = 0, j = bitvec.size() - 1; beginPaint < 5; beginPaint++,i++,j--) {
        //ui->recv_edit->append(QString::number(beginPaint) + " " + QString::number(j) + " " + QString::number(bitvec[i]));
        if(bitvec[i] == 0) {
            PotChipsList[beginPaint]->setPixmap(*bchip);
        }else {
            chipvec.push_back(CHIPSTR(beginPaint,bitvec[i]));
            PotChipsList[beginPaint]->setPixmap(chipImg[j][bitvec[i]-1]);
        }
    }
    /*
     * 633 270
     *  */

    if(chipvec.size() == 1) {
        PotChipsList[chipvec[0].i]->setGeometry(633,270,40,140);
        PotChipsList[chipvec[0].i]->show();
    }
    if(chipvec.size() == 2) {
        int x = 612;
        for(int i = 0; i < 2; i++,x+=42) {
            PotChipsList[chipvec[i].i]->setGeometry(x,270,40,140);
            PotChipsList[chipvec[i].i]->show();
        }
    }
    if(chipvec.size() == 3) {
        int x = 591;
        for(int i = 0; i < 3; i++, x+=42) {
            PotChipsList[chipvec[i].i]->setGeometry(x,270,40,140);
            PotChipsList[chipvec[i].i]->show();
        }
    }
    if(chipvec.size() == 4) {
        int x = 571;
        for(int i = 0; i < 4; i++, x+=42) {
            PotChipsList[chipvec[i].i]->setGeometry(x,270,40,140);
            PotChipsList[chipvec[i].i]->show();
        }
    }
    if(chipvec.size() == 5) {
        int x = 549;
        for(int i = 0; i < 5; i++, x+=42) {
            PotChipsList[chipvec[i].i]->setGeometry(x,270,40,140);
            PotChipsList[chipvec[i].i]->show();
        }

    }


    ui->recv_edit->append(QString::number(chipvec.size()));




}

void playdisk::setAllPoker(QPixmap* apimg) {
    ui->allpoker_label->setPixmap(*apimg);
}

void playdisk::setOwnHeadFlash(QPixmap *img) {
    ui->own_flashhead->setPixmap(*img);
}

void playdisk::setRivHeadFlash(QPixmap *img) {
    ui->riv_flashhead->setPixmap(*img);
}



void playdisk::startFlash(int va,int vb,int vc) {
    if(va == 1) {
        if(vb == 1) {

            ui->own_bar->setValue(999);

            ui->own_flashhead->show();
            ui->own_bar->show();

        }
        if(vb == 2) {

            ui->riv_bar->setValue(999);

            ui->riv_flashhead->show();
            ui->riv_bar->show();

        }
    }

    if(va == 2) {
        if(vc == 1) {
            if(vb == 1) {
                ui->own_bar->setValue(ui->own_bar->value()-1);
                if(ui->own_flashhead->isHidden()) {
                    ui->own_flashhead->show();
                } else {
                    ui->own_flashhead->hide();
                }
            }
            if(vb == 2) {
                ui->riv_bar->setValue(ui->riv_bar->value()-1);
                if(ui->riv_flashhead->isHidden()) {
                    ui->riv_flashhead->show();
                } else {
                    ui->riv_flashhead->hide();
                }
            }
        }
        if(vb == 1 && vc == 2) {
            ui->own_bar->setValue(ui->own_bar->value()-1);
        }
        if(vb == 2 && vc == 2) {
            ui->riv_bar->setValue(ui->riv_bar->value()-1);
        }
    }

    if(va == 3) {

        if(vb == 1) {
            ui->own_bar->hide();
            ui->own_flashhead->hide();
        }
        if(vb == 2) {
            ui->riv_bar->hide();
            ui->riv_flashhead->hide();
        }
    }

}

void playdisk::setPubPoker(QPixmap *blimg) {
    ui->pub_card->setPixmap(*blimg);
}

void playdisk::sendCard(int v) {

        if(ui->pub_card->isHidden()) {
            ui->pub_card->setGeometry(340,305,70,100);
            ui->pub_card->show();
        }


        if(v == 1) {
            /*
             * 340 305
             * Target Own: 390 355
             * Target Riv: 390 255
             * */

            ui->pub_card->setGeometry(ui->pub_card->geometry().x() + 1, ui->pub_card->geometry().y() + 1,70,100);


        }
        if(v == 2) {

            ui->pub_card->setGeometry(ui->pub_card->geometry().x() + 1, ui->pub_card->geometry().y() - 1,70,100);

        }
        if(v == 3) {
            ui->pub_card->hide();
        }
}

void playdisk::moveChip(int va, int vb) {

    if(va == 1) {
        int x,y;

        for(int i = 0; i < 5; i++) {
                x = OwnChipsList[i]->geometry().x();
                y = OwnChipsList[i]->geometry().y();
                OwnChipsList[i]->setGeometry(x-2,y-2,40,140);



                x = RivChipsList[i]->geometry().x();
                y = RivChipsList[i]->geometry().y();
                RivChipsList[i]->setGeometry(x+2,y+2,40,140);





        }
        x = ui->own_underchip_label->geometry().x();
        y = ui->own_underchip_label->geometry().y();
        ui->own_underchip_label->setGeometry(x-2,y-2,80,15);

        x = ui->riv_underchip_label->geometry().x();
        y = ui->riv_underchip_label->geometry().y();
        ui->riv_underchip_label->setGeometry(x+2,y+2,80,15);



    }
    if(va == 3) {

        for(int i = 0; i < 5; i++) {
            OwnChipsList[i]->hide();
            RivChipsList[i]->hide();
            ui->own_underchip_label->hide();
            ui->riv_underchip_label->hide();
        }

    }
}






void playdisk::on_raise_slider_valueChanged(int value)
{

}
