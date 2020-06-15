#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "clientsocket.h"
#include "user.h"
#include "roomlistwindow.h"
#include <QMessageBox>
#include <QPaintEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void LoadUser();
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *);

private slots:
    void on_btn_regist_clicked();                               //注册按钮
    void on_btn_login_clicked();                                //登录按钮
    void onError(QAbstractSocket::SocketError socketError);     //网络链接错误信息
    void onSigReg(user_t user);                                 //注册
    void onSigLogin(user_t user);                               //登录
    void onRoomList();                                          //获取在线房间列表

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    ClientSocket *cs;
    RoomListWindow *roomWindow;
    user_t myinfo;
};
#endif // MAINWINDOW_H
