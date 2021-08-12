#ifndef ELE_H
#define ELE_H

#define UP 1
#define DOWN -1
#define PAUSE 0
#define OPEN 1
#define CLOSE 0

#include <QObject>
#include <iostream>
#include <vector>
#include <algorithm>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QTimer>


using std::vector;

class goal
{
public:
    int floor;
    int dir;
    goal(int a=-1,int b=-1);
};

class ele:public QObject
{
    Q_OBJECT
public:
    int number;       //电梯序号
    int status;       //电梯状态。0：暂停，1：向上运行，-1：向下运行
    int run_dir;      //电梯当前运行方向
    int door;         //电梯门的状态。0：关闭，1：开启
    int now_floor;    //电梯当前所在楼层
    int floor_sum;    //楼层总数
    int alarm;        //警报状态
    vector<int> target_floors_inside;  //电梯内的目标楼层队列
    vector<goal> target_floors_outside; //电梯外的目标楼层队列

    ele(QObject *parent=nullptr,int num=0);
    ~ele();

signals:
    void reachOutside(goal fl);//电梯发送已经到达某楼层和运行方向的信号
    void reachInside(int floor);
    void addInside();

public slots:
    void open();  //电梯开门
    void close(); //电梯关门
    void up();    //电梯向上运行
    void down();  //电梯向下运行
    //void run();   //电梯开始沿依次到达目标楼层
    void stop();  //电梯暂停
    bool ifContinue(int dest); //判断是否变换运行方向
    void addTargetInside(int floor); //添加电梯内的楼层
    void addTargetOutside(goal fl);//添加电梯外的楼层
    bool find(vector<int> target,int floor);  //查看任务列表内是否有某层楼
    bool find(vector<goal> target,int floor,goal& fl);  //查看任务列表内是否有某层楼
    void deleteTarget(vector<int> *target,int floor);//删除任务
    void deleteTarget(vector<goal> *target,int floor);//删除任务
    void moveOneFloor();  //电梯移动一步————后面用于每秒自动调用
};



#endif // ELE_H
