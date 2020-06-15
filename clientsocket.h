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
    void sigRoomName(user_t user);      //开播上传房间名
    void sigRoomList(user_t user);      //房间列表
    void sigJoinRoom(user_t user);      //处理用户加入房间
    void sigQuitRoom(user_t user);      //处理用户离开房间
    void sigChatText(user_t user);      //文字聊天
    void sigMasterQuit(user_t user);    //关闭房间
    void sigBs(user_t);                 //弹幕
    void sigIp(user_t);

public slots:
    void onReadyRead();                 //读取服务器数据

private:
    QTcpSocket *socket;
    user_t myinfo;
};

#endif // CLIENTSOCKET_H
