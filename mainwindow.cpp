#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QHostAddress>
#include <QPalette>
#include <QDebug>
#include <QPainter>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("草莓红播");
    this->setWindowIcon(QIcon(":/image/icon.jpg"));
    memset(&myinfo, 0, sizeof (user_t));
    //按钮设置
    ui->btn_login->setFlat(true);
    ui->btn_regist->setFlat(true);
    //ui->btn_login->setStyleSheet("border-radius:10px;padding:2px 4px;");
    //ui->btn_regist->setStyleSheet("border-radius:10px;padding:2px 4px;");

    socket = new QTcpSocket(this);      //创建客户端套接字
    //连接服务器出错执行的操作
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));
    qRegisterMetaType<user_t>("user_t");//向Qt注册结构体

    cs = new ClientSocket(socket);
    QThread *th = new QThread;
    //连接断开删除子线程和退出子线程
    connect(socket, SIGNAL(disconnected()), cs, SLOT(deleteLater()));
    //connect(socket, SIGNAL(disconnected()), cs, SLOT(quit()));
    //连接登录和注册
    connect(cs, SIGNAL(sigReg(user_t)), this, SLOT(onSigReg(user_t)));
    connect(cs, SIGNAL(sigLogin(user_t)), this, SLOT(onSigLogin(user_t)));

    cs->moveToThread(th);//子线程处理读取服务器数据包任务
    th->start();
    //连接服务器
    socket->connectToHost(QHostAddress::LocalHost,8888);
    MainWindow::LoadUser();

}

void MainWindow::LoadUser()
{
    QFile file("D:/user.txt");
    file.open(QIODevice::ReadOnly);
    if (file.isOpen()) {
        QTextStream in(&file);
        QString text = in.readLine();
        QString temp = text;
        text = in.readLine();
        QStringList list = text.split(":");
        if (list.count() > 1) {
            ui->lineEdit_name->setText(list.at(1));
        }
        text = in.readLine();
        list = text.split(":");
        if (list.count() > 1) {
            ui->lineEdit_password->setText(list.at(1));
        }
        file.close();
        if (temp == "true") {
            ui->checkBox->setCheckState(Qt::Checked);
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap = QPixmap(":/image/main.jpg").scaled(this->size());
    painter.drawPixmap(this->rect(), pixmap);
    painter.end();
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
    char* pname = name.toLocal8Bit().data();
    strncpy(user.username, pname, 19);

    QString passwd = ui->lineEdit_password->text();
    if (passwd.isEmpty()) {
        QMessageBox::warning(this, "警告" , "请输入密码");
        return;
    }
    char *ppasswd = passwd.toLocal8Bit().data();
    strncpy(user.password, ppasswd, 19);
    user.type = REG;
    socket->write((char*)(&user), sizeof(user));
}

void MainWindow::on_btn_login_clicked()
{
    if (ui->lineEdit_name->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名或密码不能为空!");
        return;
    }
    user_t user;
    memset(&user, 0, sizeof(user_t));
    //操作名字
    QString name = ui->lineEdit_name->text();
    strncpy(user.username, name.toLocal8Bit().data(), 19);
    strncpy(myinfo.username, name.toLocal8Bit().data(), 19);
    //操作密码
    QString passwd = ui->lineEdit_password->text();
    MainWindow::on_checkBox_stateChanged(ui->checkBox->checkState());//是否记住密码

    strncpy(user.password, passwd.toLocal8Bit().data(), 19);
    strncpy(myinfo.password, passwd.toLocal8Bit().data(), 19);
    user.type = LOGIN;
    user.flag = UP;
    socket->write((char*)&user,sizeof(user));   //请求登录的用户信息发送给服务器
}
void MainWindow::onSigReg(user_t user)
{
    qDebug() << __func__;
    QString text = user.data;
    if (text != "success") {
        QMessageBox::warning(this, "警告", text);
    } else {
        QMessageBox::information(this, "提示", text);
    }
}

void MainWindow::onSigLogin(user_t user)
{
    QString text = user.data;
    if (text == "success") {
        QMessageBox::information(this, "提示", text);
        roomWindow = new RoomListWindow(cs, socket, user);
        QString title = QString::fromLocal8Bit(user.username);
        title += " 欢迎来到草莓红播";
        if (NULL == roomWindow) {
            QMessageBox::information(this, "发生了故障","请重新登录或者重启应用");
            return;
        }
        onRoomList();                                           //登录成功请求在线列表
        roomWindow->setWindowTitle(title);
        this->hide();                                           //隐藏登录界面
        roomWindow->show();                                     //显示主界面
    } else {
       QMessageBox::warning(this, "警告", text);
    }
    qDebug() << __FUNCTION__;
}

void MainWindow::onRoomList()
{
    //改变用户标志，请求房间列表
    myinfo.type = ROOMLIST;
    myinfo.flag = UP;
    socket->write((char*)&myinfo, sizeof (user_t));
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {//记住密码
        QFile file("D:/user.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        if (file.isOpen()) {
            QString str = "true\n";
            str += "name:" + ui->lineEdit_name->text() + "\n";
            str += "password:" + ui->lineEdit_password->text();
            file.write(str.toStdString().data());
            file.close();
        }
    } else {//不记住密码
        ui->lineEdit_password->clear();
    }
}
