#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QGLWidget>
#include <QSettings>

//! LEDMatrix Widget
/*!
    Extension of QGLWidget on which the actual led matrix is displayed.
    
*/
class MatrixWidget : public QGLWidget
{
    Q_OBJECT

public:
    MatrixWidget(QWidget *parent = 0);
    enum { MODE_CUBES, MODE_POINTS };
    //decides whether or not to draw the leds that are off
    bool DRAW_OFF_LEDS_AS_TRANSLUSCENT;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setZoom(int zoom);
    void setMode(int cur);
    void setSpacing(int spacing);
    void setXSize(int size);
    void setYSize(int size);
    void setZSize(int size);
    
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void setSpacingSliderEnabled(bool enabled);

protected:
    void drawCube(int x, int y, int z);
    void drawPoint(int x, int y, int z); 
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    float xCoords(int index);
    float yCoords(int index);
    float zCoords(int index);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void calcCubeSize();
    void calcZoom();
    bool isOn(int x, int y, int z);
    void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

private:
    int mode;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    float spacing;
    int xCubes;
    int yCubes;
    int zCubes;
    float ledSize;
    float xCubeSize;
    float yCubeSize;
    float zCubeSize;
    float zoom;
    QSettings * settings;
};

#endif
