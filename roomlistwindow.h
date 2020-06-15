#ifndef ROOMLISTWINDOW_H
#define ROOMLISTWINDOW_H

#include <QWidget>
#include <QVector>
#include <QCloseEvent>
#include <QVideoFrame>
#include <QListWidgetItem>
#include <QUdpSocket>
#include <QCamera>
#include <QCameraInfo>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>
#include <QKeyEvent>
#include <QAudioInput>
#include <QAudioOutput>
#include "clientsocket.h"
#include "user.h"
#include "videosurface.h"
#include "audiovedio.h"
#include "menuwindow.h"

namespace Ui {
class RoomListWindow;
}

class RoomListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RoomListWindow(ClientSocket *cs, QTcpSocket *socket, user_t user, QWidget *parent = 0);
    void uiDesign();//初始界面设计
    //virtual bool eventFilter(QObject *obj, QEvent *ev); //按enter发送消�
    ~RoomListWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *event);//回车键发送消�

private slots:
    void on_roomlist(user_t);                                               //房间列表
    void on_roomname(user_t);                                               //开播向服务器推�
    void on_joinroom(user_t);                                               //用户加入房间，通知在此房间的用户的列表
    void on_roomquit(user_t);                                               //关闭房间
    void on_roomUserQuit(user_t);                                           //房间内有用户退�
    void on_chatText(user_t);                                               //聊天内容
    void on_bs(user_t);                                                     //弹幕处理
    void on_ip(user_t);
    void deal_button_send_clicked();                                        //发送文字内容到公屏
    void deal_button_live_clicked(bool);                                    //开播按�
    void on_listWidget_roomlist_itemDoubleClicked(QListWidgetItem *item);   //双击进入房间
    void deal_checkBox_stateChanged(int arg1);                              //弹幕开�

    void deal_button_quit_clicked();

    void deal_button_gift_clicked(bool checked);
    void on_ImageChanged(QImage);                                      //视频采集成图�

    void deal_button_camera_toggled(int arg);                               //处理是否开启相�
    void deal_button_audio_changed(int arg);                                //处理是否开启音�

    void on_btn_refresh_clicked();

    void on_widget_customContextMenuRequested(const QPoint &);

    void on_task();

    void on_timeOut();

    void on_btn_top_up_clicked(bool clicked);
    void onError(QAbstractSocket::SocketError socketError);

    void on_btn_menu_toggled();
    void on_setting(const QString&,int);//��������

private:
    Ui::RoomListWindow *ui;
    ClientSocket *cs;
    QTcpSocket *socket;
    user_t myinfo;

    QLabel *label_roomname;
    QCheckBox *bs;
    QCheckBox *btn_camera;
    QCheckBox *btn_audio;
    QLabel *label_vedio;
    QPushButton *btn_quit;
    QPushButton *btn_send;
    QPushButton *btn_gift;
    QLineEdit *line_input;
    QListWidget *text_widget;//直播间公�

    //QTimer *timer;
    //QLabel *bs_label;
    AudioVedio *av;

    QHostAddress ip;
    int port;

    MenuWindow *menuwindow;//�˵�����
    QString text_color;//������ʽ��
    QString bs_color;//��Ļ��ʽ��
};

#endif // ROOMLISTWINDOW_H
