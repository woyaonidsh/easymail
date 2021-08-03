#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "registe.h"
#include "mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_btnRegester_clicked();
    void reshow();
    void on_btnLog_clicked();

    void on_lineName_textEdited(const QString &arg1);

    void on_lineCode_textEdited(const QString &arg1);
    void login_Send();
    void login_GetTorF();

private:
    Ui::Login *ui;
    QTcpServer *login_tcpServer;
    QTcpSocket *login_tcpSocket;
    QString name, password;
    MainWindow *mainwd;
signals:
    void initialsignal(QString); //初始邮件界面信号
};
#endif // LOGIN_H
