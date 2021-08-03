#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonArray>
#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>

#include <QtWidgets/QWidget>
#include "ui_server.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

    QTcpServer *server;
    QTcpSocket *socket;

signals:
    void signalReshow();

public slots:
    void startserver();
    void newConnect();
    void Read();
    void Read_Send_R();
    void Read_Send_L();
    void Read_Send_A();
    void Read_Send_GC();
    void Read_GetEmail();
    void Read_GetFile();
    void Send_Email();
    void Send_File();
    void updateProgress(qint64);
    void Trans_Email();
    void Send_Draf();
    void Dele_Friend();
    void Recover_Email();
    void ReRev_Send();

private:
    Ui::Server *ui;
    QJsonArray array;
    QTcpServer* m_tcpServer;
    QTcpSocket* m_tcpSocket;
    qint64 totalBytes = 0;
    qint64 bytesReceived = 0;
    qint64 bytesWritten = 0;
    qint64 bytesToWrite = 0;
    qint64 filenameSize;
    QString filename;
    qint64 perDataSize = 64 * 1024;
    qint64 payloadSize = 64 * 1024;
    QFile *localFile;
    QByteArray inBlock;
    QByteArray outBlock;
    int File_Flag = 0;
    QString s_times;
};
#endif // SERVER_H
