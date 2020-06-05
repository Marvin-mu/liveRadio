#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include "user.h"

/*
 * 读取服务器发送过来的数据包,并发送相应的信号
 * */
class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QTcpSocket *socket, QObject *parent = 0);

signals:
    void sigReg(user_t user);           //注册
    void sigLogin(user_t user);         //登录
    void sigRoomName(user_t user);      //房间名
    void sigRoomList(user_t user);      //房间列表
    void sigChatText(user_t user);      //文字聊天
    void sigMasterExit(user_t user);    //退出客户端

public slots:
    void onReadyRead();                 //读取服务器数据

private:
    QTcpSocket *socket;
};

#endif // CLIENTSOCKET_H
