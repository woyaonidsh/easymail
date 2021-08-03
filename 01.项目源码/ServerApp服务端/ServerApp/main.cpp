#include "server.h"

#include <QApplication>
#include<QDebug>
#include<QTcpSocket>
#include<QSqlDatabase>
#include<QSqlQuery>
bool opendatabase();
int main(int argc, char *argv[])
{
    opendatabase();
    QApplication a(argc, argv);
    Server s;
    s.startserver();
    s.show();
    return a.exec();
}
bool opendatabase()
{
    QSqlDatabase mydb=QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("E:\\QT_Document\\EasyMail\\EasyMail.db");
    if(mydb.open())
    {
        qDebug()<<"open success";
        return true;
    }
    else
    {
        qDebug()<<"open failed";
        return false;
    }
}
