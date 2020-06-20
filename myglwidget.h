#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class MyGlWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MyGlWidget(QWidget *parent=nullptr);
    void receiveImage(QImage);

protected:
    void paintEvent(QPaintEvent *e) override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    QImage image;
};

#endif // MYGLWIDGET_H
