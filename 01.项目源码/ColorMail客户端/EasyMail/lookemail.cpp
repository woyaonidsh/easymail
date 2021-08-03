#include "lookemail.h"
#include "ui_lookemail.h"
#include <QJsonArray>
#include <QJsonDocument>

lookemail::lookemail(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lookemail)
{
    ui->setupUi(this);
}

lookemail::~lookemail()
{
    delete ui;
}

void lookemail::receive_receiveemail(QString username,QString theme,QString time,QString text , int file) //显示收件箱的邮件信息
{
    qDebug() << file;
    if( file == 0 )
    {
        ui->download->hide();
    }
    else
    {
        ui->download->show();
    }
    ui->pushButton_send->hide();
    ui->label_recever->setText("发件人：");
    ui->lineEdit_recever->setText(username);
    ui->lineEdit_subject->setText(theme);
    ui->time->setText("发送时间："+time);
    ui->textEdit->setText(text);
    ui->lineEdit_recever->setReadOnly(true);
    ui->lineEdit_subject->setReadOnly(true);
    ui->textEdit->setReadOnly(true);
    ui->label_notice->clear();
    ui->label_notice->hide();
    this->show();
}

void lookemail::receive_sendemail(QString username,QString theme,QString time,QString text , int file) //显示发件箱内容
{
    if( file == 0 )
    {
        ui->download->hide();
    }
    else
    {
       ui->download->show();
    }
    ui->pushButton_send->hide();
    ui->label_recever->setText("收件人：");
    ui->lineEdit_recever->setText(username);
    ui->lineEdit_subject->setText(theme);
    ui->time->setText("发送时间："+time);
    ui->textEdit->setText(text);
    ui->lineEdit_recever->setReadOnly(true);
    ui->lineEdit_subject->setReadOnly(true);
    ui->textEdit->setReadOnly(true);
    ui->label_notice->clear();
    ui->label_notice->hide();
    this->show();
}

void lookemail::receive_draftemail(QString username, QString theme, QString time,QString text , int file) //显示草稿箱内容
{

    ui->download->hide();
    ui->pushButton_send->show();
    ui->label_recever->setText("发件人：");
    ui->lineEdit_recever->setText(username);
    ui->lineEdit_subject->setText(theme);
    ui->time->setText("保存时间："+time);
    ui->textEdit->setText(text);
//    ui->lineEdit_recever->setReadOnly(true);
//    ui->lineEdit_subject->setReadOnly(true);
//    ui->textEdit->setReadOnly(true);
    ui->label_notice->clear();
    ui->label_notice->hide();
    this->show();
}

void lookemail::receive_collectemail(QString username, QString theme,QString time,QString text , int file) //显示收藏箱内容
{
    if( file == 0 )
    {
        ui->download->hide();
    }
    else
    {
        ui->download->show();
    }
    ui->pushButton_send->hide();
    ui->label_recever->setText("发件人：");
    ui->lineEdit_recever->setText(username);
    ui->lineEdit_subject->setText(theme);
    ui->time->setText("发送时间："+time);
    ui->textEdit->setText(text);
    ui->lineEdit_recever->setReadOnly(true);
    ui->lineEdit_subject->setReadOnly(true);
    ui->textEdit->setReadOnly(true);
    ui->label_notice->clear();
    ui->label_notice->hide();
    this->show();
}

void lookemail::receive_deleteemail(QString username,QString theme,QString time,QString text , int file) //显示回收站内容
{
    if( file == 0 )
    {
        ui->download->hide();
    }
    else
    {
        ui->download->show();
    }
    ui->pushButton_send->hide();
    ui->label_recever->setText("发件人：");
    ui->lineEdit_recever->setText(username);
    ui->lineEdit_subject->setText(theme);
    ui->time->setText("发送时间："+time);
    ui->textEdit->setText(text);
    ui->lineEdit_recever->setReadOnly(true);
    ui->lineEdit_subject->setReadOnly(true);
    ui->textEdit->setReadOnly(true);
    ui->label_notice->clear();
    ui->label_notice->hide();
    this->show();
}

