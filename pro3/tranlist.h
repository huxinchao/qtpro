#ifndef TRANLIST_H
#define TRANLIST_H

#include <QMainWindow>

namespace Ui {
class tranlist;
}

class tranlist : public QMainWindow
{
    Q_OBJECT

public:
    explicit tranlist(QWidget *parent = nullptr);
    ~tranlist();

private:
    Ui::tranlist *ui;
};

#endif // TRANLIST_H
