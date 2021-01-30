#include <windows.h>
#include <QDebug>
#include <QMutex>
#include <vector>
#include "control_thread.h"
#include "playdisk.h"
#include "task_stru.h"

CON_THREAD::CON_THREAD(QObject* parent) : QThread(parent){}
CON_THREAD::~CON_THREAD() {}

extern QMutex mutex;
extern std::vector<TASK_STRU> TASK_VEC;

void CON_THREAD::run() {

    TASK_STRU task(0,0);
    std::vector<TASK_STRU> local_taskvec;
    while(1) {
        mutex.lock();
        if(!TASK_VEC.empty()) {
            local_taskvec.push_back(TASK_VEC.back());
            TASK_VEC.pop_back();
        }
        mutex.unlock();

        if(!local_taskvec.empty()) {
            task = local_taskvec.back();
            local_taskvec.pop_back();
        }

        if(task.id == 1) {
            emit startFlash(1,task.target,0);
            int i = 0;
            int time = 1000;
            int mid_closed = 0;
            while(time > 0) {
                i++;
                time--;
                if(i == 10) {
                    emit startFlash(2,task.target,1);
                    i = 0;
                } else {
                    emit startFlash(2,task.target,2);
                }

                mutex.lock();
                if(!TASK_VEC.empty()) {


                    TASK_STRU tmp_task = TASK_VEC.back();
                    TASK_VEC.pop_back();
                    if(tmp_task.id == 2 && tmp_task.target == task.target) {
                        emit startFlash(3,tmp_task.target,0);
                        mid_closed = 1;
                        mutex.unlock();
                        break;
                    } else {
                        if(tmp_task.id != 2) {
                            local_taskvec.push_back(tmp_task);
                        }
                    }
                }
                mutex.unlock();
                msleep(50);
            }
            if(!mid_closed) {
                emit startFlash(3,task.target,0);
            }


        }

        task.id=0;
        task.target=0;
    }




}




