#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>
#include "weathertool.h"

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
    mWeekList<<ui->lblWeek0<<ui->lblWeek1<<ui->lblWeek2<<ui->lblWeek3<<ui->lblWeek4<<ui->lblWeek5;
    mDateList<<ui->lblDate0<<ui->lblDate1<<ui->lblDate2<<ui->lblDate3<<ui->lblDate4<<ui->lblDate5;

    mTypeList<<ui->lblType0<<ui->lblType1<<ui->lblType2<<ui->lblType3<<ui->lblType4<<ui->lblType5;
    mTypeIconList<<ui->lblMap0<<ui->lblMap1<<ui->lblMap2<<ui->lblMap3<<ui->lblMap4<<ui->lblMap5;

    //污染指数
    mAqiList<<ui->lblQuality0<<ui->lblQuality1<<ui->lblQuality2<<ui->lblQuality3<<ui->lblQuality4<<ui->lblQuality5;
    //风力和风向
    mFxList<<ui->lblFx0<<ui->lblFx1<<ui->lblFx2<<ui->lblFx3<<ui->lblFx4<<ui->lblFx5;
    mFlList<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<<ui->lblFl4<<ui->lblFl5;
    //图片
    mTypeMap.insert("晴",":/res/type/Qing.png");

    //搜索图标显示

    //pbt_Search->setIcon(QIcon(":/res/search.png"));
    //网络请求
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);
    //直接在构造中 ，请求天气数据
    //"101010100" 是北京的城市编码
    getWeatherInfo("北京市");
     //getWeatherInfo("101010100");
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
    moffset = (event->globalPosition() - this->pos()).toPoint();

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move((event->globalPosition()-moffset).toPoint());
}

void MainWindow::getWeatherInfo(QString name)
{
    QString CityCode =  weathertool::getCityCode(name);
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
   //1.更新城市和日期
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd")+"        "+mDay[1].week);
    ui->lblCIty->setText(mToday.city);
   //2.更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap.value(mToday.type));
    ui->lblTemp0->setText(QString::number(mToday.wendu)+"℃");
    ui->lblTemp->setText(mToday.type);

    ui->lblGanmao->setText("感冒指数："+mToday.ganmao);
    ui->lblFx->setText(mToday.fx);
    ui->lblFl->setText(mToday.fl);
    ui->lblPm25->setText(("pm2.5\n") +QString::number(mToday.pm25));
    ui->lblShidu->setText(("湿度\n") +mToday.shidu);
    ui->lblQuality->setText("湿度\n"+mToday.quality);
   //3.更新6天
    for(int i = 0;i<6;i++){
       //更新日期和时间
    mWeekList[i]->setText("周"+mDay[i].week.right(1));
    ui->lblWeek0->setText("昨天");
    ui->lblWeek1->setText("今天");
    ui->lblWeek2->setText("明天");

    QStringList ymdList;
    if (!mDay[i].date.isEmpty() && mDay[i].date.contains("-")) {
        ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1]+""+ymdList[2]);
    } else {
        QString date = mDay[i].date;
        mDateList[i]->setText(date);

    }
    //更新天气类型
    mTypeList[i]->setText(mDay[i].type);
    //更新天气类型图片
    mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);
    //更新天气质量
    if(mDay[i].aqi>=0&&mDay[i].aqi<=50){
        mAqiList[i]->setText("优");
        mAqiList[i]->setStyleSheet("background-color:rgb(121,184,0)");
    }else if(mDay[i].aqi>50&&mDay[i].aqi<=100){
        mAqiList[i]->setText("良");
        mAqiList[i]->setStyleSheet("background-color:rgb(255,187,23)");
    }else if(mDay[i].aqi>100&&mDay[i].aqi<=150){
        mAqiList[i]->setText("轻度污染");
        mAqiList[i]->setStyleSheet("background-color:rgb(255,87,97)");
    }else if(mDay[i].aqi>150&&mDay[i].aqi<=200){
        mAqiList[i]->setText("中度污染");
        mAqiList[i]->setStyleSheet("background-color:rgb(235,17,27)");
    }else if(mDay[i].aqi>200&&mDay[i].aqi<=250){
        mAqiList[i]->setText("重度污染");
        mAqiList[i]->setStyleSheet("background-color:rgb(170,0,0)");
    }else {
        mAqiList[i]->setText("严重");
        mAqiList[i]->setStyleSheet("background-color:rgb(110,0,0)");
    }
    //更新风力风向
    mFxList[i]->setText(mDay[i].fx);
    mFlList[i]->setText(mDay[i].fl);
    }
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

