#include "server.h"
#include "ui_server.h"
#include <QDebug>
#include <qsqlquerymodel.h>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QString>
#include <QByteArray>

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    File_Flag = 0;
    s_times = "";
    m_tcpServer = new QTcpServer(this);
    m_tcpSocket = new QTcpSocket(this);
}

Server::~Server()
{
    delete ui;
}

void Server::startserver()
{
    if(!m_tcpServer->listen(QHostAddress::Any, 6666))
    {
        qDebug() << "connect error!";
    }
    else
    {
         bytesReceived = 0;
         totalBytes = 0;
         filenameSize = 0;
         connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnect()));
    }
}

void Server::newConnect()
{
    bytesWritten = 0;
    perDataSize = 64 * 1024;
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(Read()));
}

void Server::Read()
{
    if(File_Flag == 1)
    {
        Read_GetFile();//收到用户发来的邮件附件
    }
    QByteArray qba = m_tcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument doc(QJsonDocument::fromJson(qba, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        return;
    }
    array = doc.array();

    int flag = array[0].toInt();
    switch (flag) {
    case 0:
        Read_Send_R(); break;//注册
    case 1:
        Read_Send_L(); break;//登录
    case 2:
        Read_Send_A(); break;//添加好友
    case 3:
        Read_Send_GC();break;//拿取联系人
    case 4:
        Read_GetEmail();break;//收到用户发来的邮件内容
    case 5:
        Send_Email();break;//发送邮件内容
    case 6:
        Send_File();break;//发送邮件附件
    case 7:
        Trans_Email();break;//转换邮件类型
    case 8:
        Send_Draf();break;//发送草稿箱内容
    case 9:
        Dele_Friend();break;//删除好友
    case 10:
        Recover_Email();break;//恢复邮件
    case 11:
        ReRev_Send();break;//再次发送收件系统
    }
}

void Server::Read_Send_R()//注册
{
    QString r_name = array[1].toString();
    QString r_password = array[2].toString();
    QString r_realname = array[3].toString();
    QString r_phonenumber = array[4].toString();

    /*此函数用于比对数据库，如果用户传来的用户名没有被注册过，那么将用户传来的信息保存到
     * 数据库中去且flag值为1，若有被注册过则不保存且flag值为0
    */

    //检查注册是否重名
    QString sql1;
    QSqlQueryModel *model1=new QSqlQueryModel;
    sql1="select count (*) from EasyMail where name='"+r_name+"'";
    model1->setQuery(sql1);
    QModelIndex index1=model1->index(0,0);

    QJsonArray arr;
    if(index1.data()==0)//不存在
    {
        arr.append(1);
        QString sql3;
        sql3="insert into EasyMail(name,password,realname,phonenumber) values('"+r_name+"','"+r_password+"','"+r_realname+"','"+r_phonenumber+"')";
        QSqlQueryModel *model=new QSqlQueryModel;
        model->setQuery(sql3);
        qDebug() << "Seccessful!";
    }
    else
    {
        arr.append(0);
    }
    QJsonDocument docment;
    docment.setArray(arr);
    QByteArray byte_send = docment.toJson(QJsonDocument::Compact);

    m_tcpSocket->write(byte_send);

}

void Server::Read_Send_L()//登录
{
    QString l_name = array[1].toString();
    QString password = array[2].toString();

    /*此处开始比对用户发来的账号和密码，到数据库中查询，如果用户名存在且密码正确则返回0
     如果用户名不存在则返回1，如果密码不正确则返回2*/
    QString sql1,sql2;
    QSqlQueryModel *model1=new QSqlQueryModel;
    QSqlQueryModel *model2=new QSqlQueryModel;
    sql1="select count (*) from EasyMail where name='"+l_name+"'";
    model1->setQuery(sql1);
    QModelIndex index1=model1->index(0,0);
    sql2="select password from EasyMail where name ='"+l_name+"'";
    model2->setQuery(sql2);
    QModelIndex index2=model2->index(0,0);

    qDebug() << index2.data();
    QJsonArray arr;
    if(index1.data()==0)//用户名不存在
    {
        arr.append(1);
    }
    else
    {
        if(password!=index2.data())//密码不正确
        {
            arr.append(2);
        }
        else
        {
            arr.append(0);
        }
    }
    QJsonDocument docment;
    docment.setArray(arr);
    QByteArray byte_send = docment.toJson(QJsonDocument::Compact);

    m_tcpSocket->write(byte_send);
}

void Server::Read_Send_A()//添加好友
{
    QString l_name = array[1].toString();
    QString a_name = array[2].toString();
    QString a_n_name = array[3].toString();
    //qDebug() << l_name;
    //qDebug() << a_name;
    //qDebug() << a_n_name;


    /*此处开始比对用户发来的账号，到数据库中查询，如果用户名存在则返回0
     如果用户名不存在则返回1*/
    QString sql3;
    QSqlQueryModel *model3=new QSqlQueryModel;
    sql3="select count (*) from EasyMail where name='"+a_name+"'";
    model3->setQuery(sql3);
    QModelIndex index3=model3->index(0,0);


    //本用户是l_name，好友用户名为a_name,设置的好友的昵称为a_n_name;
    QJsonArray arr;

    QString sql5;
    QSqlQueryModel *model5=new QSqlQueryModel;
    sql5="select count (*) from Contact where name='"+l_name+"' and ctname='"+a_name+"'";
    model5->setQuery(sql5);
    QModelIndex index5=model5->index(0,0);

    if(index3.data()!=0)//要添加的好友存在
    {
        if(index5.data()==0)//用户存在且未添加过，添加成功
        {
            qDebug() << "5456465";
            QString sql4;
            sql4="insert into Contact(name,ctname,ctrealname) values('"+l_name+"','"+a_name+"','"+a_n_name+"')";
            QSqlQueryModel *model=new QSqlQueryModel;
            model->setQuery(sql4);
            emit signalReshow();
            this->setAttribute(Qt::WA_DeleteOnClose,1);

            arr.append(0);
        }
        else//用户存在但已添加过
        {
            arr.append(1);
        }
    }
    else//用户不存在，添加失败
    {
        arr.append(2);
    }
    QJsonDocument docment;
    docment.setArray(arr);
    QByteArray a_byte_send = docment.toJson(QJsonDocument::Compact);
    qDebug() << a_byte_send;
    m_tcpSocket->write(a_byte_send);
}

void Server::Read_Send_GC()//获取联系人和邮件信息
{
    QString l_name = array[1].toString();

    QString sql1,sql2,sql3;
    QSqlQueryModel *model1=new QSqlQueryModel;
    QSqlQueryModel *model2=new QSqlQueryModel;
    QSqlQueryModel *model3=new QSqlQueryModel;

    sql1="select count (*) from Contact where name='"+l_name+"'";
    model1->setQuery(sql1);
    QModelIndex index1=model1->index(0,0);
    QJsonArray arr;
    if(index1.data()==0)//没有好友
    {
        arr.append(0);
    }
    else
    {
        sql2="select ctname from Contact where name ='"+l_name+"'";
        model2->setQuery(sql2);
        sql3="select ctrealname from Contact where name ='"+l_name+"'";
        model3->setQuery(sql3);
        int i = 0;
        QModelIndex index2;
        QModelIndex index3;
        for(i = 0;;i++)
        {
            index2=model2->index(i,0);
            if(index2.data().toString()=="")//联系人没了
            {
                break;
            }
        }
        arr.append(i);
        for(int j = 0; j < i; j++)
        {
            index2=model2->index(j,0);
            index3=model3->index(j,0);
            arr.append(index2.data().toString());
            arr.append(index3.data().toString());
        }
    }

    //调用数据库，收件箱、发件箱、草稿箱等箱子把发/收件人和主题还有阅读状态发送过去
    QString s_name = array[1].toString();


    //传输收件箱信息
    QString sql_0="select sender,theme,send_time,read from Mail where receiver='"+s_name+"' and owner = '"+s_name+"' and (Draft = '"+"0"+"' or Draft IS NULL) and (delete_e is NULL or delete_e = '"+"0"+"')";
    QSqlQueryModel *model_0=new QSqlQueryModel;
    model_0->setQuery(sql_0);
    QModelIndex index_0;
    int count_0=0;
    for(count_0 = 0;;count_0++)
    {
        index_0=model_0->index(count_0,0);
        if(index_0.data().toString()=="") break;
    }
    arr.append(count_0);
    for(int i = 0;i<count_0;i++)
    {
        for(int j=0;j<4;j++)
        {
            index_0 = model_0->index(i,j);
            //qDebug() << index_0.data().toString();
            arr.append(index_0.data().toString());
        }
    }


    //传输发件箱信息
    QString sql_1="select receiver,theme,send_time from Mail where sender='"+s_name+"' and owner = '"+s_name+"'";
    QSqlQueryModel *model_1=new QSqlQueryModel;
    model_1->setQuery(sql_1);
    QModelIndex index_1;
    int count_1=0;
    for(count_1 = 0;;count_1++)
    {
        index_1=model_1->index(count_1,0);
        if(index_1.data().toString()=="") break;
    }
    arr.append(count_1);
    for(int i = 0;i<count_1;i++)
    {
        for(int j=0;j<3;j++)
        {
            index_1 = model_1->index(i,j);
            arr.append(index_1.data().toString());
        }
    }

    //传输草稿箱信息
    QString sql_2="select receiver,theme,send_time from Mail where sender='"+s_name+"' and Draft = '"+"1"+"' and owner = '"+s_name+"' ";
    QSqlQueryModel *model_2=new QSqlQueryModel;
    model_2->setQuery(sql_2);
    QModelIndex index_2;
    int count_2=0;
    for(count_2 = 0;;count_2++)
    {
        index_2=model_2->index(count_2,0);
        if(index_2.data().toString()=="") break;
    }
    arr.append(count_2);
    for(int i = 0;i<count_2;i++)
    {
        for(int j=0;j<3;j++)
        {
            index_2 = model_2->index(i,j);
            arr.append(index_2.data().toString());
        }
    }

    //传输收藏夹信息
    QString sql_3="select sender,theme,send_time from Mail where receiver='"+s_name+"'and collect_t IS NOT NULL and owner = '"+s_name+"'" ;
    QSqlQueryModel *model_3=new QSqlQueryModel;
    model_3->setQuery(sql_3);
    QModelIndex index_3;
    int count_3=0;
    for(count_3 = 0;;count_3++)
    {
        index_3=model_3->index(count_3,0);
        if(index_3.data().toString()=="") break;
    }
    arr.append(count_3);
    for(int i = 0;i<count_3;i++)
    {
        for(int j=0;j<3;j++)
        {
            index_3 = model_3->index(i,j);
            arr.append(index_3.data().toString());
        }
    }

    //传输回收站信息
    QString sql_4="select sender,theme,send_time from Mail where receiver='"+s_name+"' and delete_e ='"+"1"+"' and owner = '"+s_name+"'";
    QSqlQueryModel *model_4=new QSqlQueryModel;
    model_4->setQuery(sql_4);
    QModelIndex index_4;
    int count_4=0;
    for(count_4 = 0;;count_4++)
    {
        index_4=model_4->index(count_4,0);
        if(index_4.data().toString()=="") break;
    }
    arr.append(count_4);
    for(int i = 0;i<count_4;i++)
    {
        for(int j=0;j<3;j++)
        {
            index_4 = model_4->index(i,j);
            arr.append(index_4.data().toString());
        }
    }
    QJsonDocument docment;
    docment.setArray(arr);
    QByteArray byte_send = docment.toJson(QJsonDocument::Compact);
    m_tcpSocket->write(byte_send);

}

void Server::Read_GetEmail()//收取用户发来的邮件内容
{
     QString s_name = array[1].toString();//发件人
     QString r_name = array[2].toString();//收件人
     QString title = array[3].toString();//主题
     QString words = array[4].toString();//正文
     QString times = array[5].toString();//时间
     QString Draft_Flag = array[6].toString();//是否归为草稿
     s_times = times;

     //判断收件人是否存在
     QString sql2;
     QSqlQueryModel *model2=new QSqlQueryModel;
     sql2="select count (*) from EasyMail where name='"+r_name+"'";
     model2->setQuery(sql2);
     QModelIndex index2=model2->index(0,0);

     QJsonArray get_arr;
     if(index2.data()==0)//收件人不存在
     {
         get_arr.append(1);
     }
     else
     {
         get_arr.append(0);
         QString sql1;
         sql1="insert into Mail(receiver,sender,theme,content,read,send_time,Draft,owner) values('"+r_name+"','"+s_name+"','"+title+"','"+words+"','"+"未读"+"','"+times+"','"+Draft_Flag+"','"+r_name+"')";
         QSqlQueryModel *model=new QSqlQueryModel;
         model->setQuery(sql1);
         QString sqlb;
         sqlb="insert into Mail(receiver,sender,theme,content,read,send_time,Draft,owner) values('"+r_name+"','"+s_name+"','"+title+"','"+words+"','"+"未读"+"','"+times+"','"+Draft_Flag+"','"+s_name+"')";
         QSqlQueryModel *modelb=new QSqlQueryModel;
         modelb->setQuery(sqlb);
         if(array[7].toInt() == 1)
         {
             File_Flag = 1;
         }
     }
     QJsonDocument get_doc;
     get_doc.setArray(get_arr);
     QByteArray byte_get = get_doc.toJson(QJsonDocument::Compact);

     m_tcpSocket->write(byte_get);
}

void Server::Read_GetFile()//收用户发来的邮件附件
{
    QDataStream inFile(this->m_tcpSocket);
    inFile.setVersion(QDataStream::Qt_5_15);

    if (bytesReceived <= sizeof(qint64)* 2)
    {
        if ((m_tcpSocket->bytesAvailable() >= (sizeof(qint64)) * 2) && (filenameSize == 0))
        {
            inFile >> totalBytes >> filenameSize;
            bytesReceived += sizeof(qint64)* 2;
        }
        if ((m_tcpSocket->bytesAvailable() >= filenameSize) && (filenameSize != 0))
        {
            inFile >> filename;
            bytesReceived += filenameSize;

            filename = "E:/QT_Document/ServerApp/database/"+filename;
            //qDebug() << filename;

            QString sql7;
            QSqlQueryModel *model7=new QSqlQueryModel;
            sql7="select count (*) from Mail where send_time='"+s_times+"'";
            //qDebug() << s_times;
            model7->setQuery(sql7);
            QModelIndex index7=model7->index(0,0);
            if(index7.data()!=0)
            {
                QString sql6;
                sql6="update Mail set address = '"+filename+"' where send_time='"+s_times+"'";
                QSqlQueryModel *model6=new QSqlQueryModel;
                model6->setQuery(sql6);
            }

            localFile = new QFile(filename);
            if (!localFile->open(QFile::WriteOnly))
            {
                qDebug() << "Server::open file error!";
                return;
            }
        }
        else
            return;
    }
    if (bytesReceived < totalBytes)
    {
        bytesReceived += m_tcpSocket->bytesAvailable();
        inBlock = m_tcpSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    if (bytesReceived == totalBytes)
    {
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        File_Flag = 0;
        localFile->close();
    }
}

void Server::Send_Email()//发送邮件内容
{
    int s_type = array[1].toInt();
    QString s_name = array[2].toString();//发送者或接收者用户名
    QString s_theme = array[3].toString();//发送主题
    QString s_time = array[4].toString();//发送时间

    if(s_type == 1)
    {
        QString FLAG_READ ="已读";
        QString sql2="update Mail set read ='"+FLAG_READ+"' where theme = '"+s_theme+"' and send_time='"+s_time+"' and (receiver='"+s_name+"' or sender='"+s_name+"') and owner != '"+s_name+"'";
        QSqlQueryModel *model2=new QSqlQueryModel;
        model2->setQuery(sql2);
    }
    QJsonArray SE_array;

    //调用数据库比对以上三者，找到文本内容以及是否有附件，文本内容保存在SE_array[0]中，
    //有附件SE_array[1]为1，没有则为0
    QString sql8="select content from Mail where theme = '"+s_theme+"' and send_time='"+s_time+"' and (receiver='"+s_name+"' or sender='"+s_name+"' ) and owner != '"+s_name+"'";
    QSqlQueryModel *model8=new QSqlQueryModel;
    model8->setQuery(sql8);
    QModelIndex index8;
    index8=model8->index(0,0);
    SE_array.append(index8.data().toString());

    QString sql9="select address from Mail where theme = '"+s_theme+"' and send_time='"+s_time+"' and (receiver='"+s_name+"' or sender='"+s_name+"') and owner != '"+s_name+"'";
    QSqlQueryModel *model9=new QSqlQueryModel;
    model9->setQuery(sql9);
    QModelIndex index9;
    index9=model9->index(0,0);
    if(index9.data()=="") SE_array.append(0);
    else SE_array.append(1);

    QJsonDocument SE_doc;
    SE_doc.setArray(SE_array);
    QByteArray byte_SE = SE_doc.toJson(QJsonDocument::Compact);

    m_tcpSocket->write(byte_SE);
}

void Server::Send_File()//发送邮件附件
{
    QString f_theme = array[1].toString();
    QString f_time = array[2].toString();
    QString s_name = array[3].toString();
    QString fileName;//搜索数据库找到该主题和该时间的那行邮件，找出地址赋值给fileName

    QSqlQueryModel *model1=new QSqlQueryModel;
    QString sql1="select address from Mail where theme='"+f_theme+"' and send_time ='"+f_time+"' and owner != '"+s_name+"' ";
    model1->setQuery(sql1);
    QModelIndex index1=model1->index(0,0);
    fileName = index1.data().toString();

    m_tcpSocket->open(QIODevice::WriteOnly);
    connect(m_tcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64))); //数据发送成功更新进度条

    localFile=new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug()<<"client：open file error!";
        return;
    }
    totalBytes=localFile->size(); //文件数据大小
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_14); //序列化格式版本号
    QString currentFileName=fileName.right(fileName.size()-fileName.lastIndexOf('/')-1); //文件总大小、文件名大小、文件名

    sendOut<<qint64(0)<<qint64(0)<<currentFileName; //发送附件
    totalBytes+=outBlock.size(); //文件数据+文件信息大小
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64(outBlock.size()-sizeof(qint64)*2); //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    bytesToWrite=totalBytes-m_tcpSocket->write(outBlock); //发送完数据后剩余数据的大小
    outBlock.resize(0); //清空缓存区

}

