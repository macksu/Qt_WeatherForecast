#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "WeatherData.h"


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

    QNetworkAccessManager* mNetAccessManager;  //http请求

    Today mToday;
    Day mDay[6];

protected:
     void contextMenuEvent(QContextMenuEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     //获取天气数据
     void getWeatherInfo(QString CityCode);
     //解析json数据
     void parseJson(QByteArray& byteArray);

private:
     void onReplied(QNetworkReply* reply);
 private:
    // QString str; //
    // QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
     //QString Lab_date;
};
#endif // MAINWINDOW_H
