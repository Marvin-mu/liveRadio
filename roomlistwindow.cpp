#include "roomlistwindow.h"
#include "ui_roomlistwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QCheckBox>
#include <QListView>
#include <QLineEdit>
#include <QTime>
#include <QMenu>
#include <QPainter>
#include <QTextCodec>
#include <QMovie>
#include <QThread>
#include <QAudioDeviceInfo>

RoomListWindow::RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent)
    :QWidget(parent),
    ui(new Ui::RoomListWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags());
    this->setWindowIcon(QIcon(":/image/room1.jpg"));
    this->cs = cs;
    this->myinfo = user;
    this->socket = socket;
/*
    QPixmap p;
    QByteArray bytearray = QByteArray::fromBase64(user.portrait);
    p.loadFromData(bytearray,"jpg");
    ui->label_portrait->setPixmap(p.scaled(ui->label_portrait->size()));
*/
    uiDesign();
    av = new AudioVedio(QHostAddress("224.0.0.1"),8888,QHostAddress("224.0.0.10"),9999,this);//管理房间音视频
    //QThread *th = new QThread();
    //connect(this, &RoomListWindow::destroyed, th, &QThread::deleteLater);
    //av->moveToThread(th);
    //th->start();

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(ui->btn_live, SIGNAL(clicked(bool)), this, SLOT(deal_button_live_clicked(bool)));

    connect(cs, SIGNAL(sigRoomList(user_t)), this, SLOT(on_roomlist(user_t)));
    connect(cs, SIGNAL(sigMasterQuit(user_t)), this, SLOT(on_roomquit(user_t)));//连接退出房间的通知
    connect(cs, SIGNAL(sigRoomName(user_t)), this, SLOT(on_roomname(user_t)));//有人开播刷新房间列表
    connect(cs, SIGNAL(sigJoinRoom(user_t)), this, SLOT(on_joinroom(user_t)));
    connect(cs, SIGNAL(sigChatText(user_t)), this, SLOT(on_chatText(user_t)));
    connect(cs, SIGNAL(sigBs(user_t)), this, SLOT(on_bs(user_t)));              //连接弹幕处理
    connect(cs, SIGNAL(sigQuitRoom(user_t)), this, SLOT(on_roomUserQuit(user_t)));
    connect(cs, SIGNAL(sigIp(user_t)), this, SLOT(on_ip(user_t)));

}

//绘制背景
void RoomListWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap = QPixmap(":/image/vedio_bk.jpg").scaled(this->size());
    painter.drawPixmap(this->rect(), pixmap);
    painter.end();
}

