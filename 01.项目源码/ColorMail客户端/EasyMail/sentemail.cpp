#include "sentemail.h"
#include "ui_sentemail.h"
#include <QJsonArray>
#include <QJsonDocument>

sentEmail::sentEmail(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sentEmail)
{
    this->Draft_flag = "0";
    ui->setupUi(this);
  //  this->setWindowFlags(Qt::FramelessWindowHint);
    sent_showtime();//显示时间
    ui->pushButton_sent->setEnabled(true);
}

sentEmail::~sentEmail()
{
    ui->textEdit->clear();
    ui->lineEdit_receiver->clear();
    ui->lineEdit_subject->clear();
    ui->label_notice->clear();
    delete tcpClient;
    delete ui;
}

void sentEmail::sent_timerUpdate(void)
{
    QDateTime time = QDateTime::currentDateTime(); // 获取当前时间
    QString timestr = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    currentTime->setText(timestr);
}

void sentEmail::sent_showtime(void)
{
    currentTime = new QLabel;
    ui->statusbar->addWidget(currentTime);
    QTimer *senttimer = new QTimer(this);
    senttimer->start(1000);
    connect(senttimer,SIGNAL(timeout()),this,SLOT(sent_timerUpdate()));
}

void sentEmail::receivermessage(QString data)//接受联系人信息
{
    ui->lineEdit_receiver->setText(data);
}

void sentEmail::receiveusermessage(QListWidget * listwidget_friend)
{
    for(int i=0 ; i<listwidget_friend->count() ; i++)
    {
        ui->listWidget_contacts->addItem(listwidget_friend->item(i)->text()); //将email中的信息加入到sentemail的联系人中
    }
}

void sentEmail::on_pushButton_cancel_clicked() //取消按键
{
    ui->textEdit->clear(); //清空文本
    ui->lineEdit_receiver->clear(); //清空收件人
    ui->lineEdit_subject->clear(); //清空主题
    ui->label_notice->clear();
    Draft_flag = "0";
//    delete tcpClient;
    this->hide();
}

void sentEmail::on_pushButton_preview_clicked()
{
    new_previewpage = new previewpage();
    QObject::connect(this,SIGNAL(sentmessage(QString,QString,QString)),new_previewpage,SLOT(receivemessage(QString,QString,QString)));//连接sentemail窗口与preview窗口
    QString subjectmessage = ui->lineEdit_subject->text();
    QString receivermessage = ui->lineEdit_receiver->text();
    QString text = ui->textEdit->toPlainText();
    emit sentmessage(subjectmessage,receivermessage , text); //向preview窗口传值
    new_previewpage->show();
}

void sentEmail::on_pushButton_add_clicked() //打开文件
{
    Openfile();
}

void sentEmail::on_pushButton_sent_clicked() //连接服务器发送文件
{

    sendfile();
}

void sentEmail::Openfile()
{
    fileName=QFileDialog::getOpenFileName(this);
    qDebug() << fileName;
    if(!fileName.isEmpty())
    {
        file_flag = 1; //打开发送按钮
    }
}

void sentEmail::sendfile()
{
    tcpClient=new QTcpSocket(this);
    tcpClient->abort();
    tcpFile = new QTcpSocket(this);
    tcpFile->abort();
    connect(tcpClient,SIGNAL(connected()),this,SLOT(TransferTxt())); //服务器连接成功开始发送数据
    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(Get_Rev()));
    //  ui->pushButton_sent->setEnabled(false); //先关闭文件发送完再打开
    bytesWritten = 0; //初始发送0字节
    tcpClient->connectToHost("192.168.43.188", 6666);//连接服务器
    connect(tcpFile,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64))); //数据发送成功更新进度条
    connect(tcpFile,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));//显示错误
}

