#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class QtLogo;

class MatrixWidget : public QGLWidget
{
    Q_OBJECT

public:
    MatrixWidget(QWidget *parent = 0);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setZoom(int zoom);
    
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void drawCube(float size, float dx, float dy, float dz, bool on);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    float coords(int index);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    float spacing;
    int numCubes;
    float ledSize;
    float cubeSize;
    float zoom;
};

#endif
