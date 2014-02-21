#include "matrixwidget.h"
#include <QtOpenGL>
#include <cmath>

MatrixWidget::MatrixWidget(QWidget *parent) : QGLWidget(parent)
{
    settings = new QSettings("groupname", "LEDcube");
    mode = settings->value("drawMode", MODE_POINTS).toInt();
    xRot = yRot = zRot = 0;
    if (mode == MODE_POINTS) {
        ledSize = 0.0f;
        spacing = 0.5f;
    } else if (mode == MODE_CUBES) {
        ledSize = 0.2f;
        spacing = settings->value("spacing", 0.5f).toFloat();
    }
    
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = false;
    
    xCubes = settings->value("xSize", 20).toInt();
    yCubes = settings->value("ySize", 20).toInt();
    zCubes = settings->value("zSize", 20).toInt();
    calcCubeSize();
    calcZoom();
}

QSize MatrixWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize MatrixWidget::sizeHint() const
{
    return QSize(400, 400);
}
 
void MatrixWidget::initializeGL()
{
    glClearColor(0,0,0,0);
    
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void MatrixWidget::calcZoom() {
    zoom = maximum(xCubeSize, yCubeSize, zCubeSize);
}

void MatrixWidget::calcCubeSize() {
    xCubeSize = (ledSize*xCubes+spacing*(xCubes-1));
    yCubeSize = (ledSize*yCubes+spacing*(yCubes-1));
    zCubeSize = (ledSize*zCubes+spacing*(zCubes-1));
}

float MatrixWidget::xCoords(int index) {
    return index*ledSize+spacing*index-xCubeSize/2;
}

float MatrixWidget::yCoords(int index) {
    return index*ledSize+spacing*index-yCubeSize/2;
}

float MatrixWidget::zCoords(int index) {
    return index*ledSize+spacing*index-zCubeSize/2;
}

bool MatrixWidget::isOn(int x, int y, int z) {
    double extent = yCubes/4.0;
    int yval = round((double)sin(x*3.14/extent)*(double)extent + extent*2);
    if(y == yval /*&& z == zCubes/2*/) {
        return true;
    }
    return false;
}

void MatrixWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
       
    glRotatef(xRot,1.0f,0.0f,0.0f);
    glRotatef(yRot,0.0f,1.0f,0.0f);
    glRotatef(zRot,0.0f,0.0f,1.0f);
    
    for (float i = 0; i < xCubes; i++) {
        for (float j = 0; j < yCubes; j++) {
            for (float k = 0; k < zCubes; k++) {
                if (mode == MODE_POINTS) {
                    drawPoint(i,j,k);  
                } else if (mode == MODE_CUBES) {
                    drawCube(i,j,k);  
                }
                
            }
        }
    
    }
}

void MatrixWidget::drawCube(int x, int y, int z) 
{
    float dx = xCoords(x);
    float dy = yCoords(y);
    float dz = zCoords(z);

    float alpha;
    bool on = isOn(x,y,z);

    if (on) {
        alpha = 1.0;
    } else {
        alpha = 0.05;
    }

    if (on || DRAW_OFF_LEDS_AS_TRANSLUSCENT) {
        glBegin(GL_QUADS);
        
        glColor4f(1.0f, 1.0f, 1.0f, alpha);

        float size = ledSize;
        
        glVertex3f(size+dx, size+dy, dz);
        glVertex3f(dx, size+dy, dz);
        glVertex3f(dx, size+dy, size+dz);
        glVertex3f(size+dx, size+dy, size+dz);
        
        glVertex3f(size+dx, dy, size+dz);
        glVertex3f(dx, dy, size+dz);
        glVertex3f(dx, dy, dz);
        glVertex3f(size+dx, dy, dz);
        
        glVertex3f(size+dx, size+dy, size+dz);
        glVertex3f(dx, size+dy, size+dz);
        glVertex3f(dx, dy, size+dz);
        glVertex3f(size+dx, dy, size+dz);
        
        glVertex3f(size+dx, dy, dz);
        glVertex3f(dx, dy, dz);
        glVertex3f(dx, size+dy, dz);
        glVertex3f(size+dx, size+dy, dz);
        
        glVertex3f(dx, size+dy, size+dz);
        glVertex3f(dx, size+dy, dz);
        glVertex3f(dx, dy, dz);
        glVertex3f(dx, dy, size+dz);
        
        glVertex3f(size+dx, size+dy, dz);
        glVertex3f(size+dx, size+dy, size+dz);
        glVertex3f(size+dx, dy, size+dz);
        glVertex3f(size+dx, dy, +dz);
        
        glEnd();
    }
}

void MatrixWidget::drawPoint(int x, int y, int z) 
{
    float dx = xCoords(x);
    float dy = yCoords(y);
    float dz = zCoords(z);

    float alpha;
    bool on = isOn(x,y,z);

    if (on) {
        alpha = 1.0;
    } else {
        alpha = 0.05;
    }
    if (on || DRAW_OFF_LEDS_AS_TRANSLUSCENT) {
        glBegin(GL_POINTS);
        
        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        
        glVertex3f(dx, dy, dz);
        
        glEnd();
    }
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
        updateGL();
    }
}

void MatrixWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MatrixWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MatrixWidget::setZoom(int newZoom) {
    float cubeSize = maximum(xCubeSize, yCubeSize, zCubeSize);
    if (newZoom == 360) newZoom = 359; 
    zoom = cubeSize - newZoom*cubeSize/360;
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::setSpacing(int intspaceing) {
    spacing = (float)intspaceing / (float)10;
    settings->setValue("spacing", spacing);
    calcCubeSize();
    calcZoom();
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::setMode(int cur) {
    if(cur == 0) {
        ledSize = 0.2f;
        spacing = settings->value("spacing", 0.5f).toFloat();
        mode = MODE_CUBES;
        emit setSpacingSliderEnabled(true);
    } else if (cur == 1){
        ledSize = 0.0f;
        spacing = 0.5f;
        mode = MODE_POINTS;
        emit setSpacingSliderEnabled(false);
    }
    settings->setValue("drawMode", mode);
    calcCubeSize();
    calcZoom();
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::setXSize(int size) {
    xCubes = size;
    settings->setValue("xSize", xCubes);
    calcCubeSize();
    calcZoom();
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::setYSize(int size) {
    yCubes = size;
    settings->setValue("ySize", yCubes);
    calcCubeSize();
    calcZoom();
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::setZSize(int size) {
    zCubes = size;
    settings->setValue("zSize", zCubes);
    calcCubeSize();
    calcZoom();
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::toggleDrawOff(bool draw) {
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = draw;
    updateGL();
}

void MatrixWidget::resizeGL(int w, int h)
{
    float aspect =(float)w/ ( h ? h : 1 );

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, w, h);
    
    glOrtho(-zoom, zoom,-zoom/aspect,zoom/aspect,-(zoom*2),zoom*2);

    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}

void MatrixWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
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

void MatrixWidget::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan( fovy * M_PI / 360.0 );
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}
