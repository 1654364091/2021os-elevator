#ifndef BUILDING_H
#define BUILDING_H

#include "elevator.h"
#include <QWidget>
#include <QMetaType>

namespace Ui {
class building;
}

class building : public QWidget
{
    Q_OBJECT

public:
    explicit building(QWidget *parent = nullptr,int ele=5,int fl=20);
    ~building();

private:
    Ui::building *ui;

public:
    int ele_sum;  //电梯台数
    int floor_sum;//楼层总数
    vector<elevator*> elevators;       //存储楼内的电梯
    vector<QSlider*> elevator_position;//电梯位置“视图化”
    vector<QLabel*> elevator_current;  //电梯当前位置
    vector<QLabel*> elevator_direction;//电梯方向“视图化”
    vector<QPushButton*> up_btns;      //每层楼向上的按钮
    vector<QPushButton*> down_btns;    //每层楼向下的按钮
    QHBoxLayout* layout_direction;     //"方向"布局管理器
    QHBoxLayout* layout_position;      //"位置"布局管理器
    vector<QThread*> threads;          //为每个电梯分配一个单独的线程

    void selectElevator(int floor,int dir);//调度电梯
    void setPosition();

public slots:
    void sendRequest(int floor,int dir);//某一层发送上楼或下楼的请求
    void deleteRequest(goal fl); //删除已经完成的请求
    void draw();       //绘制相关控件的位置
    void refresh();    //刷新控件状态
    void removeTask(goal fl);//当某个电梯完成楼外任务时，发送已到达
    void reselect(int number);
};

#endif // BUILDING_H
