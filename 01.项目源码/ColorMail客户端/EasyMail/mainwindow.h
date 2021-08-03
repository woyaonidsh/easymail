#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QListWidgetItem>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonArray>
#include "sentemail.h"
#include "addfriend.h"
#include "lookemail.h"
#include <QColor>
#include "confirm_operation.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void main_showtime();
    void lookemail_information(QListWidgetItem * , int , QString ,int);

private:
    Ui::MainWindow *ui;
    QLabel *currentTimeLabel;
    sentEmail *sentemail;
    QJsonArray con_array; //联系人信息

    previewpage * preview;
    addfriend * addpeople;
    lookemail * look_email;
    QListWidgetItem * emailinformation = NULL;
    Confirm_operation * confirm;
    QTcpServer *c_tcpServer;
    QTcpSocket *c_tcpSocket;
    QTcpSocket *coltcpSocket;
    QTcpSocket *deRtcpSocket;
    QTcpSocket *deCtcpSocket;
    QTcpSocket *deFtcpSocket;
    QTcpSocket *deDtcpSocket;
    QTcpSocket *RdretcpSocket;
    QTcpSocket *RdsdtcpSocket;
    QTcpSocket *RdDrtcpSocket;
    QTcpSocket *RdCotcpSocket;
    QTcpSocket *RdDetcpSocket;
    QTcpSocket *RecovtcpSocket;
    QTcpSocket *ReRevtcpSocket;
    QString name; //登录用户名
    int friendflag = 1; //只接受一次数据库传给主界面的联系人
    int RevBoxFlag = 1;
    int SendBoxFlag = 1;
    int DrafBoxFlag = 1;
    int CollectFlag = 1;
    int DeleBoxFlag = 1;
    int newcreatflag = 1; //新建sentemail窗口中的联系人只接受email界面联系人一次传值
    int look_allemail = 0;
    int receive_confirmoperate = 0; //接收确认窗口的信息
public slots:
    void timerUpdate(void);
    void initialpage(QString);//初始邮件界面
    void receiveuser(QString,QString); //接受添加朋友用户信息
    void contacter_get();
    void receive_receivemessage(QString,QString,QString); //接收发件箱的信息
    void receive_draftmessage(QString,QString,QString); //接受草稿箱信息
    void receive_confirm(int); //接收确认窗口的信号
    void receive_draftemail(QString,QString,QString ,QString); //草稿箱发送成功保存到发件箱
    void ReRev_get();
private slots:
    void on_listWidget_menu_itemClicked(QListWidgetItem *item);
    void on_pushButton_newcreat_clicked();
//    void on_listWidget_showsomething_itemClicked(QListWidgetItem *item);
//    void on_pushButton_send_clicked();
    void on_pushButton_send_clicked(bool checked);
    void on_pushButton_collect_clicked();

    void on_pushButton_delete_clicked();

    void on_listWidget_receive_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_send_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_friend_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_draft_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_collect_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_recycle_itemDoubleClicked(QListWidgetItem *item);

    void Rev_Rev();//接收收件箱内容

    void Rev_Send();//接收发件箱内容
    void Rev_Col();//接收收藏夹内容
    void Rev_Del();//接收回收站内容
    void Rev_Drf();//接收草稿箱内容

    void on_pushButton_recover_clicked();

signals:
    void sentreceivermessage(QString); //发送联系人信息
    void sentusermessage(QListWidget *); //发送listWidget_friend的信息
    void sendname(QString); //发送注册用户的用户名
    void sendID_and_name(QString , QString); //向sentemail界面发送添加好友的信息
    void look_receiveemail(QString,QString,QString,QString,int); //查看收件箱内容
    void look_sendemail(QString,QString,QString,QString,int);//查看发件箱内容
    void look_draftemail(QString,QString,QString,QString,int); //查看发件箱内容
    void look_collectemail(QString,QString,QString,QString,int); //查看收藏箱内容
    void look_deleteemail(QString,QString,QString,QString,int); //查看回收站内容
};
#endif // MAINWINDOW_H
