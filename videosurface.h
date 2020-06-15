#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QObject>
#include <QAbstractVideoSurface>

class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    VideoSurface(QObject*parent=nullptr);
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType) const;

    bool present(const QVideoFrame &frame);
signals:
    void videoChanged(QVideoFrame);
};

#endif // VIDEOSURFACE_H
