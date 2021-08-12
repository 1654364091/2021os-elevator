#include "building.h"
#include "ui_building.h"

building::building(QWidget *parent,int ele,int fl) :
    QWidget(parent),ele_sum(ele),floor_sum(fl),
    ui(new Ui::building)
{
    ui->setupUi(this);
    this->setWindowTitle("各楼层电梯总览及电梯呼叫");

    qRegisterMetaType<goal>("goal");

    //创建电梯、电梯位置视图、电梯当前位置、电梯方向视图
    for(int i=0;i<=ele_sum;i++)
    {
        elevator * ele=new elevator(nullptr,i);
          connect(ele->el,&ele::reachOutside,this,&building::deleteRequest);//电梯每完成一个请求，该层的请求就被撤销
          connect(ele->el,&ele::reachOutside,this,&building::removeTask);
          connect(ele,&elevator::Alarm,this,&building::reselect);

          //加入到线程
          QThread* thread=new QThread;
          threads.push_back(thread);
          //connect(thread,&QThread::started,ele->el,[=]{ele->el->run();});
          ele->el->moveToThread(threads[i]);
          elevators.push_back(ele);
          threads[i]->start();

        QSlider* sli=new QSlider(ui->groupBox_elevators);
          sli->setMinimum(1);  //滑块最小值
          sli->setMaximum(floor_sum);//滑块最大值
          sli->setSingleStep(1);//一步最少走一格
          sli->setPageStep(1);
          elevator_position.push_back(sli);

        QLabel* lab=new QLabel(ui->groupBox_directions);
          lab->setText("---");
          lab->setAlignment(Qt::AlignHCenter);//居中对齐
          elevator_direction.push_back(lab);

        QLabel* current=new QLabel(ui->groupBox_elevators);
          current->setText("1");
          QFont ft;
          ft.setPointSize(16);  //设置字号
          current->setFont(ft);
          current->setAlignment(Qt::AlignHCenter);//居中对齐
          elevator_current.push_back(current);
    }
    //隐藏
    elevators[0]->close();
    elevator_position[0]->close();
    elevator_direction[0]->close();
    elevator_current[0]->close();


    //创建每层楼的上下楼按钮
    for(int i=0;i<=floor_sum;i++)
    {
        QPushButton* btn=new QPushButton(ui->groupBox_3);
          btn->setText("↑");
          connect(btn,&QPushButton::clicked,this,[=]{sendRequest(i,UP);});//点击后发送请求
          connect(btn,&QPushButton::clicked,btn,[=]{btn->setEnabled(false);}); //点击后变为不可触发态
          up_btns.push_back(btn);

        QPushButton* btn2=new QPushButton(ui->groupBox_3);
          btn2->setText("↓");
          connect(btn2,&QPushButton::clicked,this,[=]{sendRequest(i,DOWN);});//点击后发送请求
          connect(btn2,&QPushButton::clicked,btn2,[=]{btn2->setEnabled(false);});   //点击后变为不可触发态
          down_btns.push_back(btn2);
    }
    //隐藏
    up_btns[0]->close();
    down_btns[0]->close();
    up_btns[20]->close();
    down_btns[1]->close();

    layout_direction=new QHBoxLayout(ui->groupBox_directions);//创建布局管理器

    draw();
    setPosition();

    //每0.1秒刷新一次相关显示
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &building::refresh);
    timer->start(100);
}

building::~building()
{
    delete ui;
    //释放电梯vector
    for (int i = 0; i < elevators.size(); ++i)
    {
        delete elevators[i];
        elevators[i] = NULL;
    }
    elevators.clear();
}

void building::sendRequest(int floor, int dir)
{
    selectElevator(floor,dir);//将这个请求分配给合适的电梯
}