void RoomListWindow::uiDesign()
{
    menuwindow = new MenuWindow();
    connect(menuwindow, &MenuWindow::sigMes, this, &RoomListWindow::on_setting);

    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(on_timeOut()));
    //bs_label = new QLabel(ui->widget); //弹幕
    //bs_label->raise();
    //bs_label->hide();   //默认隐藏

    ui->btn_live->setStyleSheet("QPushButton{background-color: orange;"
                                "color: black;"
                                "border-radius: 10px;"
                                "border: 0px groove gray;"
                                "border-style: outset;}"
                                "QPushButton:pressed{background-color:rgb(85, 170, 255);"
                                "border-style: inset; }"
                                );
    ui->btn_top_up->setStyleSheet("QPushButton{background-color: orange;"
                                "color: black;"
                                "border-radius: 10px;"
                                "border: 0px groove gray;"
                                "border-style: outset;}"
                                "QPushButton:pressed{background-color:rgb(85, 170, 255);"
                                "border-style: inset; }"
                                );

    ui->widget->hide();//直播页面默认影藏
    ui->verticalLayout_widget->addWidget(ui->widget);
    ui->listWidget_roomlist->setIconSize(QSize(30,30));
    ui->listWidget_roomuser->setIconSize(QSize(25,25));
    ui->listWidget_roomlist->setStyleSheet("QListWidget{border:0px solid gray; color:orange; background-image: url(:/image/skin2.jpg);}"
                                           "QListWidget::Item{padding-top:10px; padding-bottom:4px; }"
                                           "QListWidget::Item:hover{background:skyblue; }"
                                           "QListWidget::item:selected{background:lightgray; color:red; }"
                                           "QListWidget::item:selected:!active{border-width:0px; background:lightgreen; }"
                                           "QScrollBar{width:10px;}"
                                           "background-image: url(:/image/skin2.jpg);"
                                           );
    ui->listWidget_roomuser->setStyleSheet("QListWidget{border:0px solid gray; color:orange; background-image: url(:/image/skin2.jpg);}"
                                           "QListWidget::Item{padding-top:10px; padding-bottom:4px; }"
                                           "QListWidget::Item:hover{background:skyblue; }"
                                           "QListWidget::item:selected{background:lightgray; color:red; }"
                                           "QListWidget::item:selected:!active{border-width:0px; background:lightgreen; }"
                                           "QScrollBar{width:10px;}"
                                           "background-image: url(:/image/skin1.jpg);");//设置滑动条宽度

    label_roomname = new QLabel("房间名",ui->widget);
    label_roomname->raise();//提升到表层
    label_roomname->setStyleSheet("color:orange");
    label_roomname->setFont(QFont("Timers",24, QFont::Bold));
    label_roomname->adjustSize();//自适应大小
    label_roomname->move(5,5);
    label_roomname->show();

    btn_quit = new QPushButton("退出",ui->widget);
    connect(btn_quit, SIGNAL(clicked()), this, SLOT(deal_button_quit_clicked()));
    btn_quit->setGeometry(800,0,40,40);
    btn_quit->show();

    bs = new QCheckBox("弹幕",ui->widget);
    connect(bs, SIGNAL(stateChanged(int)), this, SLOT(deal_checkBox_stateChanged(int)));
    bs->setGeometry(660,480,60,40);
    bs->show();

    btn_camera = new QCheckBox("相机",ui->widget);
    btn_camera->setCheckable(true);
    connect(btn_camera, SIGNAL(stateChanged(int)), this, SLOT(deal_button_camera_toggled(int)));
    btn_camera->setGeometry(720,480,60,40);
    btn_camera->show();

    btn_audio = new QCheckBox("音频",ui->widget);
    btn_audio->setCheckable(true);
    connect(btn_audio, SIGNAL(stateChanged(int)), this, SLOT(deal_button_audio_changed(int)));
    btn_audio->setGeometry(780,480,60,40);
    btn_audio->show();

    label_vedio = new QLabel("视频区",ui->widget);
    label_vedio->lower();//最底层
    label_vedio->setGeometry(0, 0, ui->widget->width(), ui->widget->height());
    label_vedio->show();

    btn_send = new QPushButton("发送",ui->widget);
    btn_send->setWindowOpacity(0.2);
    connect(btn_send, SIGNAL(clicked()), this, SLOT(deal_button_send_clicked()));
    btn_send->setGeometry(780,520,60,40);
    btn_send->show();

    btn_gift = new QPushButton("",ui->widget);
    btn_gift->setIcon(QIcon(":/image/gift.png"));
    btn_gift->setGeometry(0,520,40,40);
    connect(btn_gift, SIGNAL(clicked(bool)), this, SLOT(deal_button_gift_clicked(bool)));
    btn_gift->show();

    line_input = new QLineEdit("",ui->widget);
    line_input->setWindowOpacity(0.2);
    line_input->setGeometry(50,520,720,40);
    line_input->show();

    text_widget = new QListWidget(ui->widget);
    //text_widget->setFrameShape(QListWidget::NoFrame);//无边框
    text_widget->setAttribute(Qt::WA_TranslucentBackground);//设置透明
    //text_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //text_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//不显示滑动条
    text_color = "QScrollBar{width:10px;}"
                 "QListWidget{background:gray}";
    text_widget->setStyleSheet(text_color + "QListWidget{color:red}");
    text_widget->setGeometry(0,300,280,206);
    text_widget->show();

}

