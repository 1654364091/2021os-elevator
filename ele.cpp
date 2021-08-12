#include "ele.h"

goal::goal(int a, int b)
{
    floor=a;
    dir=b;
};

ele::ele(QObject* par,int num )
{
    //初始化数据
    this->setParent(par);
    number=num;
    status=PAUSE;  //电梯初始为暂停态
    run_dir=PAUSE;
    door=CLOSE;    //门初始为关闭状态
    now_floor=1;   //初始在一楼
    floor_sum=20;  //楼层默认有20层
    alarm=0;       //默认正常

    //每1秒电梯移动一层
    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, &ele::moveOneFloor);
    timer2->start(1000);
}

ele::~ele()
{

}

void ele::open()
{
    if(status==PAUSE&&door==CLOSE)
    {
        door=OPEN;

        QElapsedTimer timer;
        timer.start();
        while(timer.elapsed() < 2000)QCoreApplication::processEvents();//不停处理事件，模拟多线程，防止程序假死
        close();
    }
}

void ele::close()
{
    if(status==PAUSE&&door==OPEN)
    {
        door=CLOSE;
    }
}

void ele::up()
{
    if(status==PAUSE&&door==CLOSE)
    {
        status=UP;
    }
}

void ele::down()
{
    if(status==PAUSE&&door==CLOSE)
    {
        status=DOWN;
    }
}

void ele::stop()
{
    status=PAUSE;
}

bool ele::ifContinue(int dest)
{
    bool inside=false;
    bool outside=false;//内外都要查看
    int i,j;
    for(i=0;i<target_floors_inside.size();i++)
    {
        if((dest==UP&&target_floors_inside[i]>now_floor)
          ||(dest==DOWN&&target_floors_inside[i]<now_floor))
            break;    //楼内还有任务
    }
    if(i<target_floors_inside.size())inside=true;  //返回“沿原方向”的信息

    for(j=0;j<target_floors_outside.size();j++)
    {
        if((dest==UP&&target_floors_outside[i].floor>now_floor)
          ||(dest==DOWN&&target_floors_outside[i].floor<now_floor))
            break;    //楼内还有任务
    }
    if(j<target_floors_outside.size())outside=true;  //返回“沿原方向”的信息

    return inside||outside;
}

void ele::addTargetInside(int floor)
{
    if(!find(target_floors_inside,floor)) //避免重复添加任务
        target_floors_inside.push_back(floor);
}

void ele::addTargetOutside(goal fl)
{
    goal temp(-1,-1);
    if(!find(target_floors_outside,fl.floor,temp)) //避免重复添加任务
        target_floors_outside.push_back(fl);
}

bool ele::find(vector<int> target,int floor)
{
    auto result=std::find(target.begin(),target.end(),floor);
    if(result==target.end())
        return false;  //没有这层楼
    return true;       //有这层楼
}

bool ele::find(vector<goal> target,int floor,goal& fl)
{
    for(int i=0;i<target.size();i++)
    {
        if(target[i].floor==floor)
        {
            fl=target[i];
            return true;//有这层楼
        }

    }
    return false;  //没有这层楼
}

void ele::deleteTarget(vector<int> *target,int floor)
{
    auto result=std::find(target->begin(),target->end(),floor);
    if(result!=target->end()) //找到了就删除任务
    {
        *result=target->back();
        target->pop_back();
    }
}

void ele::deleteTarget(vector<goal> *target,int floor)
{
    for(int i=0;i<(*target).size();i++)
    {
        if((*target)[i].floor==floor)
        {
            (*target)[i]=(*target).back();
            (*target).pop_back();
            return ;
        }
    }
}


void ele::moveOneFloor()
{
    if(!target_floors_inside.empty()||!target_floors_outside.empty())
    {
        status=run_dir;//当前方向

        goal fl(-1,-1);//存储楼外目标层的信息
        //检查当前楼层是否是目标层
        if(find(target_floors_inside,now_floor)||find(target_floors_outside,now_floor,fl))
        {
            stop(); //停下
            if(find(target_floors_outside,now_floor,fl))
                emit reachOutside(fl);//发送到达目标楼层的信号及刚才的行进方向
            if(find(target_floors_inside,now_floor))
                emit reachInside(now_floor);
            deleteTarget(&target_floors_outside,now_floor);
            deleteTarget(&target_floors_inside,now_floor);
            open(); //开门，一段时间后自动关闭
        }
        //判断电梯是否仍需沿原方向前进
        if(ifContinue(run_dir)==true)
        {
            QElapsedTimer timer;
            timer.start();
            while(timer.elapsed() < 1000)QCoreApplication::processEvents();
            now_floor+=run_dir;//当前位置更新
        }
        else
        {
            if(!target_floors_inside.empty()||!target_floors_outside.empty())
                run_dir*=-1;  //开始变向
            else run_dir=PAUSE;
        }
        status=run_dir;//恢复运行方向


    }

    else
    {
        status=PAUSE;  //电梯任务全部完成，暂停
        run_dir=PAUSE;
    }
}
