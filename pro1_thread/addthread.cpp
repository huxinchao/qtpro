#include "addThread.h"
#include "windows.h"
#include <QLabel>
#include <QDebug>

AddThread::AddThread(QObject* parent) : QThread(parent) {}
AddThread::~AddThread() {
    qDebug() << "Exit Thread";
}

void AddThread::run() {
    for(int i = 200; i < 400; i++) {
        emit message(i,i);
        msleep(10);
    }
}