void lookemail::on_pushButton_send_clicked()
{
    QDateTime  currenttime = QDateTime::currentDateTime();
    QString time = currenttime.toString("yyyy-MM-dd hh:mm:ss"); //现在时间
    QString subject = ui->lineEdit_subject->text(); //主题
    QString text = ui->textEdit->toPlainText();
    ChngtcpSocket = new QTcpSocket(this);
    ChngtcpSocket->abort();
    ChngtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    QString old_drafttime =NULL;
    QString old_drafttheme = NULL;
    QString time_ = ui->time->text();
    for(int i=5;i<time_.size();i++)
    {
        old_drafttime+=time_[i];
    }
    old_drafttheme=ui->lineEdit_subject->text();

    QJsonArray Chng_arr;
    qDebug() << old_drafttime;
    Chng_arr.append(8);
    Chng_arr.append(old_drafttheme);
    Chng_arr.append(old_drafttime);
    Chng_arr.append(time);
    Chng_arr.append(subject);
    Chng_arr.append(ui->lineEdit_recever->text());
    Chng_arr.append(text);
    QJsonDocument Chng_doc;
    Chng_doc.setArray(Chng_arr);
    QByteArray Chng_bt = Chng_doc.toJson(QJsonDocument::Compact);
    ChngtcpSocket->write(Chng_bt);
    emit send_draftemail(ui->lineEdit_recever->text(),old_drafttheme,time, old_drafttime); //发送成功保存到发件箱
    ui->label_notice->setText("发送成功！");
    ui->label_notice->show();
}

void lookemail::on_download_clicked()
{
    QString old_time = ui->time->text();
    QString time =NULL;
    for(int i=5 ;i<old_time.size();i++)
    {
        time+=old_time[i];
    }
    QString theme = ui->lineEdit_subject->text();
    DwldtcpSocket = new QTcpSocket(this);
    DwldtcpSocket->abort();
    DwldtcpSocket->connectToHost("192.168.43.188", 6666);//连接服务器
    connect(DwldtcpSocket, SIGNAL(readyRead()), this, SLOT(startDownload()));
    QJsonArray dw_arr;
    dw_arr.append(6);
    dw_arr.append(theme);
    dw_arr.append(time);
    dw_arr.append(ui->lineEdit_recever->text());
    QJsonDocument dw_doc;
    dw_doc.setArray(dw_arr);
    QByteArray dw_bt = dw_doc.toJson(QJsonDocument::Compact);
    DwldtcpSocket->write(dw_bt);
    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;
    bytesWritten = 0;
    perDataSize = 64 * 1024;
}

void lookemail::startDownload()
{
    QDataStream inFile(this->DwldtcpSocket);
    inFile.setVersion(QDataStream::Qt_5_15);

    if (bytesReceived <= sizeof(qint64)* 2)
    {
        if ((DwldtcpSocket->bytesAvailable() >= (sizeof(qint64)) * 2) && (filenameSize == 0))
        {
            inFile >> totalBytes >> filenameSize;
            bytesReceived += sizeof(qint64)* 2;
        }
        if ((DwldtcpSocket->bytesAvailable() >= filenameSize) && (filenameSize != 0))
        {
            inFile >> filename;
            bytesReceived += filenameSize;

            filename = "/home/valanya/dl/"+filename;
            qDebug() << filename;

            localFile = new QFile(filename);
            if (!localFile->open(QFile::WriteOnly))
            {
                qDebug() << "Server::open file error!";
                return;
            }
        }
        else
            return;
    }
    if (bytesReceived < totalBytes)
    {
        bytesReceived += DwldtcpSocket->bytesAvailable();
        inBlock = DwldtcpSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    if (bytesReceived == totalBytes)
    {
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        localFile->close();
    }
}