void Server::updateProgress(qint64 numBytes)
{
    bytesWritten+=(int)numBytes; //已经发送数据的大小
    if(bytesToWrite>0)
    {
        outBlock=localFile->read(qMin(bytesToWrite,payloadSize)); //每次发送loadSize大小的数据，这里设置为64KB，如果剩余的数据不足64KB就发送剩余数据的大小
        bytesToWrite-=(int)m_tcpSocket->write(outBlock);//发送完一次数据后还剩余数据的大小
        outBlock.resize(0); //清空发送缓冲区
    }
    else
    {
        localFile->close(); //如果没有发送任何数据，则关闭文件
    }
    if(bytesWritten==totalBytes) //发送完毕
    {
        localFile->close();
        m_tcpSocket->close();
    }
}

void Server::Trans_Email()//转换邮件类型
{
    int TransType = array[1].toInt();
    QString E_theme = array[2].toString();
    QString E_time = array[3].toString();
    QString s_name = array[4].toString();
    if(TransType == 0)//把收件箱普通邮件转成收藏邮件
    {
        QString sql7;
        QString Tra_Flag = "1";
        sql7="update Mail set collect_t ='"+Tra_Flag+"' where theme = '"+E_theme+"' and send_time='"+E_time+"' and owner != '"+s_name+"' ";
        QSqlQueryModel *model=new QSqlQueryModel;
        model->setQuery(sql7);
    }
    else if(TransType == 1)//把收件箱普通邮件转到回收站
    {
        QString sql7;
        QString Tra_Flag = "1";
        sql7="update Mail set delete_e='"+Tra_Flag+"' where theme = '"+E_theme+"' and send_time='"+E_time+"' and owner != '"+s_name+"'";
        QSqlQueryModel *model=new QSqlQueryModel;
        model->setQuery(sql7);
    }
    else if(TransType == 2)//把回收站邮件彻底删除
    {
        QString sql7;
        sql7="delete from Mail where theme = '"+E_theme+"' and send_time='"+E_time+"' and delete_e='"+"1"+"' and owner != '"+s_name+"' ";
        QSqlQueryModel *model=new QSqlQueryModel;
        model->setQuery(sql7);
    }
}

