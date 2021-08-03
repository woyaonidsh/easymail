#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QString>

namespace Ui {
class previewpage;
}

class previewpage : public QMainWindow
{
    Q_OBJECT

public:
    explicit previewpage(QWidget *parent = nullptr);
    ~previewpage();
    void preview_showtime(void);
//    void getsubject();
private:
    Ui::previewpage *ui;
    QLabel *currenttime;
public slots:
    void preview_timerUpdate(void);
    void receivemessage(QString , QString , QString);//接受父窗口的值
};

#endif // PREVIEWPAGE_H
