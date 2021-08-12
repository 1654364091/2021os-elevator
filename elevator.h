#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QWidget>
#include <QMessageBox>
#include <QGroupBox>
#include <QPushButton>
#include <QElapsedTimer>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QThread>
#include <QTimer>

#include "ele.h"

namespace Ui {
class elevator;
}

class elevator : public QWidget
{
    Q_OBJECT

public:
    explicit elevator(QWidget *parent = nullptr,int num=0);
    ~elevator();

private:
    Ui::elevator *ui;

public:
    ele * el;
    vector<QHBoxLayout*> HLayouts;   //(4个)行布局管理器
    QVBoxLayout* VLayout;      //一个列布局管理器
    vector<QPushButton*> floor_btns;//用来表示电梯内部的楼层按钮
    QPushButton* open_door;    //开门按钮
    QPushButton* close_door;   //关门按钮
    QPushButton* alarm;        //警报按钮
    QLabel* door_status;  //电梯门的开关状态
    QLabel* floor;        //当前所在楼层的展示
    QLabel* direction;    //当先运行方向的显示

signals:
    void Alarm(int number);//发送某号电梯报警的消息。

public slots:

    void drawBtns();     //摆放楼梯内楼层序号按钮的位置
    void refresh();      //刷新控件
    void renew_btn(int floor);
    void alarming();     //警报
};

#endif // ELEVATOR_H
