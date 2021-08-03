#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonArray>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Email");
    main_showtime();//显示时间
    addpeople = new addfriend(this); //addfriend页面指针
    sentemail = new sentEmail(this); //sentemail页面指针
    preview = new  previewpage(this); //预览界面指针
    look_email = new lookemail(this); //查看邮件内容界面
    confirm = new Confirm_operation(this); //确认窗口
    QObject::connect(addpeople,SIGNAL(successaddfriend(QString,QString)),this,SLOT(receiveuser(QString,QString)));//连接addfriend与email窗口
    connect(this,SIGNAL(sentusermessage(QListWidget *)),sentemail,SLOT(receiveusermessage(QListWidget *)));//连接email与sentemail界面
    connect(this,SIGNAL(sendname(QString)),addpeople,SLOT(receivename(QString))); //发送注册用户的用户名
    connect(this,SIGNAL(sendname(QString)),sentemail,SLOT(receivername(QString)));
    connect(this , SIGNAL(sendID_and_name(QString,QString)),sentemail,SLOT(receiveID_and_name(QString,QString))); //向sentemail界面发送添加成功的用户信息
    connect(sentemail,SIGNAL(return_receivemessage(QString,QString,QString)),this,SLOT(receive_receivemessage(QString,QString,QString)));
    connect(sentemail,SIGNAL(return_draftmessage(QString,QString,QString)),this,SLOT(receive_draftmessage(QString,QString,QString)));
    connect(confirm,SIGNAL(confirmoperate(int)),this,SLOT(receive_confirm(int)));
    connect(look_email,SIGNAL(send_draftemail(QString,QString,QString,QString)),this,SLOT(receive_draftemail(QString,QString,QString,QString)));
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpdate(void) //更新时间
{
    QDateTime time = QDateTime::currentDateTime();
    QString timestr = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    currentTimeLabel->setText(timestr);
}

void MainWindow::main_showtime(void)
{
    currentTimeLabel = new QLabel;
    ui->statusBar->addWidget(currentTimeLabel);
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
}

void MainWindow::initialpage(QString data) //加载初始界面数据
{
    name = data;
    //联系人部分：
    c_tcpSocket = new QTcpSocket(this);
    c_tcpSocket->abort();
    c_tcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    QJsonArray server_ar;
    server_ar.append(3);
    server_ar.append(name);
    QJsonDocument con_doc;
    con_doc.setArray(server_ar);
    QByteArray con_bt = con_doc.toJson(QJsonDocument::Compact);
    c_tcpSocket->write(con_bt);
    connect(c_tcpSocket, SIGNAL(readyRead()), this, SLOT(contacter_get()));
    ui->user->setText("用户名");
    ui->information->setText("主题");
    ui->time->setText("时间");
}