//回车发送消息
void RoomListWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        RoomListWindow::deal_button_send_clicked();
    }
}
/*
bool RoomListWindow::eventFilter(QObject *obj, QEvent *ev)
{

    if(obj == line_input){
        //是否是键盘事件
        if(ev->type() == QEvent::KeyPress){
            //将QEvent*类型转化为QKeyEvent*
            QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(ev);
            //判定是否是大键盘上的enter
            if(keyEvent->key() == Qt::Key_Return){
                RoomListWindow::deal_button_send_clicked();    //执行发送按钮事件
            }else{
                //继续转发
                return false;
            }
        }else{
            return false;
        }
    }

    return RoomListWindow::eventFilter(obj, ev);
}
*/

RoomListWindow::~RoomListWindow()
{
    delete ui;
}

//关闭事件，告诉服务器此用户下线
void RoomListWindow::closeEvent(QCloseEvent *)
{
    if (myinfo.flag == MASTER) {
        myinfo.type = QUIT;
        myinfo.flag = DOWN;
        socket->write((char*)&myinfo, sizeof (user_t));     //发送关闭房间的通知
    }
    myinfo.type = EXIT;
    myinfo.flag = DOWN;
    socket->write((char*)&myinfo, sizeof(myinfo));//默认会自动发送一次
    myinfo.type = TOP_UP;
    socket->write((char*)&myinfo,sizeof(user_t));
    qDebug() << "帐户积分："<< myinfo.money;
}

//请求在线房间列表,一个一个的添加
void RoomListWindow::on_roomlist(user_t user)
{
    qDebug() << __FILE__ << __FUNCTION__ << __LINE__;
    QString room = QString::fromLocal8Bit(user.data);
    if (room == "start") {
        ui->listWidget_roomlist->clear();
    } else if (room == "end") {
        qDebug() << "房间列表请求完成";
    } else {
        QPixmap p;
        QByteArray bytearray = QByteArray::fromBase64(user.portrait);
        p.loadFromData(bytearray,"jpg");
        QListWidgetItem *item = new QListWidgetItem(QIcon(p),room, ui->listWidget_roomlist);
        ui->listWidget_roomlist->addItem(item);
    }
}

//新主播开播增加在线房间
void RoomListWindow::on_roomname(user_t user)
{
    QString roomname = QString::fromLocal8Bit(user.roomName);
    QPixmap p;
    QByteArray bytearray = QByteArray::fromBase64(user.portrait);
    p.loadFromData(bytearray,"jpg");
    QListWidgetItem *item = new QListWidgetItem(QIcon(p),roomname, ui->listWidget_roomlist);
    ui->listWidget_roomlist->addItem(item);
}

//处理服务器发送过来的房间新用户
void RoomListWindow::on_joinroom(user_t user)
{
    //处理房间用户列表
    QString name = QString::fromLocal8Bit(user.username);
    QByteArray bytearray = QByteArray::fromBase64(user.portrait);
    QPixmap p;
    p.loadFromData(bytearray,"jpg");
    QListWidgetItem *item = new QListWidgetItem(QIcon(p),name, ui->listWidget_roomuser);
    ui->listWidget_roomlist->addItem(item);
}

//有用户离开放假，处理事件
void RoomListWindow::on_roomUserQuit(user_t user)
{
    QString name = QString::fromLocal8Bit(user.username);
    if (name == QString(myinfo.username)) {
        return;
    }
    int n = ui->listWidget_roomuser->count();
    for (int i = 0; i < n; i++) {
        QListWidgetItem *it = ui->listWidget_roomuser->takeItem(i);
        if (it->text() == name) {
            ui->listWidget_roomuser->removeItemWidget(it);
            break;
        }
    }
    qDebug() << __FUNCTION__;
}

