#include "previewpage.h"
#include "ui_previewpage.h"
#include "sentemail.h"
#include "ui_sentemail.h"

previewpage::previewpage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::previewpage)
{
    ui->setupUi(this);
    this->setWindowTitle("Preview");
    preview_showtime();//显示时间
}

previewpage::~previewpage()
{
    delete ui;
}

void previewpage::preview_timerUpdate(void)
{
    QDateTime time = QDateTime::currentDateTime();
    QString timestr = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    currenttime->setText(timestr);
}

void previewpage::preview_showtime(void)
{
    currenttime = new QLabel;
    ui->statusbar->addWidget(currenttime);
    QTimer *previewtimer = new QTimer(this);
    connect(previewtimer,SIGNAL(timeout()),this,SLOT(preview_timerUpdate()));
    previewtimer->start(1000);
}

void previewpage::receivemessage(QString subjectmessage , QString receivermessage , QString text) //接受sentemail窗口的值
{
    ui->lineEdit_subject->setText(subjectmessage);
    ui->lineEdit_subject->setReadOnly(true); //设置为只读
    ui->lineEdit_recever->setText(receivermessage);
    ui->lineEdit_recever->setReadOnly(true);
    ui->textEdit->setText(text);
    ui->textEdit->setReadOnly(true);
}
