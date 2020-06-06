#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QHostAddress>
#include <QPalette>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("草莓红播");
    memset(&myinfo, 0, sizeof (user_t));
    //设置背景填充
    //QPalette palette;   //调色板
    //palette.setBrush(this->backgroundRole(),QBrush(QImage("\\image\\mainbk.jpg")));
    //this->setPalette(palette);
    //this->setAutoFillBackground(true);
    //按钮设置
    ui->btn_login->setFlat(true);       //显示按钮边框
    ui->btn_regist->setFlat(true);
    ui->btn_login->setStyleSheet("border-radius:10px;padding:2px 4px;");
    ui->btn_regist->setStyleSheet("border-radius:10px;padding:2px 4px;");

    socket = new QTcpSocket(this);      //创建客户端套接字
    //连接服务器出错执行的操作
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));
    qRegisterMetaType<user_t>("user_t");//向Qt注册结构体

    cs = new ClientSocket(socket);
    QThread *th = new QThread();
    //连接断开删除子线程和退出子线程
    connect(socket, SIGNAL(disconnected()), cs, SLOT(deleteLater()));
    //连接登录和注册
    connect(cs, SIGNAL(sigReg(user_t)), this, SLOT(onSigReg(user_t)));
    connect(cs, SIGNAL(sigLogin(user_t)), this, SLOT(onSigLogin(user_t)));
    //connect(cs, SIGNAL((sigRoomList(user_t))), this, SLOT(onSigRoomList(user_t))); //连接在线房间

    cs->moveToThread(th);//子线程处理读取服务器数据包任务
    th->start();
    //连接服务器
    socket->connectToHost(QHostAddress::LocalHost,8888);

    onRoomList();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onError(QAbstractSocket::SocketError socketError)
{
    QMessageBox::warning(this, "警告", "未连接上服务器,请稍后重启应用");
    this->close();
    qDebug() << socketError;
}

void MainWindow::on_btn_regist_clicked()
{
    user_t user;
    memset(&user, 0, sizeof(user));
    QString name = ui->lineEdit_name->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入用户名");
        return;
    }
    ui->lineEdit_name->clear();
    char* pname = name.toLocal8Bit().data();
    strncpy(user.username, pname, 20);

    QString passwd = ui->lineEdit_password->text();
    if (passwd.isEmpty()) {
        QMessageBox::warning(this, "警告" , "请输入密码");
        return;
    }
    ui->lineEdit_password->clear();
    char *ppasswd = passwd.toLocal8Bit().data();
    strncpy(user.password, ppasswd, 20);
    user.type = REG;
    socket->write((char*)(&user), sizeof(user));
}

void MainWindow::on_btn_login_clicked()
{
    user_t user;
    memset(&user, 0, sizeof(user_t));
    //操作名字
    QString name = ui->lineEdit_name->text();
    ui->lineEdit_name->clear();
    char *pname = name.toLocal8Bit().data();
    strncpy(user.username, pname, 20);
    strncpy(myinfo.username, pname, 20);
    //操作密码
    QString passwd = ui->lineEdit_password->text();
    ui->lineEdit_password->clear();
    char* ppasswd = passwd.toLocal8Bit().data();
    strncpy(user.password, ppasswd, 20);
    strncpy(myinfo.password, ppasswd, 20);
    user.type = LOGIN;
    socket->write((char*)&user,sizeof(user));   //请求登录的用户信息发送给服务器
}
void MainWindow::onSigReg(user_t user)
{
    QString text = user.data;
    if (text != "注册成功") {
        QMessageBox::warning(this, "警告", text);
    } else {
        QMessageBox::information(this, "提示", text);
    }
}

void MainWindow::onSigLogin(user_t user)
{
    QString text = user.data;
    if (text != "登录成功" || user.flag == UP) {
        QMessageBox::warning(this, "警告", text);
    } else {
        QMessageBox::information(this, "提示", text);
        roomWindow = new RoomListWindow(cs, socket, user);

        QString title = QString::fromLocal8Bit(user.username);
        title += ",欢迎来到草莓红播";
        if (NULL == roomWindow) {
            QMessageBox::information(this, "发生了故障","请重新登录或者重启应用");
            return;
        }
        roomWindow->setWindowTitle(title);
        this->hide();                                           //隐藏登录界面
        roomWindow->show();                                     //显示主界面
    }
}

void MainWindow::onRoomList()
{
    //改变用户标志，把房间名发送给服务器
    myinfo.type = ROOMLIST;
    myinfo.type = MASTER;
    socket->write((char*)&myinfo, sizeof (user_t));

}