//收到主播关播消息，提示用户
void RoomListWindow::on_roomquit(user_t user)
{
    QString str = QString::fromLocal8Bit(user.data);
    QString roomname = QString::fromLocal8Bit(user.data);
    QMessageBox::information(this, "提醒", str, Qt::NoButton);

    ui->listWidget_roomuser->clear();
    ui->widget->hide();
    ui->listWidget_roomlist->show();
    ui->label_onlineroom->show();
    ui->btn_live->show();
    ui->btn_refresh->show();
}

//处理聊天内容
void RoomListWindow::on_chatText(user_t user)
{
    QString text = QString::fromLocal8Bit(user.data);
    //QTextCodec *code = QTextCodec::codecForName("UTF-8");//设置编码格式
    QListWidgetItem *it = new QListWidgetItem(text, text_widget);
    text_widget->addItem(it);
}

void RoomListWindow::on_bs(user_t user)
{
    QString bs_text = QString::fromLocal8Bit(user.data);
    if (bs->checkState() == Qt::Unchecked) {
        RoomListWindow::on_chatText(user);
    }else {
        QStringList list = bs_text.split("\n");
        bs_text = list.last();//去除用户和时间信息
        QLabel *bs_label = new QLabel(ui->widget);
        bs_label->setText(bs_text);
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, ui->widget, [=](void)->void{
            const QPoint& p = bs_label->pos();
            if(p.x() < (ui->widget->width())){
                bs_label->move(p.x() + 10, p.y());
                bs_label->show();
                bs_label->update();
            }
            else{
                bs_label->hide();
            }
        });
        bs_label->setStyleSheet("color:orange");
        bs_label->setFont(QFont("Timers", 16 , QFont::Bold));
        int len = qrand()% (ui->widget->height() - 120);//纵坐标为随机值
        bs_label->move(ui->widget->x(), len);
        bs_label->adjustSize(); //设置label大小自适应字体长度
        timer->start(10);
    }
}


void RoomListWindow::on_ip(user_t user)
{
    QString str = QString::fromLocal8Bit(user.data);
    QStringList list = str.split("/");
    if (list.count() < 3) {
        return;
    }
    ip = QHostAddress(list.at(1));
    port = list.at(2).toInt();
    av->setIpPort(ip, port);
    if (myinfo.flag != MASTER) {
        av->init(0);//初始化音视频
    }
    qDebug() << __FUNCTION__ << ip << port;
}

//定时器事件
void RoomListWindow::on_timeOut()
{/*
    const QPoint& p = bs_label->pos();
    if(p.x() < (ui->widget->width())){
        bs_label->move(p.x() + 1, p.y());
        bs_label->show();
        bs_label->update();
    }
    else{
        bs_label->hide();
    }*/
}

//发送聊天内容
void RoomListWindow::deal_button_send_clicked()
{
    if (line_input->text().isEmpty()) {//内容为空不会发送
        return;
    }
    QString text = "";
    if (bs->checkState() == Qt::Checked) {
        myinfo.type = BS;
    } else {
        myinfo.type = TEXT;
    }
    text = QString(myinfo.username) + " "+ QTime::currentTime().toString() + "\n";
    text += line_input->text();
    strcpy(myinfo.data, text.toLocal8Bit().data());
    socket->write((char*)&myinfo, sizeof(user_t));
    qDebug() << __FUNCTION__ << __LINE__;
    line_input->clear();
}

