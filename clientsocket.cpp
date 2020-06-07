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
            emit sigReg(user);
            break;
        case LOGIN:
            emit sigLogin(user);
            break;
        case ROOMNAME:
            emit sigRoomName(user);//开播就加入直播列表
            break;
        case ROOMLIST:
            emit sigRoomList(user);//刷新
            break;
        case JOINROOM:
            emit sigJoinRoom(user);//刷新
            break;
        case TEXT:
            emit sigChatText(user);
            break;
        case QUIT:
            emit sigMasterQuit(user);
            break;
        default:break;
        }
    }
}

