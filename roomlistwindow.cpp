#include "roomlistwindow.h"
#include "ui_roomlistwindow.h"
#include <QDebug>

RoomListWindow::RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent)
    :QWidget(parent),
    ui(new Ui::RoomListWindow)
{
    ui->setupUi(this);
    this->cs = cs;
    this->myinfo = user;
    this->socket = socket;

    pushRoomList();

    connect(cs, SIGNAL(sigRoomList(user_t)), this, SLOT(on_roomlist()));
}

void RoomListWindow::pushRoomList()
{
    user_t user;
    user.type = ROOMLIST;
    socket->write((char*)&user,sizeof (user_t));
}

RoomListWindow::~RoomListWindow()
{
    delete ui;
}

void RoomListWindow::closeEvent(QCloseEvent *)
{
    myinfo.type = EXIT;
    socket->write((char*)&myinfo, sizeof(myinfo));
}

void RoomListWindow::on_roomlist()
{
    qDebug() << "roomlist";
}

void RoomListWindow::on_roomname()
{

}

void RoomListWindow::on_commandLinkButton_toggled(bool checked)
{
    if (checked) {
        ui->listWidget_roomlist->hide();
    } else {
        ui->listWidget_roomlist->show();
    }
}

void RoomListWindow::on_commandLinkButton_userlist_toggled(bool checked)
{
    if (checked) {
        ui->listView->hide();
    } else {
        ui->listView->show();
    }
}

void RoomListWindow::on_btn_send_clicked()
{

}
