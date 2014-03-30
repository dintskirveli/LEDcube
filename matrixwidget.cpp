#include "matrixwidget.h"
#include <QtOpenGL>
#include <cmath>
#include <QTimer>
#include <iostream>
#include <sys/timeb.h>
#include <stdio.h>
#include "libfreenect.h"
#include "main.h"



MatrixWidget::MatrixWidget(freenect_device *dev, freenect_context *ctx) : QGLWidget()
{
    settings = new QSettings("groupname", "LEDcube");
    mode = settings->value("drawMode", MODE_POINTS).toInt();
    ledSize =1;
    spacing = settings->value("spacing", 0.5f).toFloat();
    transparency = 0.05f;
    rawZoom = 0;
    setZoom(rawZoom);
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = false;
    
    xCubes = settings->value("xSize", 20).toInt();
    yCubes = settings->value("ySize", 20).toInt();
    zCubes = settings->value("zSize", 20).toInt();
    calcCubeSize();
    //onCoords.find(boost::make_tuple(1,2,3));
    f_ctx = ctx;
    f_dev = dev;



    if (dev != NULL && ctx != NULL) {
        data_source = SOURCE_KINECT;
    } else {
        printf("No device found\n");
        data_source = SOURCE_MATH;
    }
    setXRotation(0);
    setYRotation(0);
    setZRotation(0);

    // set up timer to call updateGL() fps times a second
    // if I recall correctly updateGL() should be a no-op
    // if the current call to paintGL is still running, so 
    // in theory this should be okay even for systems that 
    // cant handle the fps. So, fps is an upper bound
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    double fps = 30.0; //aim for 30 frames per second
    timer->start(1000/fps);

    printf("end constructor");
}

//os-portable way to get time with millisecond precision
//taken from http://www.cplusplus.com/forum/general/43203/
//Note: time.h's clock() function isn't good for animation
//because it is based on CPU ticks, the speed of which varies
static int getMilliCount()
{
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

QSize MatrixWidget::sizeHint() const
{
    return QSize(400, 400);
}
 
void MatrixWidget::initializeGL()
{
    glClearColor(0,0,0,0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //anti-aliasing
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
}

static float maximum(float x, float y, float z) {
    int max = x; 
    if (y > max) {
        max = y;
    } else if (z > max) {
        max = z;
    }
    return max;
}

void MatrixWidget::calcCubeSize() {
    xCubeSize = xCubes*delta() - spacing;
    yCubeSize = yCubes*delta() - spacing;
    zCubeSize = zCubes*delta() - spacing;
    maxCube = maximum(xCubeSize, yCubeSize, zCubeSize);
}


bool MatrixWidget::isOn(int x, int y, int z, int t) {
    if (data_source == SOURCE_MATH) {
        if (false) return true;
        if (xCubes == 1 && yCubes == 1 && zCubes == 1) {
            return true;
        }
        if(y == round(sin(z/2 + t/100)*2)+ round(sin(x/2 /*- t/100*/)*2) +yCubes/2) {
            return true;
        }
        return false;
    } else if (data_source == SOURCE_KINECT) {
        pthread_mutex_lock(&depth_mutex);
        bool d = onCoords.find(boost::make_tuple(x,y,z)) != onCoords.end();
        pthread_mutex_unlock(&depth_mutex);
        return d;
    } else {
        return false;
    }
}

float MatrixWidget::delta() {
    return spacing + (mode == MODE_POINTS ? 0 : ledSize);
}

void MatrixWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //'a' is the diagonal of the maximum cube. 
    //this is a good value to base frustum calculations on
    //...because the cube will always fit,
    //and clipping will therefore not occur.
    //also calculating a similar value in resizeGL()
    float a = (((float) maxCube) * sqrt((float) 3)); 

    glTranslatef(0,0, -4*a);
    glRotatef(xRot,1.0f,0.0f,0.0f);
    glRotatef(yRot,0.0f,1.0f,0.0f);
    glRotatef(zRot,0.0f,0.0f,1.0f);
    bool on;
    int t = getMilliCount();
    if (mode == MODE_POINTS) {
        //std::cout << spacing << "\n";
        glPointSize(spacing*10);
    }
    //std::cout << t << "\n";
    for (float i = 0; i < xCubes; i++) {
        for (float j = 0; j < yCubes; j++) {
            for (float k = 0; k < zCubes; k++) {
                glPushMatrix();
                glTranslatef(
                    i*delta() - xCubeSize/2,
                    j*delta() - yCubeSize/2,
                    k*delta() - zCubeSize/2);
                on = isOn(i,j,k,t);
                if (on || DRAW_OFF_LEDS_AS_TRANSLUSCENT) {
                    glColor4f(1.0f, 1.0f, 1.0f, on ? 1.0 : transparency);
                    if (mode == MODE_POINTS && (on || transparency)) {
                        drawPoint();  
                    } else if (mode == MODE_CUBES && (on || transparency)) {
                        drawCube();  
                    }
                }
                glPopMatrix();
                
            }
        }
    }
}

void MatrixWidget::resizeGL(int w, int h)
{
    float aspect =(float)w/ ( h ? h : 1 );

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    float a= ((float) maxCube) * sqrt((float)3); 
    glViewport(0, 0, w, h);
    float z=zoom;
    //glFrustum(-a*aspect/2-1,a*aspect/2+1,-a/2-1, a/2+1, a, 3*a);
    glFrustum(-(a/2)*aspect*z,(a/2)*aspect*z,-(a/2)*z,(a/2)*z,3*a,6*a);
    /*
    void glOrtho(   GLdouble    left,
                    GLdouble    right,
                    GLdouble    bottom,
                    GLdouble    top,
                    GLdouble    nearVal,
                    GLdouble    farVal);
                    */
    
    glMatrixMode(GL_MODELVIEW);
}

void MatrixWidget::drawCube() 
{
    glBegin(GL_QUADS);
    
    glVertex3f(ledSize, ledSize, 0);
    glVertex3f(0, ledSize, 0);
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(ledSize, ledSize, ledSize);
    
    glVertex3f(ledSize, 0, ledSize);
    glVertex3f(0, 0, ledSize);
    glVertex3f(0, 0, 0);
    glVertex3f(ledSize, 0, 0);
    
    glVertex3f(ledSize, ledSize, ledSize);
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(0, 0, ledSize);
    glVertex3f(ledSize, 0, ledSize);
    
    glVertex3f(ledSize, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, ledSize, 0);
    glVertex3f(ledSize, ledSize, 0);
    
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(0, ledSize, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, ledSize);
    
    glVertex3f(ledSize, ledSize, 0);
    glVertex3f(ledSize, ledSize, ledSize);
    glVertex3f(ledSize, 0, ledSize);
    glVertex3f(ledSize, 0, 0);

    glEnd();
    
}

void MatrixWidget::drawPoint() 
{
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);   
    glEnd();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;
}

void MatrixWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        //updateGL();
    }
}

void MatrixWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        //updateGL();
    }
}

void MatrixWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        //updateGL();
    }
}

void MatrixWidget::setZoom(int newZoom)
{
    rawZoom = newZoom;
    //map -100 - 100 to 0-2
    zoom = ((float)newZoom/-100)+1;
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::setTransparency(int percent) {
    transparency = (float) percent / 100;
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::setSpacing(int intspaceing) {
    spacing = (float)intspaceing / (float)10;
    settings->setValue("spacing", spacing);
    calcCubeSize();
    resizeGL(width(), height());
    //updateGL();
    //std::cout << spacing << "\n";
}

void MatrixWidget::setMode(int cur) {
    if(cur == 0) {  
        mode = MODE_CUBES;
    } else if (cur == 1){
        mode = MODE_POINTS;
    }
    settings->setValue("drawMode", mode);
    calcCubeSize();
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::setXSize(int size) {
    xCubes = size;
    settings->setValue("xSize", xCubes);
    calcCubeSize();
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::setYSize(int size) {
    yCubes = size;
    settings->setValue("ySize", yCubes);
    calcCubeSize();
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::setZSize(int size) {
    zCubes = size;
    settings->setValue("zSize", zCubes);
    calcCubeSize();
    resizeGL(width(), height());
    //updateGL();
}

void MatrixWidget::toggleDrawOff(bool draw) {
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = draw;
    //updateGL();
}

void MatrixWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MatrixWidget::wheelEvent(QWheelEvent* event) {
    //divide by two to decrease zoom speed
    rawZoom+=event->delta()/2;

    if (rawZoom > 100) rawZoom = 100;
    if (rawZoom < -100) rawZoom = -100;

    setZoom(rawZoom);
    emit zoomChanged(rawZoom);
}

void MatrixWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    
    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + dy);
        setYRotation(yRot + dx);
    }
    lastPos = event->pos();
}
