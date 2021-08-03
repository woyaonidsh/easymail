    #include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    QString name = "", password = "";
    mainwd = new MainWindow(this);
    this->setAutoFillBackground(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPalette pal0=palette();
    pal0.setBrush(QPalette::Background, QBrush(QPixmap(":/new/prefix1/picture/background.jpg")));
    this->setPalette(pal0);

    QPalette pal=palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,100));
    ui->InputArea->setAttribute(Qt::WA_TranslucentBackground, false);
    ui->InputArea->setAutoFillBackground(true);
    ui->InputArea->setPalette(pal);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btnRegester_clicked()
{
    this->hide();
    Registe *registe=new Registe(this);
    connect(registe,SIGNAL(signalReshow()),this,SLOT(reshow()));
    registe->show();
}

void Login::reshow(){
    this->show();
}

void Login::on_lineName_textEdited(const QString &arg1)
{
    name = arg1;
}

void Login::on_lineCode_textEdited(const QString &arg1)
{
    password = arg1;
}

void Login::on_btnLog_clicked()
{
    login_tcpSocket = new QTcpSocket(this);
    login_tcpSocket->abort();
    login_tcpSocket->connectToHost("192.168.43.188", 6666);
    this->login_Send();
    connect(login_tcpSocket, SIGNAL(readyRead()), this, SLOT(login_GetTorF()));
}

void Login::login_Send()
{
    QJsonArray login_ar;
    login_ar.append(1);
    login_ar.append(name);
    login_ar.append(password);

    QJsonDocument login_doc;
    login_doc.setArray(login_ar);
    QByteArray login_bt = login_doc.toJson(QJsonDocument::Compact);
    login_tcpSocket->write(login_bt);
}

void Login::login_GetTorF()
{
    QByteArray login_qba = login_tcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument login_getdoc(QJsonDocument::fromJson(login_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        return;
    }
    QJsonArray ar_get = login_getdoc.array();
    int login_flag = ar_get[0].toInt();
    if(login_flag == 0) //登录成功跳转到邮件界面
    {
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("登陆成功"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        this->hide();
        connect(mainwd, SIGNAL(mainwdReshow()), this, SLOT(reshow()));
        QObject::connect(this,SIGNAL(initialsignal(QString)),mainwd, SLOT(initialpage(QString)));
        emit initialsignal(name);
        mainwd->show();
    }
    else if(login_flag == 1)
    {
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("用户名不存在，请检查后重试！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
    else
    {
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox question(QMessageBox::Information,tr("提示"),tr("密码输入错误！"));
        QPushButton(question.addButton(tr("确认"), QMessageBox::YesRole));
        question.exec();
    }
}

