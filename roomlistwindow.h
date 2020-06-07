#ifndef ROOMLISTWINDOW_H
#define ROOMLISTWINDOW_H

#include <QWidget>
#include <QVector>
#include <QCloseEvent>
#include <QListWidgetItem>
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
    void on_roomlist(user_t);
    void on_roomname(user_t);
    void on_roomquit(user_t);   //关闭房间
    void on_btn_send_clicked();
    void on_btn_live_clicked(bool checked);
    void on_btn_refresh_clicked();
    void on_listWidget_roomlist_itemDoubleClicked(QListWidgetItem *item);
    void on_checkBox_bs_stateChanged(int arg1);

private:
    Ui::RoomListWindow *ui;
    ClientSocket *cs;
    QTcpSocket *socket;
    user_t myinfo;
    QVector<QString> roomusers;
};

#endif // ROOMLISTWINDOW_H