void MainWindow::contacter_get()
{
    QByteArray con_qba = c_tcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument con_getdoc(QJsonDocument::fromJson(con_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    con_array = con_getdoc.array();
    int item = 0;
    if( friendflag )//加载好友信息
    {
        QString userID; //用户ID
        QString username; //用户名
        int user = 0;
        for(user = 1 ; user <= con_array[0].toInt() *2 ; user++)
        {
            if(user % 2 ==1 )
            {
                userID = con_array[user].toString(); // 得到用户ID
            }
            else
            {
                username = con_array[user].toString(); //得到用户名
                QString userimformation = username +"     " +userID;
                ui->listWidget_friend->addItem(userimformation); //加载用户信息
            }
        }
        item = user;
        friendflag =0;
    }
    if(RevBoxFlag)//加载收件箱信息
    {
        QString Send_ID;
        QString Send_Theme;
        QString Send_Time;
        QString Read_Flag;
        int user = 0;
        for(user = item + 1; user <= ((con_array[item].toInt()) * 4 + item); user++)
        {
            if((user - item - 1) % 4 == 0)
            {
                Send_ID = con_array[user].toString();
            }
            else if((user - item - 1) % 4 == 1)
            {
                Send_Theme = con_array[user].toString();
            }
            else if((user - item - 1) % 4 == 2)
            {
                Send_Time = con_array[user].toString();
            }
            else
            {
                Read_Flag = con_array[user].toString();
                QString Revimformation = Send_ID + "  " + Send_Theme + "  " + Send_Time;
                ui->listWidget_receive->addItem(Revimformation);
                ui->listWidget_receive->setCurrentRow(ui->listWidget_receive->currentRow()+1);
                if( Read_Flag == "未读" )
                {
                    int row = ui->listWidget_receive->currentRow();
                    ui->listWidget_receive->item(row)->setBackground(QColor("red"));
                }
            }
        }
        item = user;
        RevBoxFlag = 0;
    }
    if(SendBoxFlag)//加载发件箱信息
    {
        QString Rev_ID;
        QString Email_Theme;
        QString Send_Times;
        int user = 0;
        for(user = item + 1; user <= con_array[item].toInt() * 3 + item; user++)
        {
            if((user - item - 1) % 3 == 0)
            {
                Rev_ID = con_array[user].toString();
            }
            else if((user - item - 1) % 3 == 1)
            {
                Email_Theme = con_array[user].toString();
            }
            else
            {
                Send_Times = con_array[user].toString();
                QString Sendimformation =Rev_ID+"  "+Email_Theme + "  " + Send_Times;
                ui->listWidget_send->addItem(Sendimformation);
            }
        }
        item = user;
        SendBoxFlag = 0;
    }
    if(DrafBoxFlag)//加载草稿箱信息
    {
        QString D_Rev_ID;
        QString D_Email_Theme;
        QString D_Send_Times;
        int user = 0;
        for(user = item + 1; user <= con_array[item].toInt() * 3 + item; user++)
        {
            if((user - item - 1) % 3 == 0)
            {
                D_Rev_ID = con_array[user].toString();
            }
            else if((user - item - 1) % 3 == 1)
            {
                D_Email_Theme = con_array[user].toString();
            }
            else
            {
                D_Send_Times = con_array[user].toString();
                QString D_Sendimformation = D_Rev_ID + "  " + D_Email_Theme + "  " + D_Send_Times;
                ui->listWidget_draft->addItem(D_Sendimformation);
            }
        }
        item = user;
        DrafBoxFlag = 0;
    }
    if(CollectFlag)//加载收藏夹信息
    {
        QString C_Send_ID;
        QString C_Send_Theme;
        QString C_Send_Times;
        int user = 0;
        for(user = item + 1; user <= con_array[item].toInt() * 3 + item; user++)
        {
            if((user - item - 1) % 3 == 0)
            {
                C_Send_ID = con_array[user].toString();
            }
            else if((user - item - 1) % 3 == 1)
            {
                C_Send_Theme = con_array[user].toString();
            }
            else
            {
                C_Send_Times = con_array[user].toString();
                QString C_Sendimformation = C_Send_ID + "  " + C_Send_Theme + "  " + C_Send_Times;
                ui->listWidget_collect->addItem(C_Sendimformation);
            }
        }
        item = user;
        CollectFlag = 0;
    }
    if(DeleBoxFlag)//加载回收站信息
    {
        QString Dele_ID;
        QString Dele_Theme;
        QString Dele_Send_Times;
        int user = 0;
        for(user = item + 1; user <= con_array[item].toInt() * 3 + item; user++)
        {
            if((user - item - 1) % 3 == 0)
            {
                Dele_ID = con_array[user].toString();
            }
            else if((user - item - 1) % 3 == 1)
            {
                Dele_Theme = con_array[user].toString();
            }
            else
            {
                Dele_Send_Times = con_array[user].toString();
                QString Deleimformation = Dele_ID + "  " + Dele_Theme + "  " + Dele_Send_Times;
                ui->listWidget_recycle->addItem(Deleimformation);
            }
        }
        item = user;
        DeleBoxFlag = 0;
    }
    ui->listWidget_receive->show();  //显示收件箱
    ui->listWidget_collect->hide();
    ui->listWidget_draft->hide();
    ui->listWidget_friend->hide();
    ui->listWidget_recycle->hide();
    ui->listWidget_send->hide();
    ui->listWidget_showsomething->hide();
}

void MainWindow::receiveuser(QString ID , QString name) //添加用户成功后信息返回email
{
    QString userimformation = name + "     " + ID;
    ui->listWidget_friend->addItem(userimformation); //添加用户信息
    emit sendID_and_name(ID,name); //向sentemail界面发送添加好友的信息
}

void MainWindow::ReRev_get()//再次获取收件箱内容
{
    QByteArray Rev_qba = ReRevtcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument Rev_getdoc(QJsonDocument::fromJson(Rev_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray Rev_array;
    Rev_array = Rev_getdoc.array();
    //Rev_array[0]是发件人、Rev_array[1]是主题、Rev_array[2]是发送时间、Rev_array[3]是是否已读
    ui->listWidget_receive->clear();
    QString username =NULL;
    QString theme = NULL;
    QString time = NULL;
    QString read = NULL;
    for(int i=0; i<Rev_array.size();i++)
    {
        if( i % 4 == 0 )
        {
            username = Rev_array[i].toString();
        }
        else if ( i % 4 ==1 )
        {
            theme = Rev_array[i].toString();
        }
        else if ( i % 4 ==2 )
        {
            time = Rev_array[i].toString();
        }
        else
        {
            QString information = username+"  "+theme+"  "+time;
            ui->listWidget_receive->addItem(information);
            ui->listWidget_receive->setCurrentRow(ui->listWidget_receive->currentRow()+1);
            if( Rev_array[i].toString() == "未读" )
            {
                int row = ui->listWidget_receive->currentRow();
                ui->listWidget_receive->item(row)->setBackground(QColor("red"));
            }
        }
    }
    ui->listWidget_receive->show(); //显示收件箱
    ui->listWidget_collect->hide();
    ui->listWidget_draft->hide();
    ui->listWidget_friend->hide();
    ui->listWidget_recycle->hide();
    ui->listWidget_send->hide();
    ui->listWidget_showsomething->hide();
}

void MainWindow::on_listWidget_menu_itemClicked(QListWidgetItem *item) //点击菜单栏显示对应的listWidget界面内容
{
    int row = ui->listWidget_menu->currentRow();
    if(row == 1) //收件箱
    {
        ReRevtcpSocket = new QTcpSocket(this);
        ReRevtcpSocket->abort();
        ReRevtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
        QJsonArray Rerev_ar;
        Rerev_ar.append(11);
        Rerev_ar.append(name);
        QJsonDocument Rerev_doc;
        Rerev_doc.setArray(Rerev_ar);
        QByteArray Rerev_bt = Rerev_doc.toJson(QJsonDocument::Compact);
        ReRevtcpSocket->write(Rerev_bt);
        connect(ReRevtcpSocket, SIGNAL(readyRead()), this, SLOT(ReRev_get()));
        ui->user->setText("用户名");
        ui->information->setText("主题");
        ui->time->setText("时间");
    }
    if( row == 3 ) //发件箱
    {
        ui->user->setText("用户名");
        ui->information->setText("主题");
        ui->time->setText("时间");
        ui->listWidget_receive->hide();
        ui->listWidget_collect->hide();
        ui->listWidget_draft->hide();
        ui->listWidget_friend->hide();
        ui->listWidget_recycle->hide();
        ui->listWidget_send->show(); //显示发件箱
        ui->listWidget_showsomething->hide();
    }
    if(row == 5 ) //草稿箱
    {
        ui->user->setText("用户名");
        ui->information->setText("主题");
        ui->time->setText("时间");
        ui->listWidget_receive->hide();
        ui->listWidget_collect->hide();
        ui->listWidget_draft->show(); //显示草稿箱
        ui->listWidget_friend->hide();
        ui->listWidget_recycle->hide();
        ui->listWidget_send->hide();
        ui->listWidget_showsomething->hide();
    }
    if( row == 7 ) //收藏邮件
    {
        ui->user->setText("用户名");
        ui->information->setText("主题");
        ui->time->setText("时间");
        ui->listWidget_receive->hide();
        ui->listWidget_collect->show(); //显示收藏箱
        ui->listWidget_draft->hide();
        ui->listWidget_friend->hide();
        ui->listWidget_recycle->hide();
        ui->listWidget_send->hide();
        ui->listWidget_showsomething->hide();
    }
    if( row == 9 ) //回收站
    {
        ui->user->setText("用户名");
        ui->information->setText("主题");
        ui->time->setText("时间");
        ui->listWidget_receive->hide();
        ui->listWidget_collect->hide();
        ui->listWidget_draft->hide();
        ui->listWidget_friend->hide();
        ui->listWidget_recycle->show(); //显示回收站
        ui->listWidget_send->hide();
        ui->listWidget_showsomething->hide();
    }
    if(row == 11 ) //联系人
    {
        ui->user->clear();
        ui->time->clear();
        ui->information->setText("联系人");
        ui->listWidget_friend->show(); //显示联系人窗口
        ui->listWidget_showsomething->hide();
        ui->listWidget_collect->hide();
        ui->listWidget_draft->hide();
        ui->listWidget_receive->hide();
        ui->listWidget_recycle->hide();
        ui->listWidget_send->hide();
    }
}

void MainWindow::on_pushButton_newcreat_clicked()
{
    if( newcreatflag )
    {
        emit sentusermessage(ui->listWidget_friend); //将listWidget_friend的指针传递给sentemail
        newcreatflag = 0; //关闭接受主页面的联系人传值
    }
    sentemail->show();
    emit sendname(name);
}

void MainWindow::on_pushButton_send_clicked(bool checked)
{
    emit sendname(name);
    addpeople->show();
}

void MainWindow::receive_receivemessage(QString user,QString subject,QString time) //接收sentemail返回的发件箱信息
{
    QString information = user+"  "+subject+"  "+time;
    ui->listWidget_send->addItem(information);
}

void MainWindow::receive_draftmessage(QString user, QString subject , QString time) //接受存草稿的信息
{
    QString information = user+"  "+subject+"  "+time;
    ui->listWidget_draft->addItem(information);
}

void MainWindow::on_pushButton_collect_clicked() //只有收件箱的邮件能被收藏 ????数据库标记为收藏
{
    if(ui->listWidget_menu->currentRow() == 1) //收件箱
    {
        int row = ui->listWidget_receive->currentRow();
        QString information = ui->listWidget_receive->item(row)->text();
        ui->listWidget_collect->addItem(information);
        QString username=NULL;
        QString theme=NULL;
        QString time=NULL;
        int i =0;
        for(i=0 ; i<information.size() ;i++) //得到用户名
        {
            if( information[i]!=' ' )
            {
                username+=information[i];
            }
            else
            {
                break;
            }
        }
        for(;i<information.size(); i++)
        {
            if( information[i]!=' ' )
            {
                break;
            }
        }
        for(; i <information.size();i++) //得到主题
        {
            if( information[i]!=' ' )
            {
                theme+=information[i];
            }
            else
            {
                break;
            }
        }
        for(;i<information.size(); i++)
        {
            if( information[i]!=' ' )
            {
                break;
            }
        }
        for(;i<information.size();i++)  //得到时间
        {
           time+=information[i];
        }
        coltcpSocket = new QTcpSocket(this);
        coltcpSocket->abort();
        coltcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
        QJsonArray col_ar;
        col_ar.append(7);
        col_ar.append(0);
        col_ar.append(theme);//??????????????????需要预收藏邮件的发送人、主题、时间
        col_ar.append(time);
        col_ar.append(username);
        QJsonDocument col_doc;
        col_doc.setArray(col_ar);
        QByteArray col_bt = col_doc.toJson(QJsonDocument::Compact);
        coltcpSocket->write(col_bt);
    }
}

void MainWindow::receive_confirm(int flag)
{
    receive_confirmoperate = flag;
    if( ui->listWidget_menu->currentRow() == 1 )
    {
        if( receive_confirmoperate )
        {
            int row = ui->listWidget_receive->currentRow();
            QString information = ui->listWidget_receive->item(row)->text();
            ui->listWidget_recycle->addItem(information);
            ui->listWidget_receive->takeItem(row);
            QString username=NULL;
            QString theme=NULL;
            QString time=NULL;
            int i =0;
            for(i=0 ; i<information.size() ;i++) //得到用户名
            {
                if( information[i]!=' ' )
                {
                    username+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(; i <information.size();i++) //得到主题
            {
                if( information[i]!=' ' )
                {
                    theme+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(;i<information.size();i++)  //得到时间
            {
                 time+=information[i];
            }
            deRtcpSocket = new QTcpSocket(this);
            deRtcpSocket->abort();
            deRtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
            QJsonArray deR_ar;
            deR_ar.append(7);
            deR_ar.append(1);
            deR_ar.append(theme);
            deR_ar.append(time);
            deR_ar.append(username);
            qDebug() << time;
            QJsonDocument deR_doc;
            deR_doc.setArray(deR_ar);
            QByteArray deR_bt = deR_doc.toJson(QJsonDocument::Compact);
            deRtcpSocket->write(deR_bt);
            receive_confirmoperate = 0;
        }
    }
    if( ui->listWidget_menu->currentRow() == 7 )
    {
        if( receive_confirmoperate )
        {
            int row = ui->listWidget_collect->currentRow();
            QString information = ui->listWidget_collect->item(row)->text();
            ui->listWidget_recycle->addItem(information);
            ui->listWidget_collect->takeItem(row);
            QString username=NULL;
            QString theme=NULL;
            QString time=NULL;
            int i =0;
            for(i=0 ; i<information.size() ;i++) //得到用户名
            {
                if( information[i]!=' ' )
                {
                    username+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(; i <information.size();i++) //得到主题
            {
                if( information[i]!=' ' )
                {
                    theme+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(;i<information.size();i++)  //得到时间
            {
              time+=information[i];
            }
            deCtcpSocket = new QTcpSocket(this);
            deCtcpSocket->abort();
            deCtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
            QJsonArray deC_ar;
            deC_ar.append(7);
            deC_ar.append(1);
            deC_ar.append(theme);
            deC_ar.append(time);
            deC_ar.append(username);
            QJsonDocument deC_doc;
            deC_doc.setArray(deC_ar);
            QByteArray deC_bt = deC_doc.toJson(QJsonDocument::Compact);
            deCtcpSocket->write(deC_bt);
            receive_confirmoperate = 0;
        }
    }
    if( ui->listWidget_menu->currentRow() == 11 )
    {
        if( receive_confirmoperate )
         {
            int row = ui->listWidget_friend->currentRow();
            QString information = ui->listWidget_friend->item(row)->text();
    //        ui->listWidget_recycle->addItem(information);
            ui->listWidget_friend->takeItem(row);
            QString username=NULL;
            QString theme=NULL;
            QString time=NULL;
            int i =0;
            for(i=0 ; i<information.size() ;i++) //得到用户名
            {
                if( information[i]!=' ' )
                {
                    username+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(; i <information.size();i++) //得到主题
            {
                  theme+=information[i];
            }
            deFtcpSocket = new QTcpSocket(this);
            deFtcpSocket->abort();
            deFtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
            QJsonArray deF_ar;
            deF_ar.append(9);
            deF_ar.append(name);
            deF_ar.append(theme);
            QJsonDocument deF_doc;
            deF_doc.setArray(deF_ar);
            QByteArray deF_bt = deF_doc.toJson(QJsonDocument::Compact);
            deFtcpSocket->write(deF_bt);
            receive_confirmoperate = 0;
        }
    }
    if( ui->listWidget_menu->currentRow() == 9 )
    {
        if( receive_confirmoperate )
        {
            int row = ui->listWidget_recycle->currentRow();
            QString information = ui->listWidget_recycle->item(row)->text();
            ui->listWidget_recycle->takeItem(row);
            QString username=NULL;
            QString theme=NULL;
            QString time=NULL;
            int i =0;
            for(i=0 ; i<information.size() ;i++) //得到用户名
            {
                if( information[i]!=' ' )
                {
                    username+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(; i <information.size();i++) //得到主题
            {
                if( information[i]!=' ' )
                {
                    theme+=information[i];
                }
                else
                {
                    break;
                }
            }
            for(;i<information.size(); i++)
            {
                if( information[i]!=' ' )
                {
                    break;
                }
            }
            for(;i<information.size();i++)  //得到时间
            {
                time+=information[i];

            }
            deDtcpSocket = new QTcpSocket(this);
            deDtcpSocket->abort();
            deDtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
            QJsonArray deD_ar;
            deD_ar.append(7);
            deD_ar.append(2);
            deD_ar.append(theme);
            deD_ar.append(time);
            deD_ar.append(username);
            QJsonDocument deD_doc;
            deD_doc.setArray(deD_ar);
            QByteArray deD_bt = deD_doc.toJson(QJsonDocument::Compact);
            deDtcpSocket->write(deD_bt);
            receive_confirmoperate = 0;
        }
    }

}

void MainWindow::on_pushButton_delete_clicked() //删除按钮 ?????数据库标记为删除
{
    confirm->show(); // 显示确认窗口
}

void MainWindow::on_listWidget_receive_itemDoubleClicked(QListWidgetItem *item) //双击收件箱信件查看信件内容 ???????信件内容+附件
{
    QString information  = item->text();
    emailinformation = item;
    RdretcpSocket = new QTcpSocket(this);
    RdretcpSocket->abort();
    RdretcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    connect(RdretcpSocket, SIGNAL(readyRead()), this, SLOT(Rev_Rev()));
    QJsonArray Rdre_ar;
    Rdre_ar.append(5);
    Rdre_ar.append(1);
    QString username=NULL;
    QString theme=NULL;
    QString time=NULL;
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    Rdre_ar.append(username);
    Rdre_ar.append(theme);//??????????????????需要邮件的主题、时间
    Rdre_ar.append(time);
    QJsonDocument Rdre_doc;
    Rdre_doc.setArray(Rdre_ar);
    QByteArray Rdre_bt = Rdre_doc.toJson(QJsonDocument::Compact);
    RdretcpSocket->write(Rdre_bt);
    //RdretcpSocket->readyRead();
    int row = ui->listWidget_receive->currentRow();
    ui->listWidget_receive->item(row)->setBackground(QColor("white"));  //已读邮件
}

void MainWindow::Rev_Rev()
{
    QByteArray Rdre_qba = RdretcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument Rdre_getdoc(QJsonDocument::fromJson(Rdre_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray Rdre_array = Rdre_getdoc.array();
    //内容是Rdre_array[0],是否有附件是Rdre_array[1],有附件则值为1
    QString text = Rdre_array[0].toString();
    int file = Rdre_array[1].toInt();
    qDebug() << file ;
    connect(this,SIGNAL(look_receiveemail(QString,QString,QString,QString,int)),look_email,SLOT(receive_receiveemail(QString,QString,QString,QString,int)));
    lookemail_information(emailinformation , 0 ,text , file);
}

void MainWindow::on_listWidget_send_itemDoubleClicked(QListWidgetItem *item) //双击发件箱信件查看信件内容 ????????信件内容+附件
{
    emailinformation = item;
    RdsdtcpSocket = new QTcpSocket(this);
    RdsdtcpSocket->abort();
    RdsdtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    connect(RdsdtcpSocket, SIGNAL(readyRead()), this, SLOT(Rev_Send()));
    QJsonArray Rdsd_ar;
    Rdsd_ar.append(5);
    Rdsd_ar.append(0);
    QString username;
    QString theme;
    QString time;
    QString information  = item->text();
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    Rdsd_ar.append(username);
    Rdsd_ar.append(theme);//??????????????????需要邮件的主题、时间
    Rdsd_ar.append(time);
    QJsonDocument Rdsd_doc;
    Rdsd_doc.setArray(Rdsd_ar);
    QByteArray Rdsd_bt = Rdsd_doc.toJson(QJsonDocument::Compact);
    RdsdtcpSocket->write(Rdsd_bt);
}

void MainWindow::Rev_Send()
{
    QByteArray Rdsd_qba = RdsdtcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument Rdsd_getdoc(QJsonDocument::fromJson(Rdsd_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray Rdsd_array = Rdsd_getdoc.array();
    //内容是Rdsd_array[0],是否有附件是Rdsd_array[1],有附件则值为1
    QString text = Rdsd_array[0].toString();
    int file = Rdsd_array[1].toInt();
    connect(this,SIGNAL(look_sendemail(QString,QString,QString,QString,int)),look_email,SLOT(receive_sendemail(QString,QString,QString,QString,int)));
    lookemail_information(emailinformation , 1 ,text , file);
}

void MainWindow::lookemail_information(QListWidgetItem *item , int flag ,QString text,int file)
{
    QString information = item->text();
    QString username=NULL;
    QString theme=NULL;
    QString time=NULL;
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    switch (flag)
    {
    case 0: emit look_receiveemail(username,theme,time,text,file); break; //查看收件箱内容
    case 1: emit look_sendemail(username,theme,time,text,file); break; //查看发件箱内容
    case 2: emit look_draftemail(username,theme,time,text,file); break; //查看草稿箱内容
    case 3: emit look_collectemail(username ,theme,time,text,file); break; //查看收藏箱内容
    case 4: emit look_deleteemail(username,theme,time,text,file); break; //查看回收站内容
    }
}

void MainWindow::on_listWidget_friend_itemDoubleClicked(QListWidgetItem *item) //双击联系人转到sentemail界面
{
    QObject::connect(this,SIGNAL(sentreceivermessage(QString)),sentemail,SLOT(receivermessage(QString)));//连接email与sentemail窗口
    QString receiver = item->text();
    QString receivername = NULL;
    for(int i=0 ; i<receiver.size();i++)
    {
        if( receiver[i]==' ' and receiver[i+1]!=' ' )
        {
            for( int j = i+1 ; j < receiver.size() ; j++ )
            {
                receivername+=receiver[j];
            }
            break;
        }
    }
    if( newcreatflag )
    {
        emit sentusermessage(ui->listWidget_friend); //将listWidget_friend的指针传递给sentemail
        newcreatflag = 0; //关闭接受主页面的联系人传值
    }
    emit sentreceivermessage(receivername);
    sentemail->show();
    emit sendname(name);
}

void MainWindow::on_listWidget_draft_itemDoubleClicked(QListWidgetItem *item) //双击草稿箱查看信件内容
{
    emailinformation = item;
    RdDrtcpSocket = new QTcpSocket(this);
    RdDrtcpSocket->abort();
    RdDrtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    connect(RdDrtcpSocket, SIGNAL(readyRead()), this, SLOT(Rev_Drf()));
    QJsonArray RdDr_ar;
    RdDr_ar.append(5);
    RdDr_ar.append(0);
    QString username;
    QString theme;
    QString time;
    QString information  = item->text();
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    RdDr_ar.append(username);
    RdDr_ar.append(theme);//??????????????????需要邮件的主题、时间
    RdDr_ar.append(time);
    QJsonDocument RdDr_doc;
    RdDr_doc.setArray(RdDr_ar);
    QByteArray RdDr_bt = RdDr_doc.toJson(QJsonDocument::Compact);
    RdDrtcpSocket->write(RdDr_bt);
}

void MainWindow::Rev_Drf()
{
    QByteArray RdDr_qba = RdDrtcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument RdDr_getdoc(QJsonDocument::fromJson(RdDr_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray RdDr_array = RdDr_getdoc.array();
    //内容是RdDr_array[0],是否有附件是RdDr_array[1],有附件则值为1
    QString text = RdDr_array[0].toString();
    int file = RdDr_array[1].toInt();
    connect(this,SIGNAL(look_draftemail(QString,QString,QString,QString,int)),look_email,SLOT(receive_draftemail(QString,QString,QString,QString,int)));
    lookemail_information(emailinformation , 2 ,text , file);
}

void MainWindow::on_listWidget_collect_itemDoubleClicked(QListWidgetItem *item) //双击收藏箱查看信件内容
{
    emailinformation = item;
    RdCotcpSocket = new QTcpSocket(this);
    RdCotcpSocket->abort();
    RdCotcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    connect(RdCotcpSocket, SIGNAL(readyRead()), this, SLOT(Rev_Col()));
    QJsonArray RdCo_ar;
    RdCo_ar.append(5);
    RdCo_ar.append(0);
    QString username;
    QString theme;
    QString time;
    QString information  = item->text();
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    RdCo_ar.append(username);
    RdCo_ar.append(theme);//??????????????????需要邮件的主题、时间
    RdCo_ar.append(time);
    QJsonDocument RdCo_doc;
    RdCo_doc.setArray(RdCo_ar);
    QByteArray RdCo_bt = RdCo_doc.toJson(QJsonDocument::Compact);
    RdCotcpSocket->write(RdCo_bt);
}

void MainWindow::Rev_Col()
{
    QByteArray RdCo_qba = RdCotcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument RdCo_getdoc(QJsonDocument::fromJson(RdCo_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray RdCo_array = RdCo_getdoc.array();
    //内容是Rdre_array[0],是否有附件是Rdre_array[1],有附件则值为1
    QString text = RdCo_array[0].toString();
    int file = RdCo_array[1].toInt();
    connect(this,SIGNAL(look_collectemail(QString,QString,QString,QString,int)),look_email,SLOT(receive_collectemail(QString,QString,QString,QString,int)));
    lookemail_information(emailinformation ,3 , text,file);
}

void MainWindow::on_listWidget_recycle_itemDoubleClicked(QListWidgetItem *item) //双击回收站查看信件内容
{
    emailinformation = item;
    RdDetcpSocket = new QTcpSocket(this);
    RdDetcpSocket->abort();
    RdDetcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器
    connect(RdDetcpSocket, SIGNAL(readyRead()), this, SLOT(Rev_Del()));
    QJsonArray RdDe_ar;
    RdDe_ar.append(5);
    RdDe_ar.append(0);
    QString username;
    QString theme;
    QString time;
    QString information  = item->text();
    int i =0;
    for(i=0 ; i<information.size() ;i++) //得到用户名
    {
        if( information[i]!=' ' )
        {
            username+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(; i <information.size();i++) //得到主题
    {
        if( information[i]!=' ' )
        {
            theme+=information[i];
        }
        else
        {
            break;
        }
    }
    for(;i<information.size(); i++)
    {
        if( information[i]!=' ' )
        {
            break;
        }
    }
    for(;i<information.size();i++)  //得到时间
    {
          time+=information[i];
    }
    RdDe_ar.append(username);
    RdDe_ar.append(theme);//??????????????????需要邮件的主题、时间
    RdDe_ar.append(time);
    QJsonDocument RdDe_doc;
    RdDe_doc.setArray(RdDe_ar);
    QByteArray RdDe_bt = RdDe_doc.toJson(QJsonDocument::Compact);
    RdDetcpSocket->write(RdDe_bt);
}

void MainWindow::Rev_Del()
{
    QByteArray RdDe_qba = RdDetcpSocket->readAll();
    QJsonParseError json_error;
    QJsonDocument RdDe_getdoc(QJsonDocument::fromJson(RdDe_qba, &json_error));
    if(json_error.error != QJsonParseError::NoError) //发生错误
    {
        return;
    }
    QJsonArray RdDe_array = RdDe_getdoc.array();
    //内容是Rdre_array[0],是否有附件是Rdre_array[1],有附件则值为1
    QString text = RdDe_array[0].toString();
    int file = RdDe_array[1].toInt();
    connect(this,SIGNAL(look_deleteemail(QString,QString,QString,QString,int)),look_email,SLOT(receive_deleteemail(QString,QString,QString,QString,int)));
    lookemail_information(emailinformation ,4 , text,file);
}

void MainWindow::on_pushButton_recover_clicked() //恢复接数据库？？？？？？？？？？？
{
    if( ui->listWidget_menu->currentRow() == 9 )
    {
        int row = ui->listWidget_recycle->currentRow();
        QString information = ui->listWidget_recycle->item(row)->text();
        QString username=NULL;
        QString theme=NULL;
        QString time=NULL;
        int i =0;
        for(i=0 ; i<information.size() ;i++) //得到用户名
        {
            if( information[i]!=' ' )
            {
                username+=information[i];
            }
            else
            {
                break;
            }
        }
        for(;i<information.size(); i++)
        {
            if( information[i]!=' ' )
            {
                break;
            }
        }
        for(; i <information.size();i++) //得到主题
        {
            if( information[i]!=' ' )
            {
                theme+=information[i];
            }
            else
            {
                break;
            }
        }
        for(;i<information.size(); i++)
        {
            if( information[i]!=' ' )
            {
                break;
            }
        }
        for(;i<information.size();i++)  //得到时间
        {
             time+=information[i];
        }
        ui->listWidget_recycle->takeItem(row);
        ui->listWidget_receive->addItem(username+"  "+theme+"  "+time);
        RecovtcpSocket = new QTcpSocket(this);
        RecovtcpSocket->abort();
        RecovtcpSocket->connectToHost("192.168.43.188", 6666); //连接服务器

        QJsonArray Recov_arr;
        Recov_arr.append(10);
        Recov_arr.append(theme);
        Recov_arr.append(time);
        Recov_arr.append(username);
        QJsonDocument Recov_doc;
        Recov_doc.setArray(Recov_arr);
        QByteArray Recov_bt = Recov_doc.toJson(QJsonDocument::Compact);
        RecovtcpSocket->write(Recov_bt);
    }
}

void MainWindow::receive_draftemail(QString username,QString theme , QString time ,QString old_time ) //草稿箱发送保存到发件箱
{
    QString information = username+"  "+theme+"  "+old_time;
    qDebug() << information;
    ui->listWidget_send->addItem(username+"  "+theme+"  "+time); //接收草稿箱信息
    for(int i =0 ; i<ui->listWidget_draft->count();i++)
    {
        if( information == ui->listWidget_draft->item(i)->text() )
        {
            ui->listWidget_draft->takeItem(i);
            break;
        }
    }
}
