#ifndef ADD_THREAD_H_
#define ADD_THREAD_H

#include <QThread>
#include <QLabel>
#include <QMainWindow>

class AddThread : public QThread {
    Q_OBJECT
signals:
    void message(int,int);

private:

public:
    AddThread(QObject* parent);
    ~AddThread();

    void run();
};

#endif
