#include "glwidget.h"
#include <QtOpenGL>
 
GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
 
void GLWidget::initializeGL()
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

float GLWidget::coords(int index) {
    return index*ledSize+spacing*index-cubeSize/2;
}

void GLWidget::paintGL()
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

void GLWidget::drawCube(float size, float dx, float dy, float dz, bool on) 
{
    float alpha;
    if (on) {
        alpha = 1.0;
    } else {
        alpha = 0.1;
    }
    
    glBegin(GL_QUADS);
    
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    
    glVertex3f(size+dx, size+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, size+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, size+dy, size+dz);
    glVertex3f(size+dx, size+dy, size+dz);
    
    glVertex3f(size+dx, 0.0f+dy, size+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, size+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, 0.0f+dz);
    glVertex3f(size+dx, 0.0f+dy, 0.0f+dz);
    
    glVertex3f(size+dx, size+dy, size+dz);
    glVertex3f(0.0f+dx, size+dy, size+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, size+dz);
    glVertex3f(size+dx, 0.0f+dy, size+dz);
    
    glVertex3f(size+dx, 0.0f+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, size+dy, 0.0f+dz);
    glVertex3f(size+dx, size+dy, 0.0f+dz);
    
    glVertex3f(0.0f+dx, size+dy, size+dz);
    glVertex3f(0.0f+dx, size+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, 0.0f+dz);
    glVertex3f(0.0f+dx, 0.0f+dy, size+dz);
    
    glVertex3f(size+dx, size+dy, 0.0f+dz);
    glVertex3f(size+dx, size+dy, size+dz);
    glVertex3f(size+dx, 0.0f+dy, size+dz);
    glVertex3f(size+dx, 0.0f+dy, 0.0f+dz);
    
    glEnd();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}


void GLWidget::setZoom(int newZ) {
    zoom = cubeSize - newZ*cubeSize/360;
    resizeGL(width(), height());
    updateGL();
}

void GLWidget::resizeGL(int w, int h)
{
    double Aspect;
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width)
    if(h == 0)//altura
        h = 1;
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    
    
    Aspect=(double)w/(double)h;
    
    //Reset projection matrix stack
    glMatrixMode(GL_PROJECTION);
    // Reset the coordinate system before modifying
    glLoadIdentity();
    
    
    double Range = zoom;
    
    if (w <= h)
        glOrtho(-(Range),Range,-Range/Aspect,Range/Aspect,-(Range*2),cubeSize*2);
    else
        glOrtho(-(Range*Aspect),Range*Aspect,-Range,Range,-(Range*2),cubeSize*2);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
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
