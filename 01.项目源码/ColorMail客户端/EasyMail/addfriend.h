#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QListWidgetItem>
#include <QString>
#include <QAbstractSocket>
#include <QFileDialog>
#include <QtNetwork>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

namespace Ui {
class addfriend;
}

class addfriend : public QMainWindow
{
    Q_OBJECT

public:
    explicit addfriend(QWidget *parent = nullptr);
    ~addfriend();
    void showtime();
public slots:
    void timerUpdate(void);
    void receivename(QString);//接受用户名
private slots:
    void on_pushButton_clicked();
    void add_GetTorF();

private:
    Ui::addfriend *ui;
    QLabel *currentTimeLabel;
    QTcpSocket *tcpClient;
    QString name;
signals:
    void successaddfriend(QString,QString); //向email界面传送用户信息
};

#endif // ADDFRIEND_H
