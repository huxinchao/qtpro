#ifndef TASK_STRU_H
#define TASK_STRU_H

/*----------------------------------
 * ID:1 "Start Flash Headframe" TARGET(1,2) (Own,Riv)
 * ID:2 "Stop Flash  Headframe" TARGET(1,2) (Own,Riv)
 * ID:3 "Start Send Card"       TARGET(1,2) (Own,Riv)
 * ID:4 "Move Chip To Desk"     TARGET(1,2) (Own,Riv)
 * ID:5 "Move Desk Chip To P"   TARGET(1,2) (Own,Riv)
-----------------------------------*/
struct TASK_STRU {
    int id;
    int target;
    TASK_STRU(int _id, int _target) : id(_id), target(_target) {}
};

#endif // TASK_STRU_H
