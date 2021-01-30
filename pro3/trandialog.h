#ifndef TRANDIALOG_H
#define TRANDIALOG_H

#include <QDialog>

namespace Ui {
class tranDialog;
}

class tranDialog : public QDialog
{
    Q_OBJECT

public:
    explicit tranDialog(QWidget *parent = nullptr);
    QString target_account;
    QString     target_num;
    void setAccountInfo();
    ~tranDialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();


signals:
    void sendData();
    void sendClear();


private:
    Ui::tranDialog *ui;
};

#endif // TRANDIALOG_H
