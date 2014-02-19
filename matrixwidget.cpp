#include "matrixwidget.h"
#include <QtOpenGL>
 
MatrixWidget::MatrixWidget(QWidget *parent) : QGLWidget(parent)
{
    
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
    glEnable(GL_DEPTH_TEST);
    
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    xRot=yRot=zRot=0;    
 
    spacing = 0.5f;
    numCubes = 20;
    ledSize = 0.2f;
    
    cubeSize = (ledSize*numCubes+spacing*numCubes);
    zoom = cubeSize;
}

float MatrixWidget::coords(int index) {
    return index*ledSize+spacing*index-cubeSize/2;
}

void MatrixWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
       
    glRotatef(xRot,1.0f,0.0f,0.0f);
    glRotatef(yRot,0.0f,1.0f,0.0f);
    glRotatef(zRot,0.0f,0.0f,1.0f);
   
    bool on = false;
    
    for (float i = 0; i < numCubes; i++) {
        for (float j = 0; j < numCubes; j++) {
            for (float k = 0; k < numCubes; k++) {
                if (i == j && j == k ) {
                    on = true;
                } else {
                    on = false;
                }
                drawCube(ledSize, coords(i), coords(j), coords(k), on);
                
            }
        }
    
    }
}

void MatrixWidget::drawCube(float size, float dx, float dy, float dz, bool on) 
{
    float alpha;
    if (on) {
        alpha = 1.0;
    } else {
        alpha = 0.1;
    }
    
    glBegin(GL_QUADS);
    
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    
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

void MatrixWidget::setZoom(int newZ) {
    zoom = cubeSize - newZ*cubeSize/360;
    resizeGL(width(), height());
    updateGL();
}

void MatrixWidget::resizeGL(int w, int h)
{
    if(w == 0) w = 1;
    if(h == 0) h = 1;

    double aspect =(double)w/(double)h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    
    if (w <= h)
        glOrtho(-zoom, zoom,-zoom/aspect,zoom/aspect,-(zoom*2),cubeSize*2);
    else
        glOrtho(-(zoom*aspect),zoom*aspect,-zoom,zoom,-(zoom*2),cubeSize*2);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + dy);
        setZRotation(zRot + dx);
    }
    lastPos = event->pos();
}
