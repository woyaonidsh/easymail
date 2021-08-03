#include "addfriend.h"
#include "ui_addfriend.h"
#include <QJsonArray>
#include <QJsonDocument>

addfriend::addfriend(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::addfriend)
{
    QString ID = "";
    QString n_name = "";
    ui->setupUi(this);
    ui->label_notice->setText(NULL); //初始通知消息为空
    this->setWindowTitle("AddPeople");
    showtime(); //显示时间
}

addfriend::~addfriend()
{
    ui->label_notice->clear();
    delete tcpClient;
    delete ui;
}

void addfriend::timerUpdate(void) //更新时间
{
    QDateTime time = QDateTime::currentDateTime();
    QString timestr = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    currentTimeLabel->setText(timestr);
}

void addfriend::showtime(void)
{
    currentTimeLabel = new QLabel;
    ui->statusbar->addWidget(currentTimeLabel);
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
}

void addfriend::on_pushButton_clicked()
{
    tcpClient = new QTcpSocket(this);
    tcpClient->abort();
    tcpClient->connectToHost("192.168.43.188", 6666); //连接服务器
    QString ID = ui->lineEdit->text();
    QString n_name = ui->lineEdit_2->text();
    if( ID==NULL )
    {
        ui->label_notice->setText("无效操作");
        return;
    }
    QJsonArray add_array;
    add_array.append(2);
    add_array.append(name);
    add_array.append(ID);
    add_array.append(n_name);

    QJsonDocument add_doc;
    add_doc.setArray(add_array);
    QByteArray add_byte = add_doc.toJson(QJsonDocument::Compact);

    tcpClient->write(add_byte);
    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(add_GetTorF()));
}

void addfriend::add_GetTorF()
{
    QByteArray add_qba = tcpClient->readAll();
    QJsonParseError json_error;
    QJsonDocument add_getdoc(QJsonDocument::fromJson(add_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        return;
    }
    QJsonArray ar_add = add_getdoc.array();
    int add_flag = ar_add[0].toInt();

    if(add_flag == 0)//添加成功之后显示
    {
        ui->label_notice->setText("添加成功!");
        QString userID = ui->lineEdit->text();
        QString username = ui->lineEdit_2->text();
        emit successaddfriend(userID , username); //向email发送用户信息
    }
    else if(add_flag == 1)//添加不成功显示
    {
        ui->label_notice->setText("已添加该用户！");
    }
    else
    {
        ui->label_notice->setText("用户不存在！");
    }
}

void addfriend::receivename(QString data) //接受用户名
{
    name = data;
}








