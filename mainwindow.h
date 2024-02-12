#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
private:
    QMenu* mExitMenu;  // 右键退出的菜单

    QAction* mExitAct; //退出的行为 - 菜单项

    QPoint moffset;  //窗口移动时左上角的偏移

protected:
     void contextMenuEvent(QContextMenuEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
 private:
     QString str; //
    // QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
};
#endif // MAINWINDOW_H