void Server::Send_Draf()//发送草稿箱内容
{
    //将Email数据库中的Draft值改为“0”
    QString d_theme = array[1].toString();
    QString d_time = array[2].toString();
    QString n_time = array[3].toString();
    QString n_theme = array[4].toString();
    QString n_receiver = array[5].toString();
    QString n_text = array[6].toString();

    QSqlQueryModel *model1=new QSqlQueryModel;
    QString Flag_Draft = "0";
    QString sql1="update Mail set Draft='"+Flag_Draft+"',theme='"+n_theme+"',receiver='"+n_receiver+"',content='"+n_text+"',send_time ='"+n_time+"' where theme='"+d_theme+"' and send_time='"+d_time+"' ";
    model1->setQuery(sql1);

}

void Server::Dele_Friend()//删除好友
{
    QString n_name = array[1].toString();//本人
    QString f_name = array[2].toString();//好友
    //调用数据库，删除该对好友

    QString sql1;
    sql1="delete from Contact where name = '"+n_name+"' and ctname='"+f_name+"' ";
    QSqlQueryModel *model1=new QSqlQueryModel;
    model1->setQuery(sql1);

}

void Server::Recover_Email()//恢复邮件  ____要改
{
    QString r_theme = array[1].toString();//主题
    QString r_time = array[2].toString();//时间
    QString s_name = array[3].toString();//发送者

    //调用数据库，将该邮件的delete_e改为“0”
    QSqlQueryModel *model1=new QSqlQueryModel;
    QString Flag_Draft = "0";
    QString sql1="update Mail set delete_e='"+Flag_Draft+"' where theme='"+r_theme+"' and send_time='"+r_time+"' and owner != '"+s_name+"' ";
    model1->setQuery(sql1);

}

void Server::ReRev_Send()//再次发送收件系统
{
    QString s_name = array[1].toString();

    QJsonArray S_arr;
    QString sql_a="select sender,theme,send_time,read from Mail where receiver='"+s_name+"' and (Draft = '"+"0"+"' or Draft IS NULL) and (delete_e is NULL or delete_e = '"+"0"+"') and owner = '"+s_name+"'";
    QSqlQueryModel *model_a=new QSqlQueryModel;
    model_a->setQuery(sql_a);
    QModelIndex index_a;
    int count_a=0;
    for(count_a = 0;;count_a++)
    {
        index_a=model_a->index(count_a,0);
        if(index_a.data().toString()=="") break;
    }
    for(int i = 0;i<count_a;i++)
    {
        for(int j=0;j<4;j++)
        {
            index_a = model_a->index(i,j);
            S_arr.append(index_a.data().toString());
        }
    }
    QJsonDocument S_docment;
    S_docment.setArray(S_arr);
    QByteArray S_byte_send = S_docment.toJson(QJsonDocument::Compact);
    m_tcpSocket->write(S_byte_send);
}
