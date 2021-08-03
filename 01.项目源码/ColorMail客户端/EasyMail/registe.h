#ifndef REGISTE_H
#define REGISTE_H

#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class Registe;
}

class Registe : public QDialog
{
    Q_OBJECT

public:
    explicit Registe(QWidget *parent = nullptr);
    ~Registe();

private slots:
    void on_btnSubmit_clicked();

    void on_subCode_textChanged(const QString &arg1);

    void on_CodeCon_textEdited(const QString &arg1);

    void on_subName_textChanged(const QString &arg1);

    void on_btnQuit_clicked();

    void on_subRealName_textChanged(const QString &arg1);

    void on_subPhoneNum_textChanged(const QString &arg1);

    void Send();

    void Get_TorF();
signals:
    void signalReshow();

public slots:
    void newConnect();

private:
    Ui::Registe *ui;
    QString fir, sec, name, realname, phonenumber;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;
};


#endif // REGISTE_H
