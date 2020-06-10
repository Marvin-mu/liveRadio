#ifndef AUDIOVEDIO_H
#define AUDIOVEDIO_H

#include <QObject>

class AudioVedio : public QObject
{
    Q_OBJECT
public:
    explicit AudioVedio(QObject *parent = nullptr);

signals:

};

#endif // AUDIOVEDIO_H
