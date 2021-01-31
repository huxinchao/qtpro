#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <QThread>
#include <QLabel>
#include <QMainWindow>

class CON_THREAD : public QThread {
    Q_OBJECT
private:
public:
    CON_THREAD(QObject* parent);
    ~CON_THREAD();

    void run();
signals:
    void startFlash(int,int,int);
    void sendCard(int);
    void moveChip(int,int);

};

#endif // CONTROL_THREAD_H
