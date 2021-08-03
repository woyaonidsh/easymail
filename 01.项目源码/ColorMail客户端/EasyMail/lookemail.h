#ifndef LOOKEMAIL_H
#define LOOKEMAIL_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>
#include <QTcpSocket>
#include <QFile>

namespace Ui {
class lookemail;
}

class lookemail : public QMainWindow
{
    Q_OBJECT

public:
    explicit lookemail(QWidget *parent = nullptr);
    ~lookemail();

private:
    Ui::lookemail *ui;
    QTcpSocket *ChngtcpSocket;
    QTcpSocket *DwldtcpSocket;
    qint64 totalBytes = 0;
    qint64 bytesReceived = 0;
    qint64 bytesWritten = 0;
    qint64 filenameSize;
    QString filename;
    qint64 perDataSize = 64 * 1024;
    QFile *localFile;
    QByteArray inBlock;
    QByteArray outBlock;

public slots:
    void receive_receiveemail(QString,QString,QString,QString,int); //接受收件箱内容
    void receive_sendemail(QString,QString,QString,QString,int); //接受发件箱内容
    void receive_draftemail(QString,QString,QString,QString,int); //接受草稿箱内容
    void receive_collectemail(QString,QString,QString,QString,int); //接受收藏箱内容
    void receive_deleteemail(QString,QString,QString,QString,int); //接受回收站内容
    void startDownload();
private slots:
    void on_pushButton_send_clicked();
    void on_download_clicked();

signals:
    void send_draftemail(QString,QString,QString,QString); //草稿箱发送成功
};

#endif // LOOKEMAIL_H
