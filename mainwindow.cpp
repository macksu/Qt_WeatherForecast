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
    //控件添加到控件数组
    mWeekList<<ui->lblWeek0;
    mWeekList<<ui->lblWeek1;
    //<<ui->lblWeek2<<ui->lblWeek3<<ui->lblWeek4<<ui->lblWeek5;
    mDateList<<ui->lblDate0;
    //<<ui->lblDate1<<ui->lblDate2<<ui->lblDate3<<ui->lblDate4<<ui->lblDate5;

    //污染指数
    //mAqiList<<ui->lblAqi0<<ui->lblAqi1<<ui->lblAqi2<<ui->lblAqi3<<ui->lblAqi4<<ui->lblAqi5;
    //风力和风向
    //mFxList<<ui->lblFx0<<ui->lblFx1<<ui->lblFx2<<ui->lblFx3<<ui->lblFx4<<ui->lblFx5;
    //mFlList<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<ui->lblFl4<<ui->lblFl5;

    //网络请求
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
    //qDebug<<rootobj.value("message").toString();
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
    QJsonObject objForecast = forecastArr[i].toObject();

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
    mDay[i+1].aqi = objForecast.value("aqi").toInt();
    }

    //4.解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toInt();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();
    //5.forcast 中第一个数据也是今天的数据
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;
    //6.更新UI
    UpdateUI();
}

void MainWindow::UpdateUI()
{
    //ui->lblDate->setText(mToday.date);
    ui->lblDate0->setText(QDateTime::fromString(mToday.date,"MMdd").toString("MM/dd")+" "+mDay[1].week);
    ui->lblCIty->setText(mToday.city);
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
         //打印所有接受数据
         qDebug()<<"read ALL:"<<byteArray.data();
         parseJson(byteArray);
     }
     reply->deleteLater();

}

