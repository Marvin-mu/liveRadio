#include "myglwidget.h"
#include <QPainter>

MyGlWidget::MyGlWidget(QWidget *parent)
    :QOpenGLWidget(parent)
{

}

void MyGlWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //painter.setPen(QColor(255,0,0));
    //painter.setBrush(QBrush(QColor(255,255,255),QPixmap("D:\\Desk\11.png")));
    //painter.drawRect(30,30,200,200);
    painter.drawImage(this->rect(), image);
    painter.end();
}

void MyGlWidget::receiveImage(QImage i)
{
    image = i;
    this->update();
}

void MyGlWidget::initializeGL()
{
    //initializeOpenGLFunctions();
    glClearColor(255,255,255,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void MyGlWidget::paintGL()
{
    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    glColor3f(1.0, 0.0, 0.0);

    glVertex3f(-100, -100, 0);

    glColor3f(0.0, 1.0, 0.0);

    glVertex3f( 100, -100, 0);

    glColor3f(0.0, 0.0, 1.0);

    glVertex3f( 0.0, 100, 0);

    glEnd();
    QOpenGLTexture *texture = new QOpenGLTexture(QImage("D:\\desk\\11.png").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    // Render with texture
    texture->bind();
    glDrawArrays(GL_POINTS,0,0);

    */
}

void MyGlWidget::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glOrtho(-width/2,width/2,-height/2,height/2,-1,1);

    glMatrixMode(GL_MODELVIEW);
}