void building::selectElevator(int floor,int dir)
{
    bool has_selected=false;
    for(int i=1;i<=5;i++)//检查五个电梯，同向迎来的和空闲的均对请求进行响应
    {
        if(dir==UP)
        {
            if(elevators[i]->el->alarm==0&&elevators[i]->el->run_dir==dir&&elevators[i]->el->now_floor<=floor)
            {
                elevators[i]->el->addTargetOutside(goal(floor,dir));   //加入到来自电梯外的请求
                has_selected=true;  //
            }
            else if(elevators[i]->el->alarm==0&&elevators[i]->el->target_floors_inside.empty()&&elevators[i]->el->target_floors_outside.empty())
            {
                if(elevators[i]->el->now_floor<=floor)
                    elevators[i]->el->run_dir=UP;
                else elevators[i]->el->run_dir=DOWN;
                elevators[i]->el->addTargetOutside(goal(floor,dir));
                has_selected=true;
            }
        }
        else if(dir==DOWN)
        {
            if(elevators[i]->el->alarm==0&&elevators[i]->el->run_dir==dir&&elevators[i]->el->now_floor>=floor)
            {
                elevators[i]->el->addTargetOutside(goal(floor,dir));
                has_selected=true;
            }
            else if(elevators[i]->el->alarm==0&&elevators[i]->el->target_floors_inside.empty()&&elevators[i]->el->target_floors_outside.empty())
            {
                if(elevators[i]->el->now_floor<=floor)
                    elevators[i]->el->run_dir=UP;
                else elevators[i]->el->run_dir=DOWN;
                elevators[i]->el->addTargetOutside(goal(floor,dir));
                has_selected=true;
            }
        }
    }

    if(has_selected==false)
    {
        while(1)
        {
            int number=rand()%5+1;
            if(elevators[number]->el->alarm==0)
            {
                elevators[number]->el->addTargetOutside(goal(floor,dir));
                break;
            }
        }
    }
}

void building::deleteRequest(goal fl)
{
    if(fl.dir==UP)
    {
            up_btns[fl.floor]->setEnabled(true);
    }
    else
    {
            down_btns[fl.floor]->setEnabled(true);
    }
}

void building::setPosition()
{
    int i=0;
    for(i=1;i<=ele_sum-1;i++)
    {
        elevators[i]->move(62*i+401*(i-1),20);
    }
    elevators[i]->move(62,520);
    this->move(62*2+401,520);
}

void building::draw()
{
    //方向、位置
    for(int i=1;i<=ele_sum;i++)
    {
        layout_direction->addWidget(elevator_direction[i]);

        elevator_position[i]->setGeometry(25+68*(i-1),30,20,270);
        elevator_current[i]->setGeometry(25+68*(i-1),305,20,20);
    }
    layout_direction->setSpacing(20);

    //按钮
    for(int i=1;i<=4;i++)
    {
        for(int j=1;j<=5;j++)
        {
            up_btns[(i-1)*5+j]->setGeometry(30+85*(j-1),50+100*(i-1),40,30);
            down_btns[(i-1)*5+j]->setGeometry(30+85*(j-1),90+100*(i-1),40,30);
        }
    }
}

void building::refresh()
{
    for(int i=1;i<=5;i++)
    {
        elevator_position[i]->setValue(elevators[i]->el->now_floor);//刷新滑块位置
        elevator_current[i]->setText(QString::number(elevators[i]->el->now_floor));//刷新电梯当前位置
        if(elevators[i]->el->status==UP)
            elevator_direction[i]->setText("↑");
        else if(elevators[i]->el->status==DOWN)
            elevator_direction[i]->setText("↓");
        else
            elevator_direction[i]->setText("---");
    }
}

void building::removeTask(goal fl)
{
    for(int i=1;i<=5;i++)
    {
        for(int j=0;j<elevators[i]->el->target_floors_outside.size();j++)
        {
            if(elevators[i]->el->target_floors_outside[j].floor==fl.floor
               &&elevators[i]->el->target_floors_outside[j].dir==fl.dir)
            {
                elevators[i]->el->target_floors_outside[j]=elevators[i]->el->target_floors_outside.back();
                elevators[i]->el->target_floors_outside.pop_back();
            }
        }
    }
}

void building::reselect(int number)
{
    for(int i=0;i<elevators[number]->el->target_floors_outside.size();i++)
    {
        emit elevators[number]->el->reachOutside(elevators[number]->el->target_floors_outside[i]);
    }
    elevators[number]->el->target_floors_outside.clear();
}
