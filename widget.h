#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "building.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

public:
    QPushButton* start_btn;//开始按钮
    QPushButton* exit_btn;  //结束按钮
    building* buil_ele;    //电梯楼模型

};
#endif // WIDGET_H
