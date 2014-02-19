#include <QtGui>

#include "matrixwidget.h"
#include "window.h"

//! [0]
Window::Window()
{
    matrixWidget = new MatrixWidget;

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();
    zoomSlider = createSlider();

    connect(xSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXRotation(int)));
    connect(matrixWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYRotation(int)));
    connect(matrixWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZRotation(int)));
    connect(matrixWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));

    connect(zoomSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZoom(int)));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(matrixWidget);
    mainLayout->addWidget(xSlider);
    mainLayout->addWidget(ySlider);
    mainLayout->addWidget(zSlider);
    mainLayout->addWidget(zoomSlider);
    setLayout(mainLayout);

    xSlider->setValue(0);
    ySlider->setValue(0);
    zSlider->setValue(0);
    zoomSlider->setValue(0);
    setWindowTitle(tr("LEDcube"));
}

QSlider *Window::createSlider(int range, int singleStep, int pageStep, int tickInterval)
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, range);
    slider->setSingleStep(singleStep);
    slider->setPageStep(pageStep); 
    slider->setTickInterval(tickInterval);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
