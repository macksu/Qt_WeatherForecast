#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "WeatherData.h"
#include <QList>
#include <QString>
#include <QLabel>

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
    //今天和6天的天气
    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*>mWeekList;
    QList<QLabel*>mDateList;
    //天气和天气图标
    QList<QLabel*>mTypeList;
    QList<QLabel*>mTypeIconList;

    //天气污染指数
    QList<QLabel*>mAqiList;
    //风力和风向
    QList<QLabel*>mFxList;
    QList<QLabel*>mFlList;

protected:
     void contextMenuEvent(QContextMenuEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     //获取天气数据
     void getWeatherInfo(QString CityCode);
     //解析json数据
     void parseJson(QByteArray& byteArray);
     //更新UI
     void UpdateUI();

private:
     void onReplied(QNetworkReply* reply);


};
#endif // MAINWINDOW_H
