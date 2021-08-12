#include "elevator.h"
#include "ui_elevator.h"

elevator::elevator(QWidget *parent,int num) :
    QWidget(parent),
    ui(new Ui::elevator)
{
    ui->setupUi(this);

    el=new ele(nullptr,num);
    //初始化数据
    this->setWindowTitle(QString::number(el->number)+"号电梯");
    door_status=ui->label_door_status;//门开关状态的标签
    door_status->setText("门已关闭");
    floor=ui->label_current_floor; //当前楼层展示
    floor->setText(QString::number(el->now_floor)+"楼");
    direction=ui->label_status;
    direction->setText("---");


    //创建20个楼层按钮
    for(int i=0;i<=20;i++)
    {
        QPushButton *btn=new QPushButton(ui->groupBox_floor);
        btn->setText(QString::number(i));
        //connect(btn,&QPushButton::clicked,el,[=]{emit el->addInside();});
        //创建联系，让点击后的楼层立刻加入到目标楼层队伍，并且不可再点击
        connect(btn, &QPushButton::clicked,
                this, [=] {el->addTargetInside(i);
                           floor_btns[i]->setEnabled(false);
                           if((el->target_floors_inside.size()+el->target_floors_outside.size())==1)
                           {
                               //确定电梯方向
                               int t=i-el->now_floor;
                               if(t!=0)el->run_dir=t/abs(t);
                               else el->run_dir=PAUSE;
                               //el->run();
                           }
                          });
        floor_btns.push_back(btn);
    }
    connect(el,&ele::reachInside,this,&elevator::renew_btn);//到达某楼层时恢复楼层按钮
    floor_btns[0]->setVisible(false);//隐藏第0层

    open_door=ui->btn_open; //开门按钮,和开门程序连接
      connect(open_door,&QPushButton::clicked,el,&ele::open);
    close_door=ui->btn_close; //关门按钮，和关门程序连接
      connect(close_door,&QPushButton::clicked,el,&ele::close);
    alarm=ui->btn_call;     //警报按钮
      connect(alarm,&QPushButton::clicked,this,&elevator::alarming);

    //创建布局管理器
    for(int i=0;i<4;i++)
    {
        QHBoxLayout* lay=new QHBoxLayout(); //托管给电梯楼层选择窗口
        HLayouts.push_back(lay);
    }
    VLayout=new QVBoxLayout(ui->groupBox_floor);//托管给电梯楼层选择窗口

    //绘制按钮
    drawBtns();

    //每0.1秒刷新一次相关显示
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &elevator::refresh);
    timer->start(100);
}

elevator::~elevator()
{
    delete ui;
}

void elevator::drawBtns()
{
    for(int i=0;i<4;i++)
    {
        for(int j=1;j<=5;j++) //将连续的5个按钮加入到横向布局管理器
        {
            HLayouts[i]->addWidget(floor_btns[i*5+j]);
            HLayouts[i]->setSpacing(30);
        }
        VLayout->addLayout(HLayouts[i]); //横向管理器嵌套纵向管理器中
    }
    VLayout->setSpacing(20);
}

void elevator::refresh()
{
    //电梯状态
    if(el->status==UP)
        direction->setText("↑");
    else if(el->status==DOWN)
        direction->setText("↓");
    else
        direction->setText("---");

    //电梯楼层显示
    floor->setText(QString::number(el->now_floor)+"楼");

    //电梯门的状态
    if(el->door==OPEN)
        door_status->setText("门已开启");
    else
        door_status->setText("门已关闭");
}

void elevator::renew_btn(int floor)
{
    floor_btns[floor]->setEnabled(true);
}

void elevator::alarming()
{
    emit Alarm(el->number);
    el->alarm=1;
    el->stop();
    el->run_dir=PAUSE;
    el->target_floors_inside.clear();

    for(int i=1;i<=20;i++)
    {
        floor_btns[i]->setEnabled(false);
    }
    open_door->setEnabled(false);
    close_door->setEnabled(false);

}