//开播按钮
void RoomListWindow::deal_button_live_clicked(bool checked)
{
    if (checked) {      //开播
        ui->widget->show();
        btn_quit->hide();
        label_vedio->setText("已开启直播");
        ui->btn_live->setText("开播中");
        label_roomname->setText(myinfo.username);

        myinfo.type = ROOMNAME;
        myinfo.flag = MASTER;
        strcpy(myinfo.roomName, myinfo.username);       //用户名为房间名
        socket->write((char*)&myinfo, sizeof(user_t));
        myinfo.type = IP;
        socket->write((char*)&myinfo, sizeof (user_t));
        av->init(1);//初始化音视频
    } else {        //关闭直播间
        QMessageBox::StandardButton sb = QMessageBox::information(this, "提示", "关闭直播间",
                                QMessageBox::Yes, QMessageBox::No);
        if (sb == QMessageBox::Yes) {       //确认关闭
            myinfo.type = QUIT;
            myinfo.flag = UP;
            socket->write((char*)&myinfo, sizeof (user_t));     //发送关闭房间的通知
            ui->btn_live->setText("开播");
            ui->widget->hide();
            btn_quit->show();

            av->remove();

            if (btn_camera->checkState() == Qt::Checked) {
                deal_button_camera_toggled(Qt::Unchecked);
            }
            if (btn_audio->checkState() == Qt::Checked) {
                deal_button_audio_changed(Qt::Unchecked);
            }
        } else {           //不关闭
            return;
        }
    }
}

//双击进入直播间
void RoomListWindow::on_listWidget_roomlist_itemDoubleClicked(QListWidgetItem *item)
{
    myinfo.type = IP;
    strcpy(myinfo.roomName, item->text().toLocal8Bit().data());
    socket->write((char*)&myinfo, sizeof (user_t));//请求组播IP
    QString roomname = item->text();
    //特殊处理ui，根据用户的身份不同
    if (myinfo.flag == MASTER) {//主播只能查看在线列表，不能观看
        QMessageBox::information(this, "提示","开播状态禁止观看直播",Qt::NoButton);
        return;
    }
    text_widget->clear();//清理聊天记录
    QString sysInfo =  "[系统消息：]\n";
    sysInfo += myinfo.username;
    sysInfo += " 进入直播间";
    QListWidgetItem *it = new QListWidgetItem(sysInfo,text_widget);
    text_widget->addItem(it);

    myinfo.type = JOINROOM;
    strcpy(myinfo.roomName, roomname.toLocal8Bit().data());
    strcpy(myinfo.data, sysInfo.toLocal8Bit().data());
    socket->write((char*)&myinfo, sizeof (user_t));//更新用户所处的房间
    //界面处理
    label_roomname->setText(roomname);
    //音视频处理
    //av->init(0);//初始化音视频
    QObject::connect (av, SIGNAL(sigImageUser(QImage)),
             this, SLOT(on_ImageChanged(QImage))); //更新视屏图片的连接

    ui->listWidget_roomlist->hide();
    ui->btn_refresh->hide();
    ui->label_onlineroom->hide();
    ui->btn_live->hide();
    ui->widget->show();
    btn_camera->hide();
    btn_audio->hide();

}

void RoomListWindow::deal_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked) {    //未选择状态，关闭弹幕
        QMessageBox::information(this,"","关闭弹幕");
    } else {        //选择状态
        QMessageBox::information(this,"","打开弹幕");
    }
}

//用户离开房间
void RoomListWindow::deal_button_quit_clicked()
{
    ui->listWidget_roomlist->show();
    ui->label_onlineroom->show();
    ui->btn_live->show();
    ui->widget->hide();
    ui->btn_refresh->show();
    btn_camera->show();
    btn_audio->show();
    ui->listWidget_roomuser->clear();

    av->remove();

    myinfo.type = QUITROOM;
    socket->write((char*)&myinfo, sizeof (user_t));//用户离开通知其他客户端
}

