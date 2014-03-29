#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QGLWidget>
#include <QSettings>
#include <ctime>
#include <QWheelEvent>

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
    void setTransparency(int percent);
    void setXSize(int size);
    void setYSize(int size);
    void setZSize(int size);
    void toggleDrawOff(bool draw);
    
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void setSpacingSliderEnabled(bool enabled);
    void zoomChanged(int rawZoom);

protected:
    void drawCube();
    void drawPoint(); 
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    QSize sizeHint() const;
    void calcCubeSize();
    float delta();
    bool isOn(int x, int y, int z, int t);
    void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

private:
    int rawZoom;
    int mode;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    float spacing;
    float transparency;
    int xCubes;
    int yCubes;
    int zCubes;
    float ledSize;
    float xCubeSize;
    float yCubeSize;
    float zCubeSize;
    float maxCube;
    float zoom;
    QSettings * settings;
};

#endif
