#include "audiovedio.h"
#include <QTime>

AudioVedio::AudioVedio(QHostAddress ip1,int port1, QHostAddress ip2, int port2, QObject *parent)
    : QObject(parent)
{
    vedio_group = ip1;
    vedio_port = port1;
    audio_group = ip2;
    audio_port = port2;
}

//实现房主和用户的不同初始化
void AudioVedio::init(int type)
{
    // 设置音频文件格式;
    QAudioFormat format;
    // 设置采样频率;
    format.setSampleRate(8000);
    // 设置通道数;
    format.setChannelCount(1);
    // 设置每次采样得到的样本数据位值;
    format.setSampleSize(8);
    // 设置编码方法;
    format.setCodec("audio/pcm");
    // 设置采样字节存储顺序;
    format.setByteOrder(QAudioFormat::LittleEndian);
    // 设置采样类型;
    format.setSampleType(QAudioFormat::UnSignedInt);

    if (type == 1) {
        //相机配置
        QCameraInfo info = QCameraInfo::defaultCamera();
        camera = new QCamera(info, this);
        VideoSurface *vf = new VideoSurface(this);
        camera->setViewfinder(vf);
        camera->setCaptureMode(QCamera::CaptureVideo);
        QObject::connect (vf, SIGNAL(videoChanged(QVideoFrame)),
             this, SLOT(on_videoChanged(QVideoFrame)));//发送视屏图片的连接

        //麦克风配置
        QAudioDeviceInfo in = QAudioDeviceInfo::defaultInputDevice();
        QString str=info.deviceName();
        qDebug()<<"使用的录音设备是:"<<str<<endl;
        // 判断当前设备设置是否支持该音频格式;
        if(!in.isFormatSupported(format))
        {
            format = in.nearestFormat(format);
        }
        socket_vedio = new QUdpSocket(this);
        socket_vedio->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);

        audio_input = new QAudioInput(format, this);
        audio_output = NULL;

        socket_audio = new QUdpSocket(this);
        socket_audio->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
        qDebug() << "主播初始化完成";
    } else if (type == 0) {
        camera = NULL;
        socket_vedio = new QUdpSocket(this);
        socket_vedio->bind(QHostAddress::AnyIPv4, vedio_port,
                                QUdpSocket::ReuseAddressHint|QUdpSocket::ShareAddress);
        socket_vedio->joinMulticastGroup(vedio_group);
        qDebug() << "视频加入的 ip " << vedio_group << " port" << vedio_port;
        connect(socket_vedio, SIGNAL(readyRead()), this, SLOT(ReadVedio()));
        audio_input = NULL;
        audio_output = new QAudioOutput(format, this);
        device = audio_output->start();

        socket_audio = new QUdpSocket(this);
        socket_audio->bind(QHostAddress::AnyIPv4, audio_port,
                                QUdpSocket::ReuseAddressHint|QUdpSocket::ShareAddress);
        socket_audio->joinMulticastGroup(audio_group);
        qDebug() << "音频加入的 ip " << audio_group << " port" << audio_port;
        connect(socket_audio, SIGNAL(readyRead()), this, SLOT(ReadAudio()));
        qDebug() << "听众初始化完成";
    } else {
        qDebug() << "进入直播间初始化失败";
    }
}

void AudioVedio::openCamera()
{
    camera->start();
}

void AudioVedio::openAudio()
{
    device = audio_input->start();
    connect(device, SIGNAL(readyRead()),
            this, SLOT(handerAudio()));
}

void AudioVedio::handerAudio()
{
    QByteArray array;
    array.resize(1024);
    if (device->read(array.data(), array.size()) == -1) {
        qDebug() << "audio_device reading error";
        return;
    }
    //qDebug() << "send audio size: " <<array.size();
    socket_audio->writeDatagram(array.data(), array.size(), audio_group, audio_port);
}

void AudioVedio::closeCamera()
{
   camera->stop();
}

void AudioVedio::closeAudio()
{
    audio_input->stop();
}

void AudioVedio::on_videoChanged(QVideoFrame curFrame)
{
    QVideoFrame frame(curFrame);
    frame.map(QAbstractVideoBuffer::ReadOnly);//映射到cpu地址
    //通过QVideoFrame对象构建QImage对象
    QImage image(frame.bits(),
                  frame.width(),
                 frame.height(),
                 QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));
    //将图像旋转180
    QMatrix matrix;
    matrix.rotate(180);
    image = image.transformed(matrix);
    emit sigImage(image);
    //广播图片
    QByteArray byte;
    //字节数组 要进行传输必须先转换成这个格式
    QBuffer buff(&byte);
    // 建立一个用于IO读写的缓冲区
    image.save(&buff,"JPEG");
    // image先向下转为byte的类型，再存入buff
    QByteArray compressByte = qCompress(byte,1);
    //数据压缩算法
    QByteArray base64Byte = compressByte.toBase64();
    //数据加密算法
    socket_vedio->writeDatagram(base64Byte.data(),base64Byte.size(),
                         vedio_group, vedio_port);
    emit sigImage(image);
}

void AudioVedio::ReadVedio()
{
    QByteArray datagram;
    // 让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
    datagram.resize(socket_vedio->pendingDatagramSize());

    // 接收数据报，将其存放到datagram中
    socket_vedio->readDatagram(datagram.data(), datagram.size());

    QByteArray decryptedByte;
    decryptedByte = QByteArray::fromBase64(datagram.data());//获取base64数据
    QByteArray uncompressByte = qUncompress(decryptedByte);//解压缩
    QImage image;
    image.loadFromData(uncompressByte);
    emit sigImageUser(image);
    qDebug() << __FUNCTION__;
}

void AudioVedio::ReadAudio()
{
    while (socket_audio->hasPendingDatagrams()) {
        QByteArray array;
        array.resize(1024);
        socket_audio->readDatagram(array.data(), array.size());
        device->write(array.data(), array.size());
    }
}

void AudioVedio::remove()
{
    if (NULL != camera) {
        delete camera;
    }
    if (NULL != socket_vedio) {
        delete socket_vedio;
    }
    if (NULL != audio_input) {
        delete audio_input;
    }
    if (NULL != socket_audio) {
        delete socket_audio;
    }
    if (NULL != audio_output) {
        delete audio_output;
    }
}

void AudioVedio::setIpPort(QHostAddress ip, int port)
{
    vedio_port = port;
    audio_port = port + 1;
    vedio_group = ip;
    audio_group = ip;
}