//礼物按钮
void RoomListWindow::deal_button_gift_clicked(bool)
{
    QListWidget *list_gift = new QListWidget(ui->widget);
    list_gift->setGeometry(btn_gift->pos().x(), btn_gift->pos().y()-50, 260,90);
    list_gift->setIconSize(QSize(40, 40));
    list_gift->setFlow(QListWidget::LeftToRight);
    list_gift->setViewMode(QListWidget::IconMode);
    list_gift->hide();
    QListWidgetItem *car = new QListWidgetItem(QIcon(":/image/gift/car1.png"), "跑车\n168.00", list_gift);
    list_gift->addItem(car);
    QListWidgetItem *cheers = new QListWidgetItem(QIcon(":/image/gift/cheers.png"), "干杯\n16.60", list_gift);
    list_gift->addItem(cheers);
    QListWidgetItem *momoda = new QListWidgetItem(QIcon(":/image/gift/momoda.png"), "么么哒\n5.20", list_gift);
    list_gift->addItem(momoda);
    QListWidgetItem *shuijingqiu = new QListWidgetItem(QIcon(":/image/gift/shuijingqiu.png"), "水晶球\n88.88", list_gift);
    list_gift->addItem(shuijingqiu);
    list_gift->show();
    connect(list_gift, &QListWidget::clicked, ui->widget,
            [=](QModelIndex index)->void{
            QString str = QString(myinfo.username) + " 送出了 '";
            QString giftname = list_gift->takeItem(index.row())->text();
            QStringList list = giftname.split("\n");
            str += list.at(0);
            str += "'";
            QByteArray bytearray;
            QMovie *mv;
            double money = 0;
            if (giftname == "跑车\n168.00") {
                mv = new QMovie(":/image/gift/car1.gif", bytearray ,ui->widget);
                money = 168.00;
            } else if (giftname == "干杯\n16.60") {
                mv = new QMovie(":/image/gift/cheers.gif", bytearray ,ui->widget);
                money = 16.60;
            } else if (giftname == "么么哒\n5.20") {
                mv = new QMovie(":/image/gift/momoda.gif", bytearray ,ui->widget);
                money = 5.20;
            } else if (giftname == "水晶球\n88.88") {
                mv = new QMovie(":/image/gift/shuijingqiu.gif", bytearray ,ui->widget);
                money = 88.88;
            } else {
                mv = new QMovie(ui->widget);
            }
            if (myinfo.money - money < 0) {
                list_gift->hide();
                QMessageBox::about(this,"积分不足","请前往充值中心");
                return;
            }
            myinfo.money -= money;
            QListWidgetItem *item = new QListWidgetItem(str, text_widget);
            text_widget->addItem(item);
            /*
            strcpy(temp.username, label_roomname->text().toLocal8Bit().data());//收益给当前主播
            temp.type = TOP_UP;
            socket->write((char*)&temp,sizeof(user_t));
            */
            QLabel *gif = new QLabel(ui->widget);
            gif->setScaledContents(true);//自适应大小
            gif->show();
            gif->setGeometry(ui->widget->x(),ui->widget->y(),ui->widget->width(),ui->widget->height()-100);
            QTimer *timer = new QTimer(this);
            timer->start(2100);
            connect(timer, &QTimer::timeout, ui->widget,
                    [=](void)->void{
                    mv->stop();
                    gif->hide();
                    });
            gif->raise();
            gif->setAlignment(Qt::AlignCenter);
            gif->setMovie(mv);
            mv->setSpeed(80);
            mv->start();
            list_gift->hide();
            list_gift->clear();
            });
}

void RoomListWindow::on_ImageChanged(QImage image)
{
    //显示视频
    label_vedio->setPixmap(QPixmap::fromImage(image).scaled(label_vedio->size()));
}

void RoomListWindow::deal_button_camera_toggled(int arg)
{
    qDebug() << __FUNCTION__ << __LINE__ << __FILE__;
    //开启摄像头
    if (arg == Qt::Unchecked) {
        av->closeCamera();//关闭摄像头
        QMessageBox::information(this, "提示", "关闭摄像头");
        label_vedio->clear();
    }else {
        av->openCamera();
        QObject::connect (av, SIGNAL(sigImage(QImage)),
                 this, SLOT(on_ImageChanged(QImage))); //更新视屏图片的连接
    }
}

void RoomListWindow::deal_button_audio_changed(int arg)
{
    if (arg == Qt::Unchecked) {
        QMessageBox::information(this, "提示", "关闭麦克风");
        av->closeAudio();
    } else {
        QMessageBox::information(this, "提示", "开启麦克风");
        av->openAudio();
    }
}

