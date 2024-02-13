#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
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
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);
    //直接在构造中 ，请求天气数据
    //"101010100" 是北京的城市编码
    getWeatherInfo("101010100");
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

void MainWindow::getWeatherInfo(QString CityCode)
{
     QUrl url("http://t.weather.itboy.net/api/weather/city/"+CityCode);
     mNetAccessManager->get(QNetworkRequest(url));

}

void MainWindow::parseJson(QByteArray &byteArray)
{
     QJsonParseError err;
     QJsonDocument Doc = QJsonDocument::fromJson(byteArray,&err);
     if(err.error != QJsonParseError::NoError){
         return;
     }
    QJsonObject rootobj = Doc.object();
    qDebug<<rootobj.value("message").toString();
    //1.解析日期和城市
    mToday.date = rootobj.value("date").toString();

    mToday.city =  rootobj.value("cityInfo").toObject().value("city").toString();
    //2.解析yesterday
    QJsonObject objData = rootobj.value("data").toObject();

    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("date").toString();
    mDay[0].type = objYesterday.value("type").toString();

    QString s;
    s = objYesterday.value("high").toString().split(" ").at(0);//18度
    s = s.left(s.length()-1); //18
    mDay[0].high = s.toInt();

    s = objYesterday.value("low").toString().split(" ").at(0);//18度
    s = s.left(s.length()-1); //18
    mDay[0].low = s.toInt();


    //风向风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();
    //污染指数
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //3.解析forcast中5天的数据
    QJsonArray forecastArr = objData.value("forecast").toArray();
    for(int i =0;i<5;i++){
    QJsonArray objForecast = forecastArr[i].toObject();
    mDay[i+1].week = objForecast.value("week").toString();
    mDay[i+1].date = objForecast.value("date").toString();
    //天气类型
    mDay[i+1].type = objForecast.value("type").toString();

    QString s;
    s = objForecast.value("high").toString().split(" ").at(0);//18度
    s = s.left(s.length()-1); //18
    mDay[i+1].high = s.toInt();

    s = objForecast.value("low").toString().split(" ").at(0);//18度
    s = s.left(s.length()-1); //18
    mDay[i+1].low = s.toInt();
    //风向风力
    mDay[i+1].fx = objForecast.value("fx").toString();
    mDay[i+1].fl = objForecast.value("fl").toString();
    //污染指数
    mDay[i+1].aqi = objForecast.value("aqi").toString();
    }

    //4.解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString();
    mToday.pm25 = objData.value("pm25").toString();
    mToday.quality = objData.value("quality").toString();

}

void MainWindow::onReplied(QNetworkReply *reply)
{
     //qDebug()<<"onReplied success";

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//     qDebug()<<"operation:"<<reply->operation();
//     qDebug()<<"status code"<< status_code;
//     qDebug()<<"url:"<<reply->url();
//     qDebug()<<"raw header:"<<reply->rawHeaderList();

     if(reply->error() != QNetworkReply::NoError || status_code != 200){
         qDebug()<< reply->errorString().toLatin1().data();
         QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
     }else{
         QByteArray byteArray = reply->readAll();
         qDebug()<<"read ALL:"<<byteArray.data();
         parseJson();
     }
     reply->deleteLater();

}

