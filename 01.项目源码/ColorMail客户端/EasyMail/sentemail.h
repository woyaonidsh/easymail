#ifndef SENTEMAIL_H
#define SENTEMAIL_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include "previewpage.h"
#include <QString>
#include <QAbstractSocket>
#include <QFileDialog>
#include <QtNetwork>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QListWidget>

namespace Ui {
class sentEmail;
}

class sentEmail : public QMainWindow
{
    Q_OBJECT

public:
    explicit sentEmail(QWidget *parent = nullptr);
    ~sentEmail();
    void sent_showtime();
private:
    Ui::sentEmail *ui;
    QLabel * currentTime;
    previewpage *new_previewpage;
    QTcpSocket *tcpClient;
    QTcpSocket *tcpFile;
    QFile * localFile; //传输的文件
    qint64 totalBytes = 0; //文件总大小
    qint64 bytesWritten = 0; //已经发送数据大小
    qint64 bytesToWrite = 0; //剩余数据大小
    qint64 payloadSize = 64*1024; //每次发送数据大小
    QString fileName; //传送的文件名
    QByteArray outBlock; //暂存发送的文件数据
    QString name; //登录的用户名
    int file_flag = 0;
    QString Draft_flag = "0";
public slots:
    void sent_timerUpdate(void);
    void Openfile(); //打开文件
    void sendfile(); //发送文件
    void TransferTxt();
    void Transferfile(); //发送文件大小等信息
    void updateClientProgress(qint64); //发送数据更新进度条
    void displayError(QAbstractSocket::SocketError);//显示错误
    void receivermessage(QString);//接受联系人信息
    void receiveusermessage(QListWidget *); //接受listWidget_friend的值
    void receiveID_and_name(QString ,QString); //接受email界面添加朋友成功的信息
    void receivername(QString); //接受用户名
    void Get_Rev();//接收服务器返回信息
private slots:
    void on_pushButton_cancel_clicked(); //取消键，关闭窗口
    void on_pushButton_preview_clicked(); //打开预览界面
    void on_pushButton_add_clicked(); //打开文件
    void on_pushButton_sent_clicked(); //点击发送连接服务器发送文件

    void on_listWidget_contacts_itemClicked(QListWidgetItem *item);

    void on_pushButton_Draft_clicked();

signals:
    void sentmessage(QString , QString , QString); //向preview窗口传值
    void return_receivemessage(QString , QString,QString); //向email窗口返回发件信息
    void return_draftmessage(QString,QString,QString); //向email窗口返回草稿箱信息
};

#endif // SENTEMAIL_H
