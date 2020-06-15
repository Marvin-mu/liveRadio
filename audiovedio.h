#ifndef AUDIOVEDIO_H
#define AUDIOVEDIO_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QCamera>
#include <QCameraInfo>
#include "videosurface.h"
#include <QVideoFrame>
#include <QIODevice>
#include <QBuffer>

class AudioVedio : public QObject
{
    Q_OBJECT
public:
    explicit AudioVedio(QHostAddress,int,QHostAddress,int,QObject *parent = nullptr);
    void init(int);//初始化,分为主播和观众，建立音视频通讯
    void remove();//销毁音视频通讯
    void openCamera();//打开摄像头，发送图片
    void closeCamera();//打开麦克风，发送声音

    void openAudio();
    void closeAudio();
    void setIpPort(QHostAddress,int);

signals:
    void sigImage(QImage);
    void sigImageUser(QImage);

private slots:
    void on_videoChanged(QVideoFrame);                                      //视频采集成图片
    void ReadVedio();
    void ReadAudio();
    void handerAudio();

private:
    QAudioInput *audio_input;
    QAudioOutput *audio_output;
    //缓存音频数据
    QIODevice *device = nullptr;
    QCamera *camera;
    QUdpSocket *socket_vedio;
    QUdpSocket *socket_audio;

    QHostAddress vedio_group;
    int vedio_port;
    QHostAddress audio_group;
    int audio_port;
};

#endif // AUDIOVEDIO_H
