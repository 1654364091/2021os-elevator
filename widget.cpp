#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->move(62*4+401*3,520);//设置窗口位置

    buil_ele=new building(nullptr);
    start_btn=ui->pushButton_start;
    exit_btn=ui->pushButton_exit;

    connect(start_btn,&QPushButton::clicked,buil_ele,&QWidget::show);
    connect(start_btn,&QPushButton::clicked,start_btn,&QPushButton::setEnabled);
    for(int i=1;i<=5;i++)
    {
        connect(start_btn,&QPushButton::clicked,buil_ele->elevators[i],&QWidget::show);
    }

    connect(exit_btn,&QPushButton::clicked,buil_ele,&QWidget::close);
    connect(exit_btn,&QPushButton::clicked,this,&QWidget::close);
    for(int i=0;i<=5;i++)
    {
        connect(exit_btn,&QPushButton::clicked,buil_ele->elevators[i],&QWidget::close);
    }

}

Widget::~Widget()
{
    delete ui;
}
