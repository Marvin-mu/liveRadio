#include "roomlistwindow.h"
#include "ui_roomlistwindow.h"
#include <QDebug>
#include <QMessageBox>

RoomListWindow::RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent)
    :QWidget(parent),
    ui(new Ui::RoomListWindow)
{
    ui->setupUi(this);
    this->cs = cs;
    this->myinfo = user;
    this->socket = socket;

//    pushRoomList();//构造第二界面就请求房间列表

    connect(cs, SIGNAL(sigRoomList(user_t)), this, SLOT(on_roomlist(user_t)));
}
/*
void RoomListWindow::pushRoomList()
{
    user_t user;
    user.type = ROOMLIST;
    user.flag = UP;     //标记用户在线标志
    socket->write((char*)&user,sizeof (user_t));
    qDebug() << __FUNCTION__ << __LINE__;
}
*/

RoomListWindow::~RoomListWindow()
{
    delete ui;
}

//关闭事件，告诉服务器此用户下线
void RoomListWindow::closeEvent(QCloseEvent *)
{
    myinfo.type = EXIT;
    socket->write((char*)&myinfo, sizeof(myinfo));//默认会自动发送一次
    static int i;
    qDebug() << __FUNCTION__ << i;
}

//请求在线房间列表
void RoomListWindow::on_roomlist(user_t user)
{
    qDebug() << __FILE__ << __FUNCTION__ << __LINE__;
    char data[1024];
    memset(data, 0, sizeof (data));
    strncpy(data, user.data, 1024);
    QString room = "";
    ui->listWidget_roomlist->clear();
    for (size_t i = 0; i < strlen(data); ++i) {
        if (data[i] != '*') {
            room += data[i];
        } else {
            QListWidgetItem *item = new QListWidgetItem(room, ui->listWidget_roomlist);
            ui->listWidget_roomlist->addItem(item);
            qDebug() << "roomlist:" << room;
            room = "";
        }
    }
}

//开播时执行添加在线房间操作
void RoomListWindow::on_roomname(user_t user)
{
    QString room = QString::fromLocal8Bit(user.roomName);
    QListWidgetItem *item = new QListWidgetItem(room, ui->listWidget_roomlist);
    ui->listWidget_roomlist->addItem(item);
}

//处理服务器发送过来的房间新用户
void RoomListWindow::on_joinroom(user_t user)
{
    //处理房间用户列表
    QString username = QString::fromLocal8Bit(user.username);
    QListWidgetItem *item = new QListWidgetItem(username, ui->listWidget_roomlist);
    ui->listWidget_roomlist->addItem(item);
}

//处理主播关播
void RoomListWindow::on_roomquit(user_t user)
{
    QString str = QString::fromLocal8Bit(user.data);
    QMessageBox::information(this, "提醒", str, Qt::NoButton);
    ui->listWidget_roomuser->clear();
}

//隐藏房间列表
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
        ui->listWidget_roomuser->hide();
    } else {
        ui->listWidget_roomuser->show();
    }
}

void RoomListWindow::on_btn_send_clicked()
{
    myinfo.type = TEXT;
    QString text = ui->lineEdit_input->text();
    strcpy(myinfo.data, text.toUtf8().data());
    ui->textBrowser_text->append(text);
    socket->write((char*)&myinfo, sizeof(user_t));
    qDebug() << __FUNCTION__ << __LINE__;
}


void RoomListWindow::on_btn_live_clicked(bool checked)
{
    if (checked) {      //开播
        ui->label_vedio->setText("已开启直播");
        ui->btn_live->setText("开播中");
        myinfo.type = ROOMNAME;
        strcpy(myinfo.roomName, myinfo.username);       //用户名为房间名
        socket->write((char*)&myinfo, sizeof(user_t));
        on_roomname(myinfo);
    } else {        //关闭直播间
        QMessageBox::StandardButton sb = QMessageBox::information(this, "提示", "关闭直播间",
                                QMessageBox::Yes, QMessageBox::No);
        if (sb == QMessageBox::Yes) {       //确认关闭
            myinfo.type = QUIT;
            socket->write((char*)&myinfo, sizeof (user_t));     //发送关闭房间的通知
            ui->btn_live->setText("开播");
        } else {           //不关闭
            return;
        }
    }
}
//刷新房间按钮
void RoomListWindow::on_btn_refresh_clicked()
{

}

//双击进入直播间
void RoomListWindow::on_listWidget_roomlist_itemDoubleClicked(QListWidgetItem *item)
{
    QString roomname = item->text();

    myinfo.type = JOINROOM;
    strcpy(myinfo.roomName, roomname.toUtf8().data());
    socket->write((char*)&myinfo, sizeof (user_t));//更新用户所处的放间

    ui->label_roomname->setText(roomname);
    QListWidgetItem *item1 = new QListWidgetItem(myinfo.username, ui->listWidget_roomuser);
    ui->listWidget_roomlist->addItem(item1);
}


void RoomListWindow::on_checkBox_bs_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked) {    //未选择状态，关闭弹幕

    } else {        //选择状态

    }
}
