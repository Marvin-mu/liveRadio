#ifndef ROOMLISTWINDOW_H
#define ROOMLISTWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include "clientsocket.h"
#include "user.h"

namespace Ui {
class RoomListWindow;
}

class RoomListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent = 0);
    void pushRoomList();//进入界面请求房间列表
    ~RoomListWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_commandLinkButton_toggled(bool checked);

    void on_commandLinkButton_userlist_toggled(bool checked);

    void on_roomlist();

    void on_roomname();

    void on_btn_send_clicked();

private:
    Ui::RoomListWindow *ui;
    ClientSocket *cs;
    QTcpSocket *socket;
    user_t myinfo;
};

#endif // ROOMLISTWINDOW_H
