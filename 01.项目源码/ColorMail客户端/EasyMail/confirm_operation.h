#ifndef CONFIRM_OPERATION_H
#define CONFIRM_OPERATION_H

#include <QMainWindow>

namespace Ui {
class Confirm_operation;
}

class Confirm_operation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Confirm_operation(QWidget *parent = nullptr);
    ~Confirm_operation();

private:
    Ui::Confirm_operation *ui;
signals:
    void confirmoperate(int flag); //发送确认或取消的信号
private slots:
    void on_confirm_clicked();
    void on_cancel_clicked();
};

#endif // CONFIRM_OPERATION_H
