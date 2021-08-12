#ifndef ELE_THREAD_H
#define ELE_THREAD_H

#include <Qthread>
#include "elevator.h"

class ele_thread:public QThread
{
public:
    elevator* ele;
    int dir;

    void create(QWidget* par=nullptr,int num=0);
    void run();
};


void ele_thread::create(QWidget* par,int num)
{
    ele=new elevator(par,num);
}
void ele_thread::run()
{
    ele->run(dir);
}

#endif // ELE_THREAD_H
