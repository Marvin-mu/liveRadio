#include "clientsocket.h"

ClientSocket::ClientSocket(QTcpSocket *socket, QObject *parent)
    : QObject(parent), socket(socket)
{
    QObject::connect(socket, SIGNAL(readyRead()),this,SLOT(onReadyRead())); //接收服务器数据包
}

void ClientSocket::onReadyRead()
{
    user_t user;
    memset(&user, 0, sizeof(user));
    while (socket->read((char*)&user, sizeof(user)) > 0) {
        switch (user.type) {
        case REG:
            emit sigReg(user);          //注册
            break;
        case LOGIN:
            emit sigLogin(user);        //登录
            break;
        case ROOMNAME:
            emit sigRoomName(user);     //刷新直播列表，新主播开播
            break;
        case ROOMLIST:
            emit sigRoomList(user);     //房间列表
            break;
        case JOINROOM:
            emit sigJoinRoom(user);     //用户加入房间
            break;
        case QUITROOM:
            emit sigQuitRoom(user);     //用户离开房间
            break;
        case TEXT:
            emit sigChatText(user);     //公屏文字
            break;
        case QUIT:
            emit sigMasterQuit(user);   //房主退出
            break;
        case BS:
            emit sigBs(user);           //弹幕消息
            break;
        case IP:
            emit sigIp(user);
            break;
        default:break;
        }
    }
}

