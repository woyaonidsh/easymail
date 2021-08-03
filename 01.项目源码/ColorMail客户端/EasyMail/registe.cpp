#include "registe.h"
#include "ui_registe.h"
#include <QJsonArray>
#include <QJsonDocument>
#pragma excution_character_set("utf-8")

Registe::Registe(QWidget *parent) :
    QDialog(parent,Qt::WindowTitleHint | Qt::CustomizeWindowHint),
    ui(new Ui::Registe)
{
    QString fir="",sec="",name="",realname="",phonenumber="";
    ui->setupUi(this);
}

Registe::~Registe()
{
    delete ui;
}

void Registe::on_subCode_textChanged(const QString &arg1)
{
    fir=arg1;
}

void Registe::on_CodeCon_textEdited(const QString &arg1)
{
    sec=arg1;
}


void Registe::on_subName_textChanged(const QString &arg1)
{
    name=arg1;
}

void Registe::on_subRealName_textChanged(const QString &arg1)
{
    realname=arg1;
}

void Registe::on_subPhoneNum_textChanged(const QString &arg1)
{
    phonenumber=arg1;
}

void Registe::on_btnSubmit_clicked()
{


    if(name.length()<6){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("用户名过短，请重新输入！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else if(fir == ""){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("请输入密码！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else if(fir.length()<6){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("密码过短，请重新输入！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else if(realname == ""){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("请输入您的真实姓名！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else if(fir==sec){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox
                question(QMessageBox::Question,tr("退出"),tr("确认提交吗?"));
        QPushButton *button1= (question.addButton(tr("确认"), QMessageBox::YesRole));
        QPushButton *button2= (question.addButton(tr("再想想"), QMessageBox::RejectRole));
        question.exec();
        if(question.clickedButton()==button1){

            this->newConnect();
            //qDebug() << "Seccessful!";
            emit signalReshow();
            this->close();
            this->setAttribute(Qt::WA_DeleteOnClose,1);
        }
         if(question.clickedButton()==button2){
            question.close();
        }
    }
    else{
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("两次输入的密码不一致"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
}


void Registe::on_btnQuit_clicked()
{
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox question(QMessageBox::Question,tr("退出"),tr("放弃注册吗?"));
    QPushButton *button1= (question.addButton(tr("确认"), QMessageBox::YesRole));
    QPushButton *button2= (question.addButton(tr("再想想"), QMessageBox::RejectRole));
    question.exec();
    if(question.clickedButton()==button1){
        emit signalReshow();
        this->close();
    }
     if(question.clickedButton()==button2){
        question.close();
    }
}

void Registe::newConnect()
{
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost("192.168.43.188", 6666);

    this->Send();
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(Get_TorF()));
}

void Registe::Send()
{
    QJsonArray array;
    array.append(0);
    array.append(name);
    array.append(fir);
    array.append(realname);
    array.append(phonenumber);

    QJsonDocument doc;
    doc.setArray(array);
    QByteArray byte = doc.toJson(QJsonDocument::Compact);

    m_tcpSocket->write(byte);
}

void Registe::Get_TorF()//这是判断是否注册成功的返回函数，注册成功返回flag是"1"，否则是"0"，flag是QString类型
{
    QByteArray qba = m_tcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument doc_get(QJsonDocument::fromJson(qba,&json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        return;
    }
    QJsonArray array_get = doc_get.array();
    int flag = array_get[0].toInt();
    //qDebug() << flag;
    if(flag == 1){
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("注册成功！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else{
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("用户名已存在，请更改后重试！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
}