void RoomListWindow::on_btn_refresh_clicked()
{
    myinfo.type = ROOMLIST;
    socket->write((char*)&myinfo, sizeof (user_t));
    qDebug() << __FUNCTION__ << __LINE__;
}

//右键弹出菜单选项
void RoomListWindow::on_widget_customContextMenuRequested(const QPoint &)
{
    QMenu *menu = new QMenu(this);
    QAction *info = new QAction("个人信息",this);
    info->setData(1);
    QAction *topUp = new QAction("充值",this);
    topUp->setData(2);

    menu->addAction(info);
    menu->addAction(topUp);

    connect(info, SIGNAL(triggered()), this, SLOT(on_task()));
    connect(topUp, SIGNAL(triggered()), this, SLOT(on_task()));

    menu->exec(cursor().pos());//点击位置显示菜单
    //释放内存
    QList<QAction*> list = menu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete menu;
}

//处理右键功能
void RoomListWindow::on_task()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    int iType = pEven->data().toInt();
    switch (iType) {
    case 1:
    {

        QString text = "用户名：" + QString(myinfo.username) + "\n";
        text += "积分：" + QString::number(myinfo.money) + "\n";
        text += "VIP：" + QString(myinfo.vip);
        QMessageBox::about(this,"个人信息",text);
        break;
    }
    case 2:
    {
        RoomListWindow::on_btn_top_up_clicked(true);
        break;
    }
    default:
        break;
    }
}

void RoomListWindow::on_btn_top_up_clicked(bool clicked)
{
    if (!clicked) {
        return;
    }
    QDialog *diaolog = new QDialog(this);
    diaolog->setGeometry(cursor().pos().x()-110, cursor().pos().y()-60, 230, 120);
    diaolog->setWindowTitle("充值中心");
    QLineEdit *money = new QLineEdit(diaolog);
    money->setGeometry(20,20,180,30);
    QPushButton *ok = new QPushButton("ok",diaolog);
    connect(ok, &QPushButton::clicked, diaolog,
            [=](void)->void{
                qDebug() << money->text();
                strcpy(myinfo.data,money->text().toLocal8Bit().data());
                myinfo.money += money->text().toDouble();
                myinfo.type = TOP_UP;
                socket->write((char*)&myinfo,sizeof(user_t));
                diaolog->close();}
            );
    ok->setGeometry(20,60,80,40);
    QPushButton *cancel = new QPushButton("cancel",diaolog);
    connect(cancel, &QPushButton::clicked, diaolog,
            [=](void)->void{qDebug() << "cancel";
            diaolog->close();}
    );
    cancel->setGeometry(120,60,80,40);
    diaolog->show();
}

void RoomListWindow::onError(QAbstractSocket::SocketError socketError)
{
    this->close();
    qDebug() << socketError;
}

void RoomListWindow::on_btn_menu_toggled()
{
    menuwindow->setWindowTitle("设置");
    menuwindow->show();
}

void RoomListWindow::on_setting(const QString &text, int type)
{
    switch (type) {
    case BK_USERLIST:
        qDebug() << __LINE__;
        break;
    case BK_ROOMLIST:
        qDebug() << __LINE__;
        break;
    case BK_LIVE:
    {
        this->setAutoFillBackground(true);
        QPalette pal = this->palette();
        pal.setColor(QPalette::Background, QColor(19, 54, 118));
        pal.setBrush(backgroundRole(), QPixmap(text));
        setPalette(pal);
        break;
    }
    case COLOR_TEXT:
    {
        text_widget->setStyleSheet(text_color + QString("QListWidget{%1}").arg(text));
        break;
    }
    case COLOR_BS:
    {
        //bs_label->setStyleSheet(text);
        break;
    }
    case FONT_TEXT:
    {
        QFont font;
        font.fromString(text);
        qDebug() << font;
        text_widget->setFont(font);
        break;
    }
    case FONT_BS:
    {
        break;
    }
    default:
        break;
    }
}










