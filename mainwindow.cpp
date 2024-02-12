#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint); //设置窗口无边框
    setFixedSize(width(),height());  //设置固定窗口大小
    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();

    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);

    connect(mExitAct,QAction::triggered,this,[=]{
        qApp->exit(0);
    });
}

MainWindow::~MainWindow()
{
    delete ui;


}
//重写父类的虚函数
//父类中默认的实现，是忽略右键菜单事件  重写就可以处理右键菜单事件
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());

    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
 moffset = event->globalPos() - this->pos();

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
 this->move(event->globalPos()-moffset);
}

