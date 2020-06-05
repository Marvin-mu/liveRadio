#include "roomlistwindow.h"
#include "ui_roomlistwindow.h"

RoomListWindow::RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomListWindow)
{
    ui->setupUi(this);
}

RoomListWindow::~RoomListWindow()
{
    delete ui;
}