void sentEmail::Get_Rev()
{
    if(file_flag == 1)
    {
        Transferfile();
    }
    QByteArray send_qba = tcpClient->readAll();
    //delete tcpClient;
    QJsonParseError json_error;
    QJsonDocument send_getdoc(QJsonDocument::fromJson(send_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        return;
    }
    QJsonArray send_add = send_getdoc.array();
    int send_flag = send_add[0].toInt();

    if(send_flag == 0)//发送成功之后显示
    {
        if(Draft_flag == "0")
        {
            ui->label_notice->setText("发送成功");
            QDateTime time = QDateTime::currentDateTime();
            QString timestr = time.toString("yyyy-MM-dd hh:mm:ss");
            emit return_receivemessage(ui->lineEdit_receiver->text(),ui->lineEdit_subject->text(),timestr);
        }
        else
        {
            ui->label_notice->setText("保存成功");
            QDateTime time = QDateTime::currentDateTime();
            QString timestr = time.toString("yyyy-MM-dd hh:mm:ss");
            emit return_draftmessage(ui->lineEdit_receiver->text(),ui->lineEdit_subject->text(),timestr);
        }
    }
    else//发送不成功显示
    {
        ui->label_notice->setText("发送用户不存在！");
    }
}

void sentEmail::TransferTxt()
{
    QString currenttext = ui->textEdit->toPlainText(); //邮件内容
    QString currentsubject = ui->lineEdit_subject->text(); //邮件主题
    QString currentreceiver = ui->lineEdit_receiver->text(); //邮件发送者
    QJsonArray txt_arr;
    QDateTime time = QDateTime::currentDateTime();
    QString currenttime = time.toString("yyyy-MM-dd hh:mm:ss");
    txt_arr.append(4);
    txt_arr.append(name);
    txt_arr.append(currentreceiver);
    txt_arr.append(currentsubject);
    txt_arr.append(currenttext);
    txt_arr.append(currenttime);
    txt_arr.append(Draft_flag);
    txt_arr.append(file_flag);
    QJsonDocument txt_doc;
    txt_doc.setArray(txt_arr);
    QByteArray txt_bt = txt_doc.toJson(QJsonDocument::Compact);
    tcpClient->write(txt_bt);
}

void sentEmail::Transferfile() //传送文件大小，名称等内容
{
    tcpFile->connectToHost("192.168.43.188", 6666);//连接服务器
    localFile=new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        ui->label_notice->setText("文件错误！");
        qDebug()<<"client：open file error!";
        return;
    }
    totalBytes=localFile->size(); //文件数据大小
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_14); //序列化格式版本号
    QString currentFileName=fileName.right(fileName.size()-fileName.lastIndexOf('/')-1); //文件总大小、文件名大小、文件名

    sendOut<<qint64(0)<<qint64(0)<<currentFileName; //发送附件
    totalBytes+=outBlock.size(); //文件数据+文件信息大小
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64(outBlock.size()-sizeof(qint64)*2); //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    bytesToWrite=totalBytes-tcpFile->write(outBlock); //发送完数据后剩余数据的大小
    outBlock.resize(0); //清空缓存区

}

void sentEmail::updateClientProgress(qint64 numBytes)
{
    qDebug() << "aha";
    bytesWritten+=(int)numBytes; //已经发送数据的大小
    if(bytesToWrite>0)
    {
        outBlock=localFile->read(qMin(bytesToWrite,payloadSize)); //每次发送loadSize大小的数据，这里设置为64KB，如果剩余的数据不足64KB就发送剩余数据的大小
        bytesToWrite-=(int)tcpFile->write(outBlock);//发送完一次数据后还剩余数据的大小
        outBlock.resize(0); //清空发送缓冲区
    }
    else
    {
        localFile->close(); //如果没有发送任何数据，则关闭文件
    }
    if(bytesWritten==totalBytes) //发送完毕
    {
        localFile->close();
        tcpFile->close();
    }
}

void sentEmail::displayError(QAbstractSocket::SocketError) //显示错误
{
    ui->label_notice->setText("文件错误");
    qDebug() << tcpClient->errorString();
    tcpClient->close();
 //   ui->pushButton_sent->setEnabled(true);
}


void sentEmail::on_listWidget_contacts_itemClicked(QListWidgetItem *item)  //点击联系人填入收件人一栏中
{
    int start; //得到ID在字符串中的起始位置
    QString information = item->text();
    QString ID = NULL;
    for(start=0 ; start<information.size();start++)
    {
        if( information[start] ==' ' and information[start+1]!=' ' )
        {
            start+=1;
            for(int i = start ; i<information.size(); i++)
            {
                ID+=information[i];  //得到用户的ID
            }
            break;
        }
    }
    ui->lineEdit_receiver->setText(ID); //将用户ID填入收件人中
}

void sentEmail::receiveID_and_name(QString ID , QString name) //接受email界面发送的添加成功的用户ID和名字
{
    QString userinformation = ID + "     " + name;
    ui->listWidget_contacts->addItem(userinformation); //添加好友信息
}

void sentEmail::receivername(QString data)
{
    name = data;
}

void sentEmail::on_pushButton_Draft_clicked() //存草稿
{
    Draft_flag = "1";
    sendfile();
}
